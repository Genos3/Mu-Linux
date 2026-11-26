// Scene_AssetLoad.cpp

#include "stdafx.h"
#include "Local/ClientText.h"
#include "Text/TextCore.h"
#include "globals.h"
#include "functions.h"
#include "Platform/Dreamcast/MUTrace.h"

// IDA: OpenBasicData @ 0x00510320 — Scene_LoadGameAssets
// Loads: cursor TGAs (slots 2-10), interface JPGs (0xE6-0x101, 0x500-0x507),
// effect JPGs/TGAs (0x47E-0x567), then invokes all model, data and sound preload routines.
static void LoadWorldTextures()
{
    MU_TRACE("LOAD: LoadWorldTextures start");
    // Effect textures
    DC_LoadTexture("Effect/Fire01.jpg", 0x4ab, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Fire02.jpg", 0x4ac, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Fire03.jpg", 0x4ad, 0x2601, 0x2900);
    DC_LoadTexture("Effect/PoundingBall.jpg", 0x567, 0x2601, 0x2900);
    DC_LoadTexture("Effect/fi01.jpg", 0x565, 0x2601, 0x2900);
    DC_LoadTexture("Effect/fi02.tga", 0x566, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Flame01.jpg", 0x4b0, 0x2601, 0x2900);
    DC_LoadTexture("Effect/flare01.jpg", 0x47e, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Magic_Ground2.jpg", 0x4f0, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Magic_Circle1.jpg", 0x4f1, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Spark02.jpg", 0x497, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Spark03.jpg", 0x498, 0x2601, 0x2900);
    DC_LoadTexture("Effect/smoke01.jpg", 0x4c4, 0x2601, 0x2900);
    DC_LoadTexture("Effect/smoke02.tga", 0x4c5, 0x2601, 0x2900);
    DC_LoadTexture("Effect/blood01.tga", 0x4b5, 0x2601, 0x2900);
    DC_LoadTexture("Effect/blood.tga", 0x4b6, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Explotion01.jpg", 0x4bf, 0x2601, 0x2900);
    DC_LoadTexture("Effect/DinoE.jpg", 0x4c0, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Shiny01.jpg", 0x4ce, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Shiny02.jpg", 0x4cf, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Shiny03.jpg", 0x4d0, 0x2601, 0x2900);
    DC_LoadTexture("Effect/eye01.jpg", 0x4d1, 0x2601, 0x2900);
    DC_LoadTexture("Effect/ring.jpg", 0x4d2, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Chrome01.jpg", 0x492, 0x2601, 0x2901);
    DC_LoadTexture("Effect/blur01.jpg", 0x48d, 0x2600, 0x2900);
    DC_LoadTexture("Effect/bab2.jpg", 0x493, 0x2601, 0x2901);
    DC_LoadTexture("Effect/motion_blur.jpg", 0x48e, 0x2600, 0x2900);
    DC_LoadTexture("Effect/motion_blur_r.jpg", 0x48f, 0x2600, 0x2900);
    DC_LoadTexture("Effect/lightning2.jpg", 0x4a7, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Thunder01.jpg", 0x49c, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Spark01.jpg", 0x4e9, 0x2601, 0x2900);
    DC_LoadTexture("Effect/JointThunder01.jpg", 0x4e6, 0x2601, 0x2900);
    DC_LoadTexture("Effect/JointSpirit01.jpg", 0x4e5, 0x2601, 0x2900);
    DC_LoadTexture("Effect/JointLaser01.jpg", 0x4ea, 0x2601, 0x2900);
    DC_LoadTexture("Effect/JointEnergy01.jpg", 0x4eb, 0x2601, 0x2900);
    DC_LoadTexture("Effect/JointLaser02.jpg", 0x4ed, 0x2601, 0x2901);
    DC_LoadTexture("Monster/iui03.jpg", 0x4f6, 0x2601, 0x2901);
    DC_LoadTexture("Monster/magic_H.tga", 0x4fb, 0x2601, 0x2901);
    DC_LoadTexture("Item/lower_14m.tga", 0x4fc, 0x2601, 0x2901);
    DC_LoadTexture("Skill/Skull.jpg", 0x4fa, 0x2600, 0x2900);
    DC_LoadTexture("Effect/motion_blur_r2.jpg", 0x4ee, 0x2601, 0x2901);
    DC_LoadTexture("Effect/Fire04.jpg", 0x4df, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Flare.jpg", 0x4e1, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Chrome02.jpg", 0x4f2, 0x2600, 0x2900);
    DC_LoadTexture("Effect/flareBlue.jpg", 0x4fd, 0x2601, 0x2900);
    DC_LoadTexture("Effect/NSkill.jpg", 0x4fe, 0x2601, 0x2900);
    DC_LoadTexture("Effect/Flare02.jpg", 0x4e2, 0x2601, 0x2901);
    // Model loaders
    Model_LoadPlayerAndItemMeshes(); Model_LoadPlayerEquipmentTextures(); Model_LoadItemMeshes();
    Model_AssignItemTexturePrefixes(); Model_LoadSkillEffectAssets(); UI_LoadInterfaceTextures();
    MU_TRACE("LOAD: LoadWorldTextures done");
}

void __cdecl Scene_LoadGameAssets(void) {
    CHAR local_64[100];
    MU_TRACE("LOAD: Scene_LoadGameAssets start");
    // Cursor sprites
    DC_LoadTexture("Interface/Cursor.tga", 2, 0x2601, 0x2900);
    DC_LoadTexture("Interface/CursorPush.tga", 3, 0x2601, 0x2900);
    DC_LoadTexture("Interface/CursorAttack.tga", 4, 0x2601, 0x2900);
    DC_LoadTexture("Interface/CursorGet.tga", 5, 0x2601, 0x2900);
    DC_LoadTexture("Interface/CursorTalk.tga", 6, 0x2601, 0x2900);
    DC_LoadTexture("Interface/CursorRepair.tga", 7, 0x2601, 0x2900);
    DC_LoadTexture("Interface/CursorLeanAgainst.tga", 8, 0x2601, 0x2900);
    DC_LoadTexture("Interface/CursorSitDown.tga", 9, 0x2601, 0x2900);
    DC_LoadTexture("Interface/CursorDontMove.tga", 10, 0x2601, 0x2900);
    MU_TRACE("LOAD: cursors done");
    // Interface / HUD JPGs
    DC_LoadTexture("Interface/Menu01_new.jpg", 0xe6, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu02.jpg", 0xe7, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu03_new.jpg", 0xe8, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu04.tga", 0xe9, 0x2600, 0x2900);
    // DESVIACION (Fase 4, pantalla ancha; DLL CWideScreen WDecorOld): cabezas de
    // dragon que reemplazan a las de Menu04 en las esquinas de la barra.
    DC_LoadTexture("Interface/WDecorOld.tga", 0x5f0, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu_Red.jpg", 0xea, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu_Blue.jpg", 0xeb, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu_Party.jpg", 0xec, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu_Character.jpg", 0xed, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu_Inventory.jpg", 0xee, 0x2600, 0x2900);
    DC_LoadTexture("Interface/dialogue2_1.jpg", 0xef, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Message_box.jpg", 0xf0, 0x2600, 0x2900);
    DC_LoadTexture("Interface/ok.jpg", 0xf1, 0x2600, 0x2900);
    DC_LoadTexture("Interface/ok2.jpg", 0xf2, 0x2600, 0x2900);
    DC_LoadTexture("Interface/cancel.jpg", 0xf3, 0x2600, 0x2900);
    DC_LoadTexture("Interface/cancel2.jpg", 0xf4, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Level_box.jpg", 0xf5, 0x2600, 0x2900);
    DC_LoadTexture("Interface/level01.jpg", 0xf6, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu_Green.jpg", 0xf7, 0x2600, 0x2900);
    DC_LoadTexture("Interface/dialogue1.jpg", 0xf8, 0x2600, 0x2900);
    DC_LoadTexture("Interface/dialogue2.jpg", 0xf9, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Guild.jpg", 0xfa, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Message_box1.jpg", 0xfb, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Message_box2.jpg", 0xfc, 0x2600, 0x2900);
    // NIS (new interface system) frames
    DC_LoadTexture("Interface/nis_rsframe.tga", 0x500, 0x2600, 0x2900);
    DC_LoadTexture("Interface/nis_vframe.jpg", 0x501, 0x2600, 0x2900);
    DC_LoadTexture("Interface/nis_bar.jpg", 0x502, 0x2600, 0x2900);
    DC_LoadTexture("Interface/nis_back.jpg", 0x503, 0x2600, 0x2900);
    DC_LoadTexture("Interface/nis_btnarrow.jpg", 0x504, 0x2600, 0x2900);
    DC_LoadTexture("Interface/nis_btnchat.jpg", 0x505, 0x2600, 0x2900);
    DC_LoadTexture("Interface/nis_btnsize.jpg", 0x506, 0x2600, 0x2900);
    DC_LoadTexture("Interface/nis_btnalpha.jpg", 0x507, 0x2600, 0x2900);
    DC_LoadTexture("Interface/nis_btndance.jpg", 0x508, 0x2600, 0x2900);   // menu de emotes (propio)
    DC_LoadTexture("Interface/nis_btngesture.jpg", 0x509, 0x2600, 0x2900);   // menu de gestos clasicos (propio)
    DC_LoadTexture("Interface/bar.jpg", 0xfd, 0x2600, 0x2900);
    DC_LoadTexture("Interface/back1.jpg", 0xfe, 0x2600, 0x2900);
    DC_LoadTexture("Interface/back2.jpg", 0xff, 0x2600, 0x2900);
    DC_LoadTexture("Interface/back3.jpg", 0x100, 0x2600, 0x2900);
    DC_LoadTexture("Interface/Menu03_new_AG.jpg", 0x101, 0x2600, 0x2900);
    MU_TRACE("LOAD: interface textures done");
#ifndef __DREAMCAST__
    LoadWorldTextures();
#endif
#ifndef __DREAMCAST__
    Sound_LoadGameSamples();   // PC: full preload as in the original
#else
    // DC: the ~100 ambient/skill WAVs are not needed for the login screen and
    // would consume AICA RAM + fail sfxmgr's sample limit. Loaded on entering
    // the world instead (Scene_Loading / Scene_Intro direct-to-game).
#endif
    // Data/BMD loaders (locale-sensitive)
    if (g_tex_ext_mode == '\0') {
        crt_sprintf(local_64, "Data2/Local/item_%s.txt", (char*)Data2PathPrefix);
        Item_LoadTextData(local_64);
        Item_SaveBMD("Data/Local/Item.bmd");
        crt_sprintf(local_64, "Data2/Local/skill_%s.txt", (char*)Data2PathPrefix);
        Skill_LoadTextData(local_64);
        Skill_SaveBMD("Data/Local/Skill.bmd");
        Gate_LoadTextData("Data2/Gate.txt");
        Gate_SaveBMD("Data/Gate.bmd");
        crt_sprintf(local_64, "Data2/Local/Filter_%s.txt", (char*)Data2PathPrefix);
        Filter_LoadTextFile(local_64);
        Filter_SaveBMD("Data/Local/Filter.bmd");
        crt_sprintf(local_64, "Data2/Local/FilterName_%s.txt", (char*)Data2PathPrefix);
        FilterName_LoadData(local_64);
        FilterName_SaveBMD("Data/Local/FilterName.bmd");
        // OJO: copy-paste sin corregir — esto carga Dialog_%s.txt con el loader
        // de FilterName y lo guarda encima de FilterName.bmd, o sea pisa el
        // archivo anterior y nunca produce Dialog.bmd.  Ademas IDA usa
        // "Data2\\Local\\Dialog(%s).txt" (parentesis, no guion bajo).  No se
        // corrige porque no existen Dialog_LoadData/Dialog_SaveBMD y toda esta
        // rama es codigo muerto: g_tex_ext_mode ya vale 1 (igual que en el
        // binario), asi que el convertidor Data2 -> .bmd no corre nunca.
        crt_sprintf(local_64, "Data2/Local/Dialog_%s.txt", (char*)Data2PathPrefix);
        FilterName_LoadData(local_64);
        FilterName_SaveBMD("Data/Local/FilterName.bmd");
    } else {
        Item_LoadBMD("Data/Local/Item.bmd");
        Skill_LoadBMD("Data/Local/Skill.bmd");
        Gate_LoadBMD("Data/Gate.bmd");
        OpenFilterFile("Data/Local/Filter.bmd");
        OpenNameFilterFile("Data/Local/FilterName.bmd");
        // IDA abre "Data\\Local\\Dialog.bmd" (string en 0x560B4C), igual que
        // Text.bmd.  El comentario anterior decia que solo existian las
        // variantes Dialog_{Eng,Por,Spn}.bmd y no es cierto: Dialog.bmd esta en
        // Data/Local y es byte por byte identico a Dialog_Spn.bmd, asi que esto
        // no cambia nada hoy — solo deja de hardcodear el idioma.
        // DESVIACION (DLL Language.cpp): Dialog_<idioma>.bmd si hay selección.
        char dialogPath[MAX_PATH];
        Dialog_LoadBMD(LocalizedDataPath("Data/Local/Dialog", ".bmd", dialogPath, sizeof(dialogPath)));
        CSQuest_OpenQuestScript(reinterpret_cast<BYTE*>(g_csQuest), "Data/Local/Quest.bmd");
    }
    MU_TRACE("LOAD: BMD data done");
    crt_sprintf(local_64, "Data/Local/NPCName.txt");   // cliente: archivo sin sufijo de locale
    NPCName_LoadTextData(local_64);
    // DESVIACION (Fase 4, texto Unicode): las palabras de los filtros de chat y
    // de nombres (Filter.bmd / FilterName.bmd, 1000 x 20 bytes) a UTF-8;
    // lo que ya es UTF-8 se deja y el resto se
    // toma como Windows-1252 (NormalizeTextFileInPlace).
    for (int i = 0; i < 1000; ++i) {
        TextEngine::NormalizeTextFileInPlace(WordFilterTable + i * 0x14, 0x14);
        TextEngine::NormalizeTextFileInPlace(NameFilterTable + i * 0x14, 0x14);
    }
    MU_TRACE("LOAD: text normalized");
    // Sound preloads (tail of OpenBasicData @ 0x00510F65-0x00510FC5).
    // IDs y filenames verificados contra IDA Hex-Rays 00510320_OpenBasicData.c
    // y los binary patches del companion project (Mu-linux-97K/Source/Client/
    // Main/Sound.cpp:162-166). Ghidra había inventado los nombres
    // iPickUp/iGet/iDrop/iEquip — no existen como archivos .wav en Data/Sound.
#ifndef __DREAMCAST__
    // IDA UI samples. On DC the original WAVs exceed sfxmgr's 65,534-sample
    // limit and were causing allocator OOMs before the login screen; the
    // converted ADPCM versions belong to the streaming-audio milestone.
    LoadWaveFile(4,    "Data\\Sound\\iTitle.wav",        1, '\0');
    LoadWaveFile(0x1b, "Data\\Sound\\iButtonMove.wav",   2, '\0');  // 27
    LoadWaveFile(0x19, "Data\\Sound\\iButtonClick.wav",  1, '\0');  // 25
    LoadWaveFile(0x1a, "Data\\Sound\\iButtonError.wav",  1, '\0');  // 26
    LoadWaveFile(0x1c, "Data\\Sound\\iCreateWindow.wav", 1, '\0');  // 28
    LoadWaveFile(0x25, "Data\\Sound\\iRepair.wav",       1, '\0');  // 37
    LoadWaveFile(0x26, "Data\\Sound\\iWhisper.wav",      1, '\0');  // 38
#endif
    MU_TRACE("LOAD: Scene_LoadGameAssets done");
}
#ifdef __DREAMCAST__
// DC: the ~100 ambient/skill samples from Sound_LoadGameSamples() are not
// needed at the login screen. This is called once when entering the world.
void __cdecl Scene_LoadGameSounds(void)
{
    static bool loaded = false;
    if (loaded) return;
    loaded = true;
    Sound_LoadGameSamples();
}

void __cdecl Scene_LoadWorldAssets(void)
{
    static bool loaded = false;
    if (loaded) return;
    loaded = true;
    // Test-display mode: effect textures, player/item meshes and world sounds
    // are not needed to render terrain height. Skip the whole pass to stay
    // within the 16 MB DC heap.
    if (TestSceneMapId >= 0) {
        MU_TRACE("LOAD: Scene_LoadWorldAssets SKIPPED (test-display mode)");
        return;
    }
    MU_TRACE("LOAD: Scene_LoadWorldAssets start");
    LoadWorldTextures();
    MU_TRACE("LOAD: textures done, Sound_LoadGameSamples start");
    Sound_LoadGameSamples();
    MU_TRACE("LOAD: Scene_LoadWorldAssets done");
}
#endif

// Monster_LoadStartupData (IDA: Monster_Data_Load) - implemented in src/Monster/Monster_Data.cpp
// GL_CaptureScreenshot — implemented in src/Render/GL_State.cpp
// GL_GetModelViewMatrix — implemented in src/Render/GL_State.cpp
