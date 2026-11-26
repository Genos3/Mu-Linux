#include <cstdint>
// Monster_Data.cpp
// Monster_Data_Load @ 0x00511060
//
// Loads monster script / base data files.
// Only executed if InGameModeFlag != 0 (multiplayer / server-mode flag).
//
// Sub-functions:
//   Monster_LoadScriptTable — load monster script table (enable/disable state via g_tex_ext_mode)
//   Monster_ParseSetBase2 — parse and load "Data2/MonsterSetBase2.txt" monster definitions
//
// Globals:
//   InGameModeFlag   — if non-zero: server/party-enabled mode
//   g_tex_ext_mode   — loader phase flag (0 = pre-load, 1 = post-load)
//   s_Data2_MonsterSetBase2_txt (0x00561530) — filename literal

#include "stdafx.h"

// IDA: Monster_Data_Load
void __cdecl Monster_LoadStartupData(void)
{
    if (InGameModeFlag != '\0') {
        g_tex_ext_mode = 0;
        Monster_LoadScriptTable();           // load monster script table
        g_tex_ext_mode = 1;
        if (InGameModeFlag != '\0') {
            Monster_ParseSetBase2(Path_MonsterSetBase2Txt);  // load MonsterSetBase2.txt
        }
    }
    return;
}


// FUN_0050b510 @ 0x0050b510 — Monster_LoadScriptTable
//
// Monster_LoadScriptTable — loads the monster script/model table.
//
// Calls SetMaxTextures to initialize the table with 0x41a entries.
// Then, only in pre-load phase (g_tex_ext_mode == 0), registers BMD model
// files from Data2/Monster/ for monster types 0x13d–0x14c and invokes
// OpenMonsterModel for monster IDs 0x32–0x3e to set up their sound+animation
// data.
//
// Globals:
//   g_tex_ext_mode               — loader phase (0 = pre-load)
//   Dir_Data2Monster  — "Data2/Monster/" path prefix
//   Smd_SwordBasic  — "swordbasic.smd" filename

void Monster_LoadScriptTable(void)

{
  int iVar1;

  SetMaxTextures(0x41a);
  if (g_tex_ext_mode == '\0') {
    OpenModel((int)0x13d,Dir_Data2Monster,Smd_SwordBasic);
    OpenModel((int)0x147,Dir_Data2Monster,&MonsterSmdName147);
    OpenModel((int)0x148,Dir_Data2Monster,&MonsterSmdName148);
    OpenModel((int)0x149,Dir_Data2Monster,&MonsterSmdName149);
    OpenModel((int)0x14a,Dir_Data2Monster,&MonsterSmdName14A);
    OpenModel((int)0x14b,Dir_Data2Monster,&MonsterSmdName14B);
    OpenModel((int)0x14c,Dir_Data2Monster,&MonsterSmdName14C);
    iVar1 = 0x32;
    do {
      OpenMonsterModel(iVar1);
      iVar1 = iVar1 + 1;
    } while (iVar1 < 0x3f);
  }
  return;
}


// FUN_0047d020 @ 0x0047d020 — Monster_ParseSetBase2
//
// Monster_ParseSetBase2 — parses "Data2/MonsterSetBase2.txt" to
// populate the monster spawn table.
//
// Opens the file via crt_fopen, then reads records with TextParser_GetToken.
// For each record:
//   - field 0 (ftol) = monster type
//   - fields 1,2,3  = skipped
//   - field 4 (ftol) = spawn X
//   - field 5        = skipped
//   - field 6 (ftol) = spawn Y
//   - Stops on record type 2 or when the sentinel string (SentinelEndString)
//     matches TextParserTokenString.
// Creates the monster via CreateMonster and sets field +0x84 = ' '.
//
// Globals:
//   BmdFileHandle  — file handle
//   MonsterSpawnSlotIndex  — next spawn slot index
//   HeroKey  — max spawn slot (skips if equal)
//   SentinelEndString  — sentinel string "END"

void __cdecl Monster_ParseSetBase2(LPCSTR param_1)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  char *pcVar4;
  byte *pbVar5;
  bool bVar6;
  longlong lVar7;
  longlong lVar8;
  longlong lVar9;

  BmdFileHandle = (FILE *)crt_fopen(param_1,FopenModeRb);
  if (BmdFileHandle != (FILE *)0x0) {
    TextParser_GetToken();
    while( true ) {
      iVar2 = TextParser_GetToken();
      lVar7 = (longlong)ParserTokenNumber;   // IDA sub_47D020: (std::int64_t)TokenNumber
      if (iVar2 == 2) break;
      if (iVar2 == 0) {
        pbVar5 = (byte*)TokenString;
        pbVar3 = (byte*)&SentinelEndString;
        do {
          bVar1 = *pbVar3;
          bVar6 = bVar1 < *pbVar5;
          if (bVar1 != *pbVar5) {
LAB_0047d096:
            iVar2 = (1 - (uint)bVar6) - (uint)(bVar6 != 0);
            goto LAB_0047d09b;
          }
          if (bVar1 == 0) break;
          bVar1 = pbVar3[1];
          bVar6 = bVar1 < pbVar5[1];
          if (bVar1 != pbVar5[1]) goto LAB_0047d096;
          pbVar3 = pbVar3 + 2;
          pbVar5 = pbVar5 + 2;
        } while (bVar1 != 0);
        iVar2 = 0;
LAB_0047d09b:
        if (iVar2 == 0) break;
      }
      TextParser_GetToken();
      TextParser_GetToken();
      TextParser_GetToken();
      lVar8 = (longlong)ParserTokenNumber;   // IDA sub_47D020: (std::int64_t)TokenNumber
      TextParser_GetToken();
      lVar9 = (longlong)ParserTokenNumber;   // IDA sub_47D020: (std::int64_t)TokenNumber
      TextParser_GetToken();
      if (MonsterSpawnSlotIndex == HeroKey) {
        MonsterSpawnSlotIndex = MonsterSpawnSlotIndex + 1;
      }
      iVar2 = MonsterSpawnSlotIndex;
      MonsterSpawnSlotIndex = MonsterSpawnSlotIndex + 1;
      pcVar4 = CreateMonster((uint)lVar7,(int)lVar8,(int)lVar9,iVar2,0);
      if (pcVar4 != (char *)0x0) {
        pcVar4[0x84] = ' ';
      }
    }
    crt_fclose(BmdFileHandle);
  }
  return;
}


// OpenMonsterModel @ 0x005098c0
//
// Monster_SetupSoundAnim — registers sound and animation data for one
// monster type (param_1 = monster index 0x00–0x3f).
//
// Only runs when g_tex_ext_mode == 0 (pre-load) OR when the monster has
// no existing animations (sound/anim counts == 0).
// Calls AccessModel to load the base monster script, then sets default
// animation speeds and special flags in the animation block at
// *(int*)(iVar2 + 0x30).
// Sound files are registered via LoadWaveFile and animation frame sets
// via Model_SetAnimationSlots.
//
// Globals:
//   g_tex_ext_mode  — loader phase flag
//   ModelsBase  — monster table base pointer

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl OpenMonsterModel(int param_1)

{
  int iVar1;
  intptr_t iVar2;
  intptr_t iVar3;
  float fVar4;
  int iVar5;

  iVar1 = param_1 + 0x10e;
  iVar2 = ModelsBase + iVar1 * BMD_STRIDE;
  if (((g_tex_ext_mode == '\0') || ((*(short *)(iVar2 + 0x26) < 1 && (*(short *)(iVar2 + 0x24) < 1))))
     && (AccessModel(iVar1,Dir_DataMonster,Dir_Monster,param_1 + 1),
        *(short *)(iVar2 + 0x24) != 0)) {
    OpenTexture(iVar1,Dir_MonsterSlash,0x2600,'\x01');
    *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(4)) = 0x3e800000;
    *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x14)) = 0x3e4ccccd;
    *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3eae147b;
    *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x34)) = 0x3ea8f5c3;
    *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x44)) = 0x3ea8f5c3;
    *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x54)) = 0x3f000000;
    iVar5 = 0;
    *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3f0ccccd;
    *(undefined1 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x60)) = 1;
    do {
      if (param_1 == 3) {
        iVar3 = (intptr_t)AsModel(iVar2)->Actions;
        fVar4 = *(float *)(iVar3 + 4 + iVar5) * 1.0f;
LAB_005099d1:
        *(float *)(iVar3 + 4 + iVar5) = fVar4;
      }
      else {
        if ((param_1 == 5) || (param_1 == 0x19)) {
          *(float *)((intptr_t)AsModel(iVar2)->Actions + 4 + iVar5) =
               *(float *)((intptr_t)AsModel(iVar2)->Actions + 4 + iVar5) * 0.7f;
        }
        if ((param_1 == 0x25) || (param_1 == 0x2a)) {
          iVar3 = (intptr_t)AsModel(iVar2)->Actions;
          fVar4 = *(float *)(iVar3 + 4 + iVar5) * 0.4f;
          goto LAB_005099d1;
        }
      }
      iVar5 = iVar5 + (int)sizeof(ModelAnimationAction);
    } while (iVar5 < 6 * (int)sizeof(ModelAnimationAction));
    switch(param_1) {
    case 2:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3f333333;
      break;
    case 6:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3f19999a;
      break;
    case 8:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3f333333;
      break;
    case 9:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3f99999a;
      break;
    case 10:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3e8f5c29;
      break;
    case 0xc:
    case 0x1c:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3e99999a;
      break;
    case 0xd:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3e8f5c29;
      break;
    case 0x11:
    case 0x15:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3f000000;
      break;
    case 0x13:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3f19999a;
      break;
    case 0x14:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3ecccccd;
      break;
    case 0x27:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3e6147ae;
      break;
    case 0x29:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x24)) = 0x3e3851ec;
      break;
    case 0x2a:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x34)) = 0x3eb33333;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x44)) = 0x3eb33333;
      break;
    case 0x2c:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e99999a;
    }
    switch(param_1) {
    case 0xe:
    case 0x1f:
    case 0x27:
    case 0x28:
      *(undefined1 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x10)) = 1;
    }
    switch(param_1) {
    case 0:
    case 0x1e:
      LoadWaveFile(0xaa, "Data\\Sound\\mBull1.wav",2,'\x01');
      LoadWaveFile(0xab, "Data\\Sound\\mBull2.wav",2,'\x01');
      LoadWaveFile(0xac, "Data\\Sound\\mBullAttack1.wav",2,'\x01');
      LoadWaveFile(0xad, "Data\\Sound\\mBullAttack2.wav",2,'\x01');
      LoadWaveFile(0xae, "Data\\Sound\\mBullDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0,1,2,3,4);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 270 + 0x54 + BMD_FIELD_SHIFT + param_1 * BMD_STRIDE) = 0x14;
      return;
    case 1:
      LoadWaveFile(0xaf, "Data\\Sound\\mHound1.wav",2,'\x01');
      LoadWaveFile(0xb0, "Data\\Sound\\mHound2.wav",2,'\x01');
      LoadWaveFile(0xb1, "Data\\Sound\\mHoundAttack1.wav",2,'\x01');
      LoadWaveFile(0xb2, "Data\\Sound\\mHoundAttack2.wav",2,'\x01');
      LoadWaveFile(0xb3, "Data\\Sound\\mHoundDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,5,6,7,8,9);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 271 + 0x54 + BMD_FIELD_SHIFT) = 5;
      return;
    case 2:
      LoadWaveFile(0xb4, "Data\\Sound\\mBudge1.wav",2,'\x01');
      LoadWaveFile(0xb5, "Data\\Sound\\mBudgeAttack1.wav",2,'\x01');
      LoadWaveFile(0xb6, "Data\\Sound\\mBudgeDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,10,0xb,0xb,0xb,0xc);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 272 + 0x54 + BMD_FIELD_SHIFT) = 7;
      return;
    case 3:
    case 0x1d:
    case 0x27:
      LoadWaveFile(0xb9, "Data\\Sound\\mDarkKnight1.wav",2,'\x01');
      LoadWaveFile(0xba, "Data\\Sound\\mDarkKnight2.wav",2,'\x01');
      LoadWaveFile(0xbb, "Data\\Sound\\mDarkKnightAttack1.wav",2,'\x01');
      LoadWaveFile(0xbc, "Data\\Sound\\mDarkKnightAttack2.wav",2,'\x01');
      LoadWaveFile(0xbd, "Data\\Sound\\mDarkKnightDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0xf,0x10,0x11,0x12,0x13);
      if (param_1 == 3) {
        *(undefined4 *)(ModelsBase + BMD_STRIDE * 273 + 0x54 + BMD_FIELD_SHIFT) = 0x10;
        return;
      }
      if (param_1 == 0x1d) {
        *(undefined4 *)(ModelsBase + BMD_STRIDE * 299 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
        return;
      }
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 270 + 0x54 + BMD_FIELD_SHIFT + param_1 * BMD_STRIDE) = 0x13;
      return;
    case 4:
      LoadWaveFile(0xbe, "Data\\Sound\\mWizard1.wav",2,'\x01');
      LoadWaveFile(0xbf, "Data\\Sound\\mWizard2.wav",2,'\x01');
      LoadWaveFile(0xc0, "Data\\Sound\\mWizardAttack1.wav",2,'\x01');
      LoadWaveFile(0xc1, "Data\\Sound\\mWizardAttack2.wav",2,'\x01');
      LoadWaveFile(0xc2, "Data\\Sound\\mWizardDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x14,0x15,0x16,0x17,0x18);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 274 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 5:
      LoadWaveFile(0xc3, "Data\\Sound\\mGiant1.wav",2,'\x01');
      LoadWaveFile(0xc4, "Data\\Sound\\mGiant2.wav",2,'\x01');
      LoadWaveFile(0xc5, "Data\\Sound\\mGiantAttack1.wav",2,'\x01');
      LoadWaveFile(0xc6, "Data\\Sound\\mGiantAttack2.wav",2,'\x01');
      LoadWaveFile(199, "Data\\Sound\\mGiantDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x19,0x1a,0x1b,0x1c,0x1d);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 275 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 6:
      LoadWaveFile(200, "Data\\Sound\\mLarva1.wav",2,'\x01');
      LoadWaveFile(0xc9, "Data\\Sound\\mLarva2.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x1e,0x1f,0x1e,0x1f,0x1f);
      return;
    case 7:
      LoadWaveFile(0xcd, "Data\\Sound\\mGhost1.wav",2,'\x01');
      LoadWaveFile(0xce, "Data\\Sound\\mGhost2.wav",2,'\x01');
      LoadWaveFile(0xcf, "Data\\Sound\\mGhostAttack1.wav",2,'\x01');
      LoadWaveFile(0xd0, "Data\\Sound\\mGhostAttack2.wav",2,'\x01');
      LoadWaveFile(0xd1, "Data\\Sound\\mGhostDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x23,0x24,0x25,0x26,0x27);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 277 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 8:
      LoadWaveFile(0xca, "Data\\Sound\\mHellSpider1.wav",2,'\x01');
      LoadWaveFile(0xcb, "Data\\Sound\\mHellSpiderAttack1.wav",2,'\x01');
      LoadWaveFile(0xcc, "Data\\Sound\\mHellSpiderDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x20,0x21,0x21,0x21,0x22);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 278 + 0x54 + BMD_FIELD_SHIFT) = 0x10;
      return;
    case 9:
      LoadWaveFile(0xb7, "Data\\Sound\\mSpider1.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0xd,0xd,0xd,0xd,0xd);
      return;
    case 10:
      LoadWaveFile(0xd2, "Data\\Sound\\mOgre1.wav",2,'\x01');
      LoadWaveFile(0xd3, "Data\\Sound\\mOgre2.wav",2,'\x01');
      LoadWaveFile(0xd4, "Data\\Sound\\mOgreAttack1.wav",2,'\x01');
      LoadWaveFile(0xd5, "Data\\Sound\\mOgreAttack2.wav",2,'\x01');
      LoadWaveFile(0xd6, "Data\\Sound\\mOgreDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x28,0x29,0x2a,0x2b,0x2c);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 280 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 0xb:
      LoadWaveFile(0xd7, "Data\\Sound\\mGorgon1.wav",2,'\x01');
      LoadWaveFile(0xd8, "Data\\Sound\\mGorgon2.wav",2,'\x01');
      LoadWaveFile(0xd9, "Data\\Sound\\mGorgonAttack1.wav",2,'\x01');
      LoadWaveFile(0xda, "Data\\Sound\\mGorgonAttack2.wav",2,'\x01');
      LoadWaveFile(0xdb, "Data\\Sound\\mGorgonDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x2d,0x2e,0x2f,0x30,0x31);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 281 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 0xc:
    case 0xd:
      LoadWaveFile(0xee, "Data\\Sound\\mYeti1.wav",2,'\x01');
      LoadWaveFile(0xef, "Data\\Sound\\mYeti2.wav",2,'\x01');
      LoadWaveFile(0xf0, "Data\\Sound\\mYetiAttack1.wav",2,'\x01');
      LoadWaveFile(0xf1, "Data\\Sound\\mYetiDie.wav",2,'\x01');
      Model_SetAnimationSlots(0x11a,0x44,0x44,0x46,0x46,0x47);
      Model_SetAnimationSlots(0x11b,0x44,0x45,0x46,0x46,0x47);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 282 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 283 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 0xe:
      LoadWaveFile(0xeb, "Data\\Sound\\mAssassinAttack1.wav",2,'\x01');
      LoadWaveFile(0xec, "Data\\Sound\\mAssassinAttack2.wav",2,'\x01');
      LoadWaveFile(0xed, "Data\\Sound\\mAssassinDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0xffff,0xffff,0x41,0x42,0x43);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 284 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x14)) = 0x3eb33333;
      return;
    case 0xf:
      LoadWaveFile(0xdc, "Data\\Sound\\mIceMonster1.wav",2,'\x01');
      LoadWaveFile(0xdd, "Data\\Sound\\mIceMonster2.wav",2,'\x01');
      LoadWaveFile(0xde, "Data\\Sound\\mIceMonsterDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x32,0x33,0x32,0x32,0x34);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 285 + 0x54 + BMD_FIELD_SHIFT) = 0x13;
      return;
    case 0x10:
      LoadWaveFile(0xe2, "Data\\Sound\\mHomord1.wav",2,'\x01');
      LoadWaveFile(0xe3, "Data\\Sound\\mHomord2.wav",2,'\x01');
      LoadWaveFile(0xe4, "Data\\Sound\\mHomordAttack1.wav",2,'\x01');
      LoadWaveFile(0xe5, "Data\\Sound\\mHomordDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x38,0x39,0x3a,0x3a,0x3b);
      return;
    case 0x11:
      LoadWaveFile(0xdf, "Data\\Sound\\mWorm1.wav",2,'\x01');
      LoadWaveFile(0xe0, "Data\\Sound\\mWorm2.wav",2,'\x01');
      LoadWaveFile(0xe1, "Data\\Sound\\mWormDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x35,0x35,0x37,0x37,0x37);
      return;
    case 0x12:
      LoadWaveFile(0xe6, "Data\\Sound\\mIceQueen1.wav",2,'\x01');
      LoadWaveFile(0xe7, "Data\\Sound\\mIceQueen2.wav",2,'\x01');
      LoadWaveFile(0xe8, "Data\\Sound\\mIceQueenAttack1.wav",2,'\x01');
      LoadWaveFile(0xe9, "Data\\Sound\\mIceQueenAttack2.wav",2,'\x01');
      LoadWaveFile(0xea, "Data\\Sound\\mIceQueenDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x3c,0x3d,0x3e,0x3f,0x40);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 288 + 0x54 + BMD_FIELD_SHIFT) = 0x10;
      return;
    case 0x13:
      LoadWaveFile(0xf2, "Data\\Sound\\mGoblin1.wav",2,'\x01');
      LoadWaveFile(0xf3, "Data\\Sound\\mGoblin2.wav",2,'\x01');
      LoadWaveFile(0xf4, "Data\\Sound\\mGoblinAttack1.wav",2,'\x01');
      LoadWaveFile(0xf5, "Data\\Sound\\mGoblinAttack2.wav",2,'\x01');
      LoadWaveFile(0xf6, "Data\\Sound\\mGoblinDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x48,0x49,0x4a,0x4b,0x4c);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 289 + 0x54 + BMD_FIELD_SHIFT) = 6;
      return;
    case 0x14:
      LoadWaveFile(0xf7, "Data\\Sound\\mScorpion1.wav",2,'\x01');
      LoadWaveFile(0xf8, "Data\\Sound\\mScorpion2.wav",2,'\x01');
      LoadWaveFile(0xf9, "Data\\Sound\\mScorpionAttack1.wav",2,'\x01');
      LoadWaveFile(0xfa, "Data\\Sound\\mScorpionAttack2.wav",2,'\x01');
      LoadWaveFile(0xfb, "Data\\Sound\\mScorpionDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x4d,0x4e,0x4f,0x50,0x51);
      return;
    case 0x15:
      LoadWaveFile(0xfc, "Data\\Sound\\mBeetle1.wav",2,'\x01');
      LoadWaveFile(0xfd, "Data\\Sound\\mBeetleAttack1.wav",2,'\x01');
      LoadWaveFile(0xfe, "Data\\Sound\\mBeetleDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x52,0x52,0x53,0x53,0x54);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 291 + 0x54 + BMD_FIELD_SHIFT) = 5;
      return;
    case 0x16:
      LoadWaveFile(0xff, "Data\\Sound\\mHunter1.wav",2,'\x01');
      LoadWaveFile(0x100, "Data\\Sound\\mHunter2.wav",2,'\x01');
      LoadWaveFile(0x101, "Data\\Sound\\mHunterAttack1.wav",2,'\x01');
      LoadWaveFile(0x102, "Data\\Sound\\mHunterAttack2.wav",2,'\x01');
      LoadWaveFile(0x103, "Data\\Sound\\mHunterDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x55,0x56,0x57,0x58,0x59);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 292 + 0x54 + BMD_FIELD_SHIFT) = 6;
      return;
    case 0x17:
      LoadWaveFile(0x104, "Data\\Sound\\mWoodMon1.wav",2,'\x01');
      LoadWaveFile(0x105, "Data\\Sound\\mWoodMon2.wav",2,'\x01');
      LoadWaveFile(0x106, "Data\\Sound\\mWoodMonAttack1.wav",2,'\x01');
      LoadWaveFile(0x107, "Data\\Sound\\mWoodMonAttack2.wav",2,'\x01');
      LoadWaveFile(0x108, "Data\\Sound\\mWoodMonDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x5a,0x5b,0x5c,0x5d,0x5e);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 293 + 0x54 + BMD_FIELD_SHIFT) = 6;
      return;
    case 0x18:
      LoadWaveFile(0x109, "Data\\Sound\\mArgon1.wav",2,'\x01');
      LoadWaveFile(0x10a, "Data\\Sound\\mArgon2.wav",2,'\x01');
      LoadWaveFile(0x10b, "Data\\Sound\\mArgonAttack1.wav",2,'\x01');
      LoadWaveFile(0x10c, "Data\\Sound\\mArgonAttack2.wav",2,'\x01');
      LoadWaveFile(0x10d, "Data\\Sound\\mArgonDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x5f,0x60,0x61,0x62,99);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 294 + 0x54 + BMD_FIELD_SHIFT) = 0x10;
      return;
    case 0x19:
      LoadWaveFile(0x10e, "Data\\Sound\\mGolem1.wav",2,'\x01');
      LoadWaveFile(0x10f, "Data\\Sound\\mGolem2.wav",2,'\x01');
      LoadWaveFile(0x110, "Data\\Sound\\mGolemAttack1.wav",2,'\x01');
      LoadWaveFile(0x111, "Data\\Sound\\mGolemAttack2.wav",2,'\x01');
      LoadWaveFile(0x112, "Data\\Sound\\mGolemDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,100,0x65,0x66,0x67,0x68);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 295 + 0x54 + BMD_FIELD_SHIFT) = 5;
      return;
    case 0x1a:
      LoadWaveFile(0x113, "Data\\Sound\\mYeti1.wav",2,'\x01');
      LoadWaveFile(0x114, "Data\\Sound\\mSatanAttack1.wav",2,'\x01');
      LoadWaveFile(0x115, "Data\\Sound\\mYetiDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x69,0x69,0x6a,0x6a,0x6b);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 296 + 0x54 + BMD_FIELD_SHIFT) = 6;
      return;
    case 0x1b:
      LoadWaveFile(0x116, "Data\\Sound\\mBalrog1.wav",2,'\x01');
      LoadWaveFile(0x117, "Data\\Sound\\mBalrog2.wav",2,'\x01');
      LoadWaveFile(0x118, "Data\\Sound\\mWizardAttack2.wav",2,'\x01');
      LoadWaveFile(0x119, "Data\\Sound\\mGorgonAttack2.wav",2,'\x01');
      LoadWaveFile(0x11a, "Data\\Sound\\mBalrogDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x6c,0x6d,0x6e,0x6f,0x70);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 297 + 0x54 + BMD_FIELD_SHIFT) = 6;
      *(undefined1 *)(ModelsBase + BMD_STRIDE * 297 + 0x88 + BMD_FIELD_SHIFT) = 1;
      return;
    case 0x1c:
      LoadWaveFile(0x11b, "Data\\Sound\\mShadow1.wav",2,'\x01');
      LoadWaveFile(0x11c, "Data\\Sound\\mShadow2.wav",2,'\x01');
      LoadWaveFile(0x11d, "Data\\Sound\\mShadowAttack1.wav",2,'\x01');
      LoadWaveFile(0x11e, "Data\\Sound\\mShadowAttack2.wav",2,'\x01');
      LoadWaveFile(0x11f, "Data\\Sound\\mShadowDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x71,0x72,0x73,0x74,0x75);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 298 + 0x54 + BMD_FIELD_SHIFT) = 5;
      return;
    case 0x1f:
      LoadWaveFile(0x125, "Data\\Sound\\mYeti1.wav",2,'\x01');
      LoadWaveFile(0x126, "Data\\Sound\\mBullAttack1.wav",2,'\x01');
      LoadWaveFile(0x127, "Data\\Sound\\mYetiDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x7b,0x7b,0x7c,0x7c,0x7d);
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x34)) = 0x3f000000;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x44)) = 0x3f333333;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x14)) = 0x3f4ccccd;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x74)) = 0x3f4ccccd;
      return;
    case 0x20:
      LoadWaveFile(0x128, "Data\\Sound\\mBali1.wav",2,'\x01');
      LoadWaveFile(0x129, "Data\\Sound\\mBali2.wav",2,'\x01');
      LoadWaveFile(0x12a, "Data\\Sound\\mBaliAttack1.wav",2,'\x01');
      LoadWaveFile(299, "Data\\Sound\\mBaliAttack2.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x7e,0x7f,0x80,0x81,0x7f);
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x84)) = 0x3ecccccd;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x94)) = 0x3ecccccd;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x74)) = 0x3ecccccd;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0xa4)) = 0x3ecccccd;
      AsModel(iVar2)->BoneHead = 6;
      return;
    case 0x21:
      LoadWaveFile(300, "Data\\Sound\\mBahamut1.wav",2,'\x01');
      LoadWaveFile(0x12d, "Data\\Sound\\mYeti1.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x82,0x82,0x83,0x83,0x82);
      return;
    case 0x22:
      LoadWaveFile(0x12e, "Data\\Sound\\mBepar1.wav",2,'\x01');
      LoadWaveFile(0x12f, "Data\\Sound\\mBepar2.wav",2,'\x01');
      LoadWaveFile(0x130, "Data\\Sound\\mBalrog1.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x84,0x85,0x68,0x68,0x85);
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x34)) = 0x3f000000;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x44)) = 0x3f000000;
      AsModel(iVar2)->BoneHead = 0x14;
      return;
    case 0x23:
      LoadWaveFile(0x131, "Data\\Sound\\mValkyrie1.wav",2,'\x01');
      LoadWaveFile(0x132, "Data\\Sound\\mBaliAttack2.wav",2,'\x01');
      LoadWaveFile(0x133, "Data\\Sound\\mValkyrieDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x87,0x87,0x88,0x88,0x89);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 305 + 0x54 + BMD_FIELD_SHIFT) = 0x13;
      return;
    case 0x24:
    case 0x28:
      LoadWaveFile(0x134, "Data\\Sound\\mLizardKing1.wav",2,'\x01');
      LoadWaveFile(0x135, "Data\\Sound\\mLizardKing2.wav",2,'\x01');
      LoadWaveFile(0x136, "Data\\Sound\\mGorgonDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x8a,0x8b,0x8a,0x8b,0x8c);
      if (param_1 == 0x24) {
        *(undefined4 *)(ModelsBase + BMD_STRIDE * 306 + 0x54 + BMD_FIELD_SHIFT) = 0x13;
        return;
      }
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 270 + 0x54 + BMD_FIELD_SHIFT + param_1 * BMD_STRIDE) = 6;
      return;
    case 0x25:
      LoadWaveFile(0x137, "Data\\Sound\\mHydra1.wav",2,'\x01');
      LoadWaveFile(0x138, "Data\\Sound\\mHydraAttack1.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0x8d,0x8d,0x8e,0x8e,0x8d);
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x34)) = 0x3e19999a;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(0x44)) = 0x3e19999a;
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e4ccccd;
      return;
    case 0x29:
      LoadWaveFile(0x139,Wav_Iron1,2,'\x01');
      LoadWaveFile(0x13a,Wav_IronAttack1,2,'\x01');
      Model_SetAnimationSlots(iVar1,0x8f,0x8f,0x90,0x90,0x90);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 311 + 0x54 + BMD_FIELD_SHIFT) = 3;
      return;
    case 0x2a:
      LoadWaveFile(0x13b,Wav_Jaikan1,2,'\x01');
      LoadWaveFile(0x13c,Wav_Jaikan2,2,'\x01');
      LoadWaveFile(0x13d,Wav_JaikanAttack1,2,'\x01');
      LoadWaveFile(0x13e,Wav_JaikanAttack2,2,'\x01');
      LoadWaveFile(0x13f,Wav_JaikanDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0x91,0x92,0x93,0x94,0x95);
      OpenJPG(Img_MonsterBv01_2,0x56b,0x2601,0x2901,0,'\x01');
      OpenJPG(Img_MonsterBv02_2,0x56c,0x2601,0x2901,0,'\x01');
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 312 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 0x2b:
      LoadWaveFile(0x140,Wav_Blood1,2,'\x01');
      LoadWaveFile(0x141,Wav_BloodAttack1,2,'\x01');
      LoadWaveFile(0x142,Wav_BloodAttack2,2,'\x01');
      LoadWaveFile(0x143,Wav_BloodDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0x96,0x96,0x97,0x98,0x99);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 313 + 0x54 + BMD_FIELD_SHIFT) = 7;
      return;
    case 0x2c:
      LoadWaveFile(0x144,Wav_Death1,2,'\x01');
      LoadWaveFile(0x145,Wav_DeathAttack1,2,'\x01');
      LoadWaveFile(0x146,Wav_DeathDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0x9a,0x9a,0x9b,0x9b,0x9c);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 314 + 0x54 + BMD_FIELD_SHIFT) = 6;
      return;
    case 0x2d:
      LoadWaveFile(0x147,Wav_Mutant1,2,'\x01');
      LoadWaveFile(0x148,Wav_Mutant2,2,'\x01');
      LoadWaveFile(0x149,Wav_MutantAttack1,2,'\x01');
      Model_SetAnimationSlots(iVar1,0x9d,0x9e,0x9f,0x9f,0x9f);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 315 + 0x54 + BMD_FIELD_SHIFT) = 6;
      return;
    case 0x2e:
      LoadWaveFile(0x14c,Wav_OrcArcherAttack1,2,'\x01');
      LoadWaveFile(0xae, "Data\\Sound\\mBullDie.wav",2,'\x01');
      Model_SetAnimationSlots(iVar1,0xffff,0xffff,0xa2,0xa2,4);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 316 + 0x54 + BMD_FIELD_SHIFT) = 7;
      return;
    case 0x2f:
      LoadWaveFile(0x100, "Data\\Sound\\mHunter2.wav",2,'\x01');
      LoadWaveFile(0xae, "Data\\Sound\\mBullDie.wav",2,'\x01');
      LoadWaveFile(0x14b,Wav_OrcCapAttack1,2,'\x01');
      Model_SetAnimationSlots(iVar1,0x56,0x56,0xa1,0xa1,4);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 317 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 0x30:
      LoadWaveFile(0x14a,Wav_CursedKing1,2,'\x01');
      LoadWaveFile(0x14e,Wav_CursedKing2,2,'\x01');
      LoadWaveFile(0x14d,Wav_CursedKingDie1,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xa0,0xa4,0xffff,0xffff,0xa3);
      *(undefined4 *)(ModelsBase + BMD_STRIDE * 318 + 0x54 + BMD_FIELD_SHIFT) = 0x14;
      return;
    case 0x31:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e6147ae;
      LoadWaveFile(0x15b,Wav_Molt1,2,'\x01');
      LoadWaveFile(0x15c,Wav_MoltAttack1,2,'\x01');
      LoadWaveFile(0x15d,Wav_MoltDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xb1,0xb1,0xb2,0xb2,0xb3);
      return;
    case 0x32:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e6147ae;
      LoadWaveFile(0x159,Wav_AlquamosAttack1,2,'\x01');
      LoadWaveFile(0x15a,Wav_AlquamosDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xaf,0xaf,0xaf,0xaf,0xb0);
      return;
    case 0x33:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e6147ae;
      LoadWaveFile(0x155,Wav_Rainner1,2,'\x01');
      LoadWaveFile(0x156,Wav_RainnerAttack1,2,'\x01');
      LoadWaveFile(0x157,Wav_RainnerDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xab,0xffff,0xac,0xac,0xad);
      return;
    case 0x34:
      OpenTGA(Img_MonsterIui02,0x1ed,0x2600,0x2900,0,'\x01');
      OpenTGA(Img_MonsterIui03,0x1ef,0x2600,0x2900,0,'\x01');
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e6147ae;
      LoadWaveFile(0x15e,Wav_MegaCrust1,2,'\x01');
      LoadWaveFile(0x15f,Wav_MegaCrustAttack1,2,'\x01');
      LoadWaveFile(0x160,Wav_MegaCrustDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xb4,0xb4,0xb5,0xb5,0xb6);
      return;
    case 0x35:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e6147ae;
      LoadWaveFile(0x152,Wav_Phantom1,2,'\x01');
      LoadWaveFile(0x153,Wav_PhantomAttack1,2,'\x01');
      LoadWaveFile(0x154,Wav_PhantomDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xa8,0xa8,0xa9,0xa9,0xaa);
      return;
    case 0x36:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e6147ae;
      LoadWaveFile(0x14f,Wav_Drakan1,2,'\x01');
      LoadWaveFile(0x150,Wav_DrakanAttack1,2,'\x01');
      LoadWaveFile(0x151,Wav_DrakanDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xa5,0xa5,0xa6,0xa6,0xa7);
      return;
    case 0x37:
      LoadWaveFile(0x161,Wav_Phoenix1,2,'\x01');
      LoadWaveFile(0x162,Wav_Phoenix1,2,'\x01');
      LoadWaveFile(0x163,Wav_PhoenixAttack1,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xb7,0xb8,0xb9,0xb9,0xffff);
    case 0x38:
      *(undefined4 *)((intptr_t)AsModel(iVar2)->Actions + ModelActionByte(100)) = 0x3e6147ae;
      return;
    case 0x39:
      LoadWaveFile(0x16a,Wav_RedSkull,2,'\x01');
      LoadWaveFile(0x16b,Wav_RedSkullDie,2,'\x01');
      LoadWaveFile(0x16c,Wav_RedSkullAttack,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xc0,0xffff,0xc2,0xffff,0xc1);
      break;
    case 0x3a:
      LoadWaveFile(0x166, "Data\\Sound\\mHunter2.wav",2,'\x01');
      LoadWaveFile(0x169,Wav_GhaintOrgerDie,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xbc,0xffff,0xbe,0xffff,0xbf);
      return;
    case 0x3b:
      LoadWaveFile(0xae,Wav_BullDie,2,'\x01');
      LoadWaveFile(0x166, "Data\\Sound\\mHunter2.wav",2,'\x01');
      LoadWaveFile(0x167,Wav_BlackSkullDie,2,'\x01');
      LoadWaveFile(0x168,Wav_BlackSkullAttack,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xbc,0xffff,0xbe,0xffff,0xbd);
      return;
    case 0x3e:
      LoadWaveFile(0x164,Wav_MagicSkull,2,'\x01');
      LoadWaveFile(0x165,Wav_MagicSkull,2,'\x01');
      Model_SetAnimationSlots(iVar1,0xba,0xffff,0xffff,0xffff,0xbb);
      return;
    }
  }
  return;
}
