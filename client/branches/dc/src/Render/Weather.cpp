#include <cstdint>
// Weather.cpp
// IDA: Weather_Update (0x00500E80)
//
// Weather_Update — per-frame weather particle system update (846 lines).
//
// Manages 40 weather object slots at WeatherSlot0ActiveFlag (stride 0x1bc = 444 bytes).
// Also uses HashTable anti-tamper pattern around MoveSceneFrame.
//
// Weather types (short @ slot+2):
//   0xae  = falling cloud / puff (LoginScene)
//   0xaf  = snow flake
//   0xb0  = rain drop
//   0xb6  = snow variant A (World==7)
//   0xb7  = snow variant B
//   0xb8  = tree leaves / debris
//   0x10a = firefly / butterfly (circular orbit with sin/cos)
//   0x12d = thunder cloud (WeatherModeState == rain-mode)
//   0x1f  = thunder sound (played via OpenMonsterModel)
//
// Slot layout (all offsets relative to iVar14, stride=0x1bc):
//   pcVar3         = &WeatherSlot0ActiveFlag + iVar14*0x1bc  — slot base
//   pcVar3[0]      — active flag (char)
//   iVar2          = iVar14 * 0x1bc                — byte offset
//   (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] — type (short, stride=0xde*2=0x1bc/1)
//   WSLOT_DW(0x004) — mode flag (char)
//   (&WeatherSlot0AngSpeed)[iVar14*(OBJECT_STRIDE / 4)] — angular speed (float)
//   (&WeatherSlot0PosX)[iVar14*(OBJECT_STRIDE / 4)] — pos_x (float)
//   (&WeatherSlot0PosY)[iVar14*(OBJECT_STRIDE / 4)] — pos_y (float)
//   (&WeatherSlot0PosZ)[iVar14*(OBJECT_STRIDE / 4)] — pos_z / altitude (float)
//   (&WeatherSlot0VelX)[iVar14*(OBJECT_STRIDE / 4)] — vel_x (float)
//   (&WeatherSlot0VelY)[iVar14*(OBJECT_STRIDE / 4)] — vel_y (float)
//   (&WeatherSlot0Phase)[iVar14*(OBJECT_STRIDE / 4)] — phase angle (float)
//   (&WeatherSlot0ColorR)[iVar14*(OBJECT_STRIDE / 4)] — color R or -1
//   (&WeatherSlot0ColorB)[iVar14*(OBJECT_STRIDE / 4)] — color G or -1
//   (&WeatherSlot0RotAngle)[iVar14*(OBJECT_STRIDE / 4)] — rotation angle (float)
//   (&WeatherSlot0DirVec+iVar2)        — direction vector (float[3])
//   (&WeatherHeadPosX+iVar2)        — cached head pos X (float)
//   (&WeatherHeadPosY+iVar2)        — cached head pos Y (float)
//   (&WeatherSlot0VelZ+iVar2)        — velocity Z (float)
//   (&WeatherSlot0Scale)[iVar14*(OBJECT_STRIDE / 4)] — scale (float)
//   (&WeatherSize+iVar2)        — size (float)
//   (&WeatherSlot0AltLimit+iVar2)        — altitude limit (float)
//   (&WeatherRenderFlag)[iVar14*(OBJECT_STRIDE / 4)] — render flag (char)
//   (&WeatherSlot0ColorRF+iVar2)        — color R (float)
//   (&WeatherSlot0ColorGF+iVar2)        — color G (float)
//   (&WeatherSlot0ColorBF+iVar2)        — color B (float)
//   (&WeatherSlot0State)[iVar14*1]    — state byte (cloud life phase 0..3)
//   (&WeatherSlot0BlendMode)[iVar14*1]    — blend mode
//   (&WeatherSlotByte106+iVar2)        — ???
//   (&WeatherSlot0RotF+iVar2)        — rotation float
//   (&WeatherSlotByte10C+iVar2)        — ???
//   (&WeatherSlotFlag161)[iVar2]       — something_byte
//   (&WeatherSlot0Alpha)[iVar14*(OBJECT_STRIDE / 4)] — alpha fade
//   (&RenderPoolA)[iVar14*(OBJECT_STRIDE / 4)] — alpha start
//
// Cloud life phases (state byte at bdb4):
//   0 = rising:   check distance, set altitude
//   1 = hovering: bob up/down via altitude offset
//   2 = descending: wait for player anim then transition
//   3 = falling away: reduce scale until gone
//
// Firefly/butterfly (0x10a) orbit logic:
//   pos_x += sin(phase) * scale
//   pos_y -= cos(phase) * scale
//   phase += cos(sin(...) * angle_increment)
//   Kill if > max_dist from player or random 1/5120 chance
//
// Globals:
//   WeatherSlot0ActiveFlag   — weather slot table base (40 slots × 0x1bc bytes)
//   ModelsBase   — particle model table base (stride 0xbc, indexed by type)
//   Hero   — local player entity ptr
//   WeatherModeState   — weather mode (0=sun/clear, 1=rain, 2=snow, 3=storm)
//   TerrainWallFlags   — per-tile terrain attribute array
//   World   — World
//   MAIN_HASH_CLASS   — HashTable context (anti-tamper)
//   MoveSceneFrame   — anti-tamper key
//   GridToWorldScale  — world grid scale
//   0.1f  — PI/180
//   0.01f  — grid unit
//   25.0f  — particle speed scale
//   0.2f  — frame delta time
//   AlphaFadeStep  — angle increment
//   WeatherCeiling  — altitude_max
//   AnimSpeedB  — scale step
//   0.005f  — scale decay
//   WeatherMinAltitude  — min altitude
//   WeatherMaxAltitude  — max altitude
//   WeatherNearDist  — near distance (sound trigger)
//   WeatherDespawnDist  — far distance (despawn)
//   WeatherOrbitFreq  — orbit angle step
//   Alpha   — commit particle / advance state
//   Particle_PathUpdate   — particle path update
//   SetAction   — set particle animation
//   BMD__PlayAnimation   — BMD_Animation tick
//   AddTerrainLight   — spawn ground ripple
//   Matrix_BuildFromEuler   — Vec3_Normalize or direction update
//   Vector_Rotate   — Vec3_Scale (multiply direction by speed)
//   OpenMonsterModel   — play thunder sound
//   Math_Fabs   — terrain height at angle

#include "stdafx.h"
#include "Game/RenderFxSnapshots.h"

// Campos float del slot (OBJECT de 444 bytes).  Las macros DAT_0839bXXX
// estan tipadas como unsigned int; WSF reinterpreta los bits.  Leerlos con
// (float)(&X)[i] convertia el entero y la posicion salia en ~1e9.
#define WSF(field, idx) (*(float *)&(&field)[idx])
// Decompilation artifacts: suppress truncation/conversion/uninitialized warnings
#pragma warning(disable: 4244 4305 4309 4700)


// Accesor de los campos DWORD del slot de clima: fija el paso en 0x1bc bytes
// y el ancho en 4, que es lo que el binario hace (`0x6f * 4 == 0x1bc`).
// No indexar con el tipo declarado del simbolo: `(&WeatherRenderFlag)[iVar2]`
// (unsigned int) multiplica el offset en bytes por 4 y escribe fuera del pool
// -- encima del pool de joints, que esta justo despues --, y
// `(&WeatherModeFlag)[iVar14*(OBJECT_STRIDE / 4)]` (char) deja el paso en 0x6f y pisa los slots
// vecinos (el campo +0x04 tambien es un DWORD: es un contador, `(int)... + 1`,
// `if (1 < (int)...)`).
#define WSLOT_DW(off) (*(unsigned int *)&g_WeatherSlotPool[EntityOffset(off) + iVar14 * OBJECT_STRIDE])
uint __cdecl Weather_Update(void)
{
    float  *pfVar1;
    int     iVar2;
    char   *pcVar3;
    char    cVar4;
    short   sVar5;
    float   fVar7;
    intptr_t    uVar8;
    void   *pvVar9;
    DWORD  *puVar10;
    int     iVar11, iVar12;
    uint    uVar13;
    int     iVar14;
    const unsigned char* puVar16;
    unsigned char *pbVar17, *pbVar18;
    char   *pcVar19;
    bool    bVar20, bVar21;
    float10 fVar22, fVar23;
    long long lVar24;
    float   fVar25;
    uint    uStack_48;
    DWORD *puStack_44;
    DWORD *local_40;
    int     iStack_3c;
    // Los doce fStack_* son slots CONSECUTIVOS del frame original
    // (ebp-0x38 .. ebp-0x0c, de 4 en 4) y el codigo los pasa por direccion como
    // vectores de 3 floats: `&fStack_38`, `&fStack_2c`, `&fStack_20`.
    // Declarados como escalares sueltos MSVC no garantiza que queden contiguos
    // ni en ese orden, asi que el callee leia los componentes 1 y 2 de otro lado.
    float   __fr[12] = { 0.0f };
    float  &fStack_38 = __fr[0],  &fStack_34 = __fr[1],  &fStack_30 = __fr[2];
    float  &fStack_2c = __fr[3],  &fStack_28 = __fr[4],  &fStack_24 = __fr[5];
    float  &fStack_20 = __fr[6],  &fStack_1c = __fr[7],  &fStack_18 = __fr[8];
    float  &fStack_14 = __fr[9],  &fStack_10 = __fr[10], &fStack_c  = __fr[11];

    // ── Anti-tamper: HashTable reference-count increment for MoveSceneFrame ─────
    local_40 = &MoveSceneFrame;
    uVar8 = (**(unsigned int (**)(void *))((char *)MAIN_HASH_CLASS + 3 * sizeof(void*)))(&MoveSceneFrame);
    uVar13 = 0;
    local_40 = (DWORD *)0x0;
    if (HashTableCapacity != 0) {
        do {
            bVar20 = false; iVar14 = 0; bVar21 = true;
            pbVar18 = (unsigned char *)(HashTableKeys + uVar8 * 4);
            iVar12 = 4; const unsigned char* ppuVar15 = reinterpret_cast<const unsigned char*>(&local_40); pbVar17 = pbVar18;
            do {
                if (iVar12 == 0) break;
                iVar12--; bVar20 = *ppuVar15 < *pbVar17;
                bVar21 = *ppuVar15 == *pbVar17;
                ++ppuVar15; pbVar17++;
            } while (bVar21);
            if (!bVar21) iVar14 = (1 - (uint)bVar20) - (uint)(bVar20 != 0);
            if (iVar14 == 0) goto LAB_00500f16;
            iVar12 = 4; bVar20 = false; iVar14 = 0; bVar21 = true;
            puVar16 = reinterpret_cast<const unsigned char*>(&uStack_48);
            do {
                if (iVar12 == 0) break;
                iVar12--; bVar20 = (unsigned char)*puVar16 < *pbVar18;
                bVar21 = (unsigned char)*puVar16 == *pbVar18;
                ++puVar16; pbVar18++;
            } while (bVar21);
            if (!bVar21) iVar14 = (1 - (uint)bVar20) - (uint)(bVar20 != 0);
            if (iVar14 == 0) {
                if (uVar8 == 0xffffffff) goto LAB_00500f16;
                uVar8 = HashTable_GetIndex(&MAIN_HASH_CLASS, &MoveSceneFrame);
                if (uVar8 != 0xffffffff) {
                    puVar10 = *(DWORD **)(HashTableValues + uVar8 * 4);
                    if (puVar10 != nullptr) {
                        cVar4 = *(char *)(puVar10 + 1);
                        *(unsigned char *)(puVar10 + 1) = cVar4 + 1U;
                        if ((unsigned char)(cVar4 + 1U) < 2)
                            Packet_DecryptDword(&MoveSceneFrame, puVar10);
                    }
                }
                goto LAB_00500f34;
            }
            uVar8 = (uVar8 + 1) % HashTableCapacity;
            uVar13++;
        } while (uVar13 < HashTableCapacity);
    }
    CErrorReport_Write(&ErrorReportCtx, Err_HashTableFullGetIndex);
LAB_00500f16:
    pvVar9 = AntiTamper_HashNode();
    *(static_cast<unsigned char*>(pvVar9) + 4) = 1;
    HashTable_Insert(&MAIN_HASH_CLASS, pvVar9, &MoveSceneFrame);
LAB_00500f34:
    // Anti-tamper: HashTable reference-count decrement
    fStack_38 = MoveSceneFrame;
    puStack_44 = &MoveSceneFrame;
    uVar8 = (**(unsigned int (**)(void *))((char *)MAIN_HASH_CLASS + 3 * sizeof(void*)))(&MoveSceneFrame);
    uVar13 = 0;
    puStack_44 = (DWORD *)0x0;
    if (HashTableCapacity != 0) {
        do {
            iVar12 = 4; bVar20 = false; iVar14 = 0; bVar21 = true;
            pbVar18 = (unsigned char *)(HashTableKeys + uVar8 * 4);
            const unsigned char* ppuVar15b = reinterpret_cast<const unsigned char*>(&puStack_44); pbVar17 = pbVar18;
            do {
                if (iVar12 == 0) break;
                iVar12--; bVar20 = *ppuVar15b < *pbVar17;
                bVar21 = *ppuVar15b == *pbVar17;
                ++ppuVar15b; pbVar17++;
            } while (bVar21);
            if (!bVar21) iVar14 = (1 - (uint)bVar20) - (uint)(bVar20 != 0);
            if (iVar14 == 0) goto LAB_00501064;
            iVar12 = 4; bVar20 = false; iVar14 = 0; bVar21 = true;
            puVar16 = reinterpret_cast<const unsigned char*>(&uStack_48);
            do {
                if (iVar12 == 0) break;
                iVar12--; bVar20 = (unsigned char)*puVar16 < *pbVar18;
                bVar21 = (unsigned char)*puVar16 == *pbVar18;
                ++puVar16; pbVar18++;
            } while (bVar21);
            if (!bVar21) iVar14 = (1 - (uint)bVar20) - (uint)(bVar20 != 0);
            if (iVar14 == 0) {
                if (uVar8 != 0xffffffff) {
                    uVar8 = HashTable_GetIndex(&MAIN_HASH_CLASS, &MoveSceneFrame);
                    if (uVar8 != 0xffffffff) {
                        puVar10 = *(DWORD **)(HashTableValues + uVar8 * 4);
                        if (puVar10 != nullptr) {
                            cVar4 = *(char *)(puVar10 + 1);
                            *(char *)(puVar10 + 1) = cVar4 - 1;
                            if ((char)(cVar4 - 1) == '\0')
                                Packet_EncryptDword(puVar10, &MoveSceneFrame);
                        }
                    }
                }
                goto LAB_00501064;
            }
            uVar8 = (uVar8 + 1) % HashTableCapacity;
            uVar13++;
        } while (uVar13 < HashTableCapacity);
    }
    CErrorReport_Write(&ErrorReportCtx, Err_HashTableFullGetIndex);
LAB_00501064:

    // ── Weather-mode: random rain effects (WeatherModeState != 0) ─────────────────
    const char* const rainHero = Hero;
    if (WeatherModeState != 0) {
        // Spawn occasional rain splash particle (1/40 chance)
        iVar12 = _rand();
        if (iVar12 % 0x28 == 0) {
            iVar12 = _rand();
            const int splashOffsetX = iVar12 % 600 - 200;
            fStack_20 = (float)splashOffsetX + *(float *)(Hero + 0x10);
            iVar12 = _rand();
            const int splashOffsetY = iVar12 % 400 + 200;
            fStack_1c = (float)splashOffsetY + *(float *)(Hero + 0x14);
            fStack_18 = *(float *)(Hero + 0x18) + WeatherCeiling;
            fStack_2c = 0.0f; fStack_28 = 0.0f; fStack_24 = 0.0f;
            fStack_38 = 1.0f; fStack_34 = 1.0f; fStack_30 = 1.0f;
            CreateEffect(0xbf, &fStack_20, &fStack_2c, &fStack_38,
                         (float *)0x3, (float *)0x0, (float *)0xffffffff, (float *)0x0, 0);
            PlayBuffer(0x2e, 0, 0);
        }
        // Ground ripple effect
        fStack_38 = -0.3f; fStack_34 = -0.3f; fStack_30 = -0.2f;
        AddTerrainLight(*(const float *)(rainHero + 0x10), *(const float *)(rainHero + 0x14), (float*)&fStack_38, 0x10, (float*)&PrimaryTerrainLightStorage[0]);
    }

    // ── World == 10: spawn distant lightning clouds ─────────────────
    if ((gMapManager.GetCurrentMap() == 10) && (_rand() % 10 == 0)) {
        iVar12 = _rand();
        const int cloudOffsetX = iVar12 % 5000 - 2500;
        fStack_38 = (float)cloudOffsetX + *(float *)(Hero + 0x10);
        iVar12 = _rand();
        const int cloudOffsetY = iVar12 % 5000 - 2500;
        fStack_34 = (float)cloudOffsetY + *(float *)(Hero + 0x14);
        fStack_30 = *(float *)(Hero + 0x18) - WeatherDist1000;
        fStack_20 = 1.0f; fStack_1c = 1.0f; fStack_18 = 1.0f;
        fStack_2c = 0.0f; fStack_28 = 0.0f; fStack_24 = 0.0f;
        CreateEffect(0x47e, &fStack_38, &fStack_2c, &fStack_20,
                     (float *)0x0, (float *)0x0, (float *)0xffffffff, (float *)0x0, 0);
    }

    // Get terrain tile at player position
    // IDA: Terrain_Load((std::int64_t)(Hero.x * 0.01), (std::int64_t)(Hero.y * 0.01)).
    uVar8  = Terrain_GetTileIndex((uint)(long long)(*(float *)(Hero + 0x10) * 0.01f),
                          (uint)(long long)(*(float *)(Hero + 0x14) * 0.01f));
    iVar14 = 0;
    iVar12 = gMapManager.GetCurrentMap();
    uStack_48 = uVar8;

    // ── Main weather slot loop (40 slots) ─────────────────────────────────────
    do {
        // Limit active slots by game state
        if (iVar12 != 7) {
            if (iVar12 == 10) {
                if (0xc < iVar14) return uVar8;
            } else if (4 < iVar14) return uVar8;
        }

        iVar2    = iVar14 * OBJECT_STRIDE;
        pcVar3   = &WeatherSlot0ActiveFlag + iVar2;

        if ((&WeatherSlot0ActiveFlag)[iVar14 * OBJECT_STRIDE] == '\0') {
            // ── Free slot: try to spawn new weather object ───────────────────
            if (WeatherModeState != 0) {
                // Thunder/storm mode: random thunder cloud (1/300 chance)
                iVar11 = _rand();
                uVar8  = iVar11 / 300;
                iVar12 = gMapManager.GetCurrentMap();
                if (iVar11 % 300 == 0) {
                    RenderFxSnapshots_New(pcVar3);
                    *pcVar3 = '\x01';
                    OpenMonsterModel(0x1f);   // play thunder sound
                    (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] = 0x12d;  // type = lightning
                    iVar12 = _rand();
                    (*(unsigned int*)&g_WeatherSlotPool[EntityOffset(0x168) + iVar14 * OBJECT_STRIDE]) = 0x3f800000;
                    (&WeatherSlot0Alpha)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3f800000;
                    (&WeatherSlot0Scale)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3f000000;
                    WSLOT_DW(0x0dc) = 1;
                    (&WeatherSlotFlag161)[iVar2] = 0;
                    WSLOT_DW(0x004) = 0;
                    (&WeatherSlot0ColorR)[iVar14 * (OBJECT_STRIDE / 4)] = 0xffffffff;
                    (&WeatherSlot0ColorB)[iVar14 * (OBJECT_STRIDE / 4)] = 0xffffffff;
                    WSF(WeatherSlot0AngSpeed, iVar14 * (OBJECT_STRIDE / 4)) = (float)((float)(iVar12 % 3 + 6) * 0.1f);
                    uVar8 = _rand(); uVar8 &= 0x8000007f;
                    if ((int)uVar8 < 0) uVar8 = (uVar8 - 1 | 0xffffff80) + 1;
                    (&WeatherSlot0ColorG)[iVar14 * (OBJECT_STRIDE / 4)] = uVar8 + 0x80;
                    iVar11 = _rand();
                    (&WeatherSlot0VelX)[iVar14 * (OBJECT_STRIDE / 4)] = 0;
                    iVar12 = WeatherModeState;
                    (&WeatherSlot0VelY)[iVar14 * (OBJECT_STRIDE / 4)] = 0;
                    (&WeatherSlot0Phase)[iVar14 * (OBJECT_STRIDE / 4)] = 0xc2b40000;
                    WSF(WeatherSlot0RotAngle, iVar14 * (OBJECT_STRIDE / 4)) = (float)((float)(iVar11 % 10) * 0.1f);
                    if (iVar12 == 3)
                        WSLOT_DW(0x004) = 1;
                    iVar12 = _rand();
                    WSF(WeatherSlot0PosX, iVar14 * (OBJECT_STRIDE / 4)) = (float)((float)(iVar12 % 600 + -100) + *(float *)(Hero + 0x10));
                    iVar11 = _rand();
                    uVar8 = (intptr_t)Hero;
                    iVar12 = gMapManager.GetCurrentMap();
                    WSF(WeatherSlot0PosY, iVar14 * (OBJECT_STRIDE / 4)) = (float)((float)(iVar11 % 400 + 200) + *(float *)(Hero + 0x14));
                    WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4)) = (float)(*(float *)(uVar8 + 0x18) + WeatherCeiling);
                }
                goto LAB_0050172b;
            }

            // Non-storm: spawn by game state.
            //
            // IDA (0x00500E80 L690-706) manda los TRES caminos al MISMO spawn (los tres
            // hacen `break` del while y caen en el `memset(v17, 0, 0x1BC)` + init del
            // slot):
            //     if ( !v15 || v15 == 1 || v15 == 3 || v15 == 4 || v15 == 10 ) break;
            //     if ( v15 == 7 ) { v24 = TerrainWall[v113];
            //                       if (!v24 || v24 == 2) break; }
            //     else if ( v15 >= 11 && v15 <= 16 ) break;
            //
            // Nota: el DLL de inyeccion NOPea justo estos dos tests en 0x00501292
            // ("Fix Atlans and Icarus Goldens Overflow"), o sea el binario SI los
            // usa -- lo que el DLL hace es desactivarlos.
            bool __spawn = (iVar12 == 0) || (iVar12 == 1) || (iVar12 == 3) ||
                           (iVar12 == 4) || (iVar12 == 10);
            if (!__spawn && iVar12 == 7) {
                const unsigned char __attr = (unsigned char)TerrainWallFlags[uStack_48];
                __spawn = (__attr == 0) || (__attr == 2);
            } else if (!__spawn && iVar12 >= 11 && iVar12 <= 16) {
                __spawn = true;
            }
            if (__spawn)
            {
                // Clear all 0x1bc bytes of slot, then initialize
                char *pcVar19b = pcVar3;
                for (int n = 0x6f; n != 0; n--) {
                    pcVar19b[0] = '\0'; pcVar19b[1] = '\0';
                    pcVar19b[2] = '\0'; pcVar19b[3] = '\0';
                    pcVar19b += 4;
                }
                RenderFxSnapshots_New(pcVar3);
                *pcVar3 = '\x01';
                (&WeatherSlot0Scale)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3f800000;   // scale = 1.0
                WSLOT_DW(0x0dc) = 1;
                (&WeatherSlot0ColorG)[iVar14 * (OBJECT_STRIDE / 4)] = 0;
                WSLOT_DW(0x004) = 0;
                *(unsigned int *)(&WeatherSlot0ColorRF + iVar2) = 0x3f000000;
                *(unsigned int *)(&WeatherSlot0ColorGF + iVar2) = 0x3f000000;
                *(unsigned int *)(&WeatherSlot0ColorBF + iVar2) = 0x3f000000;
                (*(unsigned int*)&g_WeatherSlotPool[EntityOffset(0x168) + iVar14 * OBJECT_STRIDE]) = 0;
                (&WeatherSlot0Alpha)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3f800000;
                *(unsigned int *)(&WeatherSlot0AltLimit + iVar2) = 0x41500000;
                (&WeatherSlot0State)[iVar2] = 0;
                (&WeatherSlot0BlendMode)[iVar2] = 0;

                // Tipo por mundo (IDA 0x500E80, switch (World) tras el memset).
                // Solo nieve, luciernagas y peces pasan por LABEL_62, que sube la
                // luz del cuerpo (+0xE8..F0) de 0.5 a 1.0; los pajaros de Lorencia
                // y los murcielagos quedan en 0.5.
                bool __light1 = false;
                if (iVar12 == 0) {
                    (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] = 0xae;   // cloud (connecting)
                } else if ((iVar12 == 1) || (iVar12 == 4)) {
                    (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] = 0xb0;   // rain (entering)
                } else if (iVar12 == 3) {
                    (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] = 0xaf;   // snow
                    (&WeatherSlot0Scale)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3e99999a;
                    WSLOT_DW(0x0dc) = 0;
                    __light1 = true;
                } else if (iVar12 == 10) {
                    if (iVar14 < 3) {
                        // Slots 0-2: los dragones que vuelan por debajo de Icarus.
                        // IDA 0x500E80 (case 10 del switch por mundo): el init completo
                        // y despues `return` -- el slot recien se tickea el frame que
                        // sigue.  El port solo ponia el tipo y seguia: con la posicion
                        // en (0,0,0) el chequeo de distancia (4000 para el 301) lo
                        // apagaba en el mismo frame y nunca se veia ninguno.
                        *pcVar3 = '\x01';
                        OpenMonsterModel(0x1f);
                        (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] = 0x12d;
                        const int __rScale = _rand();
                        (*(unsigned int*)&g_WeatherSlotPool[EntityOffset(0x168) + iVar14 * OBJECT_STRIDE]) = 0x3f800000;
                        (&WeatherSlot0Alpha)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3f800000;
                        WSF(WeatherSlot0AngSpeed, iVar14 * (OBJECT_STRIDE / 4)) = (float)(__rScale % 3 + 6) * 0.050000001f;
                        WSF(WeatherSlot0Scale, iVar14 * (OBJECT_STRIDE / 4)) = (float)(_rand() % 10 + 10) * 0.02f;
                        const int __rSpeed = _rand();
                        WSLOT_DW(0x0dc) = 1;
                        (&WeatherSlotFlag161)[iVar2] = 0;
                        WSLOT_DW(0x004) = 0;
                        (&WeatherSlot0BlendMode)[iVar2] = 7;
                        (&WeatherSlot0ColorR)[iVar14 * (OBJECT_STRIDE / 4)] = 0xffffffff;
                        (&WeatherSlot0ColorB)[iVar14 * (OBJECT_STRIDE / 4)] = 0xffffffff;
                        *(float *)(&WeatherSlot0AltLimit + iVar2) = (float)(__rSpeed % 10 + 10) * 0.050000001f;
                        SetAction(AsObject(pcVar3), 7);
                        (&WeatherSlot0VelX)[iVar14 * (OBJECT_STRIDE / 4)] = 0;
                        (&WeatherSlot0VelY)[iVar14 * (OBJECT_STRIDE / 4)] = 0;
                        WSF(WeatherSlot0Phase, iVar14 * (OBJECT_STRIDE / 4)) = (float)(_rand() % 360);
                        WSF(WeatherSlot0PosX, iVar14 * (OBJECT_STRIDE / 4)) =
                            (float)(_rand() % 4000 - 2000) + *(float *)(Hero + 0x10);
                        WSF(WeatherSlot0PosY, iVar14 * (OBJECT_STRIDE / 4)) =
                            (float)(_rand() % 4000 - 2000) + *(float *)(Hero + 0x14);
                        WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4)) =
                            *(float *)(Hero + 0x18) - 600.0f;
                        return uVar8;
                    }
                    (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] = 0x10a;  // firefly
                    (&WeatherSlot0Scale)[iVar14 * (OBJECT_STRIDE / 4)] = 0x400ccccd;
                    WSLOT_DW(0x0dc) = 0;
                    (&WeatherSlot0ColorG)[iVar14 * (OBJECT_STRIDE / 4)] = 0x2580;
                    (&WeatherSlot0State)[iVar2] = 2;
                    __light1 = true;
                } else if (iVar12 != 7) {
                    // Mundos 11-16 (Blood Castle): tipo 184.
                    (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] = 0xb8;
                } else if (*(float *)(Hero + 0x14) * 0.01f < 128.0f) {
                    // Atlans, mitad sur: peces 182/183.
                    const short __fish = (short)(_rand() % 2);
                    *(unsigned int *)(&WeatherSlot0AltLimit + iVar2) = 0x41700000;   // 15.0
                    (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] = __fish + 0xb6;
                    (&WeatherSlot0Scale)[iVar14 * (OBJECT_STRIDE / 4)] = (_rand() % 100 < 90) ? 0x3e99999a   // 0.3
                                                                           : 0x3e800000;  // 0.25
                    WSLOT_DW(0x0dc) = 0;
                    __light1 = true;
                } else {
                    // Atlans, mitad norte: IDA apaga el slot y sale de la funcion.
                    *pcVar3 = '\0';
                    return 0;
                }

                if (__light1) {
                    *(unsigned int *)(&WeatherSlot0ColorRF + iVar2) = 0x3f800000;
                    *(unsigned int *)(&WeatherSlot0ColorGF + iVar2) = 0x3f800000;
                    *(unsigned int *)(&WeatherSlot0ColorBF + iVar2) = 0x3f800000;
                }

                (&WeatherSlotFlag161)[iVar2] = 1;
                (&WeatherSlot0AngSpeed)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3f4ccccd;
                *(unsigned int *)(&WeatherSize + iVar2) = 0x41200000;
                (&WeatherSlot0ColorR)[iVar14 * (OBJECT_STRIDE / 4)] = 0xffffffff;
                (&WeatherSlot0ColorB)[iVar14 * (OBJECT_STRIDE / 4)] = 0xffffffff;
                iVar12 = _rand();
                pfVar1 = (float *)(&WeatherSlot0PosX + iVar14 * (OBJECT_STRIDE / 4));
                WSF(WeatherSlot0RotAngle, iVar14 * (OBJECT_STRIDE / 4)) = (float)((float)(iVar12 % 0x13a) * 0.01f);
                uVar8 = _rand(); uVar8 &= 0x800003ff;
                if ((int)uVar8 < 0) uVar8 = (uVar8 - 1 | 0xfffffc00) + 1;
                *pfVar1 = (float)(int)(uVar8 - 0x200) + *(float *)(Hero + 0x10);
                uVar13 = _rand(); uVar8 = (intptr_t)Hero;
                uVar13 &= 0x800003ff;
                if ((int)uVar13 < 0) uVar13 = (uVar13 - 1 | 0xfffffc00) + 1;
                sVar5 = (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)];
                WSF(WeatherSlot0PosY, iVar14 * (OBJECT_STRIDE / 4)) = (float)((float)(int)(uVar13 - 0x200) + *(float *)(uVar8 + 0x14));
                (&WeatherSlot0PosZ)[iVar14 * (OBJECT_STRIDE / 4)] = *(unsigned int *)(uVar8 + 0x18);
                iVar12 = gMapManager.GetCurrentMap();

                // Firefly: init joint chain
                if (sVar5 == 0x10a) {
                    (&WeatherSlot0PosZ)[iVar14 * (OBJECT_STRIDE / 4)] = *(unsigned int *)(uVar8 + 0x18);
                    if ((iVar12 < 0xb) || (0x10 < iVar12)) {
                        uVar8 = (uint)(uintptr_t)Joint_Create(0x10a, pfVar1, pfVar1,
                                             (float *)(&WeatherSlot0VelX + iVar14 * (OBJECT_STRIDE / 4)), 1,
                                             (intptr_t)pcVar3, 25.0f, -1, 0);
                    } else {
                        uVar8 = (uint)(uintptr_t)Joint_Create(0x10a, pfVar1, pfVar1,
                                             (float *)(&WeatherSlot0VelX + iVar14 * (OBJECT_STRIDE / 4)), 3,
                                             (intptr_t)pcVar3, 25.0f, -1, 0);
                    }
                } else {
                    fVar22 = RequestTerrainHeight(WSF(WeatherSlot0PosX, iVar14*(OBJECT_STRIDE / 4)), WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)));
                    WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4)) = (float)(_rand() % 200 + 0x96) + (float)fVar22;   // IDA: rand() % 200 + 150
                }

                // Initialize velocity
                (&WeatherSlot0VelX)[iVar14 * (OBJECT_STRIDE / 4)] = 0;
                (&WeatherSlot0VelY)[iVar14 * (OBJECT_STRIDE / 4)] = 0;
                (&WeatherSlot0Phase)[iVar14 * (OBJECT_STRIDE / 4)] = 0;
                if (sVar5 == 0x10a) {
                    iVar12 = _rand();
                    WSF(WeatherSlot0Phase, iVar14 * (OBJECT_STRIDE / 4)) = (float)((float)(iVar12 % 0x168));
                }
            }
        }

LAB_0050172b:
        if (*pcVar3 == '\0') goto LAB_00501734;

LAB_00501734:
        if (*pcVar3 != '\0') {
            sVar5  = (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)];
            fVar25 = 1.0f;
            pvVar9 = (void *)(ModelsBase + sVar5 * BMD_STRIDE);
            if (sVar5 == 0x12d) fVar25 = 0.5f;

            if ((WeatherModeState == 0) || (sVar5 != 0x12d)) {
                pfVar1 = (float *)(&WeatherSlot0PosX + iVar14 * (OBJECT_STRIDE / 4));
                *(static_cast<unsigned char*>(pvVar9) + 0xa0 + BMD_FIELD_SHIFT) = (&WeatherSlot0BlendMode)[iVar2];
                BMD__PlayAnimation(pvVar9,
                             (float *)(&WeatherSlot0RotF + iVar2),
                             (float *)(&WeatherSlotByte10C + iVar2),
                             &WeatherSlotByte106 + iVar2, fVar25);
                uVar8 = (intptr_t)Hero;
                sVar5 = (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)];

                // ── Update by type ────────────────────────────────────────────
                if ((sVar5 == 0xae) || (sVar5 == 0xb8)) {
                    // Cloud life-cycle: rising(0) → hovering(1) → descending(2) → fading(3)
                    if ((&WeatherSlot0State)[iVar2] == '\0') {
                        // Phase 0: move upward, check distance
                        lVar24 = (long long)WorldTime;   // IDA: (std::int64_t)WorldTime
                        uVar8  = (uint)lVar24 & 0x80001fff;
                        if ((int)uVar8 < 0) uVar8 = (uVar8 - 1 | 0xffffe000) + 1;
                        if (((int)uVar8 < 0x800) &&
                            (fVar25 = *pfVar1 - *(float *)(Hero + 0x10),
                             fVar7  = WSF(WeatherSlot0PosY, iVar14 * (OBJECT_STRIDE / 4)) - *(float *)(Hero + 0x14),
                             fVar25 = SQRT(fVar25*fVar25 + fVar7*fVar7),
                             WeatherMinAltitude <= fVar25) && (fVar25 <= WeatherMaxAltitude)) {
                            (&WeatherSlot0State)[iVar2] = 1;
                        }
                        (&WeatherSlot0Scale)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3f800000;
                        uVar8 = _rand(); uVar8 &= 0x8000000f;
                        if ((int)uVar8 < 0) uVar8 = (uVar8 - 1 | 0xfffffff0) + 1;
                        fVar25 = (float)(int)(uVar8 - 8) + WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4));
                        (&WeatherSlot0PosZ)[iVar14 * (OBJECT_STRIDE / 4)] = (unsigned int)(*(int *)&fVar25);
                        if (WeatherMinAltitude <= fVar25) {
                            if (WeatherNearDist < fVar25)
                                *(unsigned int *)(&WeatherSlot0VelZ + iVar2) = 0xc1200000;
                        } else {
                            *(unsigned int *)(&WeatherSlot0VelZ + iVar2) = 0x41200000;
                        }
                    }
                    if ((&WeatherSlot0State)[iVar2] == '\x01') {
                        // Phase 1: el pajaro baja (velocidad Z -20).
                        //
                        // DESVIACION (fix del DLL, Patchs.cpp FixPigeons, hook en
                        // 0x00501C26): en el 0.97k, al tocar el suelo pasaba al
                        // estado 3 (volver a volar), o sea nunca se posaba -- y el
                        // estado 2 (en el suelo, se vuela cuando el heroe camina)
                        // quedaba inalcanzable.  El DLL lo deja en el suelo:
                        //   Z = altura del terreno, estado 2 (BOID_GROUND),
                        //   velocidad (rand() % 4 + 6) * 0.05, accion 1.
                        // Original IDA 0x500E80:
                        //   if (RequestTerrainHeight(x, y) > z) { AI = 3;
                        //       Velocity = 1.1; Direction[2] = 20; CurrentAction = 0; }
                        *(unsigned int *)(&WeatherSlot0VelZ + iVar2) = 0xc1a00000;
                        fVar22 = RequestTerrainHeight(WSF(WeatherSlot0PosX, iVar14*(OBJECT_STRIDE / 4)), WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)));
                        if ((float10)WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4)) < fVar22) {
                            WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4)) = (float)fVar22;
                            (&WeatherSlot0State)[iVar2] = 2;
                            WSF(WeatherSlot0Scale, iVar14 * (OBJECT_STRIDE / 4)) = (float)(_rand() % 4 + 6) * 0.05f;
                            (&WeatherSlot0BlendMode)[iVar2] = 1;
                        }
                    }
                    if ((&WeatherSlot0State)[iVar2] == '\x02') {
                        // Phase 2: descend when player animation is idle
                        if (*(unsigned char *)&((OBJECT*)(uintptr_t)(Hero))->CurrentAction < 0xd) {
                            uVar8 = _rand(); uVar8 &= 0x800000ff;
                            bVar20 = (uVar8 == 0);
                            if ((int)uVar8 < 0) bVar20 = ((uVar8-1|0xffffff00)==0xffffffff);
                            if (!bVar20) goto LAB_00501cb5;
                        }
                        (&WeatherSlot0State)[iVar2] = 3;
                        (&WeatherSlot0Scale)[iVar14 * (OBJECT_STRIDE / 4)] = 0x3f8ccccd;
                        *(unsigned int *)(&WeatherSlot0VelZ + iVar2) = 0x41a00000;
                        (&WeatherSlot0BlendMode)[iVar2] = 0;
                    }
LAB_00501cb5:
                    if ((&WeatherSlot0State)[iVar2] == '\x03') {
                        // Phase 3: fade out / scale down
                        uVar8 = _rand(); uVar8 &= 0x8000000f;
                        if ((int)uVar8 < 0) uVar8 = (uVar8 - 1 | 0xfffffff0) + 1;
                        WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4)) = (float)
                            ((float)(int)(uVar8-8) + WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)));
                        fVar25 = WSF(WeatherSlot0Scale, iVar14 * (OBJECT_STRIDE / 4)) - 0.005f;
                        (&WeatherSlot0Scale)[iVar14 * (OBJECT_STRIDE / 4)] = (unsigned int)(*(int *)&fVar25);
                        if (fVar25 <= 1.0f)
                            (&WeatherSlot0State)[iVar2] = 0;
                    }
                } else if (sVar5 == 0xb0) {
                    // Rain: follow terrain height via sine
                    fVar22 = RequestTerrainHeight(WSF(WeatherSlot0PosX, iVar14*(OBJECT_STRIDE / 4)), WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)));
                    WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4)) = (float)fVar22;
                    fVar22 = (float10)fsin((float10)WSF(WeatherSlot0RotAngle, iVar14*(OBJECT_STRIDE / 4)));
                    fVar22 = Math_Fabs((float)fVar22);
                    WSF(WeatherSlot0PosZ, iVar14 * (OBJECT_STRIDE / 4)) = (float)
                        ((float10)WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)) - fVar22*(float10)150.0f
                         + (float10)350.0f);
                    WSF(WeatherSlot0RotAngle, iVar14 * (OBJECT_STRIDE / 4)) = (float)
                        (WSF(WeatherSlot0RotAngle, iVar14*(OBJECT_STRIDE / 4)) + 0.2f);
                } else if (sVar5 == 0xaf) {
                    // Snow: random drift
                    uVar8 = _rand(); uVar8 &= 0x8000001f;
                    bVar20 = (uVar8 == 0);
                    if ((int)uVar8 < 0) bVar20 = ((uVar8-1|0xffffffe0)==0xffffffff);
                    if (bVar20) {
                        iVar12 = _rand();
                        WSF(WeatherSlot0Phase, iVar14*(OBJECT_STRIDE / 4)) = (float)((float)(iVar12 % 0x168));
                        iVar12 = _rand();
                        *(float *)(&WeatherSlot0VelZ + iVar2) = (float)(iVar12 % 0xf - 7);
                    }
                    iVar12 = _rand();
                    *(float *)(&WeatherSlot0VelZ + iVar2) =
                        (float)(iVar12 % 0xf - 7) * 0.2f + *(float *)(&WeatherSlot0VelZ + iVar2);
                    fVar22 = RequestTerrainHeight(WSF(WeatherSlot0PosX, iVar14*(OBJECT_STRIDE / 4)), WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)));
                    if ((float10)WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)) < fVar22 + (float10)50.0f) {
                        *(float *)(&WeatherSlot0VelZ + iVar2) =
                            *(float *)(&WeatherSlot0VelZ+iVar2) * 0.8f + 1.0f;
                    }
                    if (fVar22 + (float10)WeatherCeiling < (float10)WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4))) {
                        *(float *)(&WeatherSlot0VelZ + iVar2) =
                            *(float *)(&WeatherSlot0VelZ+iVar2) * 0.8f - 1.0f;
                    }
                    iVar12 = _rand();
                    WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)) = (float)
                        ((float)(iVar12%0xf - 7) * 0.3f + WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)));
                } else if (sVar5 == 0x10a) {
                    // Firefly/butterfly: circular orbit
                    fVar22 = (float10)fsin((float10)WSF(WeatherSlot0Phase, iVar14*(OBJECT_STRIDE / 4)));
                    *pfVar1 = (float)(fVar22 * (float10)WSF(WeatherSlot0Scale, iVar14*(OBJECT_STRIDE / 4)) + (float10)*pfVar1);
                    fVar22 = (float10)fcos((float10)WSF(WeatherSlot0Phase, iVar14*(OBJECT_STRIDE / 4)));
                    WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)) = (float)
                        ((float10)WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)) - fVar22*(float10)WSF(WeatherSlot0Scale, iVar14*(OBJECT_STRIDE / 4)));
                    fVar22 = (float10)fsin((float10)(iStack_3c + iVar14*0x1429 + 0x4544) * (float10)WeatherOrbitFreq);
                    fVar23 = (float10)fcos((float10)(iStack_3c + iVar14*0xa139 + 0x870b) * (float10)WeatherOrbitFreq);
                    WSF(WeatherSlot0Phase, iVar14*(OBJECT_STRIDE / 4)) = (float)
                        (fVar23 * fVar22 * (float10)0.01f + (float10)WSF(WeatherSlot0Phase, iVar14*(OBJECT_STRIDE / 4)));
                    fVar25 = *pfVar1 - *(float *)(uVar8 + 0x10);
                    fVar7  = WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)) - *(float *)(uVar8 + 0x14);
                    if (WeatherDespawnDist <= SQRT(fVar25*fVar25 + fVar7*fVar7)) *pcVar3 = '\0';
                    if (_rand() % 0x1400 == 0) *pcVar3 = '\0';
                    if (((10 < gMapManager.GetCurrentMap()) && (gMapManager.GetCurrentMap() < 0x11)) &&
                        ((int)(&WeatherSlot0ColorG)[iVar14*(OBJECT_STRIDE / 4)] < 1)) *pcVar3 = '\0';
                }

                // ── Movement: apply direction vector ─────────────────────────
                if ((&WeatherSlot0State)[iVar2] != '\x02') {
                    if ((&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)] != 0xaf) {
                        // Pasar la dirección real de WeatherSlot0ActiveFlag, no el literal `0x839bcb0` del
                        // binario fuente (en este build el símbolo vive en otra dirección).
                        Particle_PathUpdate(AsObject(pcVar3), iVar14, &WeatherSlot0ActiveFlag, 0x28);
                    }
                    Matrix_BuildFromEuler((float *)(&WeatherSlot0VelX + iVar14*(OBJECT_STRIDE / 4)), (float *)(&WeatherSlot0DirVec + iVar2));

                    // Scale velocity by sub-state
                    if (gMapManager.GetCurrentMap() == 7) {
                        if (5.0f <= WSF(WeatherSlot0RotAngle, iVar14*(OBJECT_STRIDE / 4))) {
                            uVar8 = _rand(); uVar8 &= 0x8000001f;
                            if ((int)uVar8 < 0) uVar8 = (uVar8-1|0xffffffe0)+1;
                            fStack_30 = *(float *)(&WeatherSlot0VelZ + iVar2);
                            *(unsigned int *)(&WeatherSlot0AltLimit + iVar2) = 0x40a00000;
                            fStack_38 = (float)(int)(uVar8+0x20) * WSF(WeatherSlot0Scale, iVar14*(OBJECT_STRIDE / 4));
                        } else if (iVar14 < 0x23) {
                            uVar8 = _rand(); uVar8 &= 0x8000000f;
                            if ((int)uVar8 < 0) uVar8 = (uVar8-1|0xfffffff0)+1;
                            fStack_30 = *(float *)(&WeatherSlot0VelZ + iVar2);
                            *(unsigned int *)(&WeatherSlot0AltLimit + iVar2) = 0x41700000;
                            fStack_38 = (float)(int)(uVar8+8) * WSF(WeatherSlot0Scale, iVar14*(OBJECT_STRIDE / 4));
                        } else {
                            uVar8 = _rand(); uVar8 &= 0x8000000f;
                            if ((int)uVar8 < 0) uVar8 = (uVar8-1|0xfffffff0)+1;
                            fStack_30 = *(float *)(&WeatherSlot0VelZ + iVar2);
                            *(unsigned int *)(&WeatherSlot0AltLimit + iVar2) = 0x41700000;
                            fStack_38 = (float)(int)(uVar8+0x10) * WSF(WeatherSlot0Scale, iVar14*(OBJECT_STRIDE / 4));
                        }
                        fVar25 = WSF(WeatherSlot0RotAngle, iVar14*(OBJECT_STRIDE / 4)) + 0.1f;
                        (&WeatherSlot0RotAngle)[iVar14*(OBJECT_STRIDE / 4)] = (unsigned int)(*(int *)&fVar25);
                        if (10.0f <= fVar25)
                            (&WeatherSlot0RotAngle)[iVar14*(OBJECT_STRIDE / 4)] = 0;
                    } else {
                        fStack_38 = WSF(WeatherSlot0Scale, iVar14*(OBJECT_STRIDE / 4)) * 25.0f;
                        fStack_30 = *(float *)(&WeatherSlot0VelZ + iVar2);
                    }
                    fStack_34 = 0.0f;
                    Vector_Rotate(&fStack_38, (float *)(&WeatherSlot0DirVec + iVar2), &fStack_2c);
                    fVar7  = fStack_2c + *pfVar1;
                    sVar5  = (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)];
                    puStack_44 = (DWORD *)0x44bb8000;
                    *pfVar1 = fVar7;
                    fVar25 = fStack_28 + WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4));
                    (&WeatherSlot0PosY)[iVar14*(OBJECT_STRIDE / 4)] = (unsigned int)(*(int *)&fVar25);
                    uVar8 = (intptr_t)Hero;
                    WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)) = (float)(fStack_24 + WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)));
                    *(float *)(&WeatherHeadPosX + iVar2) = fStack_2c * 3.0f + fVar7;
                    *(float *)(&WeatherHeadPosY + iVar2) = fStack_28 * 3.0f + fVar25;
                    fVar7  = fVar7 - *(float *)(uVar8 + 0x10);
                    fVar25 = fVar25 - *(float *)(uVar8 + 0x14);
                    { float f_dist = SQRT(fVar7*fVar7 + fVar25*fVar25);
                      float f_maxdist;
                      if (sVar5 == 0x12d) {
                          DWORD dw_maxdist = 0x457a0000;
                          f_maxdist = *(float*)&dw_maxdist;  // float 1000.0f
                      } else {
                          DWORD dw_maxdist = 0x44bb8000;
                          f_maxdist = *(float*)&dw_maxdist;  // float 1500.0f
                          uVar8 = _rand(); uVar8 &= 0x800001ff;
                          bVar20 = (uVar8 == 0);
                          if ((int)uVar8 < 0) bVar20 = ((uVar8-1|0xfffffe00)==0xffffffff);
                          if (bVar20) *pcVar3 = '\0';
                      }
                      if (f_maxdist <= f_dist) *pcVar3 = '\0';
                    }
                }

                // Loop-kill conditions (state 7, terrain type = grass)
                if ((((int)(&WeatherSlot0ColorG)[iVar14*(OBJECT_STRIDE / 4)] < 1) && (gMapManager.GetCurrentMap() == 7)) &&
                    ((unsigned char)TerrainWallFlags[uStack_48] == 0x01)) {
                    fVar25 = WSF(WeatherSlot0Phase, iVar14*(OBJECT_STRIDE / 4)) + AngleDeg180;
                    bVar20 = (AngleDeg360 <= fVar25);
                    (&WeatherSlot0Phase)[iVar14*(OBJECT_STRIDE / 4)] = (unsigned int)(*(int *)&fVar25);
                    if (bVar20) WSF(WeatherSlot0Phase, iVar14*(OBJECT_STRIDE / 4)) = (float)(fVar25 - AngleDeg360);
                    (&WeatherSlot0ColorG)[iVar14*(OBJECT_STRIDE / 4)] = 10;
                    WSLOT_DW(0x004) = (int)WSLOT_DW(0x004) + 1;
                }
                if (1 < (int)WSLOT_DW(0x004)) *pcVar3 = '\0';
                (&WeatherSlot0ColorG)[iVar14*(OBJECT_STRIDE / 4)] = (int)(&WeatherSlot0ColorG)[iVar14*(OBJECT_STRIDE / 4)] - 1;

                // Proximity effects (cloud type: chance to play sound/effect)
                fVar25 = *pfVar1 - *(float *)(Hero + 0x10);
                fVar7  = WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)) - *(float *)(Hero + 0x14);
                if (SQRT(fVar25*fVar25 + fVar7*fVar7) < WeatherNearDist) {
                    sVar5 = (&WeatherSlot0Type)[iVar14 * (OBJECT_STRIDE / 2)];
                    pcVar19 = pcVar3;
                    if (sVar5 == 0xae) {
                        uVar8 = _rand(); uVar8 &= 0x800001ff;
                        bVar20 = (uVar8 == 0);
                        if ((int)uVar8 < 0) bVar20 = ((uVar8-1|0xfffffe00)==0xffffffff);
                        if (bVar20) PlayBuffer(0xc, AsObject(pcVar3), 0);
                        uVar8 = _rand(); uVar8 &= 0x800001ff;
                        bVar20 = (uVar8 == 0);
                        if ((int)uVar8 < 0) bVar20 = ((uVar8-1|0xfffffe00)==0xffffffff);
                        if (bVar20) PlayBuffer(0xd, AsObject(pcVar19), 0);
                    } else if (sVar5 == 0xb0) {
                        uVar8 = _rand(); uVar8 &= 0x800000ff;
                        bVar20 = (uVar8 == 0);
                        if ((int)uVar8 < 0) bVar20 = ((uVar8-1|0xffffff00)==0xffffffff);
                        if (bVar20) PlayBuffer(0xe, AsObject(pcVar19), 0);
                    } else if ((sVar5 == 0xb8) && ((unsigned char)TerrainWallFlags[uStack_48] == 0x01)) {
                        uVar8 = _rand(); uVar8 &= 0x8000007f;
                        bVar20 = (uVar8 == 0);
                        if ((int)uVar8 < 0) bVar20 = ((uVar8-1|0xffffff80)==0xffffffff);
                        if (bVar20) PlayBuffer(0x6d, AsObject(pcVar19), 0);
                    }
                }
            } else {
                // WeatherModeState != 0 && type == 0x12d: thunder cloud — full animation
                SetAction(AsObject(pcVar3), 7);
                *(static_cast<unsigned char*>(pvVar9) + 0xa0 + BMD_FIELD_SHIFT) = (&WeatherSlot0BlendMode)[iVar2];
                BMD__PlayAnimation(pvVar9,
                             (float *)(&WeatherSlot0RotF + iVar2),
                             (float *)(&WeatherSlotByte10C + iVar2),
                             &WeatherSlotByte106 + iVar2, fVar25);
                Matrix_BuildFromEuler((float *)(&WeatherSlot0VelX + iVar14*(OBJECT_STRIDE / 4)), (float *)(&WeatherSlot0DirVec + iVar2));
                fStack_20 = WSF(WeatherSlot0AngSpeed, iVar14*(OBJECT_STRIDE / 4)) * 40.0f;
                fStack_1c = 0.0f; fStack_18 = 0.0f;
                Vector_Rotate(&fStack_20, (float *)(&WeatherSlot0DirVec + iVar2), &fStack_14);
                WSF(WeatherSlot0PosX, iVar14*(OBJECT_STRIDE / 4)) = (float)(fStack_14 + WSF(WeatherSlot0PosX, iVar14*(OBJECT_STRIDE / 4)));
                WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)) = (float)(fStack_10 + WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)));
                WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)) = (float)(fStack_c  + WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)));
                fVar22 = RequestTerrainHeight(WSF(WeatherSlot0PosX, iVar14*(OBJECT_STRIDE / 4)), WSF(WeatherSlot0PosY, iVar14*(OBJECT_STRIDE / 4)));
                WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)) = (float)(fVar22 + (float10)WeatherCeiling);
                fVar22 = (float10)fsin((float10)WSF(WeatherSlot0RotAngle, iVar14*(OBJECT_STRIDE / 4)));
                fVar23 = Math_Fabs((float)fVar22);
                fVar22 = (float10)GridToWorldScale;
                iVar12 = (&WeatherSlot0ColorG)[iVar14*(OBJECT_STRIDE / 4)];
                (&WeatherSlot0ColorG)[iVar14*(OBJECT_STRIDE / 4)] = iVar12 - 1;
                WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)) = (float)
                    ((float10)WSF(WeatherSlot0PosZ, iVar14*(OBJECT_STRIDE / 4)) - fVar23*fVar22 + fVar22);
                WSF(WeatherSlot0RotAngle, iVar14*(OBJECT_STRIDE / 4)) = (float)
                    (WSF(WeatherSlot0AngSpeed, iVar14*(OBJECT_STRIDE / 4)) * AlphaFadeStep + WSF(WeatherSlot0RotAngle, iVar14*(OBJECT_STRIDE / 4)));
                if (iVar12 - 1 < 1) *pcVar3 = '\0';
                uVar8 = _rand(); uVar8 &= 0x8000007f;
                bVar20 = (uVar8 == 0);
                if ((int)uVar8 < 0) bVar20 = ((uVar8-1|0xffffff80)==0xffffffff);
                if (bVar20) PlayBuffer(0x126, 0, 0);
            }
        }

        Alpha(AsObject(pcVar3));
        uVar8 = 0;  // return value unused; Alpha returns void
        iVar12 = gMapManager.GetCurrentMap();

        iVar14++;
        if (0x27 < iVar14) return uVar8;
    } while (true);
}
