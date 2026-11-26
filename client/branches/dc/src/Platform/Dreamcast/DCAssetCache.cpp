#include "DCAssetCache.h"
#ifdef __DREAMCAST__

#include "DCPlatform.h"
#include "Platform/IFileSystem.h"
#include <kos/thread.h>
#include <kos/sem.h>
#include <kos/mutex.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

extern "C" bool DC_ResolveDtPath(const char* name, char* out, unsigned cap);

namespace {
// Four 40 KB slots cover all six login-ship textures (each <= 32800 bytes).
// The runtime manifest stores only a 64-bit hash + size per asset (~28 KB for
// 1784 entries) instead of the full 64-byte name.
constexpr int kSlots = 4;
constexpr unsigned kSlotSize = 40 * 1024;
constexpr int kQueue = 16;
constexpr int KEY_LEN = 80;
constexpr int MANIFEST_PATH_LEN = 64;

std::uint64_t HashName(const char* s)
{
    std::uint64_t h = 1469598103934665603ULL;
    while (*s) { h ^= static_cast<unsigned char>(*s++); h *= 1099511628211ULL; }
    return h;
}

struct ManifestEntry {
    std::uint64_t hash;
    unsigned size;
};

struct Slot {
    char key[KEY_LEN];
    unsigned char* data;
    unsigned size;
    volatile int state;   // 0 free, 1 loading, 2 ready, 3 in use
};

ManifestEntry* s_entries = nullptr;
unsigned s_count = 0;
Slot s_slots[kSlots];
char s_queue[kQueue][KEY_LEN];
int s_head = 0, s_tail = 0;
semaphore_t s_requestSem;
mutex_t s_fileMutex;
kthread_t* s_thread = nullptr;
bool s_inited = false;

// Strip "Data\"/"Data2\" prefixes and the extension so cache keys are stable
// regardless of which g_tex_ext_mode produced the path.
void Normalize(const char* path, char* out, unsigned cap)
{
    const char* p = path;
    if (std::strncmp(p, "/cd/", 4) == 0) p += 4;
    if (std::strncmp(p, "Data2\\", 6) == 0 || std::strncmp(p, "Data2/", 6) == 0) p += 6;
    else if (std::strncmp(p, "Data\\", 5) == 0 || std::strncmp(p, "Data/", 5) == 0) p += 5;

    char tmp[96];
    unsigned n = 0;
    while (*p && n < sizeof(tmp) - 1) {
        tmp[n++] = (*p == '\\') ? '/' : *p;
        ++p;
    }
    tmp[n] = 0;
    char* dot = std::strrchr(tmp, '.');
    if (dot) *dot = 0;
    if (cap) {
        std::strncpy(out, tmp, cap - 1);
        out[cap - 1] = 0;
    }
}

const ManifestEntry* Find(const char* key)
{
    std::uint64_t h = HashName(key);
    for (unsigned i = 0; i < s_count; ++i)
        if (s_entries[i].hash == h) return &s_entries[i];
    return nullptr;
}

void LoadManifest()
{
    std::FILE* f = Platform::GetFileSystem().Open("Data/assets.manifest", "rb");
    if (!f) return;
    char hdr[12];
    if (std::fread(hdr, 1, sizeof(hdr), f) != sizeof(hdr) ||
        std::memcmp(hdr, "DCAM", 4) != 0) { std::fclose(f); return; }
    unsigned count = 0;
    std::memcpy(&count, hdr + 8, 4);
    if (count > 4096) { std::fclose(f); return; }
    s_entries = new ManifestEntry[count];
    char name[MANIFEST_PATH_LEN];
    // gen_manifest.py packs "<IHHI": size(4) + w(2) + h(2) + pvr_type(4) = 12 bytes.
    unsigned char packed[12];
    unsigned loaded = 0;
    for (unsigned i = 0; i < count; ++i) {
        if (std::fread(name, 1, sizeof(name), f) != sizeof(name) ||
            std::fread(packed, 1, sizeof(packed), f) != sizeof(packed)) break;
        name[sizeof(name) - 1] = 0;
        s_entries[i].hash = HashName(name);
        std::memcpy(&s_entries[i].size, packed, 4);
        ++loaded;
    }
    std::fclose(f);
    s_count = loaded;
}

void* LoaderMain(void*)
{
    for (;;) {
        sem_wait(&s_requestSem);
        char key[KEY_LEN];
        std::memcpy(key, s_queue[s_head], KEY_LEN);
        s_head = (s_head + 1) % kQueue;

        int idx = -1;
        for (int i = 0; i < kSlots; ++i)
            if (s_slots[i].state == 0) { idx = i; break; }
        if (idx < 0) continue;   // ring full: drop this request

        Slot& slot = s_slots[idx];
        std::memcpy(slot.key, key, KEY_LEN);
        slot.state = 1;

        // Size comes from the offline manifest; the path is rebuilt from the
        // normalized key (every .dt lives under Data/ on the disc).
        const ManifestEntry* entry = Find(key);
        if (!entry || entry->size > kSlotSize) { slot.state = 0; continue; }

        char path[128];
        std::strcpy(path, "Data\\");
        std::strcat(path, key);
        std::strcat(path, ".dt");

        DCAssetCache::LockFile();
        std::FILE* f = Platform::GetFileSystem().Open(path, "rb");
        if (f) {
            std::size_t got = std::fread(slot.data, 1, entry->size, f);
            std::fclose(f);
            if (got == entry->size) { slot.size = entry->size; slot.state = 2; }
            else slot.state = 0;
        } else {
            slot.state = 0;
        }
        DCAssetCache::UnlockFile();
    }
    return nullptr;
}
} // namespace

namespace DCAssetCache {

void Init()
{
    if (s_inited) return;
    s_inited = true;
    LoadManifest();
    for (int i = 0; i < kSlots; ++i) {
        s_slots[i].data = static_cast<unsigned char*>(std::malloc(kSlotSize));
        s_slots[i].state = s_slots[i].data ? 0 : -1;
        s_slots[i].size = 0;
    }
    mutex_init(&s_fileMutex, MUTEX_TYPE_NORMAL);
    sem_init(&s_requestSem, 0);
    s_thread = thd_create(0, LoaderMain, nullptr);
    Platform::DCLog("[Cache] ring: %d x %uKB", kSlots, kSlotSize / 1024);
}

void Prefetch(const char* name)
{
    Init();
    if (!name) return;
    char dtPath[128];
    if (!DC_ResolveDtPath(name, dtPath, sizeof(dtPath))) return;
    char key[KEY_LEN];
    Normalize(dtPath, key, KEY_LEN);

    for (int i = 0; i < kSlots; ++i)
        if (s_slots[i].state != -1 && std::strcmp(s_slots[i].key, key) == 0 &&
            (s_slots[i].state == 1 || s_slots[i].state == 2)) return;

    std::memcpy(s_queue[s_tail], key, KEY_LEN);
    s_tail = (s_tail + 1) % kQueue;
    sem_signal(&s_requestSem);
}

const unsigned char* Acquire(const char* dtPath, unsigned* size)
{
    Init();
    if (!dtPath) return nullptr;
    char key[KEY_LEN];
    Normalize(dtPath, key, KEY_LEN);
    for (int i = 0; i < kSlots; ++i) {
        if (s_slots[i].state == 2 && std::strcmp(s_slots[i].key, key) == 0) {
            s_slots[i].state = 3;
            if (size) *size = s_slots[i].size;
            return s_slots[i].data;
        }
    }
    return nullptr;
}

void Release(const char* dtPath)
{
    if (!dtPath) return;
    char key[KEY_LEN];
    Normalize(dtPath, key, KEY_LEN);
    for (int i = 0; i < kSlots; ++i)
        if (s_slots[i].state == 3 && std::strcmp(s_slots[i].key, key) == 0)
            s_slots[i].state = 0;
}

void LockFile()   { mutex_lock(&s_fileMutex); }
void UnlockFile() { mutex_unlock(&s_fileMutex); }

} // namespace DCAssetCache
#endif
