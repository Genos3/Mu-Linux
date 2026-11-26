// Scene_ObjectsStreaming.cpp
//
// Phase 1 of map-object streaming (based on the user's 32x grid + r_buffer
// design). Keeps the pre-decrypted Terrain<N>.bin resident (92 KB for
// Lorencia), builds a 16x16 linear grid of object-record indices, and only
// instantiates OBJECT structs for records in cells near the camera. Out-of-view
// instances are unlinked from the scene grid and freed.
//
// Model data is still loaded eagerly by the existing paths; streaming the
// model payload through a ring buffer is phase 2.

#include "stdafx.h"
#include "Platform/IFileSystem.h"
#include "Platform/Dreamcast/MUTrace.h"
#include "globals.h"
#include "functions.h"
#include "Scene/ObjectRegistry.h"
#include "Scene/ModelRing.h"
#include <cstring>

extern void __cdecl operator_delete(void* ptr);

namespace {
struct Rec {
    uint16_t type;
    uint16_t pad;      // aligns the floats; original .obj records are 30 bytes
    float    pos[3];
    float    tgt[3];
    float    scale;
};
static_assert(sizeof(Rec) == 32, "DOBJ record layout");

unsigned char* s_raw = nullptr;
uint32_t       s_count = 0;
uint16_t       s_cellCount[256] = {};
uint16_t       s_cellStart[257] = {};
uint16_t*      s_cellItems = nullptr;     // s_count entries, sorted by cell
void*          s_inst[4096] = {};          // OBJECT* per record (max 4096)
uint32_t       s_stamp[4096] = {};         // frame stamp for visible records
uint16_t       s_typeRefs[160] = {};       // active instances per object type
uint16_t       s_active[4096] = {};        // active record indices
uint16_t       s_activeCount = 0;
uint32_t       s_frame = 0;
int            s_radius = 1;               // cells of 1600 units around camera

const Rec& rec(uint32_t i) { return reinterpret_cast<const Rec*>(s_raw + 12)[i]; }

int cellOf(float x, float y) {
    int cx = static_cast<int>(x * 0.000625f);   // 1/1600
    int cy = static_cast<int>(y * 0.000625f);
    if (cx < 0) cx = 0; if (cx > 15) cx = 15;
    if (cy < 0) cy = 0; if (cy > 15) cy = 15;
    return cy * 16 + cx;
}
}

void ObjectsStream_Init(const char* path) {
    ObjectsStream_Shutdown();
    ModelRing_Init(1200 * 1024);   // bounded model cache for visible types
    FILE* f = Platform::GetFileSystem().Open(path, "rb");
    if (!f) { MU_TRACE("OBJSTREAM: open failed %s", path); return; }
    std::fseek(f, 0, SEEK_END);
    long sz = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (sz < 12 || sz > 4 * 1024 * 1024) { std::fclose(f); return; }
    s_raw = new unsigned char[sz];
    if (!s_raw || std::fread(s_raw, 1, sz, f) != static_cast<size_t>(sz)) {
        delete[] s_raw; s_raw = nullptr; std::fclose(f); return;
    }
    std::fclose(f);
    if (std::memcmp(s_raw, "DOBJ", 4) != 0) {
        delete[] s_raw; s_raw = nullptr; return;
    }
    uint32_t version = 0;
    std::memcpy(&version, s_raw + 4, 4);
    std::memcpy(&s_count, s_raw + 8, 4);
    if (version != 1 || s_count == 0 || s_count > 4096 ||
        static_cast<long>(12 + s_count * sizeof(Rec)) > sz) {
        delete[] s_raw; s_raw = nullptr; s_count = 0; return;
    }

    std::memset(s_cellCount, 0, sizeof(s_cellCount));
    for (uint32_t i = 0; i < s_count; ++i) {
        const Rec& r = rec(i);
        ++s_cellCount[cellOf(r.pos[0], r.pos[1])];
    }
    s_cellStart[0] = 0;
    for (int c = 0; c < 256; ++c)
        s_cellStart[c + 1] = static_cast<uint16_t>(s_cellStart[c] + s_cellCount[c]);
    s_cellItems = new uint16_t[s_count];
    uint16_t cursor[256];
    for (int c = 0; c < 256; ++c) cursor[c] = s_cellStart[c];
    for (uint32_t i = 0; i < s_count; ++i)
        s_cellItems[cursor[cellOf(rec(i).pos[0], rec(i).pos[1])]++] = static_cast<uint16_t>(i);

    std::memset(s_inst, 0, sizeof(s_inst));
    std::memset(s_stamp, 0, sizeof(s_stamp));
    std::memset(s_typeRefs, 0, sizeof(s_typeRefs));
    s_activeCount = 0; s_frame = 0;
    MU_TRACE("OBJSTREAM: %u objects, %u bytes, grid built", s_count, sz);
}

void ObjectsStream_Shutdown() {
    ModelRing_Shutdown();
    for (uint16_t k = 0; k < s_activeCount; ++k) {
        uint32_t i = s_active[k];
        void* obj = s_inst[i];
        if (!obj) continue;
        int cell = cellOf(rec(i).pos[0], rec(i).pos[1]);
        Entity_GridUnlink(obj, &g_ObjectBucketGrid[cell]);
        SceneObjects::Unregister(obj);
        operator_delete(obj);
        s_inst[i] = nullptr;
        const int type = static_cast<int>(rec(i).type);
        if (type >= 0 && type < 160 && --s_typeRefs[type] == 0)
            ModelRing_Release(type);
    }
    s_activeCount = 0;
    delete[] s_cellItems; s_cellItems = nullptr;
    delete[] s_raw;       s_raw = nullptr;
    s_count = 0;
}

int ObjectsStream_Ready() { return s_raw && s_cellItems ? static_cast<int>(s_count) : 0; }

// The normal Object_TickUpdate only fades roof objects when their bucket is
// marked visible; the test scene's buckets are static, so apply the tavern
// roof hide directly to the streamed instances (matches PCWorldMapType==4).
void ObjectsStream_SetRoofHidden(bool hide) {
    for (uint16_t k = 0; k < s_activeCount; ++k) {
        void* obj = s_inst[s_active[k]];
        if (!obj) continue;
        const int type = static_cast<int>(rec(s_active[k]).type);
        if (type != 125 && type != 126) continue;
        float value = hide ? 0.0f : 1.0f;
        AsObject(obj)->Alpha = value;
        AsObject(obj)->AlphaTarget = value;
    }
}

void ObjectsStream_Update(float camX, float camY) {
    if (!s_raw || !s_cellItems) return;
    ++s_frame;
    int ccx = static_cast<int>(camX * 0.000625f);
    int ccy = static_cast<int>(camY * 0.000625f);

    for (int dy = -s_radius; dy <= s_radius; ++dy) {
        int cy = ccy + dy; if (cy < 0 || cy > 15) continue;
        for (int dx = -s_radius; dx <= s_radius; ++dx) {
            int cx = ccx + dx; if (cx < 0 || cx > 15) continue;
            const int cell = cy * 16 + cx;
            for (int p = s_cellStart[cell]; p < s_cellStart[cell + 1]; ++p) {
                uint32_t i = s_cellItems[p];
                s_stamp[i] = s_frame;
                if (s_inst[i]) continue;
                const Rec& r = rec(i);
                const int type = static_cast<int>(r.type);
                if (type >= 0 && type < 160 && s_typeRefs[type]++ == 0)
                    ModelRing_Take(type);       // bounded cache: load/enable model
                void* obj = CreateObject(type,
                                         const_cast<float*>(r.pos),
                                         const_cast<float*>(r.tgt), r.scale);
                if (obj) {
                    s_inst[i] = obj;
                    s_active[s_activeCount++] = static_cast<uint16_t>(i);
                } else if (type >= 0 && type < 160 && --s_typeRefs[type] == 0) {
                    ModelRing_Release(type);
                }
            }
        }
    }

    // Retire instances whose cell is no longer visible (swap-remove).
    for (uint16_t k = 0; k < s_activeCount;) {
        uint32_t i = s_active[k];
        if (s_stamp[i] == s_frame) { ++k; continue; }
        void* obj = s_inst[i];
        if (obj) {
            int cell = cellOf(rec(i).pos[0], rec(i).pos[1]);
            Entity_GridUnlink(obj, &g_ObjectBucketGrid[cell]);
            SceneObjects::Unregister(obj);
            operator_delete(obj);
            s_inst[i] = nullptr;
        }
        const int type = static_cast<int>(rec(i).type);
        if (type >= 0 && type < 160 && s_typeRefs[type] && --s_typeRefs[type] == 0)
            ModelRing_Release(type);
        s_active[k] = s_active[--s_activeCount];
    }

    if (s_frame % 5 == 0)
        MU_TRACE("OBJSTREAM: active=%u/%u camCell=(%d,%d)", s_activeCount, s_count, ccx, ccy);
}
