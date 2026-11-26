// Scene_OpenWorld.cpp

#include "stdafx.h"
#include "Game/RenderFxSnapshots.h"
#include "globals.h"
#include "functions.h"
#include "Platform/IFileSystem.h"
#include "Platform/Dreamcast/MUTrace.h"

// IDA: OpenWorld (0x0050E5A0)
// Per IDA decompile (1500 bytes).
// Loads all terrain and tile textures for the current world map.
//
// World name = "World<N>" where N = World+1 (capped at 12 for dungeons 11-16).
// Loads EncTerrain<N>.map, Terrain<N>.att, Terrain<N>.bin (map objects),
// TerrainHeight.bmp, TerrainLight.jpg, then 14 tile JPGs (slots 0x23-0x30) +
// 3 alpha-overlay TGAs (slots 0x32-0x34) + leaf01/02 + rain01/02 (always from
// World1) + rain03 (always from World10).
// Paths como IDA: "World%d", "Data/%s/Terrain%d", "Data/%s/terrain%d"; rain01/02
// usan "World1" y rain03 "World10" fijos.
void __cdecl OpenWorld(void) {
    BYTE  uVar1;
    CHAR  world_name[32];
    CHAR  local_40[64];

    MU_TRACE("WORLD: OpenWorld start");
    DeleteObjects();             // DeleteObjects
    DeleteNpcs();             // DeleteNpcs
    DeleteMonsters();             // DeleteMonsters
    ClearItems();             // ClearItems
    ClearCharacters(HeroKey); // ClearCharacters(HeroKey)

    RenderFxSnapshots_Reset();
    // Limpiar TODOS los pools de char-select que sobreviven al world load (si no,
    // los tick-functions iteran slots con punteros basura).
    memset(SpritePoolBase, 0, sizeof(SpritePoolBase));   // particle pool (3000×0x70)
    memset(SkillEffectPool, 0, sizeof(SkillEffectPool));   // skill effect pool (200×0x70)
    memset(EffectPoolBase, 0, kEffectPoolBaseBytes);   // effect pool (124×0x1bc)
    memset(JointPoolBase, 0, kJointPoolBytes);   // joint pool (200×0x9d8)
    memset(g_PlayerRenderPool, 0, sizeof(g_PlayerRenderPool)); // planos: 100×0x1bc
    memset(SparkEffectPool, 0, sizeof(SparkEffectPool));   // spark pool (100×0x70)
    memset(g_PointerPool, 0, sizeof(g_PointerPool)); // pointers: 100×0x70
    memset(FishObjectPool, 0, sizeof(FishObjectPool));   // boids + partículas ambientales (10×0x1bc)
    memset(ChatBubblePool, 0, sizeof(ChatBubblePool));   // tooltip pool (26×0x254)

    MU_TRACE("WORLD: pools cleared, OpenWorldModels start");
    OpenWorldModels();             // OpenWorldModels
    MU_TRACE("WORLD: OpenWorldModels done");

    int iVar2 = gMapManager.GetCurrentMap() + 1;
    if (gMapManager.GetCurrentMap() >= 11 && gMapManager.GetCurrentMap() <= 16) iVar2 = 12;

    crt_sprintf(world_name, "World%d", iVar2);

    // Desviación: los archivos reales del cliente son EncTerrain%d.{map,att,obj}
    // (versiones encriptadas), y los loaders de map/att/obj no tienen fallback de
    // extensión como el de texturas.
    crt_sprintf(local_40, "Data/%s/EncTerrain%d.map", world_name, iVar2);
    OpenTerrainMapping(local_40);     // OpenTerrainMapping

    // `EncTerrain%d.att` mide 131076 bytes (formato encriptado custom) y
    // `OpenTerrainAttribute` solo acepta 65539 (formato vanilla 0.97k): se prueba
    // primero el `Terrain%d.att` sin encriptar (el formato que espera IDA) y si no
    // existe, EncTerrain*.att.
    crt_sprintf(local_40, "Data/%s/Terrain%d.att", world_name, iVar2);
    if (OpenTerrainAttribute(local_40) == 0) {
        crt_sprintf(local_40, "Data/%s/EncTerrain%d.att", world_name, iVar2);
        OpenTerrainAttribute(local_40);     // OpenTerrainAttribute(FileName)
    }
    MU_TRACE("WORLD: terrain map+att loaded");

    // Streaming objects: keep the 92 KB binary resident and instantiate only
    // the OBJECTs in cells near the camera. Falls back to eager loading if the
    // stream file is missing/invalid.
    crt_sprintf(local_40, "Data/%s/Terrain%d.bin", world_name, iVar2);
    ObjectsStream_Init(local_40);

    uVar1 = g_tex_ext_mode;
    // Test-display mode must keep g_tex_ext_mode == 1 so .dt textures resolve
    // under Data/ (there is no Data2/ directory on the DC disc).
    if (InGameModeFlag != '\0' && TestSceneMapId < 0) g_tex_ext_mode = 0;

    // Desviación: los archivos reales son OZ* (encriptados), no .bmp/.jpg/.tga, y
    // OpenJPG sólo cambia la extensión si g_tex_ext_mode != 0 (in-game vale 0): se
    // usan las extensiones reales.
    crt_sprintf(local_40, "%s/TerrainHeight.OZB", world_name); CreateTerrain(local_40);
    MU_TRACE("WORLD: CreateTerrain (height) done");
    if (TestSceneMapId >= 0) {
        // TerrainLight.dt is an offline-converted texture; OpenJpegBuffer expects
        // the original JPEG and would abort the game. Use neutral ambient light.
        for (int i = 0; i < 256 * 256 * 3; ++i) TerrainLightAmbientBuf[i] = 1.0f;
        CreateTerrainNormal();
        CreateTerrainLight();
        MU_TRACE("WORLD: TerrainLight neutral: ambient=%.3f normal=%.3f light=%.3f",
                 TerrainLightAmbientBuf[0], TerrainNormalTable[0], TerrainLightData[0]);
    } else {
        crt_sprintf(local_40, "%s/TerrainLight.OZJ",  world_name); OpenTerrainLight(local_40);
        MU_TRACE("WORLD: TerrainLight done");
    }

    // Tile textures (OZJ: slots 0x23-0x30; OZT alpha overlays: slots 0x32-0x34)
    crt_sprintf(local_40, "%s/TileGrass01.OZJ",  world_name); DC_LoadTexture(local_40, 0x23, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileGrass01.OZT",  world_name); DC_LoadTexture(local_40, 0x32, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileGrass02.OZT",  world_name); DC_LoadTexture(local_40, 0x33, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileGrass03.OZT",  world_name); DC_LoadTexture(local_40, 0x34, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileGrass02.OZJ",  world_name); DC_LoadTexture(local_40, 0x24, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileGround01.OZJ", world_name); DC_LoadTexture(local_40, 0x25, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileGround02.OZJ", world_name); DC_LoadTexture(local_40, 0x26, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileGround03.OZJ", world_name); DC_LoadTexture(local_40, 0x27, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileWater01.OZJ",  world_name); DC_LoadTexture(local_40, 0x28, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileWood01.OZJ",   world_name); DC_LoadTexture(local_40, 0x29, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileRock01.OZJ",   world_name); DC_LoadTexture(local_40, 0x2a, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileRock02.OZJ",   world_name); DC_LoadTexture(local_40, 0x2b, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileRock03.OZJ",   world_name); DC_LoadTexture(local_40, 0x2c, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileRock04.OZJ",   world_name); DC_LoadTexture(local_40, 0x2d, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileRock05.OZJ",   world_name); DC_LoadTexture(local_40, 0x2e, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileRock06.OZJ",   world_name); DC_LoadTexture(local_40, 0x2f, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/TileRock07.OZJ",   world_name); DC_LoadTexture(local_40, 0x30, 0x2600, 0x2901);
    crt_sprintf(local_40, "%s/leaf01.OZT",  world_name); DC_LoadTexture(local_40, 100, 0x2600, 0x2900);
    crt_sprintf(local_40, "%s/leaf01.OZJ",  world_name); DC_LoadTexture(local_40, 100, 0x2600, 0x2900);
    crt_sprintf(local_40, "%s/leaf02.OZJ",  world_name); DC_LoadTexture(local_40, 0x65, 0x2600, 0x2900);
    // rain01/02 always loaded from World1; rain03 always from World10 (per IDA).
    crt_sprintf(local_40, "World1/rain01.OZT"); DC_LoadTexture(local_40, 0x66, 0x2600, 0x2900);
    crt_sprintf(local_40, "World1/rain02.OZT"); DC_LoadTexture(local_40, 0x67, 0x2600, 0x2900);
    crt_sprintf(local_40, "World10/rain03.OZT"); DC_LoadTexture(local_40, 0x68, 0x2600, 0x2900);

    MU_TRACE("WORLD: tiles: slot35=%u slot36=%u slot37=%u slot40=%u",
             Bitmaps[35].TextureNumber, Bitmaps[36].TextureNumber,
             Bitmaps[37].TextureNumber, Bitmaps[40].TextureNumber);
    MU_TRACE("WORLD: tile textures done, OpenWorld exit");
    if (InGameModeFlag != '\0') g_tex_ext_mode = uVar1;
}

// IDA: OpenFont (0x0050F690)
// Resets font state, loads FontInput.tga (slot 0) and FontTest.tga (slot 1) as TGA,
// then builds the font DIB (FUN_0050f5f0) and renderer (FUN_0040f570).
void __cdecl OpenFont(void) {
    PathFinder_ResetContext();
    DC_LoadTexture("Interface/FontInput.tga", 0, 0x2600, 0x2900);
    DC_LoadTexture("Interface/FontTest.tga", 1, 0x2600, 0x2900);
    Font_CreateTextDib(((DWORD)(uintptr_t)gWindow.GetHdc()));
    Font_CreateRenderer(UiRenderTextObj, static_cast<int>(TextOutMode), ((DWORD)(uintptr_t)gWindow.GetHdc()));
}
// Scene resource load/unload routines are implemented in src/Scene/Scene_Resources.cpp.
