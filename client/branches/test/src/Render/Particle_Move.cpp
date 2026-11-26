// Particle_Move.cpp

#include "stdafx.h"
#include <cstdint>
#include "globals.h"
#include "functions.h"

// MoveParticles @ 0x00477090 (~1130 lines) — FULL IMPLEMENTATION
// Per-frame particle system update.
// 1. Random particle drift with clamping (not the camera's EarthQuake)
// 2. Per active particle: decrement lifetime, MovePosition,
//    switch on type for gravity/terrain snap/fade/scale/color
//
// Particle slot layout (stride 0x70, base SpritePoolBase):
//   +0x00 active  +0x04 type  +0x08 sub  +0x0C scale
//   +0x10 posX    +0x14 posY  +0x18 posZ
//   +0x1C dirX    +0x20 angle
//   +0x28 lightR  +0x2C lightG +0x30 lightB
//   +0x38 lifetime +0x3C entity_ptr +0x40 rotation +0x44 frame
//   +0x48 gravity  +0x4C velX +0x50 velY +0x54 velZ
//   +0x58 extra0   +0x5C extra1 +0x60 extra2
//   +0x64 targetX  +0x68 targetY +0x6C targetZ
//
// NOTE: Ghidra shows unaff_EBX/EBP/ESI/EDI phantom params in the AngleMatrix/VectorRotate
//       calls for type 0x4c6 — these are 63 phantom stack params (anti-tamper artifact).
//       The AngleMatrix+VectorRotate path for type 0x4c6 is implemented structurally but
//       the phantom register values cannot be reconstructed; it runs the math correctly
//       with the actual particle data.
//
// Bitmaps access: LegacyBitmapTable + type * 0x38 + 0x24 = height field per bitmap slot.
// Models access: ModelsBase + entity_type * 0xBC = BMD struct base.
// WorldTime = WorldTime (global animation tick).

// Helper macros for particle field access
// SpritePoolBase es un array `char[N]`: usarlo directo (decae a char*). Con
// `&SpritePoolBase` sería pointer-to-array (aritmética × sizeof[N]).
#define P_BASE      (SpritePoolBase)
#define P_ACTIVE(o) (*(char*)(P_BASE + (o)))
#define P_TYPE(o)   (*(int*)(P_BASE + (o) + 0x04))
#define P_SUB(o)    (*(int*)(P_BASE + (o) + 0x08))
#define P_SCALE(o)  (*(float*)(P_BASE + (o) + 0x0C))
#define P_POSX(o)   (*(float*)(P_BASE + (o) + 0x10))
#define P_POSY(o)   (*(float*)(P_BASE + (o) + 0x14))
#define P_POSZ(o)   (*(float*)(P_BASE + (o) + 0x18))
#define P_DIRX(o)   (*(float*)(P_BASE + (o) + 0x1C))
#define P_ANGLE(o)  (*(float*)(P_BASE + (o) + 0x20))
#define P_LR(o)     (*(float*)(P_BASE + (o) + 0x28))
#define P_LG(o)     (*(float*)(P_BASE + (o) + 0x2C))
#define P_LB(o)     (*(float*)(P_BASE + (o) + 0x30))
#define P_LIFE(o)   (*(int*)(P_BASE + (o) + 0x38))
#define P_ENT(o)    (g_ParticleOwner[(o) / 0x70])
#define P_ROT(o)    (*(float*)(P_BASE + (o) + 0x40))
#define P_FRAME(o)  (*(int*)(P_BASE + (o) + 0x44))
#define P_GRAV(o)   (*(float*)(P_BASE + (o) + 0x48))
#define P_VELX(o)   (*(float*)(P_BASE + (o) + 0x4C))
#define P_VELY(o)   (*(float*)(P_BASE + (o) + 0x50))
#define P_VELZ(o)   (*(float*)(P_BASE + (o) + 0x54))
#define P_EX0(o)    (*(float*)(P_BASE + (o) + 0x58))
#define P_EX1(o)    (*(float*)(P_BASE + (o) + 0x5C))
#define P_EX2(o)    (*(float*)(P_BASE + (o) + 0x60))
#define P_TGX(o)    (*(float*)(P_BASE + (o) + 0x64))
#define P_TGY(o)    (*(float*)(P_BASE + (o) + 0x68))
#define P_TGZ(o)    (*(float*)(P_BASE + (o) + 0x6C))
// Bitmaps height: g_BitmapsRaw + type * 0x38 + 0x24 = height field per bitmap slot
#define BMP_HEIGHT(t)  (*(float*)(g_BitmapsRaw + (t) * TEXTURE_STRIDE + 0x24))

void __stdcall MoveParticles(void)
{
    float fVar15, fVar19, fVar3;
    float10 fVar11, fVar12;
    int iVar4, iVar5, iVar6, iVar9;
    unsigned int uVar7;

    // ── 1. Particle drift: random perturbation + clamping ───────────────────
    // Velocity array: LegacyParticleDriftVelX (X), LegacyParticleDriftVelY (Y) — two floats
    float* shakeVel = g_ParticleDriftVelocity;
    for (int s = 0; s < 2; s++) {
        iVar4 = _rand();
        fVar19 = (float)(iVar4 % 0x7d1 - 1000) * ShakeRandomScale + shakeVel[s];
        fVar15 = fVar19;
        if (fVar19 < 0.6f && fVar19 >= ShakeBounce) {
            // nop — keep fVar15 = fVar19
        } else {
            fVar15 = 0.6f;
        }
        if (fVar19 < ShakeBounce) {
            fVar15 = ShakeBounce;
        }
        shakeVel[s] = fVar15;
    }

    // Accumulate shake velocity into position, clamp
    // LegacyParticleDriftPosX (X), LegacyParticleDriftPosY (Y) — two floats
    float* shakePos = g_ParticleDriftPosition;
    for (int s = 0; s < 2; s++) {
        fVar19 = shakeVel[s] + shakePos[s];
        shakePos[s] = fVar19;
        fVar15 = fVar19;
        if (fVar19 < ShakeClampMax && fVar19 >= ShakeClampMin) {
            // nop
        } else {
            fVar15 = ShakeClampMax;
        }
        if (fVar19 < ShakeClampMin) {
            fVar15 = ShakeClampMin;
        }
        shakePos[s] = fVar15;
    }

    // ── 2. Per-particle update loop ──────────────────────────────────────────
    for (iVar4 = 0; iVar4 < kParticlePoolSize; iVar4++) {
        iVar9 = iVar4 * 0x70;

        if (P_ACTIVE(iVar9) == '\0') continue;

        // Decrement lifetime
        iVar5 = P_LIFE(iVar9);
        P_LIFE(iVar9) = iVar5 - 1;
        if (iVar5 - 1 < 1) {
            P_ACTIVE(iVar9) = '\0';
        }

        // MovePosition(pos, dir, vel) — advances position along velocity rotated by direction
        float* pPos = &P_POSX(iVar9);
        float* pDir = &P_DIRX(iVar9);
        float* pVel = &P_VELX(iVar9);
        Vector_AddRotated(pPos, pDir, pVel);

        fVar15 = 1.0f;
        iVar5 = P_TYPE(iVar9);

        // ── Main type switch ─────────────────────────────────────────────────
        // The original binary has a complex nested switch. We replicate the
        // structure exactly, including fall-through and goto patterns.

        // IDA 0.98j MoveParticles (sub_484310): tipos 103/104, el polvo que
        // levanta el Tamachan al caminar (no existen en el 0.97k).
        //     if (life <= 0) flag = 0;
        //     Scale = SubType == 1 ? Scale + (+72) : Scale + 0.03;
        //     Light[0..2] -= 0.05;
        if (iVar5 == 103 || iVar5 == 104) {
            if (P_LIFE(iVar9) <= 0) P_ACTIVE(iVar9) = 0;
            if (P_SUB(iVar9) == 1) P_SCALE(iVar9) = P_GRAV(iVar9) + P_SCALE(iVar9);
            else                   P_SCALE(iVar9) = P_SCALE(iVar9) + 0.03f;
            P_LR(iVar9) -= 0.05f;
            P_LG(iVar9) -= 0.05f;
            P_LB(iVar9) -= 0.05f;
            continue;
        }

        if (iVar5 > 0x4c4) {
            // High type range: 0x4c5..0x599
            switch (iVar5) {

            case 0x4c5: {
                fVar15 = (float)P_LIFE(iVar9) * 0.03125f;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                P_SCALE(iVar9) += 0.08f;
                P_POSX(iVar9) += P_VELX(iVar9);
                P_POSY(iVar9) += P_VELY(iVar9);
                P_POSZ(iVar9) += P_VELZ(iVar9);
                P_VELX(iVar9) *= 0.9f;
                P_VELY(iVar9) *= 0.9f;
                P_VELZ(iVar9) *= 0.9f;
                fVar15 = RequestTerrainHeight(P_POSX(iVar9), P_POSY(iVar9));
                P_POSZ(iVar9) = BMP_HEIGHT(P_TYPE(iVar9)) * P_SCALE(iVar9) * HalfScale + fVar15;
                break;
            }

            case 0x4c6: {
                if (P_LIFE(iVar9) < 10) {
                    fVar15 = 1.0f - (float)(10 - P_LIFE(iVar9)) * 0.1f;
                }
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                // AngleMatrix + VectorRotate to transform local offset by entity bone
                // NOTE: Ghidra shows phantom register args (unaff_EBX/EBP/ESI/EDI).
                // We use the particle's stored direction as angles and target as local offset.
                {
                    float angles3[3], mat3x4[3][4];
                    // (audit #11) removed unused: float in1_3[3], in2_3[3][4], out3[3];
                    angles3[0] = P_DIRX(iVar9);
                    angles3[1] = 0.0f;
                    angles3[2] = 0.0f;
                    AngleMatrix(angles3, mat3x4);
                    // VectorRotate: transform stored offset by matrix
                    float localOff[3];
                    localOff[0] = P_TGX(iVar9);
                    localOff[1] = P_TGY(iVar9);
                    localOff[2] = P_TGZ(iVar9); // placeholder
                    float worldOff[3];
                    Vector_Rotate(localOff, (float*)mat3x4, worldOff);
                    // Add to entity position
                    intptr_t entPtr = P_ENT(iVar9);
                    P_POSX(iVar9) = worldOff[0] + *(float*)(entPtr + 0x10);
                    P_POSY(iVar9) = worldOff[1] + *(float*)(entPtr + 0x14);
                    P_POSZ(iVar9) = worldOff[2] + *(float*)(entPtr + 0x18);
                }
                P_ANGLE(iVar9) += 5.0f;
                P_SCALE(iVar9) -= 0.01f;
                break;
            }

            case 0x4ce: {
                fVar11 = (float10)fsin((float10)P_LIFE(iVar9) * (float10)ParticleWaveFreqC);
                P_SCALE(iVar9) = (float)fVar11;
                if (P_SUB(iVar9) == 1) {
                    P_SCALE(iVar9) = (float)(fVar11 * (float10)AspectRatioScale);
                    P_ROT(iVar9) -= 12.0f;
                }
                break;
            }

            case 0x4cf: {
                if (P_SUB(iVar9) == 99) {
                    intptr_t entPtr = P_ENT(iVar9);
                    P_POSX(iVar9) = *(float*)(entPtr + 0x10);
                    P_POSY(iVar9) = *(float*)(entPtr + 0x14);
                    P_POSZ(iVar9) = *(float*)(entPtr + 0x18) + 50.0f;
                    P_ROT(iVar9) -= 1.0f;
                } else {
                    fVar12 = (float10)fsin((float10)P_LIFE(iVar9) * (float10)ParticleWaveFreqB);
                    fVar11 = (float10)5.0f;
                    P_SCALE(iVar9) = (float)(fVar12 * fVar11);
                    if (P_SUB(iVar9) < 2) {
                        ItemDrop_SetupRenderRef(reinterpret_cast<BYTE*>(P_BASE + iVar9));
                    } else {
                        P_SCALE(iVar9) = (float)(fVar12 * fVar11 * (float10)AspectRatioScale);
                        P_ROT(iVar9) -= 12.0f;
                        ItemDrop_SetupRenderRef(reinterpret_cast<BYTE*>(P_BASE + iVar9));
                    }
                }
                break;
            }

            case 0x4d0: {
                fVar11 = (float10)fsin((float10)P_LIFE(iVar9) * (float10)ParticleWaveFreqC);
                P_SCALE(iVar9) = (float)(fVar11 * (float10)3.0f);
                ItemDrop_SetupRenderRef(reinterpret_cast<BYTE*>(P_BASE + iVar9));
                break;
            }

            case 0x4d2: {
                if (P_SUB(iVar9) == 0) {
                    fVar11 = (float10)fsin((float10)P_LIFE(iVar9) * (float10)ParticleWaveFreqC);
                    P_SCALE(iVar9) = (float)(fVar11 * (float10)3.0f + (float10)2.0f);
                }
                fVar15 = (float)P_LIFE(iVar9) * ParticleLightC;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                break;
            }

            case 0x4d9: {
                P_FRAME(iVar9) += 1;
                iVar5 = _rand();
                P_POSX(iVar9) += (float)(iVar5 % 0x14 - 10) * P_SCALE(iVar9) * LightningSpeed;
                iVar5 = _rand();
                P_POSY(iVar9) += (float)(iVar5 % 0x14 - 10) * P_SCALE(iVar9) * LightningSpeed;
                iVar5 = _rand();
                P_POSZ(iVar9) += (float)(iVar5 % 0x14 + 10) * P_SCALE(iVar9) * LightningSpeed;
                break;
            }

            case 0x4da:
            case 0x4db:
            case 0x4dc: {
                if (P_FRAME(iVar9) == 0) {
                    uVar7 = _rand();
                    uVar7 = uVar7 & 0x8000001f;
                    if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xffffffe0) + 1;
                    P_VELX(iVar9) += (float)(int)(uVar7 - 0x10) * 0.1f;

                    uVar7 = _rand();
                    uVar7 = uVar7 & 0x8000001f;
                    if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xffffffe0) + 1;
                    P_VELY(iVar9) += (float)(int)(uVar7 - 0x10) * 0.1f;

                    uVar7 = _rand();
                    uVar7 = uVar7 & 0x8000001f;
                    if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xffffffe0) + 1;
                    P_VELZ(iVar9) += (float)(int)(uVar7 - 0x10) * 0.1f;

                    P_POSX(iVar9) += P_VELX(iVar9);
                    P_POSY(iVar9) += P_VELY(iVar9);
                    P_POSZ(iVar9) += P_VELZ(iVar9);
                }
                fVar15 = RequestTerrainHeight(P_POSX(iVar9), P_POSY(iVar9));
                if (P_POSZ(iVar9) < fVar15) {
                    P_POSZ(iVar9) = fVar15;
                    P_FRAME(iVar9) = 1;
                    P_VELX(iVar9) = 0.0f;
                    P_VELY(iVar9) = 0.0f;
                    P_VELZ(iVar9) = 0.0f;
                }
                uVar7 = _rand();
                uVar7 = uVar7 & 0x8000000f;
                if ((int)uVar7 < 0) uVar7 = ((uVar7 - 1) | 0xfffffff0) + 1;
                P_ROT(iVar9) += (float)(int)uVar7;
                break;
            }

            case 0x4df: {
                P_VELZ(iVar9) -= HalfScale;
                break;
            }

            case 0x4e1: {
                iVar5 = P_SUB(iVar9);
                float lifeF = (float)P_LIFE(iVar9);
                if (iVar5 == 0 || iVar5 == 3) {
                    fVar11 = ((float10)(int)P_LIFE(iVar9) + (float10)P_VELX(iVar9)) * (float10)0.1f;
                    fVar12 = (float10)fsin((double)fVar11);
                    P_POSX(iVar9) = (float)(fVar12 * (float10)40.0f + (float10)P_TGX(iVar9));
                    fVar11 = (float10)fcos((double)fVar11);
                    P_POSY(iVar9) = (float)((float10)P_TGY(iVar9) - fVar11 * (float10)40.0f);
                    fVar15 = P_GRAV(iVar9);
                    P_POSZ(iVar9) += fVar15;
                    P_SCALE(iVar9) -= ZBobFreq;
                } else if (iVar5 == 2) {
                    P_POSX(iVar9) = P_TGX(iVar9);
                    P_POSY(iVar9) = P_TGY(iVar9);
                    float steps = (float)((0x3c - P_LIFE(iVar9)) / 10);
                    fVar15 = steps * P_GRAV(iVar9);
                    P_POSZ(iVar9) += fVar15;
                    P_SCALE(iVar9) -= ZBobFreq;
                }

                if (iVar5 == 4) {
                    fVar11 = ((float10)(int)lifeF + (float10)P_VELX(iVar9)) * (float10)0.1f;
                    fVar12 = (float10)fsin((double)fVar11);
                    P_POSX(iVar9) = (float)(fVar12 * (float10)40.0f + (float10)P_TGX(iVar9));
                    fVar11 = (float10)fcos((double)fVar11);
                    P_POSY(iVar9) = (float)((float10)P_TGY(iVar9) - fVar11 * (float10)40.0f);
                    P_POSZ(iVar9) += P_GRAV(iVar9);
                    P_SCALE(iVar9) -= 0.004f;
                    if ((int)lifeF < 0x1f) {
                        goto label_color_decay;
                    }
                } else if ((int)lifeF < 0x15) {
                    label_color_decay:
                    P_LR(iVar9) *= ParticleColorDecayA;
                    P_LG(iVar9) *= ParticleColorDecayA;
                    P_LB(iVar9) *= ParticleColorDecayA;
                }

                if (iVar5 == 0) {
                    unsigned char animState = *(unsigned char*)(P_ENT(iVar9) + EntityOffset(0x105));
                    if ((animState > 0xc && animState < 0x22) || (animState > 0x37 && animState < 0x3d)) {
                        P_SUB(iVar9) = 1;
                        if ((int)lifeF > 0x14) {
                            lifeF = 20.0f;
                        }
                        P_LIFE(iVar9) = (int)lifeF;
                        P_VELX(iVar9) = 0.0f;
                        P_VELY(iVar9) = 0.0f;
                        P_VELZ(iVar9) = 0.0f;
                        P_TGY(iVar9) = P_POSY(iVar9);
                        P_TGX(iVar9) = P_POSX(iVar9);
                        P_TGZ(iVar9) = P_POSZ(iVar9);
                    }
                }
                break;
            }

            case 0x4f4: {
                if (P_SUB(iVar9) == 6) {
                    P_POSX(iVar9) += P_VELX(iVar9);
                    P_POSY(iVar9) += P_VELY(iVar9);
                    P_POSZ(iVar9) += P_VELZ(iVar9);
                    P_VELX(iVar9) *= ParticleDampGeneral;
                    P_VELY(iVar9) *= ParticleDampGeneral;
                    P_VELZ(iVar9) += 0.6f;

                    uVar7 = _rand(); uVar7 &= 0x80000003;
                    if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
                    P_POSX(iVar9) += (float)(int)(uVar7 - 2);

                    uVar7 = _rand(); uVar7 &= 0x80000003;
                    if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
                    P_POSY(iVar9) += (float)(int)(uVar7 - 2);

                    uVar7 = _rand(); uVar7 &= 0x80000003;
                    if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
                    P_POSZ(iVar9) += (float)(int)(uVar7 - 2) * 0.8f;

                    P_SCALE(iVar9) += AlphaFadeStep;
                    fVar15 = (float)P_LIFE(iVar9) * EntitySinPeriod;
                    P_LR(iVar9) = fVar15;
                    P_LG(iVar9) = fVar15;
                    P_LB(iVar9) = fVar15;

                    uVar7 = _rand(); uVar7 &= 0x80000001;
                    if ((int)uVar7 < 0) uVar7 = ((uVar7 - 1) | 0xfffffffe) + 1;
                    P_ROT(iVar9) += (float)(int)uVar7 + 1.0f;
                } else {
                    // Non-sub6: check entity death flag
                    if (*(char*)(P_ENT(iVar9) + EntityOffset(0x160)) == '\x01') {
                        P_LIFE(iVar9) = 0x32;
                    }
                    if (P_LIFE(iVar9) < 1) {
                        *(int*)(P_ENT(iVar9) + 0x58) = 0;
                        P_ACTIVE(iVar9) = '\0';
                    }
                    fVar11 = (float10)fsin(((float10)WorldTime + (float10)P_GRAV(iVar9)) * (float10)ParticleWaveFreqE);
                    P_POSZ(iVar9) = (float)(fVar11 * (float10)20.0f + (float10)P_TGZ(iVar9));
                }
                // Sub-type rotation
                switch (P_SUB(iVar9)) {
                case 1:
                case 4:
                    P_ROT(iVar9) = (float)WorldTime * P_EX0(iVar9) * AnimSpeedB + P_TGY(iVar9);
                    break;
                case 2:
                case 5:
                    P_ROT(iVar9) = (float)WorldTime * P_EX0(iVar9) * AnimSpeedA + P_TGY(iVar9);
                    break;
                }
                break;
            }

            case 0x565: {
                P_POSX(iVar9) += P_VELX(iVar9);
                P_POSY(iVar9) += P_VELY(iVar9);
                P_POSZ(iVar9) += P_VELZ(iVar9);
                P_VELX(iVar9) *= ParticleDampGeneral;
                P_VELY(iVar9) *= ParticleDampGeneral;
                fVar15 = (float)P_LIFE(iVar9) * JointLerpStep;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                P_VELZ(iVar9) += 0.3f;
                if (P_SUB(iVar9) == 0) {
                    P_SCALE(iVar9) += ParticleScaleDeltaB;
                } else {
                    P_SCALE(iVar9) += ParticleScaleDeltaA;
                }
                break;
            }

            case 0x566: {
                P_POSX(iVar9) += P_VELX(iVar9);
                P_POSY(iVar9) += P_VELY(iVar9);
                P_POSZ(iVar9) += P_VELZ(iVar9);
                P_VELX(iVar9) *= ParticleDampGeneral;
                P_VELY(iVar9) *= ParticleDampGeneral;
                P_VELZ(iVar9) += 0.6f;

                uVar7 = _rand(); uVar7 &= 0x80000003;
                if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
                P_POSX(iVar9) += (float)(int)(uVar7 - 2);

                uVar7 = _rand(); uVar7 &= 0x80000003;
                if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
                P_POSY(iVar9) += (float)(int)(uVar7 - 2);

                uVar7 = _rand(); uVar7 &= 0x80000003;
                if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
                P_POSZ(iVar9) += (float)(int)(uVar7 - 2) * 0.8f;

                P_SCALE(iVar9) += AlphaFadeStep;
                fVar15 = (float)P_LIFE(iVar9) * 0.04f;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;

                uVar7 = _rand(); uVar7 &= 0x80000001;
                if ((int)uVar7 < 0) uVar7 = ((uVar7 - 1) | 0xfffffffe) + 1;
                P_ROT(iVar9) += (float)(int)uVar7 + 1.0f;
                break;
            }

            case 0x567: {
                fVar15 = P_GRAV(iVar9) + 0.004f;
                P_GRAV(iVar9) = fVar15;
                P_SCALE(iVar9) -= AnimSpeedB;
                fVar15 = fVar15 * 10.0f;
                P_FRAME(iVar9) = (0x17 - P_LIFE(iVar9)) / 6;
                P_POSZ(iVar9) += fVar15;
                break;
            }

            case 0x596: {
                P_FRAME(iVar9) = P_SUB(iVar9);
                break;
            }

            case 0x598: {
                float lifeF2 = (float)P_LIFE(iVar9);
                P_GRAV(iVar9) = (float)(5 - P_LIFE(iVar9)) * 0.222222224f;
                fVar15 = lifeF2 * SpriteSheetColStep;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                break;
            }

            case 0x599: {
                float grav = P_GRAV(iVar9);
                P_GRAV(iVar9) = grav - 0.2f;
                fVar12 = (float10)fsin((float10)P_LIFE(iVar9) * (float10)ParticleWaveFreqA);
                P_SCALE(iVar9) = (float)(fVar12 + (float10)(grav - 0.2f) + (float10)1.0f);
                fVar15 = (float)P_LIFE(iVar9) * CameraLerpSpeed;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                break;
            }

            default:
                break;
            } // end high-type switch
            continue;
        } // end if iVar5 > 0x4c4

        if (iVar5 == 0x4c4) {
            // ── Type 0x4c4: multi-sub particle (flame/fire) ──────────────────
            switch (P_SUB(iVar9)) {
            case 0: {
                fVar15 = (float)P_LIFE(iVar9) * ParticleLightB;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                goto label_4c4_0_4_shared;
            }
            case 1: {
                fVar15 = (float)P_LIFE(iVar9) * EntitySinPeriod;
                P_LR(iVar9) = fVar15 * HalfScale;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15 * 0.8f;
                P_VELX(iVar9) *= 0.4f;
                P_VELY(iVar9) *= 0.4f;
                P_VELZ(iVar9) *= 0.4f;
                P_SCALE(iVar9) += AlphaFadeStep;
                break;
            }
            case 2: {
                fVar15 = (float)P_LIFE(iVar9) * EntitySinPeriod;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                fVar15 = P_GRAV(iVar9) - 0.1f;
                P_GRAV(iVar9) = fVar15;
                P_POSX(iVar9) -= fVar15 * 0.2f;
                P_POSZ(iVar9) += fVar15;
                P_SCALE(iVar9) -= 0.01f;
                break;
            }
            case 3: {
                fVar15 = (float)P_LIFE(iVar9) * ParticleLightB;
                fVar3 = fVar15 * 0.8f;
                P_LR(iVar9) = fVar3;
                P_LG(iVar9) = fVar3;
                P_LB(iVar9) = fVar15;
                P_VELX(iVar9) *= 0.4f;
                P_VELY(iVar9) *= 0.4f;
                P_VELZ(iVar9) *= 0.4f;
                P_SCALE(iVar9) += 0.1f;
                break;
            }
            case 4: {
                fVar15 = (float)P_LIFE(iVar9) * ParticleLightB;
                P_LR(iVar9) = fVar15 * ParticleRedFactorC;
                P_LG(iVar9) = fVar15 * ParticleGreenFactorC;
                fVar15 = fVar15 * ParticleBlueFactorC;
                label_4c4_0_4_shared:
                P_LB(iVar9) = fVar15;
                fVar15 = P_GRAV(iVar9) + 0.2f;
                P_GRAV(iVar9) = fVar15;
                P_POSZ(iVar9) += fVar15;
                P_SCALE(iVar9) += AlphaFadeStep;
                break;
            }
            case 5: {
                fVar15 = (float)P_LIFE(iVar9) * JointLerpStep;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                P_GRAV(iVar9) -= 0.1f;
                uVar7 = _rand(); uVar7 &= 0x80000001;
                if ((int)uVar7 < 0) uVar7 = ((uVar7 - 1) | 0xfffffffe) + 1;
                P_POSX(iVar9) -= (float)(int)(uVar7 + 1) * 0.2f * P_GRAV(iVar9);
                P_POSZ(iVar9) += P_GRAV(iVar9);
                P_SCALE(iVar9) -= 0.01f;
                break;
            }
            case 6: {
                fVar11 = (float10)WorldTime;
                P_LIFE(iVar9) = 10;
                fVar11 = (float10)fsin((double)((fVar11 + (float10)P_GRAV(iVar9)) * (float10)ParticleWaveFreqE));
                P_POSZ(iVar9) = (float)(fVar11 * (float10)20.0f + (float10)P_ROT(iVar9));
                {
                    unsigned long long uv = (unsigned long long)(long long)(double)((float10)WorldTime * (float10)SinFreqWorldTime);
                    int modVal = (int)((long long)uv % 0x708);
                    P_LR(iVar9) = 0.36f;  // 0x3eb851ec
                    P_LG(iVar9) = 0.3f;   // 0x3e99999a
                    P_LB(iVar9) = 0.24f;  // 0x3e75c290
                    fVar11 = (float10)fsin((float10)modVal * (float10)ParticleWaveFreqD);
                    P_SCALE(iVar9) = (float)(fVar11 * (float10)HalfScale + (float10)1.8f);
                }
                break;
            }
            case 7: {
                P_SCALE(iVar9) += GemScaleStep;
                float lifeF3 = (float)P_LIFE(iVar9);
                fVar3 = P_GRAV(iVar9) + 1.0f;
                P_GRAV(iVar9) = fVar3;
                P_VELY(iVar9) -= 0.1f;
                P_POSZ(iVar9) -= fVar3;
                if (P_LIFE(iVar9) < 5) {
                    fVar15 = lifeF3 * ParticleLightB;
                    P_SCALE(iVar9) -= 0.1f;
                }
                P_LR(iVar9) = fVar15 * ParticleRedFactorB;
                P_LG(iVar9) = fVar15 * ParticleGreenFactorB;
                P_LB(iVar9) = fVar15 * ParticleBlueFactorB;
                break;
            }
            case 8: {
                fVar15 = P_GRAV(iVar9) + AnimSpeedB;
                float lifeF4 = (float)P_LIFE(iVar9);
                P_GRAV(iVar9) = fVar15;
                if (P_LIFE(iVar9) < 6) {
                    fVar15 = P_LR(iVar9) * HalfScale;
                    P_VELX(iVar9) = 0.0f;
                    P_VELY(iVar9) = 0.0f;
                    P_VELZ(iVar9) = 0.0f;
                    P_LR(iVar9) = fVar15;
                    P_LG(iVar9) *= HalfScale;
                    P_LB(iVar9) *= HalfScale;
                } else {
                    P_SCALE(iVar9) += fVar15;
                    P_POSZ(iVar9) += fVar15 * 20.0f;
                    fVar15 = lifeF4 * ParticleLightA;
                    P_LR(iVar9) = fVar15;
                    P_LG(iVar9) = fVar15;
                    P_LB(iVar9) = fVar15;
                    P_VELX(iVar9) *= ParticleDampA;
                    P_VELY(iVar9) *= ParticleDampA;
                    P_VELZ(iVar9) *= ParticleDampA;
                }
                break;
            }
            case 9: {
                P_LB(iVar9) = 0.0f;
                fVar15 = (float)P_LIFE(iVar9) * JointLerpStep;
                P_LR(iVar9) = fVar15 * ParticleRedFactorA;
                P_LG(iVar9) = fVar15 * ParticleGreenFactorA;
                fVar15 = P_GRAV(iVar9) + HalfScale;
                P_GRAV(iVar9) = fVar15;
                P_POSZ(iVar9) += fVar15;
                P_SCALE(iVar9) += 0.1f;
                break;
            }
            case 10: {
                fVar15 = (float)P_LIFE(iVar9) * 0.0625f;
                P_LR(iVar9) = fVar15;
                fVar15 = fVar15 * 0.1f;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                P_SCALE(iVar9) += AlphaFadeStep;
                break;
            }
            case 0xb: {
                fVar15 = (float)P_LIFE(iVar9) * EntitySinPeriod;
                P_LR(iVar9) = fVar15 * P_EX0(iVar9);
                P_LG(iVar9) = fVar15 * P_EX1(iVar9);
                P_LB(iVar9) = fVar15 * P_EX2(iVar9);
                P_VELX(iVar9) *= 0.4f;
                P_VELY(iVar9) *= 0.4f;
                P_VELZ(iVar9) *= 0.4f;
                P_SCALE(iVar9) += AlphaFadeStep;
                P_POSZ(iVar9) -= 1.0f;
                break;
            }
            } // end 0x4c4 sub switch
            continue;
        } // end iVar5 == 0x4c4

        // ── Low type range: 0x67..0x4c3 ──────────────────────────────────────
        if (iVar5 < 0x4a7) {
            if (iVar5 == 0x4a6) {
                // Type 0x4a6: terrain light particle
                uVar7 = _rand(); uVar7 &= 0x80000003;
                if ((int)uVar7 < 0) uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
                P_FRAME(iVar9) = (int)uVar7;
                float lightF = (float)P_LIFE(iVar9) * ParticleLightC;
                P_LR(iVar9) = lightF;
                P_LG(iVar9) = lightF;
                P_LB(iVar9) = lightF;
                // AddTerrainLight — first call
                {
                    float lt0[3];
                    lt0[0] = lightF;  // placeholder
                    lt0[1] = lightF;
                    lt0[2] = lightF;
                    AddTerrainLight(P_POSX(iVar9), P_POSY(iVar9), (float*)lt0, 6, (float*)&PrimaryTerrainLightStorage[0]);
                }
                // AddTerrainLight — second call with color modulation
                {
                    float lt1[3];
                    lt1[0] = lightF * 0.2f;
                    lt1[1] = lightF * 0.4f;
                    lt1[2] = lightF;
                    AddTerrainLight(P_POSX(iVar9), P_POSY(iVar9), (float*)lt1, 4, (float*)&PrimaryTerrainLightStorage[0]);
                }
                continue;
            }

            if (iVar5 > 0x497) {
                if (iVar5 == 0x498) {
                    // Type 0x498: multi-sub particle
                    switch (P_SUB(iVar9)) {
                    case 0:
                        fVar15 = P_SCALE(iVar9) - HalfScale;
                        P_SCALE(iVar9) = fVar15;
                        if (fVar15 < 0.2f) P_ACTIVE(iVar9) = '\0';
                        break;
                    case 1:
                        fVar15 = P_SCALE(iVar9) - 2.0f;
                        P_SCALE(iVar9) = fVar15;
                        if (fVar15 < 0.2f) P_ACTIVE(iVar9) = '\0';
                        break;
                    case 2:
                        if (P_LIFE(iVar9) < 1) P_ACTIVE(iVar9) = '\0';
                        if (P_LIFE(iVar9) > 5 &&
                            *reinterpret_cast<const char*>(P_ENT(iVar9)) == '\0') {
                            // El owner conserva su campo de 32 bits; no depende de la base del pool.
                            P_LIFE(iVar9) = 5;
                        }
                        {
                            float lt2 = (float)P_LIFE(iVar9) * ParticleLightC;
                            P_LR(iVar9) = lt2;
                            P_LG(iVar9) = lt2;
                            P_LB(iVar9) = lt2;
                        }
                        P_SCALE(iVar9) += AlphaFadeStep;
                        P_POSX(iVar9) = P_TGX(iVar9) + P_VELX(iVar9);
                        {
                            float newY = P_VELY(iVar9) + P_TGY(iVar9);
                            P_POSY(iVar9) = newY;
                            float newZ = P_VELZ(iVar9) + P_TGZ(iVar9);
                            P_TGY(iVar9) = newY;
                            P_POSZ(iVar9) = newZ;
                            P_TGX(iVar9) += P_VELX(iVar9);
                            P_TGZ(iVar9) = newZ;
                        }
                        break;
                    case 3:
                        P_SCALE(iVar9) *= 0.8f;
                        break;
                    }
                } else {
                    if (iVar5 == 0x499) goto label_4b5;
                    if (iVar5 == 0x49c) {
                        P_ROT(iVar9) += 20.0f;
                    }
                }
                continue;
            }

            if (iVar5 == 0x497) {
                // Type 0x497: bounce + glow decay
                fVar15 = (float)P_LIFE(iVar9) * 0.0625f;
                P_LR(iVar9) = fVar15;
                P_LG(iVar9) = fVar15;
                P_LB(iVar9) = fVar15;
                P_POSZ(iVar9) += P_GRAV(iVar9);
                P_GRAV(iVar9) -= 2.0f;
                fVar15 = RequestTerrainHeight(P_POSX(iVar9), P_POSY(iVar9));
                if (P_POSZ(iVar9) < fVar15) {
                    P_POSZ(iVar9) = fVar15;
                    fVar15 = P_GRAV(iVar9) * ShakeBounce;
                    P_LIFE(iVar9) -= 4;
                    P_GRAV(iVar9) = fVar15;
                }
                P_POSX(iVar9) += P_VELX(iVar9);
                P_POSY(iVar9) += P_VELY(iVar9);
                P_POSZ(iVar9) += P_VELZ(iVar9);
                continue;
            }

            // Types < 0x497
            if (iVar5 > 0x47f) {
                if (iVar5 == 0x490) {
                    fVar15 = P_GRAV(iVar9) + 0.01f;
                    P_GRAV(iVar9) = fVar15;
                    P_SCALE(iVar9) += fVar15;
                    fVar15 = (float)P_LIFE(iVar9) * JointLerpStep;
                    P_LR(iVar9) = fVar15;
                    P_LG(iVar9) = fVar15;
                    P_LB(iVar9) = fVar15;
                }
                continue;
            }

            if (iVar5 == 0x47f) {
                P_POSZ(iVar9) -= P_GRAV(iVar9);
                P_POSY(iVar9) -= 1.0f;
                P_GRAV(iVar9) += 1.0f;
                P_SCALE(iVar9) *= ParticleDampGeneral;
                continue;
            }

            if (iVar5 < 0x67) continue;

            if (iVar5 > 0x68) {
                if (iVar5 == 0x47e) {
                    // Type 0x47e: complex multi-sub with camera shake
                    iVar5 = P_SUB(iVar9);
                    if (iVar5 == 0) {
                        fVar19 = 0.5f;
                    } else if (iVar5 == 2) {
                        fVar19 = 4.0f;
                    } else if (iVar5 == 1 || iVar5 == 5) {
                        P_LR(iVar9) *= 0.9f;
                        P_LG(iVar9) *= 0.9f;
                        P_LB(iVar9) *= 0.9f;
                        P_SCALE(iVar9) *= ParticleDampGeneral;
                        continue;
                    } else if (iVar5 == 3) {
                        intptr_t entPtr = P_ENT(iVar9);
                        P_LR(iVar9) *= 0.9f;
                        P_LG(iVar9) *= 0.9f;
                        P_LB(iVar9) *= 0.9f;
                        P_SCALE(iVar9) *= ParticleDampD;
                        fVar15 = P_GRAV(iVar9) + 5.0f;
                        P_GRAV(iVar9) = fVar15;
                        P_POSX(iVar9) = *(float*)(entPtr + 0x10);
                        fVar3 = P_POSX(iVar9) + 2.0f;
                        P_POSY(iVar9) = *(float*)(entPtr + 0x14);
                        P_POSZ(iVar9) = *(float*)(entPtr + 0x18);
                        P_POSX(iVar9) = fVar3;
                        P_POSY(iVar9) -= 2.0f;
                        P_POSZ(iVar9) += fVar15;
                        continue;
                    } else if (iVar5 == 4) {
                        // Subtipo 4: la particula se RE-ANCLA cada tick a un hueso
                        // del dueño.  Es lo que hace que el aura de Swell Life
                        // (efecto 0x47e/1, que spawnea estas particulas) siga al
                        // personaje en vez de quedarse donde se casteo.
                        //
                        // IDA 0x477090:
                        //   TransformPosition(Models + 188*ownerType, boneMatrix,
                        //                     Position, (float*)v5 + 4, 0);
                        //   *v8             = *(float*)(v14+16) + *v8;
                        //   *((float*)v5+5) = *(float*)(v14+20) + ...;
                        //   *((float*)v5+6) = *(float*)(v14+24) + ...;
                        //
                        // El indice de hueso vive en +0x40 (P_ROT): lo escribe
                        // Particle_Spawn case 4 con su parametro Scale, que el
                        // caller toma de la tabla ParticleEffectIdTable {25,26,27,20,34,
                        // 35,36,0}.  El +0x0C del slot queda fijo en 2.0f, asi
                        // que leer el hueso de ahi daba siempre el mismo.
                        intptr_t entPtr = P_ENT(iVar9);
                        if (entPtr != 0) {
                            float localP[3] = { 0.0f, 0.0f, 0.0f };
                            void* mdl = (void*)((char*)ModelsBase + *(short*)(entPtr + 2) * BMD_STRIDE);
                            intptr_t boneTab = *(intptr_t*)&AsObject(entPtr)->BoneMatrices;
                            long long lBone = (long long)(double)P_ROT(iVar9); // __ftol analog
                            // Guard del port (IDA no lo tiene): sin tabla de huesos
                            // el deref cae fuera de todo lo mapeado, y un indice
                            // mayor que numBones (model+34) lee pasado el alloc.
                            int nBones = (boneTab != 0) ? (int)*(short*)((char*)mdl + 34) : 0;
                            if (boneTab != 0 && lBone >= 0 && (int)lBone < nBones) {
                                // BMD__TransformPosition(this, BoneMatrix, Pos, WorldPos, Translate)
                                BMD_TransformPosition(mdl,
                                    (float*)((intptr_t)lBone * 0x30 + boneTab),
                                    localP, &P_POSX(iVar9), 0);
                                P_POSX(iVar9) += *(float*)(entPtr + 0x10);
                                P_POSY(iVar9) += *(float*)(entPtr + 0x14);
                                P_POSZ(iVar9) += *(float*)(entPtr + 0x18);
                            }
                        }
                        iVar5 = _rand();
                        P_GRAV(iVar9) += (float)(iVar5 % 0x28 + 0x3c) * ParticleGravScale;
                        iVar5 = _rand();
                        P_SCALE(iVar9) -= (float)(iVar5 % 400 + 400) * ParticleScaleDecay;
                        P_POSZ(iVar9) += P_GRAV(iVar9);
                        P_LR(iVar9) *= ParticleColorDecayB;
                        P_LG(iVar9) *= ParticleColorDecayB;
                        P_LB(iVar9) *= ParticleColorDecayB;
                        continue;
                    } else {
                        continue;
                    }
                    // Shared path for sub 0 and 2: camera shake + drift
                    iVar5 = P_SUB(iVar9);
                    iVar6 = _rand();
                    fVar15 = (float)(_rand() % 0x7d1 - 1000) * ParticleDriftScale;
                    float fDrift = (float)(iVar6 % 0x7d1 - 1000) * ParticleDriftScale + LegacyParticleDriftPosY;
                    float fShakeZ = ParticleDriftPositionZ * fVar19;
                    P_POSX(iVar9) += (LegacyParticleDriftPosX + fVar15) * fVar19;
                    P_POSY(iVar9) += fDrift * fVar19;
                    P_POSZ(iVar9) += fVar19 * 5.0f + fShakeZ;
                    if (iVar5 == 0) {
                        fVar15 = P_SCALE(iVar9) - AlphaFadeStep;
                    } else if (iVar5 == 1) {
                        fVar15 = P_SCALE(iVar9) * ParticleDampC;
                    } else {
                        fVar15 = P_SCALE(iVar9) * ParticleDampGeneral;
                    }
                    P_SCALE(iVar9) = fVar15;
                    P_LR(iVar9) = fVar15;
                    P_LG(iVar9) = fVar15;
                    P_LB(iVar9) = fVar15;
                    if (iVar5 == 1) {
                        fVar15 = P_SCALE(iVar9) * 0.3f;
                        P_LR(iVar9) = fVar15;
                        P_LG(iVar9) = fVar15;
                        P_LB(iVar9) = fVar15;
                    }
                    if (iVar5 == 5) {
                        fVar15 = P_SCALE(iVar9) * 0.3f;
                        P_LR(iVar9) = fVar15;
                        P_LG(iVar9) = fVar15;
                        P_LB(iVar9) = fVar15;
                    }
                    if ((iVar5 == 0 && P_SCALE(iVar9) <= 0.1f) ||
                        (iVar5 == 2 && P_SCALE(iVar9) <= 0.3f)) {
                        P_LIFE(iVar9) = -1;
                        P_ACTIVE(iVar9) = '\0';
                    }
                    continue;
                }
                continue; // types between 0x68 and 0x47e not handled above
            }

            // Types 0x67..0x68: glow + fade
            if (P_LIFE(iVar9) < 1) P_ACTIVE(iVar9) = '\0';
            if (P_SUB(iVar9) == 1) {
                fVar15 = P_GRAV(iVar9) + P_SCALE(iVar9);
            } else {
                fVar15 = P_SCALE(iVar9) + GemScaleStep;
            }
            P_SCALE(iVar9) = fVar15;
            {
                float lt3[3] = { 0.05f, 0.05f, 0.05f };
                // fall through to color decay label
            }
            goto label_light_decay;
        } // end iVar5 < 0x4a7

        // ── Types 0x4a7..0x4c3 ───────────────────────────────────────────────
        switch (iVar5) {

        case 0x4a7: {
            // Terrain light + positioning
            unsigned long long uv = (unsigned long long)(long long)(double)((float10)WorldTime * (float10)SinFreqWorldTime);
            P_ROT(iVar9) = (float)(int)((long long)(uv) % 1000) * SinFreqWorldTime;
            float lightA = (float)P_LIFE(iVar9) * JointLerpStep;
            float ltA0 = lightA * HalfScale;
            float ltA1 = lightA;
            float ltA2 = lightA * 0.8f;
            {
                float ltBuf[3] = { ltA0, ltA1, ltA2 };
                AddTerrainLight(P_POSX(iVar9), P_POSY(iVar9), (float*)ltBuf, 3, (float*)&PrimaryTerrainLightStorage[0]);
            }
            if (P_SUB(iVar9) == 2) {
                intptr_t entPtr = P_ENT(iVar9);
                P_LR(iVar9) = ltA0;
                P_LG(iVar9) = ltA1;
                P_SCALE(iVar9) += 0.1f;
                P_LB(iVar9) = ltA2;
                P_POSX(iVar9) = *(float*)(entPtr + 0x10);
                P_POSY(iVar9) = *(float*)(entPtr + 0x14);
                P_POSZ(iVar9) = *(float*)(entPtr + 0x18) + 80.0f;
            }
            break;
        }

        case 0x4ab:
        case 0x4ad: {
            float grav2 = P_GRAV(iVar9) + 0.004f;
            P_GRAV(iVar9) = grav2;
            switch (P_SUB(iVar9)) {
            case 5:
            case 6:
                P_SCALE(iVar9) -= 0.04f;
                P_ROT(iVar9) += 5.0f;
                break;
            case 7:
                P_FRAME(iVar9) = (0xf - P_LIFE(iVar9)) / 6;
                // fall through
            case 0:
                P_SCALE(iVar9) -= 0.04f;
                break;
            case 8:
                P_SCALE(iVar9) *= ParticleDampGeneral;
                P_ROT(iVar9) += 5.0f;
                break;
            case 9: {
                intptr_t entPtr = P_ENT(iVar9);
                P_POSX(iVar9) = *(float*)(entPtr + 0x10) + P_TGX(iVar9);
                P_POSY(iVar9) = *(float*)(entPtr + 0x14) + P_TGY(iVar9);
                P_POSZ(iVar9) = *(float*)(entPtr + 0x18) + P_TGZ(iVar9);
                iVar5 = _rand();
                float rndG = (float)((iVar5 % 0x3c + 0x3c) / 100);
                fVar15 = P_GRAV(iVar9);
                P_GRAV(iVar9) = rndG + fVar15;
                P_SCALE(iVar9) -= (rndG + fVar15) * 0.0111111114f;
                break;
            }
            case 10:
                P_SCALE(iVar9) *= ParticleDampGeneral;
                break;
            default:
                P_SCALE(iVar9) = grav2 + P_SCALE(iVar9);
                P_VELX(iVar9) *= ParticleDampC;
                P_VELY(iVar9) *= ParticleDampC;
                P_VELZ(iVar9) *= ParticleDampC;
                break;
            }
            P_POSZ(iVar9) += P_GRAV(iVar9) * 10.0f;
            P_FRAME(iVar9) = (0x17 - P_LIFE(iVar9)) / 6;
            break;
        }

        case 0x4ac: {
            fVar15 = P_GRAV(iVar9) + AnimSpeedB;
            P_GRAV(iVar9) = fVar15;
            P_SCALE(iVar9) += fVar15;
            P_VELX(iVar9) *= ParticleDampB;
            P_VELY(iVar9) *= ParticleDampB;
            P_VELZ(iVar9) *= ParticleDampB;
            P_POSZ(iVar9) += fVar15 * 20.0f;
            fVar15 = (float)P_LIFE(iVar9) * 0.2f;
            P_LR(iVar9) = fVar15;
            P_LG(iVar9) = fVar15;
            P_LB(iVar9) = fVar15;
            break;
        }

        case 0x4b0: {
            if (P_LIFE(iVar9) < 1) P_ACTIVE(iVar9) = '\0';
            switch (P_SUB(iVar9)) {
            case 1:
                if (P_LIFE(iVar9) == 10) {
                    P_VELY(iVar9) += ParticleVelYAdd;
                    P_SCALE(iVar9) -= 0.15f;
                }
                fVar15 = (float)(_rand() % 0x168);
                break;
            case 2:
                fVar15 = (float)(_rand() % 0x168);
                break;
            case 3:
                P_ROT(iVar9) = (float)(_rand() % 0x168);
                continue;
            case 4: {
                intptr_t entPtr = P_ENT(iVar9);
                P_POSX(iVar9) += *(float*)(entPtr + 0x10) - P_TGX(iVar9);
                P_POSY(iVar9) += *(float*)(entPtr + 0x14) - P_TGY(iVar9);
                P_POSZ(iVar9) += *(float*)(entPtr + 0x18) - P_TGZ(iVar9) + P_GRAV(iVar9);
                fVar15 = P_GRAV(iVar9) + 0.1f;
                P_TGX(iVar9) = *(float*)(entPtr + 0x10);
                P_TGY(iVar9) = *(float*)(entPtr + 0x14);
                P_TGZ(iVar9) = *(float*)(entPtr + 0x18);
                P_GRAV(iVar9) = fVar15;
                fVar15 = (float)(_rand() % 0x168);
                break;
            }
            case 5:
                if (P_LIFE(iVar9) == 10) {
                    P_VELY(iVar9) += ParticleVelYAdd;
                    P_SCALE(iVar9) *= 0.8f;
                }
                fVar15 = (float)(_rand() % 0x168);
                break;
            default:
                continue;
            }
            P_ROT(iVar9) = fVar15;
            label_light_decay:
            P_LR(iVar9) -= AlphaFadeStep;
            P_LG(iVar9) -= AlphaFadeStep;
            P_LB(iVar9) -= AlphaFadeStep;
            break;
        }

        case 0x4b5:
        case 0x4b6:
        label_4b5: {
            fVar15 = P_VELX(iVar9) * ParticleDampGeneral;
            P_VELX(iVar9) = fVar15;
            float vy = P_VELY(iVar9) * ParticleDampGeneral;
            P_VELY(iVar9) = vy;
            float vz = P_VELZ(iVar9) * ParticleDampGeneral;
            P_VELZ(iVar9) = vz;
            P_FRAME(iVar9) = (0xc - P_LIFE(iVar9)) / 3;
            P_POSX(iVar9) += fVar15;
            P_POSY(iVar9) += vy;
            P_POSZ(iVar9) += vz;
            break;
        }

        case 0x4bf: {
            P_FRAME(iVar9) = (0x14 - P_LIFE(iVar9)) / 2;
            if (P_SUB(iVar9) != 1) {
                float ltBF = (float)P_LIFE(iVar9) * 0.05f;
                float lt4[3] = { ltBF * HalfScale, ltBF * 0.3f, ltBF * 0.1f };
                AddTerrainLight(P_POSX(iVar9), P_POSY(iVar9), (float*)lt4, 4, (float*)&PrimaryTerrainLightStorage[0]);
            }
            break;
        }

        case 0x4c0: {
            P_FRAME(iVar9) = (0xc - P_LIFE(iVar9)) / 3;
            break;
        }

        default:
            break;
        } // end types 0x4a7..0x4c3 switch

    } // end particle loop
}

// Undefine particle helper macros to avoid polluting other translation units
#undef P_BASE
#undef P_ACTIVE
#undef P_TYPE
#undef P_SUB
#undef P_SCALE
#undef P_POSX
#undef P_POSY
#undef P_POSZ
#undef P_DIRX
#undef P_ANGLE
#undef P_LR
#undef P_LG
#undef P_LB
#undef P_LIFE
#undef P_ENT
#undef P_ROT
#undef P_FRAME
#undef P_GRAV
#undef P_VELX
#undef P_VELY
#undef P_VELZ
#undef P_EX0
#undef P_EX1
#undef P_EX2
#undef P_TGX
#undef P_TGY
#undef P_TGZ
#undef BMP_HEIGHT
