// Terrain_Water.cpp
// IDA: Terrain_Water (0x004F95E0)
//
// World-tick / terrain water wave animation.
// Two responsibilities:
//   1) Copy per-tile lighting data from the "shadow" buffer (TerrainLightData) to
//      the live buffer (PrimaryTerrainLightStorage) for the current viewport rectangle.
//      Viewport: columns [FrustrumBoundMinX_1 .. FrustrumBoundMaxX_1+3],
//                rows    [FrustrumBoundMinY_1 .. FrustrumBoundMaxY_1+3]
//      Each tile entry is 12 bytes (3 × float), layout:
//        PrimaryTerrainLightStorage[tile*3+0] = TerrainLightData[tile*3+0]  (R or X)
//        PrimaryTerrainLightStorage[tile*3+1] = TerrainLightDataG[tile*3+0]  (G or Y)
//        PrimaryTerrainLightStorage[tile*3+2] = TerrainLightDataB[tile*3+0]  (B or Z)
//
//   2) Compute sine-wave height offsets into WaterWaveHeightTable (float table, row*256+col)
//      for each tile in the same viewport rectangle.
//      If WeatherModeState == 0: fast ocean waves (period 720000 sub-ticks, scale ZBobFreq)
//      If WeatherModeState != 0: slow terrain waves (period 36000, scale 0.01f)
//      Per-tile wave: sin(col * waveFreq + timePhase) * 10.0f
//      waveFreq = 5.0f (normal) or 50.0f (game state 8 = CharSelectInit)
//      col step = 1.0f
//
// Globals:
//   FrustrumBoundMinX_1/94  — viewport top-left (col, row)
//   FrustrumBoundMaxX_1/778 — viewport extents (+3 gives inclusive max)
//   WeatherModeState     — 0=ocean wave mode, else terrain mode
//   World     — World
//   WaterWaveHeightTable     — water height table [row][col] floats
//   PrimaryTerrainLightStorage     — live per-tile lighting buffer (3 floats/tile)
//   TerrainLightData/0c/10 — shadow/pre-computed lighting buffers

#include "stdafx.h"
#include <vector>
#include <cstring>

int __cdecl Terrain_Water(void)
{
    int   iVar4;
    float fVar1;
    int   iVar2, iVar3;
    uint  uVar5, uVar7, uVar8;
    float *pfVar6;
    float10 fVar11;
    float10 fVar12, fVar13;
    long long uVar14;

    // ── 1) Copy lighting data into live buffer for viewport rect ──────────────
    int rowMax = FrustrumBoundMaxY_1 + 3;
    uVar8 = FrustrumBoundMinY_1;  // viewport start row
    if ((int)FrustrumBoundMinY_1 <= rowMax) {
        int colMax = FrustrumBoundMaxX_1 + 3;
        uVar7 = FrustrumBoundMinY_1;
        do {
            if ((int)FrustrumBoundMinX_1 <= colMax) {
                uVar5 = FrustrumBoundMinX_1;
                do {
                    // tile index = col + row*256 (0x100)
                    iVar2 = (uVar5 & 0xff) + (uVar7 & 0xff) * 0x100;
                    iVar3 = iVar2 * 0xc;  // byte offset (3 floats × 4 bytes)
                    uVar5++;
                    // PrimaryTerrainLightStorage está declarado DWORD: castear a char* para que la suma sea
                    // aritmética de bytes (stride real 12 bytes = 3 floats; disasm @
                    // 0x004f9620-23 confirma byte-offset = iVar2*3*4).
                    // IDA copia el float (PrimaryTerrainLight[i][0]=BackTerrainLight[i][0]):
                    // bit-cast del source para preservarlo, igual que G/B.
                    *(unsigned int *)((char*)&PrimaryTerrainLightStorage + iVar3) = *(unsigned int*)&TerrainLightData[iVar2 * 3];
                    *(unsigned int *)((char*)&PrimaryTerrainLightG + iVar3) = (&TerrainLightDataG)[iVar2 * 3];
                    *(unsigned int *)((char*)&PrimaryTerrainLightB + iVar3) = (&TerrainLightDataB)[iVar2 * 3];
                    uVar8 = FrustrumBoundMinY_1;
                } while ((int)uVar5 <= colMax);
            }
            uVar7++;
        } while ((int)uVar7 <= rowMax);
    }

    // IDA: Terrain_Water (0x004F95E0): la fase usa WorldTime, no otra lectura del reloj.
    // DESVIACION: mantener 64 bits hasta el módulo evita truncar el uptime a 32 bits.
    if (WeatherModeState == 0) {
        // Ocean / water mode: long period (720000 units)
        uVar14 = (long long)WorldTime;
        iVar4  = (int)((long long)uVar14 / 720000);
        fVar11 = (float10)(int)((long long)uVar14 % 720000) * (float10)ZBobFreq;
    } else {
        // Terrain wave mode: short period (36000 units)
        uVar14 = (long long)WorldTime;
        iVar4  = (int)((long long)uVar14 / 36000);
        fVar11 = (float10)(int)((long long)uVar14 % 36000) * (float10)0.01f;
    }

    int iVar2b = gMapManager.GetCurrentMap();

    // ── 3) Fill water height table with sine wave ─────────────────────────────
    if ((int)uVar8 <= rowMax) {
        int iVar9 = (int)uVar8 << 8;   // row * 256
        int colMax2 = FrustrumBoundMaxX_1 + 3;
        int rowCount = (rowMax - (int)uVar8) + 1;
        if ((int)FrustrumBoundMinX_1 <= colMax2) {
            // DESVIACION: la onda depende de columna y tiempo, no de fila.
            // Calcular una vez con la misma aritmética y copiar el resultado
            // preserva también el orden de escritura si las filas se solapan.
            const int columns=(colMax2-FrustrumBoundMinX_1)+1;
            static std::vector<float> waveColumns;
            waveColumns.resize(columns);
            fVar12=(float10)(int)FrustrumBoundMinX_1;
            for(int column=0;column<columns;++column) {
                fVar1=iVar2b==8 ? 50.0f : 5.0f;
                fVar13=(float10)fsin(fVar12*(float10)fVar1+fVar11);
                waveColumns[column]=(float)(fVar13*(float10)10.0f);
                fVar12+=(float10)1.0f;
            }
            do {
                pfVar6=(float*)((char*)&WaterWaveHeightTable+(iVar9+FrustrumBoundMinX_1)*4);
                std::memcpy(pfVar6,waveColumns.data(),waveColumns.size()*sizeof(float));
                iVar9+=0x100;
            } while(--rowCount!=0);
            iVar4=0; // el contador de columnas original termina en cero.
        }
    }

    return iVar4;
}
