// ModelRing.cpp - bounded, evicting model-data cache (user's r_buffer design).
//
// A loaded BMD consists of many separately allocated buffers. To make it
// cacheable in one contiguous ring, we serialize them into a single blob and
// rewrite every pointer in the BMD/mesh structures to point into that blob.
// The original allocations are freed. Entries track enabled/offset/prev/next;
// disabled entries at the ring tail are overwritten first.

#include "stdafx.h"
#include "Scene/ModelRing.h"
#include "Platform/Dreamcast/MUTrace.h"
#include "globals.h"
#include "functions.h"
#include "Model/ModelTypes.h"
#include "Model/BMDFileTypes.h"
#include "Physics/ClothTypes.h"
#include <cstring>

extern void __cdecl operator_delete(void* ptr);

namespace {
struct Entry {
    uint16_t type = 0;
    uint16_t enabled = 0;
    uint32_t offset = 0;
    uint32_t size = 0;
    int16_t prev = -1, next = -1;
};

Entry    s_entries[160];
bool     s_wasSerialized[160] = {};  // sticky: slot ever serialized into the ring
uint8_t* s_buf = nullptr;
uint32_t s_capacity = 0, s_head = 0;
int16_t  s_headIdx = -1, s_tailIdx = -1;

uint32_t Align4(uint32_t v) { return (v + 3u) & ~3u; }

void ListRemove(int16_t i) {
    if (i < 0) return;
    if (s_entries[i].prev >= 0) s_entries[s_entries[i].prev].next = s_entries[i].next;
    else s_headIdx = s_entries[i].next;
    if (s_entries[i].next >= 0) s_entries[s_entries[i].next].prev = s_entries[i].prev;
    else s_tailIdx = s_entries[i].prev;
    s_entries[i].prev = s_entries[i].next = -1;
}

void ListAppend(int16_t i) {
    s_entries[i].prev = s_tailIdx;
    s_entries[i].next = -1;
    if (s_tailIdx >= 0) s_entries[s_tailIdx].next = i;
    else s_headIdx = i;
    s_tailIdx = i;
}

// A model serialized into the ring keeps NumMeshs > 0 and pointers into s_buf.
// If its block is later overwritten, LoadMapObjectModel() would early-return
// ("already loaded") and hand us stale/garbage pointers. Force a real reload by
// clearing the slot before the BMD loader touches it again.
static void ResetModelSlot(int type) {
    BMD* m = AsModel(ModelsBase) + type;
    m->NumMeshs = 0;
    m->NumBones = 0;
    m->NumActions = 0;
    m->Meshs = nullptr;
    m->Bones = nullptr;
    m->Actions = nullptr;
    m->Textures = nullptr;
    m->IndexTexture = nullptr;
    m->LightMaps = nullptr;
}

// Invalidate any cached entry overlapping [offset, offset+bytes). Called before
// writing a new blob so overwritten entries are forced to reload on next Take.
static void InvalidateRange(uint32_t offset, uint32_t bytes) {
    const uint32_t end = offset + bytes;
    for (int i = 0; i < 160; ++i) {
        Entry& e = s_entries[i];
        if (e.size == 0) continue;
        const uint32_t eEnd = e.offset + e.size;
        if (e.offset < end && offset < eEnd) {
            ResetModelSlot(i);
            ListRemove(i);
            e = Entry();
            e.prev = e.next = -1;
        }
    }
}

// Copy src into the ring at cursor, rewrite the caller's pointer, advance.
template<class T>
uint8_t* Put(uint8_t*& cursor, T*& ptr, uint32_t bytes) {
    uint8_t* dst = cursor;
    if (ptr && bytes) std::memcpy(dst, ptr, bytes);
    ptr = reinterpret_cast<T*>(dst);
    cursor += Align4(bytes);
    return dst;
}
}

void ModelRing_Init(unsigned capacityBytes) {
    ModelRing_Shutdown();
    s_capacity = Align4(capacityBytes);
    if (s_capacity < 64 * 1024) s_capacity = 64 * 1024;
    s_buf = new uint8_t[s_capacity];
    if (!s_buf) { MU_TRACE("MODELRING: OOM allocating %u bytes", s_capacity); s_capacity = 0; return; }
    s_head = 0;
    s_headIdx = s_tailIdx = -1;
    std::memset(s_entries, 0, sizeof(s_entries));
    for (int i = 0; i < 160; ++i) s_entries[i].prev = s_entries[i].next = -1;
    MU_TRACE("MODELRING: %u KB", s_capacity / 1024);
}

void ModelRing_Shutdown() {
    delete[] s_buf; s_buf = nullptr;
    s_capacity = s_head = 0;
    s_headIdx = s_tailIdx = -1;
    for (int i = 0; i < 160; ++i) {
        s_entries[i] = Entry();
        s_entries[i].prev = s_entries[i].next = -1;
        s_wasSerialized[i] = false;
    }
}

int ModelRing_Ready() { return s_buf ? static_cast<int>(s_capacity) : 0; }

// Return the serialized size of a loaded model.
static uint32_t ModelBytes(BMD* m) {
    uint32_t n = sizeof(BMD);
    n += Align4(static_cast<uint32_t>(m->NumMeshs) * sizeof(ClothModelMesh));
    n += Align4(static_cast<uint32_t>(m->NumBones) * sizeof(ModelAnimationBone));
    n += Align4(static_cast<uint32_t>(m->NumActions) * sizeof(ModelAnimationAction));
    n += Align4(static_cast<uint32_t>(m->NumMeshs) * 32);
    n += Align4(static_cast<uint32_t>(m->NumMeshs) * sizeof(int16_t));
    if (m->LightMaps) n += Align4(4096);
    for (int i = 0; i < m->NumMeshs; ++i) {
        ClothModelMesh* mesh = reinterpret_cast<ClothModelMesh*>(m->Meshs) + i;
        n += Align4(static_cast<uint32_t>(mesh->VertexCount) * BmdFile::VertexBytes);
        n += Align4(static_cast<uint32_t>(mesh->NormalCount) * BmdFile::NormalBytes);
        n += Align4(static_cast<uint32_t>(mesh->TexCoordCount) * BmdFile::TexCoordBytes);
        n += Align4(static_cast<uint32_t>(mesh->TriangleCount) * sizeof(ModelRuntimeTriangle));
        if (mesh->TextureFlags) n += Align4(4);
    }
    return n;
}

// Return the offset at which a `need`-byte blob can be written. The bump
// pointer wraps once; any entries we overwrite are invalidated by the caller.
static uint32_t MakeRoom(uint32_t need) {
    if (!s_buf || need > s_capacity / 2) return 0xFFFFFFFFu;
    if (s_head + need > s_capacity) s_head = 0;
    return s_head;
}

static bool Serialize(int type, uint32_t offset, uint32_t& used) {
    BMD* m = AsModel(ModelsBase) + type;
    if (m->NumMeshs <= 0) return false;
    uint8_t* cursor = s_buf + offset;
    uint8_t* begin = cursor;
    uint32_t bytes = ModelBytes(m);
    if (offset + bytes > s_capacity) return false;

    // Save original pointers before Put() rewrites them.
    void* pMeshs = m->Meshs;
    void* pBones = m->Bones;
    void* pActions = m->Actions;
    void* pTextures = m->Textures;
    void* pIndex = m->IndexTexture;
    void* pLight = m->LightMaps;

    const int meshes = m->NumMeshs;
    const int bones = m->NumBones, actions = m->NumActions;
    Put(cursor, m->Meshs,    static_cast<uint32_t>(meshes) * sizeof(ClothModelMesh));
    Put(cursor, m->Bones,    static_cast<uint32_t>(bones) * sizeof(ModelAnimationBone));
    Put(cursor, m->Actions,  static_cast<uint32_t>(actions) * sizeof(ModelAnimationAction));
    Put(cursor, m->Textures, static_cast<uint32_t>(meshes) * 32);
    Put(cursor, m->IndexTexture, static_cast<uint32_t>(meshes) * sizeof(int16_t));
    if (m->LightMaps) Put(cursor, m->LightMaps, 4096);

    for (int i = 0; i < meshes; ++i) {
        ClothModelMesh* mesh = reinterpret_cast<ClothModelMesh*>(m->Meshs) + i;
        Put(cursor, mesh->Vertices,  static_cast<uint32_t>(mesh->VertexCount)   * BmdFile::VertexBytes);
        Put(cursor, mesh->Normals,   static_cast<uint32_t>(mesh->NormalCount)   * BmdFile::NormalBytes);
        Put(cursor, mesh->TexCoords, static_cast<uint32_t>(mesh->TexCoordCount) * BmdFile::TexCoordBytes);
        Put(cursor, mesh->Triangles, static_cast<uint32_t>(mesh->TriangleCount) * sizeof(ModelRuntimeTriangle));
        if (mesh->TextureFlags) Put(cursor, mesh->TextureFlags, 4);
    }

    // Free the original scattered allocations.
    if (pMeshs) operator_delete(pMeshs);
    if (pBones) operator_delete(pBones);
    if (pActions) operator_delete(pActions);
    if (pTextures) operator_delete(pTextures);
    if (pIndex) operator_delete(pIndex);
    if (pLight) operator_delete(pLight);
    for (int i = 0; i < meshes && pMeshs; ++i) {
        ClothModelMesh* mesh = reinterpret_cast<ClothModelMesh*>(pMeshs) + i;
        if (mesh->Vertices) operator_delete(mesh->Vertices);
        if (mesh->Normals) operator_delete(mesh->Normals);
        if (mesh->TexCoords) operator_delete(mesh->TexCoords);
        if (mesh->Triangles) operator_delete(mesh->Triangles);
        if (mesh->TextureFlags) operator_delete(mesh->TextureFlags);
    }

    used = static_cast<uint32_t>(cursor - begin);
    if (used > bytes) {
        MU_TRACE("RING OVERFLOW: id=%d used=%u computed=%u", type, used, bytes);
        return false;
    }
    return true;
}

bool ModelRing_Take(int type) {
    if (!s_buf || type < 0 || type >= 160) return false;
    Entry& e = s_entries[type];
    if (e.type == type && e.size) {
        // Block is still intact (InvalidateRange clears entries it overwrites).
        e.enabled = 1;
        return true;
    }

    // A previously serialized model still has NumMeshs > 0 with pointers into
    // ring memory that may have been overwritten. Clear the slot so the loader
    // actually re-reads the BMD instead of early-returning on stale pointers.
    if (s_wasSerialized[type]) ResetModelSlot(type);
    LoadMapObjectModel(type);
    BMD* m = AsModel(ModelsBase) + type;
    if (m->NumMeshs <= 0) return false;
    const uint32_t need = ModelBytes(m);
    const uint32_t offset = MakeRoom(need);
    if (offset == 0xFFFFFFFFu) {
        MU_TRACE("MODELRING: OOM full, cannot cache id %d (%u bytes of %u)",
                 type, need, s_capacity);
        return false;
    }

    InvalidateRange(offset, need);

    uint32_t used = 0;
    if (!Serialize(type, offset, used)) {
        MU_TRACE("MODELRING: OOM serialize failed for id %d (%u bytes at %u)",
                 type, need, offset);
        return false;
    }
    s_wasSerialized[type] = true;
    static int s_takeLog = 0;
    if (s_takeLog < 8) {
        ++s_takeLog;
        MU_TRACE("MODELRING: cached id=%d bytes=%u used=%u/%u", type, need, used, s_capacity);
    }
    e.type = static_cast<uint16_t>(type);
    e.enabled = 1;
    e.offset = offset;
    e.size = used;
    e.prev = e.next = -1;
    ListAppend(type);
    s_head = offset + used;
    return true;
}

void ModelRing_Release(int type) {
    if (!s_buf || type < 0 || type >= 160) return;
    Entry& e = s_entries[type];
    if (e.type == type && e.enabled) e.enabled = 0;
}
