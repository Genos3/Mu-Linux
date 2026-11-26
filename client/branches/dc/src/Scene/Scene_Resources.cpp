// Scene_Resources.cpp
// Texture and model resource loading/unloading for login/account/char-select scenes.
//
// OpenLogoSceneData @ 0x0050fcf0 — Scene_LoadAccountResources
// OpenCharacterSceneData @ 0x0050ff50 — Scene_LoadCharSelectResources
// ReleaseLogoSceneData @ 0x0050ff10 — Scene_UnloadAccountResources
// IDA: ReleaseCharacterSceneData (0x005102C0)

#include "stdafx.h"
#include "Render/MeshDraw.h"
#ifdef __DREAMCAST__
#include "Platform/Dreamcast/DCAssetCache.h"
#endif
#include <vector>
#include <algorithm>

namespace {
bool selectBackdropWide = false;
std::vector<int> selectBackdropTextures;

const char* SelectBackdropPath()
{
  return gWindow.IsWide() ? "Data\\Logo\\Wide\\" : Dir_DataLogo;
}
}

void Scene_RefreshCharSelectBackdrop()
{
  if (selectBackdropWide == gWindow.IsWide()) return;
  BMD* model = AsModel(ModelsBase) + 0xa4;
  // Mantener las texturas de ambas variantes durante esta escena. El modelo
  // comparte texturas con otras mallas y puede alternar repetidas veces.
  const int count = model->NumMeshs;
  const short* textures = model->IndexTexture;
  for (int i = 0; i < count; ++i) {
    const int id = textures[i];
    if (std::find(selectBackdropTextures.begin(), selectBackdropTextures.end(), id)
        == selectBackdropTextures.end()) {
      ++g_BitmapsRaw[id * TEXTURE_STRIDE + 0x30];
      selectBackdropTextures.push_back(id);
    }
  }
  // Las mallas residentes guardan punteros a la topología que se libera.
  MeshDraw::Release();
  BMD__Release(model);
  AccessModel(0xa4,SelectBackdropPath(),(const char*)&ModelBaseNameLogo,5);
  OpenTexture(0xa4,Dir_Logo,0x2601,'\x01');
  selectBackdropWide = gWindow.IsWide();
}


// OpenLogoSceneData — Scene_LoadAccountResources
// Loads textures and 3D models for the Account Creation / New Character screens.
// Textures: Logo_0Account_new.tga, On_Botton.jpg, etc. (slots 0xc–0x14).
// Models: ship.smd (0x3c), logo objects (0xa0–0xa3).
// Preloads model data and activates rendering for slots 0x2601.
// IDA: FUN_0050FCF0
void Scene_LoadAccountResources(void)
{
  int iVar1;
  int iVar2;

#ifdef __DREAMCAST__
  // Prototype: warm the ring cache with the login ship textures while the UI
  // textures and models are still loading. OpenTexture(0x3c) below then finds
  // them in RAM instead of reading them from the disc a second time.
  DCAssetCache::Prefetch("Object1/ship01.dt");
  DCAssetCache::Prefetch("Object1/ship03.dt");
  DCAssetCache::Prefetch("Object1/ship04.dt");
  DCAssetCache::Prefetch("Object1/ship05.dt");
  DCAssetCache::Prefetch("Object1/ship06.dt");
  DCAssetCache::Prefetch("Object1/ship07.dt");
#endif

  DC_LoadTexture(Img_LogoAccountNew, 0xc, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoOnButton, 0xd, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoOnButton2, 0xe, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoTextBox, 0xf, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoNewAccount01, 0x10, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoNewAccount02, 0x11, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoBox, 0x12, 0x2600, 0x2900);
  DC_LoadTexture(Img_ProgressBack, 0x13, 0x2600, 0x2900);
  DC_LoadTexture(Img_Progress, 0x14, 0x2600, 0x2900);
  if (g_tex_ext_mode == '\0') {
    OpenModel(0x3c,Dir_Data2Object1,Smd_Ship);
    OpenModel(0xa0,Dir_Data2Logo,(const char*)&SmdNameBackground);
    OpenModel(0xa1,Dir_Data2Logo,(const char*)&SmdNameBackground2);
    OpenModel(0xa2,Dir_Data2Logo,Smd_Mu);
    OpenModel(0xa3,Dir_Data2Logo,Smd_Sun);
  }
  // Login preview characters use the player model slot (0x186) plus the
  // class equipment slots, but the world loader that normally fills them
  // never runs in the login scene. Load the body, its textures and the three
  // preview classes here so the ships have characters standing on them.
  // Use a trimmed, pre-decrypted copy of player.bmd: the full v12 file needs
  // ~3.4 MB of peak buffers on the 16 MB DC heap and OOMs. The login scene
  // only needs a few animation slots.
  BMD__Open(AsModel(ModelsBase) + 0x186, "Data\\Player\\", "player_login.bmd", 0);
  // player.bmd is skeleton-only (numMesh=0): the visible body is assembled
  // from five equipment BMDs per class, exactly like Model_Items.cpp does for
  // the world loader. Login used to load only ArmorClass, so the previews had
  // no head, hands, legs or boots. Load the complete set for classes 1-3.
  for (int cls = 1; cls <= 3; ++cls) {
    AccessModel(cls + 0x38f, "Data\\Player\\", "HelmClass",  cls);
    AccessModel(cls + 0x396, "Data\\Player\\", "ArmorClass", cls);
    AccessModel(cls + 0x39d, "Data\\Player\\", "PantClass",  cls);
    AccessModel(cls + 0x3a4, "Data\\Player\\", "GloveClass", cls);
    AccessModel(cls + 0x3ab, "Data\\Player\\", "BootClass",  cls);
  }
  // OpenTexture reads the texture names from each BMD slot, so the models
  // must exist before their textures can be bound.
  Model_LoadPlayerEquipmentTextures();

  AccessModel(0x3c,Dir_DataObject1,(const char*)&ModelBaseNameShip,1);
  iVar2 = 0xa0;
  do {
    AccessModel(iVar2,Dir_DataLogo,(const char*)&ModelBaseNameLogo,iVar2 + -0x9f);
    iVar1 = iVar2 + -0x9f;
    iVar2 = iVar2 + 1;
  } while (iVar1 < 4);
  SetMaxTextures(0x56a);
  OpenTexture(0x3c,Dir_Object1,0x2601,'\x01');
  iVar2 = 0;
  do {
    OpenTexture(iVar2 + 0xa0,Dir_Logo,0x2601,'\x01');
    iVar2 = iVar2 + 1;
  } while (iVar2 < 4);
  return;
}


// OpenCharacterSceneData — Scene_LoadCharSelectResources
// Loads textures and 3D models for the Character Selection screen.
// Textures: Logo_Interface01–04.tga, New_Character201.tga, etc. (slots 0xc–0x1d).
// Models: main.smd (0xa4), warrior/fairy/dark/etc. (0xaa–0xad).
// Also sets scale for character preview models (0xbc–0x2f0) from ModelsBase.
// IDA: FUN_0050FF50
void Scene_LoadCharSelectResources(void)
{
  int iVar1;
  intptr_t iVar2;

  DC_LoadTexture(Img_LogoInterface01, 0xc, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoInterface02, 0xd, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoInterface03, 0xe, 0x2600, 0x2900);
  DC_LoadTexture(Img_LogoInterface04, 0xf, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacter201, 0x10, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacter202, 0x11, 0x2600, 0x2900);
  DC_LoadTexture(Img_Delete01, 0x12, 0x2600, 0x2900);
  DC_LoadTexture(Img_Delete02, 0x13, 0x2600, 0x2900);
  DC_LoadTexture(Img_Ok01, 0x14, 0x2600, 0x2900);
  DC_LoadTexture(Img_Ok02, 0x15, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacter01, 0x16, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacter02, 0x17, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacterCancel, 0x18, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacterOk, 0x19, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacter001, 0x1a, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacter002, 0x1b, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacter003, 0x1c, 0x2600, 0x2900);
  DC_LoadTexture(Img_NewCharacter004, 0x1d, 0x2600, 0x2900);
  if (g_tex_ext_mode == '\0') {
    OpenModel(0xa4,Dir_Data2Logo,Smd_Main);
    OpenModel(0xaa,Dir_Data2Logo,(const char*)&SmdNameWizardFace);
    OpenModel(0xab,Dir_Data2Logo,Smd_Warrior);
    OpenModel(0xac,Dir_Data2Logo,Smd_Fairy);
    OpenModel(0xad,Dir_Data2Logo,(const char*)&SmdNameSwordsmanFace);
  }
  // El diorama clásico termina en los bordes del encuadre 4:3. La variante
  // amplia agrega suelo y bosque lateral para el viewport real del mundo.
  AccessModel(0xa4,SelectBackdropPath(),(const char*)&ModelBaseNameLogo,5);
  selectBackdropWide = gWindow.IsWide();
  iVar2 = 0xaa;
  do {
    AccessModel(iVar2,Dir_DataLogo,(const char*)&ModelBaseNameFace,iVar2 + -0xa9);
    iVar1 = iVar2 + -0xa9;
    iVar2 = iVar2 + 1;
  } while (iVar1 < 4);
  SetMaxTextures(0x56a);
  OpenTexture(0xa4,Dir_Logo,0x2601,'\x01');
  iVar2 = 0;
  do {
    OpenTexture(iVar2 + 0xaa,Dir_Logo,0x2600,'\x01');
    iVar2 = iVar2 + 1;
  } while (iVar2 < 4);
  iVar2 = 0;
  do {
    iVar1 = iVar2 + 0xbc;
    ModelActionSpeed(AsModel(ModelsBase + BMD_STRIDE * (170 + iVar2 / 0xbc))->Actions, 0) = 0.3f;
    ModelActionSpeed(AsModel(ModelsBase + BMD_STRIDE * (170 + iVar2 / 0xbc))->Actions, 1) = 0.3f;
    iVar2 = iVar1;
  } while (iVar1 < 0x2f0);
  return;
}


// ReleaseLogoSceneData — Scene_UnloadAccountResources
// Frees texture slots 0xc–0x14 and 0x56a–0x5a9 used by the account scene,
// then calls DeleteObjects (clear model cache) and ClearCharacters(-1) (reset entities).
// IDA: FUN_0050FF10
void Scene_UnloadAccountResources(void)
{
  int iVar1;

  iVar1 = 0;
  do {
    UnloadImage(iVar1 + 0xc);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 9);
  iVar1 = 0x56a;
  do {
    UnloadImage(iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x5aa);
  DeleteObjects();
  ClearCharacters(-1);
  return;
}


// IDA: ReleaseCharacterSceneData (0x005102C0)
// Frees texture slots 0xc–0x1d and 0x56a–0x5a9 used by the char-select scene,
// clears model data for slots 0x7cd8–0x7fc8 (stride 0xbc),
// then calls DeleteObjects and ClearCharacters(-1).
// IDA: ReleaseCharacterSceneData (0x005102C0)
void ReleaseCharacterSceneData(void)
{
  int iVar1;

  for (int texture : selectBackdropTextures) UnloadImage(texture);
  selectBackdropTextures.clear();

  iVar1 = 0;
  do {
    UnloadImage(iVar1 + 0xc);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x12);
  iVar1 = 0x56a;
  do {
    UnloadImage(iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x5aa);
  DeleteObjects();
  for (int slot = 0xaa; slot < 0xae; ++slot)
    BMD__Release(AsModel(ModelsBase) + slot);
  ClearCharacters(-1);
  return;
}
