#pragma once
#ifdef __DREAMCAST__
// Ring-buffer asset cache prototype.
//
// Data/assets.manifest (generated offline by tools/gen_manifest.py) indexes
// every .dt on the disc. Prefetch() asks a worker thread to read an asset into
// one of four 64 KB main-RAM slots; DCPVR_LoadDT calls Acquire() to use those
// bytes instead of hitting the CD, then Release() to free the slot.
namespace DCAssetCache {
void Init();                                             // lazy: safe to call repeatedly
void Prefetch(const char* name);                         // legacy name or ".dt" path
const unsigned char* Acquire(const char* dtPath, unsigned* size);  // 0 = miss / still loading
void Release(const char* dtPath);
void LockFile();                                         // serialize disc reads
void UnlockFile();
}
#endif
