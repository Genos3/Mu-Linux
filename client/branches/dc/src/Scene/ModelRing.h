#pragma once

// Bounded model-data cache: resident models live in one fixed-size ring
// buffer. Entries carry the user's 32x-style enabled/offset/prev/next state.
void ModelRing_Init(unsigned capacityBytes);
void ModelRing_Shutdown();
bool ModelRing_Take(int type);     // ensure model data resident + enabled
void ModelRing_Release(int type);  // no active instances left for this type
int  ModelRing_Ready();
