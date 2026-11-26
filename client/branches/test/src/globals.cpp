#include <cstdint>
// globals.cpp — Definitions for all DAT_ global variables.
// In the original binary these are fixed addresses; here they are regular
// zero-initialized globals that the linker places wherever it chooses.
// To test as an overlay patch (DLL injection) these definitions would be
// replaced by pointer aliases to the original binary's addresses.

#include "stdafx.h"
#include "Render/ResourceTables.h"
#include "Platform/ISocket.h"
#include "Platform/ITimer.h"

// ── Named globals: defined in WinMain.cpp, only extern-declared here ─────────
// int SceneFlag — ver WinMain.cpp. g_hWnd, g_hInst y g_hDC viven en CWindow (Core/Window.h).

// ── Low-address constants ─────────────────────────────────────────────────────
float    GuardPageScratch_10 = 0.0f;
float    GuardPageScratch_14 = 0.0f;
float    GuardPageScratch_18 = 0.0f;
float    GuardPageScratch_1C = 0.0f;
float    GuardPageScratch_20 = 0.0f;
float    GuardPageScratch_24 = 0.0f;

// ── Game math / render constants ─────────────────────────────────────────────
// Kept float-literal storage: these 10 are referenced by ADDRESS (&) in tunable
// tables (Render/MoveEffect.cpp, Render/Particle_Spawn.cpp), so they cannot be
// inlined; all other FloatConst literals were replaced by their values.
float    ImpulseDriftVertB = 0.1f;
float    ImpulseDriftVertA = 0.2f;
float    FadeAlphaFactor = 0.333333343f;
float    ImpulseDriftHorizA = 0.4f;
float    ImpulseScaleBaseB = 1.0f;
float    FragmentGravityA = 3.0f;
float    FragmentGravityB = 4.0f;
float    PuffDriftFactor = -0.1f;
float    SmokeDriftFactor = -0.3f;
float    FragmentBounceKick = -0.5f;
float    GridToWorldScale = 100.0f;  // grid-to-world scale = TERRAIN_SCALE
DWORD    GridToWorldScaleBits  = 0;
float    SinFreqWorldTime = 0.001f;
DWORD    SinFreqWorldTimeBits  = 0;
float    HalfScale = 0.5f;
DWORD    HalfScaleBits  = 0;
float    ClothGridUvScale = 0.399999976f;
DWORD    ClothGridUvScaleBits  = 0;
float    PartyHpRowHeight = 19.0f;   // Party HP bar row height (SecondPassword_HoverCheck)
float    SpringEdgeThreshold = 0.0f;
DWORD    SpringEdgeThresholdBits  = 0;
DWORD    FloatZeroBBits  = 0;
float    EffectScaleUnit = 0.01f;
DWORD    EffectScaleUnitBits  = 0;
// ConvertYScale = 1/480 (Y scale), ConvertXScale = 1/640 (X scale) — compile-time constants.
// ConvertX(x) = WindowWidth * x * ConvertXScale ; ConvertY(y) = WindowHeight * y * ConvertYScale
// Used by GL_DrawTexture (RenderBitmap), Screen_ToGLX/80 (ConvertX/Y), Camera_SetupFrustum, etc.
float    ConvertYScale = 1.0f / 480.0f;
DWORD    ConvertYScaleBits  = 0x3B088889;   // 1/480 as DWORD bits
float    ConvertXScale = 1.0f / 640.0f;
DWORD    ConvertXScaleBits  = 0x3ACCCCCD;   // 1/640 as DWORD bits (float bits)
DWORD    WeatherMaxAltitudeBits  = 0;
float    FloatEpsilon = 0.0001f;  // epsilon for near-zero float checks
DWORD    FloatEpsilonBits  = 0x38D1B717;  // 0.0001f as DWORD bits
DWORD    AngleDeg360Bits  = 0x43B40000;  // 360.0f as DWORD bits
DWORD    AlphaFadeStepBits  = 0;
float   CatmullWeightA  = 0.5625f;
float   CatmullWeightB  = 0.8671875f;
float   CatmullWeightC  = 0.0234375f;
float   CatmullWeightD  = 0.2265625f;
float   DeltaTimeScale  = 0.001f;
float    DegToRad2 = 0.0174532942f;
DWORD    DegToRad2Bits  = 0;
DWORD    SpriteSheetColStepBits  = 0;
float    ZBobFreq = 0.002f;
DWORD    ZBobFreqBits  = 0;
float    MoveJointZOffset = 110.0f;  // MoveJoint Z-offset constant
float    GemScaleStep = 0.03f;
DWORD    GemScaleStepBits  = 0;
float    SpinFreqTime = -0.0005f;
DWORD    SpinFreqTimeBits  = 0;
float    BmdAnimFreq = -0.0001f;
DWORD    BmdAnimFreqBits  = 0;
float    ParticleZOffset = 135.0f;
DWORD    ParticleZOffsetBits  = 0;
float    JointLerpStep = 0.1f;
DWORD    JointLerpStepBits  = 0;
float    WindRotScale = 0.005f;
DWORD    WindRotScaleBits  = 0;
float    BigSmokeSizeScale = 0.008f;
DWORD    BigSmokeSizeScaleBits  = 0;
float    AspectRatioScale = 0.75f;
DWORD    AspectRatioScaleBits  = 0;
DWORD    HeightNormScaleBits  = 0;
float   FacingDtMax  = 3600.0f;
float    ScrollWaveFreq = 0.0002f;
DWORD    ScrollWaveFreqBits  = 0;
float    ItemSpinSpeed = 0.45f;          // item spin speed
float    InvCursorBoundXMax = 285.0f;
DWORD    InvCursorBoundXMaxBits  = 0;
float    InvCursorBoundYMax = 175.0f;
DWORD    InvCursorBoundYMaxBits  = 0;
float    InvCursorBoundXMin = 245.0f;
DWORD    InvCursorBoundXMinBits  = 0;
DWORD    ObjectScaleFactor  = 0;
float    HalfDegToRad = 0.0087266462f;  // 0.5*PI/180 — verificado bit-pattern 0x3C0EFA33 en binario original. Se usa para tan(FOV/2) en gluPerspective2 (GL_SetPerspective) y frustum (Camera_SetupFrustum).
DWORD    HalfDegToRadBits  = 0;
// World → object-bucket grid factor (1/1600; celda = 1600 unidades de mundo)
float    WorldToBucketGrid = 6.25e-4f;   // 0.000625 = 1/1600
DWORD    WorldToBucketGridBits  = 0;
float    IntroLogoOffsetY = 97.5f;
DWORD    IntroLogoOffsetYBits  = 0;
float    IntroLogoFactorY = 0.375f;
DWORD    IntroLogoFactorYBits  = 0;
float    ServerSelectX = 248.0f;
DWORD    ServerSelectXBits  = 0;
float    ServerSelectRowStep = 14.0f;
DWORD    ServerSelectRowStepBits  = 0;
DWORD    GameGuardOkStatus  = 0;

// ── Entity / render constants ─────────────────────────────────────────────────
// 16-byte XOR key table — usado por Crypto.cpp/CreateEffect.cpp/Net_PacketSession etc.
// Indexado como (&LegacyPacketXorKey16)[i&0xf] o LegacyPacketXorKey16[i%16] (nombres de Ghidra).
BYTE     PacketXorKey16[16] = {0}; // LegacyPacketXorKey16 (nombre de Ghidra)
float    ClothGravityScalar = 400.0f;  // Verlet physics damping/gravity scalar
DWORD    ClothGravityScalarBits  = 0;
// g_bUseChatListBox. Default IDA = 1 (verificado: bytes en 0x5590ac
// = 01 00 00 00, seguidos de flt_5590B0/B4/B8 = 295/417/18 coords del input dialog).
// Con =1, sub_480980 corre in-world → mensajes de sistema/GM salen ARRIBA-IZQUIERDA
// (no en el área del ChatListBox abajo). Fiel a IDA. El doble render se evita con
// el skip de mode 1/2 en ChatLB_renderLine: no bajarlo a 0 para taparlo.
// IDA: g_bUseChatListBox (0x005590AC)
DWORD    g_bUseChatListBox  = 1; // IDA: g_bUseChatListBox (0x005590AC)
// flt_5590B0 / flt_5590B4 / flt_5590B8 — layout de los 3 botones popup del
// ChatListBox (ver-chat / tamaño-historial / transparencia).
// Valores LEÍDOS DEL BINARIO en 0x5590B0 (12 bytes: 00 80 93 43 | 00 80 d0 43 |
// 00 00 90 41) → 295.0f, 417.0f, 18.0f.  Los usan sub_40E400 (hit-test, slot 26)
// y sub_40DEF0 (render, slot 24 vía el thunk sub_40D600).
// IDA: DAT_005590B0
float    ChatListBox_TabButtonsX  = 295.0f;   // X del primer botón
// IDA: DAT_005590B4
float    ChatListBox_TabButtonsY  = 417.0f;   // Y de los tres
// IDA: DAT_005590B8
float    ChatListBox_TabButtonSpacing  = 18.0f; // separación horizontal entre botones
// Version (5 bytes) @ 0x0055961c: obfuscated as Version[i]-i-1 in login packet.
//
// HISTORICAL VALUE from original main.exe (MD5 eb95ac0785e40a7ad60c9ddb5d8bef34):
//   { 0x31, 0x3B, 0x3A, 0x35, 0x35 }  // unobfuscates to "09710"
//
// In the original distribution model the InfoEncoder.exe tool re-patches
// main.exe at this offset to match whatever ClientVersion is set in
// Encoder/MainInfo.ini.  Since we now build from source, we bake the value
// directly here.  The current MuEmu reference server
// (MuServer/GameServer/DATA/GameServerInfo - StartUp.dat: ServerVersion=0.97.11)
// expects "09711"; bytes are computed as target_char + (i + 1) so that
// (DAT[i] - i - 1) = "09711":
//     '0'+1=0x31, '9'+2=0x3B, '7'+3=0x3A, '1'+4=0x35, '1'+5=0x36
// IDA: Version (0x0055961C)
BYTE     Version[5]  = { 0x31, 0x3B, 0x3A, 0x35, 0x36 };
// Serial (16 bytes) @ 0x00559624: sent raw in login packet.
//
// HISTORICAL VALUE from original main.exe:
//   "b9xGyHABOa9m2NHY"
//
// Current value matches MuServer Encoder/MainInfo.ini ClientSerial=TbYehR2hFUPBKgZj.
// Server compares strict equality after CSM decode, so client and server must
// agree on this 16-byte string verbatim.
// IDA: Serial (0x00559624)
BYTE     Serial[16] = { 'T', 'b', 'Y', 'e', 'h', 'R', '2', 'h',
                              'F', 'U', 'P', 'B', 'K', 'g', 'Z', 'j' };
// IDA: PacketXorKey3 (0x00559678)
DWORD    PacketXorKey3  = 0; // IDA: PacketXorKey3 (0x00559678)
DWORD    CachedShopSlotIndex  = 0;
char     PathFinderDebugString  = 'p';               // PathFinder debug string sentinel
// IDA .data 0x5597C4 = 1. Con 1, Skeleton_Transform (0x4404E0) usa la luz del
// mundo (1.3, 0, 2). Con 0 usa la luz horizontal de la vista previa de crear
// personaje (0, -1.5, 0), que oscurece las caras que miran hacia arriba.
// Scene_CharSelect lo pone en 0 sólo para esa vista previa y lo vuelve a 1.
DWORD    RenderSubModeFlag  = 1;
float    EntityLodScale = 1.0f;
DWORD    EntityLodScaleBits  = 0;
DWORD    MonsterNameA  = 0;
DWORD    MonsterNameB  = 0;
DWORD    MonsterNameC  = 0;
// IDA: m_bBlockWhisper (0x00559BF0) -- interruptor de susurros (F3; icono 239
// del chat).  En .data vale 1: arrancan activados.
DWORD    WhisperMuteFlag  = 1;
// DAT_00559c4c/50 are defined below in the named hover/targeting section.
DWORD    m_dwTextColor  = 0xffffffff;
// 0x00559C7C — IDA `SetTextColor_0`: color del PREFIJO (nombre de guild) en la
// composición de burbujas de chat (`sub_47F360`).  Lo escriben `RenderBoolean`
// (3 sitios, según el sub-modo del mensaje) y `RenderPartyHP`.  Se define acá
// con el nombre de IDA; `LegacySetTextColor0` es un macro-alias en globals.h.
extern "C" { DWORD SetTextColor_0 = 0xffffffff; }
char     GmAdminNameFilterStr[64] = "";   // GM/admin name filter string (Entity_FindNearby)
DWORD    SetBackgroundTextColor  = 0;
DWORD    InputEnable  = 0; // IDA: InputEnable (0x00559C84)
DWORD    InputNumber  = 0;
DWORD    LegacyInputTextWidth  = 0;
DWORD    ActiveInputSlot  = 0;
// InputTextMax[] vive en 0x00559c94 en el binario original como UN array int
// contiguo (stride 4): `(&InputTextMax)[1]` es el slot del password. Por eso todos
// los alias apuntan dentro de este array y son `int&` (IDA: `int InputTextMax[8]`),
// nunca float ni globals sueltos.
int      _InputTextMaxArr[8] = {0,0,0,0,0,0,0,0};
int      InputTextMaxClusters[10] = {};
int&     InputTextMax = _InputTextMaxArr[0];
int&     InputTextMaxB = _InputTextMaxArr[1];
DWORD&   InputTextMaxBBits  = reinterpret_cast<DWORD&>(_InputTextMaxArr[1]);
DWORD    ChatHistoryIndex  = 0;
// 0x00559CCC/CD0/CD4 son m_iMatchTime / m_iMaxKillMonster / m_iKillMonster.
// Estaban declarados DOS veces (aca como DAT_ y mas abajo con su nombre), o
// sea eran memorias distintas.  Ahora los DAT_ son alias (globals.h).
DWORD    ImeOldSentMode  = 0;
DWORD    BuffNameFmt  = 0;
DWORD    BuffStatFmt2  = 0;
DWORD    BuffStatFmt3  = 0;
char     StoragePinOkFlag  = 0;
DWORD    HoveredSkillIndex  = 0xffffffff;  // hovered-skill index (-1 = none); IDA inits to -1
DWORD    ChaosMixInfo[4] = {0};       // chaos-mix info (ReceiveTalk sub 3 copia 4 bytes acá; 0x55A3E8..0x55A3F4)
int      EventType     = 0;           // 0/1 tipo de EventWindow (ReceiveTalk sub 4/6)
extern "C" { int g_bServerDivisionEnable = 0; int g_bServerDivisionAccept = 0; }  // ReceiveTalk sub 5
DWORD    FrustrumBoundMaxX_1  = 0;
DWORD    FrustrumBoundMaxY_1  = 0;
DWORD    FrustrumBoundMaxX_2  = 0;
DWORD    FrustrumBoundMaxY_2  = 0;
char     Fmt_FileNotFound[] = "%s file not found";
DWORD    TerrainFileSuffix  = 0;
char     DataPathPrefix[] = "Data\\";    // texture/asset path prefix ("Data mode")
char     Data2PathPrefix[] = "Data2\\";   // texture/asset path prefix ("Data2 / pak mode")
// IDA: World (0x0055A7AC)
// Estado del mapa actual: CMapManager::GetCurrentMap().
float    CharSelectResetScratch = 0.0f;

// ── Game loop / scene state ───────────────────────────────────────────────────
// Static camera defaults from PE .data @ 0x0056154c:
//   CameraViewNear = 20.0f  (0x0056154c)
//   CameraViewFar  = 2000.0f (0x00561550)
//   CameraFOV      = 55.0f  (0x00561554)
// These are DWORDs in the variable table but the binary reads them as floats.
DWORD    CameraViewNearStorage  = 0x41A00000;  // 20.0f    CameraViewNear
DWORD    CameraViewFarStorage  = 0x44FA0000;  // 2000.0f  CameraViewFar
DWORD    CameraFOVStorage  = 0x425C0000;  // 55.0f    CameraFOV (MoveMainCamera lo reescribe a 35.0)
float    CameraDistanceTarget  = 0.0f;  // DAT_005616B4: MoveMainCamera target
float    CameraDistance  = 0.0f;        // DAT_083A45D0: current MoveMainCamera distance
DWORD    BoundTextureSlotCache  = 0;
// Buffer de la IP del server: Config_ReadServerAddr lo llena desde Config/ServerConfig.h.
// szServerIpAddress apunta a este buffer; el valor inicial se reemplaza siempre.
char     g_ServerIPBuf[128] = "connect.muonline.co.kr";
// IDA: szServerIpAddress (0x005615B8)
char    *szServerIpAddress  = g_ServerIPBuf; // IDA: szServerIpAddress (0x005615B8)
// IDA: g_ServerPort (0x005615BC)
WORD     g_ServerPort  = 55901; // IDA: g_ServerPort (0x005615BC)

// ── ConnectServer flow ──────────────────────────────────────────
// Con ConnectServer en ServerConfig: szServerIpAddress/g_ServerPort = ConnectServer
// y gNetwork.HasConnectServer() activa el flujo original: conectar al CS → recibir lista+load (F4/04/F4/02) → al
// elegir server mandar F4/03 → redirect al GameServer → login.
// SceneFlag (above)
// g_lpszMp3 (IDA 0x005615C4, rutas de los BGM): ver CMapManager (Game/MapManager.cpp).
DWORD    DeleteCharSlotIdx  = 0;
DWORD    CurrentCameraCount  = 0;

// ──────────────────────────────────────────────────────────────────────────
//  CameraWalk[6][6] — static waypoint table from PE @ 0x005615ec..0x0056167b
//  Each waypoint is 6 floats: pos[3] + angle[3] (degrees).
//  Read by MoveCamera (0x0051E4E0) + Game_SceneUpdate login init.
//  MUST BE CONTIGUOUS — MoveCamera indexes CameraWalk[i*6+k] off the base
//  pointer. Defining the six DAT_ names at f0/f4/f8/fc/00/... as separate
//  DWORDs lets MSVC scatter them → table read returns garbage and the
//  login-scene camera fly-through never activates.
//  Values recovered from `main.exe` (MD5 eb95ac0...) .data section.
// ──────────────────────────────────────────────────────────────────────────
float CameraWalkTable[42] = {
    //  pos.x     pos.y     pos.z     ang.pitch  ang.yaw  ang.roll
    //  wp0: distant frontal
        0.0f,   -1000.0f,   500.0f,   -80.0f,    0.0f,     0.0f,
    //  wp1..wp4: closer frontal
        0.0f,   -1100.0f,   500.0f,   -80.0f,    0.0f,     0.0f,
        0.0f,   -1100.0f,   500.0f,   -80.0f,    0.0f,     0.0f,
        0.0f,   -1100.0f,   500.0f,   -80.0f,    0.0f,     0.0f,
        0.0f,   -1100.0f,   500.0f,   -80.0f,    0.0f,     0.0f,
    //  wp5: angled side (used by Scene_EnterWorld camera transition)
      200.0f,    -800.0f,   300.0f,   -80.0f,    0.0f,   -10.0f,
    //  wp6: angled side end (PE @0x0016167C) — completes the transition pan
      200.0f,    -900.0f,   300.0f,   -80.0f,    0.0f,   -10.0f,
};
// Legacy DAT_ names are references into the contiguous table.
DWORD& CameraWalk0PosX = *(DWORD*)&CameraWalkTable[0];   // wp0.pos.x
DWORD& CameraWalk0PosY = *(DWORD*)&CameraWalkTable[1];   // wp0.pos.y
DWORD& CameraWalk0PosZ = *(DWORD*)&CameraWalkTable[2];   // wp0.pos.z
DWORD& CameraWalk0Pitch = *(DWORD*)&CameraWalkTable[3];   // wp0.ang.pitch
DWORD& CameraWalk0Yaw = *(DWORD*)&CameraWalkTable[4];   // wp0.ang.yaw
DWORD& CameraWalk0Roll = *(DWORD*)&CameraWalkTable[5];   // wp0.ang.roll
DWORD& CameraWalk5PosX = *(DWORD*)&CameraWalkTable[30];  // wp5.pos.x
DWORD& CameraWalk5PosY = *(DWORD*)&CameraWalkTable[31];  // wp5.pos.y
DWORD& CameraWalk5PosZ = *(DWORD*)&CameraWalkTable[32];  // wp5.pos.z
DWORD& CameraWalk5Pitch = *(DWORD*)&CameraWalkTable[33];  // wp5.ang.pitch
DWORD& CameraWalk5Yaw = *(DWORD*)&CameraWalkTable[34];  // wp5.ang.yaw
DWORD& CameraWalk5Roll = *(DWORD*)&CameraWalkTable[35];  // wp5.ang.roll
DWORD    ServerSelectHi  = 0;
DWORD    ServerSelectLo  = 0;
DWORD    LoginErrorFlag  = 0;
// dialog Y / camera X — usados como SIGNED int en las animaciones de
// Game_SceneUpdate y Game_EnterWorldTick.  Si los declarás DWORD, la
// fórmula divide-by-3 (`(int)((long long)x * 0x55555555LL) >> 32`)
// overflowea con valores negativos → cX saltaba a 0x2AAAAAA4 → state
// machine oscilaba 0x18 ↔ 0x17 sin parar.
int      AnimatedDialogY  = 0;
int      AnimatedCameraX  = 0;
DWORD    SelectedHero  = 0; // IDA: SelectedHero (0x005616AC)
DWORD    SelectedCharConfirm  = 0;
DWORD    TimeRemain  = 0; // IDA: TimeRemain (0x005616B8)
DWORD    IntroDebugString  = 0;
char     CharSelectNameFormat[]  = "%s";  // Scene_CharSelect — name label (top, bold)
DWORD    TickFormatStub  = 0;
char     TextureExtJpgStr[8] = "OZJ";   // OpenJPG extension suffix (Data mode: .jpg→.OZJ)

// String literals (read-only — actual game strings come from the binary)
char     Img_LocalWebzenlogoJpg[]  = "Local/Webzenlogo.jpg";
char     Img_LocalEveryoneJpg[]    = "Local/Everyone.jpg";
char     Img_LocalLoading01Jpg[]   = "Local/Loading01.jpg";
char     Img_LocalLoading02Jpg[]   = "Local/Loading02.jpg";
char     Img_LocalLoading03Jpg[]   = "Local/Loading03.jpg";
char     Host_ConnectMuonlineCoKr[] = "connect.muonline.co.kr";
char     Dir_Dialog[]                = "Dialog";
char     Err_HashTableFullGetIndex[] = "Hash table full!!! GetIndex";
char     Err_HashTableFullInsert[] = "Hash table full!!! Insert";
char     Txt_MacroTime[]            = "Macro_Time";
char     Txt_Dash[]                  = " - ";
char     Txt_DashPadR[]                = " -  ";
char     Txt_DashPadL[]                = "  - ";
char     Fmt_IntSlashInt[]                 = "%d/%d";
char     Fmt_HpSlashMaxHp[]                 = "%d / %d";
char     Fmt_NameSpLevel[]                 = "%s %d";
// Scene_CharSelect "account-blocked" warning lines. La tabla original de
// strings del 0.97k está stripped (ambos quedan todo-ceros en el PE), así que
// fijamos literales razonables en inglés para que el render muestre algo.
char     MsgAccountBlocked[128]             = "Account is blocked.";
char     MsgContactOperator[128]             = "Please contact the operator.";
char     ServerInfoLine1[64]              = {};  // server info line 1
char     ServerInfoLine2[64]              = {};  // server info line 2
char     ServerInfoLine3[64]              = {};  // server info line 3
char     Fmt_ServerRowNonPvp_A[]     = "%s%d Non PVP %s";
char     Fmt_ServerRowNonPvp_B[]     = "%s%d Non PVP %s";
char     Fmt_ServerRowNonPvp_C[]     = "%s%d Non PVP %s";
char     Fmt_ServerRow_A[]              = "%s%d %s";
char     Fmt_ServerRow_B[]              = "%s%d %s";
char     Fmt_ServerRow_C[]              = "%s%d %s";
char     Fmt_ServerRow_D[]              = "%s%d %s";
char     Fmt_ServerRow_E[]              = "%s%d %s";
char     Fmt_ServerRow_F[]              = "%s%d %s";
char     Fmt_ServerRowFull_A[]                 = "%s%s";
char     Fmt_ServerRowFull_B[]                 = "%s%s";
char     Fmt_ServerRowFull_C[]                 = "%s%s";

// ── Misc game globals ─────────────────────────────────────────────────────────
// g_csQuest es un alias de g_csQuest (ver globals.h).
// g_EnableSound, LegacyEnable3DSound, LegacySoundLoadCount, LegacyLpDS, LegacyWavefile —
// now macro aliases for g_EnableSound / g_Enable3DSound / SoundLoadCount /
// g_lpDS / wavefile respectively. See globals.h.

// ── Engine / Window handles ───────────────────────────────────────────────────
DWORD    LegacyEnableSound  = 0;   // g_EnableSound
DWORD    SoundChannelIndexOffset  = 0;   // sound channel index offset
DWORD    ScreenRateXBits  = 0;
// ScreenRateX defined in Config_Load.cpp
DWORD    ScreenRateYBits  = 0;
// ScreenRateY = g_fScreenRate_y (overlaps ScreenRateYBits as float)
DWORD    ChatFocusIdB  = 0;
// ConfigLoginVersion (IDA: m_ExeVersion) defined in Config_Load.cpp as char[12]
// HashTable obfuscation. Original binary has a real hash-table object at
// 0x055c9bc8..0x055c9bd4 (contiguous). 40+ inlined callers deref the vtable
// at offset +0xC, and insert/lookup helpers read capacity at offset +0xC from
// the CONTEXT (&MAIN_HASH_CLASS + 3 * sizeof(void*) = HashTableCapacity).
//
// Estrategia SAFE SENTINEL SLOT (con capacity=0, los sitios sin el guard de bd4
// terminan en `puVar = NULL → *(NULL + 0x161)`):
// capacity = 1, hash function returns 0 (always slot 0), slot 0
// stores (key=0, value=&g_HashSentinelNode). g_HashSentinelNode is a 0x584-byte
// buffer that absorbs ALL anti-tamper ref-counts and XOR encryption writes:
//   - cVar5 = sentinel[0x161]  → reads byte at offset 353 of the buffer (valid)
//   - sentinel[0x161]++/-- → writes byte 353 of the buffer (valid)
//   - 0x584-byte XOR pass over sentinel → overwrites the buffer with garbage,
//     which is fine because nothing else reads it
// Real game data structures never get hit by anti-tamper writes because the
// hash never holds keys for them.
//
// The vtable function at offset +0xC is the hash function (called as
// `(*hash)(key)`). Returns 0 = slot index of the sentinel. Internal callers
// that loop over slots use this index safely (slot 0 has key=0 + value=
// sentinel, so the key-mismatch path drops out via the iteration limit
// without indexing out of bounds).
//
// External callers using HashTable_GetIndex (functions.h wrapper) get
// 0xFFFFFFFF instead, so their `if (idx != -1)` guard skips the subsequent
// HashTable_GetNode lookup + NULL deref.
static unsigned int __cdecl HashFn_Sentinel(void*) { return 0; }
static void* g_FakeHashVtable[8] = {
    nullptr, nullptr, nullptr,
    (void*)HashFn_Sentinel,              // slot 3 / byte offset 0xC
    nullptr, nullptr, nullptr, nullptr
};

// Buffer centinela del nodo anti-tamper.  Mide 0x585, NO 0x584.
//
// DESBORDE (2026-10-02, issue #75): estaba en 0x584 (indices 0..0x583) pero el
// protocolo escribe el byte de refcount en el indice 0x584 -- ver los ~20 sitios
// de `*(BYTE*)(node + 0x584) = ...`.  Ese byte caia JUSTO sobre g_HashValueArr,
// que el linker pone pegado:
//
//     g_HashSentinelNode  0x0256A800 .. +0x584
//     g_HashValueArr      0x0256AD84          <- 0x0256A800 + 0x584
//
// y g_HashValueArr[0] guarda el puntero AL PROPIO NODO.  Al pisarle el byte bajo,
// el acceso siguiente usa un puntero corrido y el `memcpy(v9, v7, 0x584)` de la
// rama "found" lee memoria arbitraria y la vuelca sobre CharacterMachine -- de
// ahi los saltos a direcciones que son texto ("Vers" = 0x73726556).
//
// En Debug el layout deja padding entre los dos globals y el byte perdido no
// molestaba; en Release quedan pegados y la corrupcion es directa.  Por eso el
// sintoma era "Debug anda, Release no".
//
// AntiTamper_HashNode() (System_Legacy.cpp) ya usaba 0x585; al centinela se le
// habia pasado.
alignas(4) static unsigned char g_HashSentinelNode[0x2000] = {0};

// Single-slot hash arrays. Both point into static storage so we don't need
// to allocate. Slot 0 of g_HashValueArr is initialized to &sentinel; slot 0
// of g_HashKeyArr is left at 0 (matches any "key=0" lookup; for key!=0 the
// caller's key-check fails and the iteration limit drops out).
static uintptr_t g_HashValueArr[64] = { 0 };  // values (filled by ctor below)
static DWORD g_HashKeyArr  [1] = { 0 };  // keys

// Buffer layout (original binary): +0 vtable, +4 values, +8 keys, +0xC capacity.
uintptr_t g_HashTableCtx[4] = { 0, 0, 0, 0 };
struct HashCtxInit_t {
    HashCtxInit_t() {
        g_HashValueArr[0]   = (uintptr_t)g_HashSentinelNode;
        g_HashTableCtx[0] = (uintptr_t)g_FakeHashVtable;        // vtable
        g_HashTableCtx[1] = (uintptr_t)g_HashValueArr;          // value array
        g_HashTableCtx[2] = (uintptr_t)g_HashKeyArr;            // key array
        g_HashTableCtx[3] = 1;                               // capacity = 1
    }
} g_HashCtxInitObj;
BYTE*    HashTableBaseAlloc  = nullptr;
float    HashTableResizeScatter = 0.0f;
BYTE*    HashValuesAlloc  = nullptr;
BYTE*    HashKeysAlloc  = nullptr;
DWORD    ErrorReportCtx  = 0;
DWORD    LocaleStringPtr  = 0;
DWORD    ServerAddressStr  = 0;
std::uintptr_t ItemAttrTableBase = 0; // Reserva de atributos primarios de items.
std::uintptr_t EntityArrayAlloc = 0;
int      RandomTable[100] = {};   // RandomTable — lo siembra WinMain
BYTE*    ChatHistoryWidget  = nullptr;  // objeto del historial de chat
BYTE*    WhisperChatWidget  = nullptr;  // objeto de susurros
uintptr_t UiRenderTextObj = 0;
// DAT_055c9ffc = g_hWnd (gWindow.GetHwnd())
// Font memory DC (GDI-only, DIB-backed). Set by Font_BuildLayout.
// NOT the window DC — that is g_hDC (DAT_055ca004) que vive en CWindow.
// DAT_055ca000 = g_hInst (above)
// DAT_055ca004 = g_hDC — defined via macro in stdafx.h (no separate storage)
DWORD    g_hRC  = 0;
DWORD    AppQuitFlag  = 0;
char     g_IME_Active  = 0;
DWORD    g_FatalError  = 0;
DWORD    g_iNoMouseTime  = 0;
char     g_TextBufNearFull  = 0;
DWORD    g_DBCS_Counter  = 0;
DWORD    EnterKeyPressedFlag  = 0;
DWORD    g_MouseX_at_down  = 0;
DWORD    g_MouseY_at_down  = 0;
DWORD    LoginTokenBuf  = 0;
// Socket context struct — contiguous buffer at 0x055ca160 in original binary.
// Layout: +0 vtable/flags, +4 g_bGameServerConnected, +8 SOCKET,
//         +0xC send_buf[0x2000], +0x200C send_len, +0x2010 recv_buf[0x2000],
//         +0x4010 recv_index, +0x4014 dispatch_flag, +0x4018 padding,
//         +0x401C onwards: 300 packet slots × 0x2008 bytes each
//                          (slot = 4-byte flag + 4-byte len + 0x2000 data)
//         Total size = 0x401C + 300 * 0x2008 = 0x25BF04, round up to 0x260000.
// IDA confirms 300 slots / stride 0x2008 in sub_43DF90 + CWsctlc::GetReadMsg.
// Callers pass the literal 0x55ca160 as pointer — now redirected via macro in globals.h.
//
// Tiene que cubrir los 300 slots completos: si queda más chico, cualquier paquete
// >12 bytes pisa los globals vecinos (p.ej. g_SimpleModulusSC, las claves Dec2) y
// todo C3 falla con checksum mismatch.
// IDA: SocketClient (0x055CA160)
char     SocketClient[0x260000] = {};
// Static init: socket field must start as INVALID_SOCKET (0xFFFFFFFF)
struct NetCtxInit_t { NetCtxInit_t() { *(SOCKET*)(SocketClient + 8) = Platform::InvalidSocket; } } g_NetCtxInitObj;
// IDA: SocketClient.m_LogPrint (0x055CE174)
DWORD    SocketClientLogPrint  = 0;

// ── Network / login state ─────────────────────────────────────────────────────
DWORD    DecryptedCharName  = 0;
DWORD    LegacyTradeRequester  = 0;
DWORD    LegacyTradeAccepted  = 0;
DWORD    SoccerTime  = 0; // IDA: SoccerTime (0x05826C08)
// CSimpleModulus XOR key table @ 0x00562E48 (.rdata in the original binary).
// Used by CSimpleModulus_LoadEncryptionKey / LoadDecryptionKey (sub_53D1C0)
// to de-obfuscate the key DWORDs read from Enc1.dat / Dec2.dat.
// Extracted from main.exe rdata segment (MD5 eb95ac0785e40a7ad60c9ddb5d8bef34).
DWORD    SimpleModulusXorKeys[4] = {
    0x3F08A79B, 0xE25CC287, 0x93D27AB9, 0x20DEA7BF
};

// CSimpleModulus objects — 17 DWORDs each (68 bytes):
//   [0]      : vtable/header (unused by crypto math)
//   [1..4]   : ModKey[4]      (this+4  .. this+19)
//   [5..8]   : EncKey[4]      (this+20 .. this+35)
//   [9..12]  : DecKey[4]      (this+36 .. this+51)
//   [13..16] : XorKey[4]      (this+52 .. this+67)
// Loaded at WinMain startup from Data\Enc1.dat (CS) and Data\Dec2.dat (SC).
// IDA: g_SimpleModulusCS (0x05826C10)
DWORD    g_SimpleModulusCS[17] = {0};
// IDA: g_SimpleModulusSC (0x05826C58)
DWORD    g_SimpleModulusSC[17] = {0};
// IDA: m_nTempMyTradeGold (0x05826C9C)
DWORD    m_nTempMyTradeGold  = 0;
// IDA: HeroIndex (0x05826CA0)
DWORD    HeroIndex  = 0;
// IDA: HeroKey (0x05826CAC)
DWORD    HeroKey  = 0;
// IDA: CurrentProtocolState (0x05826CB0)
DWORD    CurrentProtocolState  = 0;
// IDA: ChatWhisperID (0x05826CB4)
char     ChatWhisperID[41] = {0};   // 0.97.40: nombre de 40 bytes
DWORD    ServerInfoField1  = 0;
DWORD    ServerInfoField2  = 0;
char     ServerInfoField3  = 0;
// IDA: DAT_05826CD4 (0x05826CD4)
char     LogInID[16]  = {0};
// IDA: seriales 0x05826CEB/CEC: miembros de CNetwork.
// IDA: g_dwLatestMagicTick (0x05826CF4)
DWORD    g_dwLatestMagicTick  = 0;
// IDA: DAT_05826CF8 (0x05826CF8)
DWORD    LogIn  = 0;
// IDA: g_bGameServerConnected (0x05826CF0)
DWORD    g_bGameServerConnected  = 0; // IDA: g_bGameServerConnected (0x05826CF0)
// IDA: ChatTime (0x05826D08)
DWORD    ChatTime  = 0;
char     Teleport  = 0; // IDA: Teleport (0x05826D14)
// IDA: BuyCost (0x05826D18)
DWORD    BuyCost  = 0;
// EnableUse = EnableUse (IDA 0x05826D1C), definido mas abajo; ver globals.h.
DWORD    DeleteCharResult  = 0;
DWORD    SummonLife  = 0; // IDA: SummonLife (0x05826D24)
int      AttackPlayer  = 0;   // 0x05826D28 — indice de slot del ultimo atacante
DWORD    LegacyTradeActive  = 0;
char     LegacyDuelMode  = 0;
char     LegacyTradeParam  = 0;
char     SoccerObserver  = 0; // IDA: SoccerObserver (0x05826D33)
DWORD    TradeResultLabels  = 0;
PathContext* PathFindContext = nullptr;
// IDA: DAT_05826e04
DWORD    FpsWindowStartTimeMs  = 0;
float    WorldTime  = 0.0f; // IDA: WorldTime (0x05826E08)
float    g_AttackEffectMatrix_04D[3][4] = {};
float    g_AttackEffectMatrix_04D_Alt[3][4] = {};
float    g_AttackEffectMatrix_04D_Aux[3][4] = {};
// IDA: DAT_05826e0c
DWORD    FpsTimerInitialized  = 0;
DWORD    FpsWindowFrameCount  = 0;
// BoneQuaternion @ 0x05826E18 — scratch de cuaterniones por hueso que llena
// BMD_Animation (0x440060 L157-166: `(char *)&unk_5826E18 + 16 * boneIdx`).
// MAX_BONES = 200, igual que g_BoneScratch → 200 x 16 bytes (paso de 16 por hueso).
char     BoneQuaternionScratch[200 * 0x10] = {0};
std::uintptr_t ModelsBase = 0; // Base de Models, sin truncar la dirección.
void*    ModelMemoryPool  = nullptr;  // model memory pool

// BMD bounding-box scratch arrays (BMD_CreateBoundingBox)
// Tablas scratch de BMD_CreateBoundingBox (0x442E60), la unica funcion del
// binario que las toca.  Se recorren UNA ENTRADA POR HUESO hasta numBones
// (= *(short*)(model+34)): word_77D87FC[bone] es el contador de vertices y
// flt_5827A98 / flt_6F42A5C son el max/min del bbox, 3 floats por hueso.
// MAX_BONES = 200, igual que g_BoneScratch / BoneTransform / BoneQuaternion: tienen
// que ser arrays por hueso (con escalares, desde el hueso 1 se pisan globals vecinos).
short    BoneVertexCounts[200]    = {0};   // contador de vertices por hueso
float    BoneBBoxMax[200*3]  = {0};   // bbox max, 3 floats por hueso
float    BoneBBoxMin[200*3]  = {0};   // bbox min, 3 floats por hueso

// UI name-list panel data (ShowCheckBox)
// DialogButton1Rect — macro dentro de DialogButtonRects (dialog button rects)
// MessageBoxCustomBuf ES g_lpszMessageBoxCustom — buffer de diálogo de 7 líneas × 0x26
// bytes (char[7 * 0x26] = 266 bytes) que usan CreateOkMessageBox, RenderErrorMessage,
// los diálogos de CSQuest, SecondPassword, UI_StatsPanel, etc.; LegacyMsgBoxLine1 es un
// macro alias al offset +0x26 (line[1]) dentro del buffer.
// (Definición más abajo en el archivo, junto a los otros DAT_083a4*.)
byte     ItemClassNameFormatPtrA  = 0;
byte     ItemClassNameFormatPtrB  = 0;
byte     ItemClassNameFormatPtrC  = 0;
byte     ItemClassNameFormatPtrD  = 0;
char     Fmt_PaddedName[] = " %s ";

// ── Bone / skeleton data ──────────────────────────────────────────────────────
// Bone-matrix scratch buffer. In the original binary this region (0x06970a9c..
// ~0x0697163c) is a contiguous pool where Sprite_Draw / Entity_Render_3D write
// interpolated bone matrices (stride 0x30 bytes = float[3][4] per bone). Ghidra
// labels DAT_06970XXX were placed where specific slot addresses are referenced.
// We back all of them with a single char buffer and expose each sibling as a
// DWORD lvalue at the correct offset via macros (see globals.h), so:
//   &BoneScratchRoot  → g_BoneScratch + 0x000   (DWORD* into buffer)
//   &BoneScratchSecA  → g_BoneScratch + 0x030
//   …
// This keeps addresses contiguous and makes the dynamic indexing
//   (float*)((char*)&BoneScratchRoot + boneIdx * 0x30)
// in Entity_Render_3D.cpp work byte-accurately.
//
// Es el `BoneTransform[MAX_BONES][3][4]` del original (MU 5.2 ZzzBMD.h: MAX_BONES=200
// → 200*0x30 = 0x2580 bytes). No achicarlo: un modelo con más huesos que el buffer
// desborda sobre lo que el linker ponga después (en este layout, CharSelPreviewEntity, el
// preview char de char-select).
char     g_BoneScratch[200 * 0x30] = {0};   // = 0x2580 (BoneTransform[200][3][4])

// ── Preview character entity (0x07abf050) ─────────────────────────────────────
// Buffer de entidad COMPLETO para el preview char de char-select:
// CreateCharacterPointer escribe hasta +908, así que no puede ser un escalar.
// 0x580 = el spacing del original hasta el array 0x07abf5d0; los campos se
// acceden por macros (ver globals.h).
char     CharSelPreviewEntity[0x580] = {0};
char     ClassNameTable[9000] = {};  // class name table (stride 300, 30 slots) — símbolo aparte
std::uintptr_t CharactersClient = 0; // IDA: CharactersClient (0x07ABF5D0)
int      VisibleEntityCount  = 0;
char    *Hero  = NULL; // IDA: Hero (0x07ABF5D8)
DWORD    EntitySlotCount  = 0;
float    RenderExtraFlag = 0.0f;
DWORD    RenderExtraFlagBits  = 0;
// Particle pool — 3000 slots × 0x70 (112) bytes = 336000 bytes total.
// Binario original: 0x07abf5f0..0x07b11670 = 0x52080 bytes = 3000 slots exactos,
// los que itera MoveParticles.
char     SpritePoolBase[kParticlePoolSize * 0x70] = {0};   // particle pool base

// ── Skill effects / player render pools ──────────────────────────────────────
DWORD    MapEffectPoolBase  = 0;
DWORD    ParticlePoolBase  = 0;
DWORD    ItemDropPoolBase  = 0;
// SkillEffect pool — 200 slots × 0x70 bytes (= 22400 bytes); SkillEffect_Render
// lo recorre entero, así que necesita el tamaño real.
unsigned char SkillEffectPool[200 * 0x70] = {};
// Joint/Trail/Blur shared render pool — 100 slots × 0x2f0 = 76800 bytes.
// IDA layout: each slot starts at offset 0; JointRenderPool is the +12 "anchor"
// field within slot[0]. We back the entire pool here and project the anchor
// through a macro (see globals.h).
char     g_RenderPool[100 * 0x2f0] = {};
uintptr_t g_ParticleOwner[kParticlePoolSize] = {};
uintptr_t g_TrailOwner[100] = {};
uintptr_t g_OperateObjects[200] = {};
DWORD    WeatherIntensityTarget  = 0;
DWORD    WeatherIntensityCurrent  = 0;    // BGM track 1 enable flag
// Player render pool: 100 entries × 0x1BC bytes (= 444 bytes/slot).
// Original spans [0x07c74e68, 0x07c7fcc4). El walker v1 de Player_Render
// arranca en `&PlayerRenderV1AnchorPtr` y lee offsets NEGATIVOS hasta -0xEC, accediendo
// a los primeros 0xEC bytes del slot. Por eso PlayerRenderV1AnchorPtr NO es el inicio
// real del pool — el inicio es 0xEC bytes antes (= DAT_07c74e68).
//
// g_PlayerRenderPool cubre los 100 slots completos (cabecera de 0xEC bytes
// incluida) y PlayerRenderV1AnchorPtr es un alias dentro del array en el offset 0xEC
// (= anchor v1 del slot 0).
char     g_PlayerRenderPool[100 * OBJECT_STRIDE] = {};
DWORD*   PlayerRenderV1AnchorPtr = (DWORD*)(g_PlayerRenderPool + 0xEC);

// ── g_CharData XOR block ───────────────────────────────────────────────────────
void    *CharacterMachinePtr  = nullptr;  // g_CharData pointer (0x584-byte XOR-encoded)

// ── UI / HUD data ─────────────────────────────────────────────────────────────
DWORD   FacingAnglePacket  = 0;    // facing angle (float, movement packet)
DWORD    WorldMapType  = 0;    // world/map type
DWORD    MacroCooldown  = 0;    // MacroTime (0x07E11D7C)
DWORD    SkillNotifyType  = 0;
DWORD    SkillNotifyStartTick  = 0;
DWORD    ImeOldConv  = 0;
DWORD    ImeCurrConv  = 0;
DWORD    BlinkFrameCounter  = 0;
DWORD    PacketBufSlotCount  = 0;
DWORD    MoveStepCounter  = 0;
int      FacingAngle  = 0;
char     MoveLockFlag  = 0;
DWORD    NpcDialogActiveFlag  = 0;   // NPC script: dialog-active flag (set 1 when dialog in progress)
DWORD    NpcKeepaliveTick  = 0;   // NPC script: keepalive timer (GetTickCount at last send)
DWORD    TotalPacketSize  = 0;
DWORD    _PartyNumber  = 0;   // NPC script chat-log slot 0
DWORD    _PartyKey  = 0;   // NPC script chat-log slot 1
DWORD    NpcScriptChatLogSlot2  = 0;   // NPC script chat-log slot 2
DWORD    NpcScriptChatLogSlot3  = 0;   // NPC script chat-log slot 3
DWORD    TargetInvSlot  = 0;
DWORD    PartyHPBarsTable  = 0;
// LegacyPartySlotArray: alias de Party + 0x1C (globals.h)
char     HudTextVisibleFlag           = 0;
char     LockInputStatus           = 0; // IDA: LockInputStatus (0x07E11D6F)
int      g_WorldLoading         = 0;   // >0 mientras corre OpenWorld (ver WinMain WM_USER)
// LegacyUsernameDispBuf / LegacyPasswordDispBuf son GlobalText[450] / [451] (ver el bloque de alias
// al final de globals.h).
// Scene_Login credential dialog + version footer. La tabla de strings del
// 0.97k está stripped: get_xrefs_to en Ghidra confirma que NADA escribe estos
// buffers en el binario (se renderizan vacíos). Rellenamos con defaults
// sensatos para que el panel muestre botones/texto legible.
char     BtnTextOk[128] = "OK";          // botón OK del panel de credenciales
char     BtnTextExit[128] = "Exit";        // botón Exit/Cancel del panel
// LegacyCharNameFmt (char name format string), LegacyVersionStr1 / LegacyVersionStr2 / LegacyVersionStr3Fmt
// (líneas de versión) son GlobalText[459] / [454] / [455] / [456] (ver el bloque de
// alias al final de globals.h).
char     MsgConnecting[128] = "Connecting...";
DWORD    GroundItemArray  = 0;
// El pool de items en el suelo es de 1000 entradas × 0x204 bytes (≈504 KB).
// El slot base es GroundItemPool + key*0x204; CreateItem escribe active@ip+72,
// model@ip+74, pos@ip+88 y el render (Entity_Render, en Render/Render_WorldHelpers.cpp)
// los lee en los mismos offsets (active@slot+72). Ambos alineados sobre
// GroundItemPool.
unsigned char GroundItemPool[1000 * GROUND_ITEM_STRIDE] = {};
DWORD    PlayerSpritePoolBase  = 0;
char     PickedItemData[0x44]  = {0};
DWORD    ItemPickedFlag  = 0;
byte     InvGridSelCol  = 0;   // UI grid selected column
byte     InvGridSelRow  = 0;   // UI grid selected row
short    PinShuffleDigits[10] = {0};   // digitos barajados del teclado del PIN
char     MaskedFieldPrefix  = 0;        // IDA: byte_7EAA1A4 (prefijo del campo enmascarado)
DWORD    ScreenBannerText  = 0;
DWORD    CharSelectClearScratch  = 0;
DWORD    GuildListStartYStorage  = 0;
DWORD    GuildListStartXStorage  = 0;
DWORD    TradeSlotEntityIds  = 0;
// Tabla de nombres (guild members / buffs), stride 80 (0x50). En el binario
// va de 0x07E919BC al centinela dword_7EA51EC (0x07EA51EC) = 0x13C30 bytes.
// Todos sus consumidores la indexan como `&GuildInfoTable + N*80`: tiene que ser el
// array completo. Ver la nota en globals.h.
char     GuildInfoTable[0x13C30] = {};
// LegacyInventory: alias de Inventory (globals.h)
DWORD    SourceInvSlot  = 0;
DWORD    HudBtn1PosX  = 0;
DWORD    HudBtn1PosY  = 0;
DWORD    LegacyInventoryItems  = 0;
DWORD    LegacyInventoryLevels  = 0;
// Equip grid buffer — see globals.h header for layout rationale.  Each
// 68-byte row contains 8 ITEM cells at stride 544 bytes (so cells overlap
// across rows in a tiled layout).  Initialise all Type fields (every 56-th
// byte at row-relative offset -56, but here we just zero-fill and
// stamp 0xFFFF in HUD_InitInventoryPools).
// unk_7EA9504 / unk_7EA9328 son POSICIONES DENTRO DEL INVENTARIO REAL, no un
// buffer aparte.  OffsetInventoryItems esta en 0x07EA8410 y la cuenta cierra
// exacta con slots de 0x44 y el campo Key en +56:
//     0x7EA9504 - 0x07EA8410 = 4340 = 63 * 0x44 + 56   -> slot 63, Key
//     0x7EA9328 - 0x07EA8410 = 3864 = 56 * 0x44 + 56   -> slot 56, Key
// El walker de FindQuestItemsInInven / FUN_004824c0 / FUN_00482850 recorre los
// slots 63..56 por afuera (paso -0x44) y por adentro salta de a -8 slots
// (paso -544), o sea cubre los 64 slots del grid 8x8.  Lo que lee en cada uno:
//     v7          = slot + 56  -> Key   ( > 0 = celda ocupada )
//     v7 - 56     = slot + 0   -> Type
//     v7 - 52     = slot + 4   -> Level
int   *p_DAT_07ea9504_ = (int*)&OffsetInventoryItems[63 * 0x44 + 56];
int   *p_DAT_07ea9328_ = (int*)&OffsetInventoryItems[56 * 0x44 + 56];
BYTE*    InventoryBasePtr  = nullptr;
BYTE*    g_ItemMoveSourcePool = nullptr;
BYTE*    g_ItemMoveTargetPool = nullptr;
DWORD    EquipTargetSlot  = 0;
// Buffer de texto del teclado del PIN (0x07EA9814): hasta 10 digitos + NUL, y
// atras la copia del PIN de la primera pasada (dword_7EA981F).  Estaba partido
// en escalares sueltos, asi que lo que se tipeaba no llegaba a los lectores.
char     PinBytesPart0Bits[16] = {0};
// PinBytes4to7 / 981c / 981e / 981f: alias dentro de PinBytesPart0Bits (globals.h)
DWORD    CharacterInfoStartXStorage  = 0;   // Screen3 panel origin X
DWORD    CharacterInfoStartYStorage  = 0;   // Screen3 panel origin Y
char     TradePartnerName[41] = {};   // 0.97.40: nombre de 40 bytes + NUL
char     PinResetFlag  = 0;
DWORD    TradePartnerKey  = 0;
DWORD    GuildInviteKey  = 0;
// PartyNumber: alias de PartyNumber (globals.h)
DWORD    PartyInviteKey  = 0;
DWORD    TradeOtherGold  = 0;
DWORD    TradeMyGold  = 0;
int      RepairGoldTotal  = 0;   // item repair counter
DWORD    m_bYourConfirm  = 0;
char     m_bMyConfirm  = 0;
int      TradeYourWait = 0;
int      TradeMyWait = 0;
DWORD    TradeRemoteGuildKey = 0;
WORD     TradeRemoteLevel = 0;
char     GuildOpened  = 0;
char     PartyOpened  = 0;
char     CharacterOpened  = 0;
char     InventoryOpened  = 0;
char     ShopOpened  = 0;
char     WarehouseOpened  = 0;
char     ChaosMixOpened  = 0;
char     TradeOpened  = 0;
char     EventWindowOpened  = 0;
DWORD    GuildCreatorOpened  = 0;
DWORD    GoldenArcherOpenType  = 0;
DWORD    ItemActionMode  = 0;
DWORD    MixType  = 0;
DWORD    g_iKeyPadEnable  = 0;
DWORD    PinInputModeState  = 0;
DWORD    ServerContextId  = 0;
DWORD    TextLineCount  = 0;
uintptr_t CheckInventoryPtr  = 0;
char     ItemMoveInFlightFlag  = 0;
DWORD    MixRecipeLevel  = 0;   // TextureEnable (GL texture state cache)
char     PinInputActiveFlag  = 0;
char     InventoryDropErrorText  = 0;   // inventory drop error message ID string
// Frustum world corners: 5 × 3 floats at 0x07eab1b0
float    FrustrumVertex  = 0.0f;  // corner 0 X
float    FrustrumCorner0Y  = 0.0f;  // corner 0 Y
float    FrustrumCorner0Z  = 0.0f;  // corner 0 Z
float    FrustrumCorner1X  = 0.0f;  // corner 1 X
float    FrustrumCorner1Y  = 0.0f;  // corner 1 Y
float    FrustrumCorner1Z  = 0.0f;  // corner 1 Z
float    FrustrumCorner2X  = 0.0f;  // corner 2 X
float    FrustrumCorner2Y  = 0.0f;  // corner 2 Y
float    FrustrumCorner2Z  = 0.0f;  // corner 2 Z
float    FrustrumCorner3X  = 0.0f;  // corner 3 X
float    FrustrumCorner3Y  = 0.0f;  // corner 3 Y
float    FrustrumCorner3Z  = 0.0f;  // corner 3 Z
float    FrustrumCorner4X  = 0.0f;  // corner 4 X
float    FrustrumCorner4Y  = 0.0f;  // corner 4 Y
float    FrustrumCorner4Z  = 0.0f;  // corner 4 Z
DWORD    TerrainTileIdxTL  = 0;
DWORD    TerrainTileIdxTR  = 0;
DWORD    TerrainTileIdxBL  = 0;
DWORD    TerrainTileIdxBR  = 0;
// Water-wave height table — 256×256 floats = 256KB (binario: 0x07EAB200). Se
// escribe vía `(char*)&WaterWaveHeightTable + (row*256+col)*4` (Terrain_Water.cpp +
// Terrain_Light.cpp), así que tiene que ser el array completo.
float    WaterWaveHeightTable[256 * 256] = {};
DWORD    FrustrumFaceD  = 0;
DWORD    FrustrumPlane1Dist  = 0;
DWORD    FrustrumPlane2Dist  = 0;
DWORD    FrustrumPlane3Dist  = 0;
DWORD    FrustrumPlane4Dist  = 0;
float    WaterMove  = 0.0f;   // WaterMove — terrain water UV scroll offset (RenderTerrain)
// TestFrustrum2D (Frustum_IsVisible) lee 4 floats consecutivos desde cada array y
// Camera_SetMatrix escribe los 4 corners: tienen que ser arrays contiguos (si no,
// el cull del frustum rechaza todos los chunks).
float    FrustrumY[4] = {0};   // frustum quad Y[4]
float    FrustrumX[4] = {0};   // frustum quad X[4]
// OpenJpegBuffer escribe 256x256 RGB floats (= 196608 floats) usados como
// TerrainLight RGB ambiente.
float    TerrainLightAmbientBuf[256 * 256 * 3] = {};
DWORD    TerrainUvX  = 0;
DWORD    TerrainUvY  = 0;
// Tile pick corners buffer — 12 floats contiguos (4 vec3 corners del quad
// clickeado); glVertex3fv los lee de corrido.
float    g_TilePickBuf[12] = {};
// TerrainNormal[256*256][3] float array.
// FUN_004f70b0 (CreateTerrainNormal) escribe 256*256*3 = 196608 floats acá.
float    TerrainNormalTable[256 * 256 * 3] = {};

// ── Large game data arrays ────────────────────────────────────────────────────
// El código (InitTerrainMappingLayer, Terrain_Clear y otros) indexa estos cuatro
// hasta [65535]: el IDA decomp expresa los accesos como `(int)&DAT_xxxx + iVar2`,
// así que tienen que ser arrays del tamaño real (no DWORD sueltos).
//   TerrainMappingLayer2 = TileTex2[256*256] (BYTE)   — segundo índice de textura
//   TerrainMappingLayer1 = TileTex1[256*256] (BYTE)   — primer índice de textura
//   TerrainMappingAlpha = TerrainHeight[256*256] (float) — altura de tile
//   TerrainNoiseTable = TerrainNoise[256*256] (float)  — ruido aleatorio init
unsigned char  TerrainMappingLayer2[0x10000] = {};
unsigned char  TerrainMappingLayer1[0x10000] = {};
// BackTerrainHeight[256*256] float array.
float    BackTerrainHeight[0x10000] = {};
float    TerrainNoiseTable[0x10000] = {};
float    g_TerrainTexCoord[8] = {};   // TerrainTextureCoord[4][2] (RenderTerrainFace/FaceTexture)
DWORD    WaterRenderDisableFlag  = 0;
BYTE     g_TerrainObjTable[0x328] = {};   // ambient terrain-object table (sub_4F7060); &LegacyTerrainObjTable=&[5]
// Live per-tile lighting buffer — 256×256 tiles × 3 floats = 786432 bytes.
// Terrain_Water escribe vía `(char*)&PrimaryTerrainLightStorage + iVar2*12` (y análogo con
// cb60c, cb610). En el binario cb608/cb60c/cb610 son los tres DWORDs del slot 0;
// las macros proyectan cb60c/cb610 dentro del mismo buffer.
float    PrimaryTerrainLightStorage[256 * 256 * 3] = {};
// TerrainLightData[256*256][3].
// FUN_004f71c0 (Terrain_FinalizeLighting) escribe 196608 floats acá.
float    TerrainLightData[256 * 256 * 3] = {};
float    TerrainMappingAlpha[0x10000] = {};
// Frustum plane normals: 5 planes × 3 floats
float    FrustrumFaceNormal  = 0.0f;  // plane 0 normal X
float    FrustrumPlane0NormalY  = 0.0f;  // plane 0 normal Y
float    FrustrumPlane0NormalZ  = 0.0f;  // plane 0 normal Z
float    FrustrumPlane1NormalX  = 0.0f;  // plane 1 normal X
float    FrustrumPlane1NormalY  = 0.0f;  // plane 1 normal Y
float    FrustrumPlane1NormalZ  = 0.0f;  // plane 1 normal Z
float    FrustrumPlane2NormalX  = 0.0f;  // plane 2 normal X
float    FrustrumPlane2NormalY  = 0.0f;  // plane 2 normal Y
float    FrustrumPlane2NormalZ  = 0.0f;  // plane 2 normal Z
float    FrustrumPlane3NormalX  = 0.0f;  // plane 3 normal X
float    FrustrumPlane3NormalY  = 0.0f;  // plane 3 normal Y
float    FrustrumPlane3NormalZ  = 0.0f;  // plane 3 normal Z
float    FrustrumPlane4NormalX  = 0.0f;  // plane 4 (near) normal X
float    FrustrumPlane4NormalY  = 0.0f;  // plane 4 (near) normal Y
float    FrustrumPlane4NormalZ  = 0.0f;  // plane 4 (near) normal Z
// BMPHeader[1080] (BITMAPFILEHEADER + DIB header + palette); OpenTerrainHeight
// escribe 1080 bytes acá.
unsigned char BmpHeaderBuffer[1080] = {};
DWORD    TerrainFlag  = 0;
char     TerrainWallFlags[0x10000] = {};  // terrain walk flags (per-tile byte array, 256x256 grid)
DWORD    TerrainLightBufToggle  = 0;            // terrain-light double-buffer toggle (RenderTerrain)
DWORD    TerrainNoDrawFlag  = 0;
DWORD    WaterTextureNumber  = 0;
DWORD    FrustrumBoundMinX_1  = 0;
DWORD    FrustrumBoundMinY_1  = 0;
DWORD    FrustrumBoundMinX_2  = 0;
DWORD    FrustrumBoundMinY_2  = 0;
DWORD    RenderPoolA  = 0;

// ── Scene / state machine vars ────────────────────────────────────────────────
float    EarthQuakeStrength = 0.0f;  // EarthQuake
DWORD    EarthQuakeStrengthBits  = 0;
// Celdas de escena; el layout x86 permanece idéntico al de IDA 0x083A0218.
SceneObjectCell g_ObjectBucketGrid[256] = {};
DWORD    NpcVisiblePool  = 0;
DWORD    RenderPoolB  = 0;
DWORD    WeatherModeState  = 0;
char     InGameModeFlag  = 0;
// ── MouseRayEndPoint — mouse-ray endpoint world pos (3 floats) ───────────────────
// En el binario original Camera_MouseRay (Camera_BuildMouseRay) escribe out_ray[0..2]
// arrancando en 0x083a4110. Si se declara como UN único DWORD el linker no
// reserva los 12 bytes y los writes a out_ray[1]/[2] caen en globals vecinos.
DWORD    MouseRayEndPointStorage[3] = {0};
DWORD&   MouseRayEndPoint = MouseRayEndPointStorage[0];
DWORD    MouseLButtonPush  = 0;
DWORD    MouseLButtonPop  = 0;
// ─── View/camera 3x4 matrix (48 bytes = 12 DWORDs) ───────────────────────────
// En el binario original 0x083a4140..0x083a416f es UN único buffer que
// GL_GetModelViewMatrix llena con 3 filas × 4 floats.
// CameraTranslateXBits / CameraTranslateYBits / CameraTranslateZBits son los 4-th elementos de cada
// fila (offsets 0x0c, 0x1c, 0x2c) — no globales independientes.
// Si se declaran por separado el linker los reubica y GL_GetModelViewMatrix
// desborda 48 bytes sobre globales vecinos (p.ej. InitLogIn Scene_Login
// init flag → loop de re-init cada frame).
DWORD    CameraMatrix[12] = {0};
DWORD&   CameraTranslateXBits = CameraMatrix[3];
DWORD&   CameraTranslateYBits = CameraMatrix[7];
DWORD&   CameraTranslateZBits = CameraMatrix[11];
// GrabFileName — "Screen %02d %02d %02d %02d - %04d" screenshot filename buffer.
// Original declarado como DWORD pero crt_sprintf lo usa como char[]: la escritura
// real es ~26 chars. Reservamos el gap completo hasta MouseY (0x104 bytes).
char     GrabFileName[0x104] = {0};
DWORD    MouseY  = 0;  // MouseY
DWORD    MouseX  = 0;  // MouseX
DWORD    OpenglWindowWidth  = 0;  // viewport width
// ── CameraRayOriginX..428c — camera world position (3 floats) ────────────────────
// Camera_MouseRay (Camera_BuildMouseRay) escribe via Vector_InverseRotate(... &CameraRayOriginX) los
// 3 floats consecutivos. DEBEN ser contiguos. Las definiciones en líneas 1318-
// 1320 (_CameraRayOriginX..428c) ahora son referencias a este array para que las
// lecturas via float (camPos, etc.) vean lo que Camera_MouseRay escribió.
DWORD    CameraRayOriginX_arr[3] = {0};
DWORD&   CameraRayOriginX = CameraRayOriginX_arr[0];  // _CameraRayOriginXBits
DWORD&   CameraRayOriginY = CameraRayOriginX_arr[1];  // _CameraRayOriginYBits
DWORD&   CameraRayOriginZ = CameraRayOriginX_arr[2];  // _CameraRayOriginZBits
char     MouseDblClickFlag  = 0;
DWORD    ViewportCenterX  = 0;  // DAT_083A429C
DWORD    ViewportCenterY  = 0;  // DAT_083A42A0
DWORD    PerspectiveScaleXBits  = 0;
DWORD    PerspectiveScaleYBits  = 0;
DWORD    MouseRButton  = 0;  // MouseRButton
DWORD    OpenglWindowHeight  = 0;  // viewport height
// CameraAngle[3] @ 0x083a42b8 — 3 floats read by BeginOpengl via glRotatef and
// written by MoveCamera/Scene_CharSelect. Must be contiguous; DWORD aliases
// are provided so legacy sites that stored float-bits as DWORD still link.
// See bottom of file for the moved `float CameraAngle[3]` definition — here
// we emit the storage directly so memory layout matches the binary.
// (Defined in one place below via references — see CameraAngle_storage)
extern float    CameraAngle[3];         // defined later in this TU
DWORD&   LegacyCameraAngle0 = *(DWORD*)&CameraAngle[0];
DWORD&   LegacyCameraAngle1 = *(DWORD*)&CameraAngle[1];
DWORD&   LegacyCameraAngle2 = *(DWORD*)&CameraAngle[2];
DWORD    MouseLButton  = 0;  // MouseLButton
DWORD    OpenglWindowX  = 0;  // viewport x offset
DWORD    OpenglWindowY  = 0;  // viewport y offset
// CameraPosition[3] @ 0x083a42d4 — 3 floats read by BeginOpengl via glTranslatef
// and written by MoveCamera. Must be contiguous. DWORD views are aliases.
extern float    CameraPosition[3];      // defined later in this TU
float&  CameraPositionX = CameraPosition[0];
DWORD&   CameraPositionXBits = *(DWORD*)&CameraPosition[0];
float&  CameraPositionY = CameraPosition[1];
DWORD&   CameraPositionYBits = *(DWORD*)&CameraPosition[1];
float&  CameraPositionZ = CameraPosition[2];
DWORD&   CameraPositionZBits = *(DWORD*)&CameraPosition[2];
char     CameraTopViewEnabled  = 0;  // DAT_083A42E9
char     FogEnable  = 0;  // FogEnable
char     AutoDropTriggerFlag  = 0;  // auto-drop trigger flag (inventory)
DWORD    ScreenshotModeFlag  = 0;
DWORD    LoginRetryCount  = 0;
DWORD    SelectedServerCode  = 0;
// CurrentCameraPosition[3] @ 0x083a432c — 3 contiguous floats read by MoveCamera
// via `(float*)&LegacyCurrentCamPos0`. Defining three separate DWORDs lets MSVC scatter
// them so index [1] and [2] would miss → camera position never integrates.
float    CurrentCameraPosition[3] = {0.0f, 0.0f, 0.0f};
DWORD&   LegacyCurrentCamPos0  = *(DWORD*)&CurrentCameraPosition[0];
DWORD&   LegacyCurrentCamPos1  = *(DWORD*)&CurrentCameraPosition[1];
float&  CurrentCameraPosZ  = CurrentCameraPosition[2];
DWORD&   CurrentCameraPosZBits  = *(DWORD*)&CurrentCameraPosition[2];
float    LoginBgAnimValue  = 0.0f;  // Scene_Dispatch — login background animation angle (sin-product)
// ServerList buffer: 24 entries * stride 0x21e + padding. End bounds in the
// original code were absolute (0x83a7ac6, 0x83a7ada). We make it a contiguous
// buffer and the Scene_Login_ServerSelect loops use ((int)&ServerListBuffer + 0x34ee).
char     ServerListBuffer[0x3600] = {};  // 13824 bytes covers the original 0x3502 span.
// Overlapping sub-symbols at the same address — accessed via macros in globals.h.

// Local server-name table (0x07D52C34). En el binario original es un buffer BSS
// que ReceiveServerList (F4/02) lee para el nombre de cada grupo (stride 300,
// indexado por ServerCode/20). Se puebla en runtime con el paquete F4/04
// (CCCustomServerListSend del ConnectServer: {WORD ServerCode; char Name[32]}).
// 24 grupos * 300 bytes de stride.
char     ServerNameTable[24 * 300] = {};
DWORD    ConnectStartTick  = 0;   // 32 bits: guarda GetTickCount (Game_SceneUpdate 0x0051F900)
char     PinUiHeightParam  = 0;
// CurrentCameraAngle[3] @ 0x083a7ad0 — 3 floats (pitch, yaw, roll).
// Prior definition used `char` which would truncate float writes to 1 byte
// AND let MSVC scatter the three 1-byte slots away from each other →
// MoveCamera's `(float*)&LegacyCurrentCamAngle0 + i` reads garbage for i=1,2 and stomps
// adjacent globals when writing.
float    CurrentCameraAngle[3] = {0.0f, 0.0f, 0.0f};
float&   LegacyCurrentCamAngle0 = CurrentCameraAngle[0];
float&   LegacyCurrentCamAngle1 = CurrentCameraAngle[1];
float&   LegacyCurrentCamAngle2 = CurrentCameraAngle[2];
DWORD    CameraWalkCut  = 0;
DWORD    MoveSceneFrame  = 0; // IDA: MoveSceneFrame (0x083A7C00)
DWORD    EnableMainRender  = 0;
// IDA: LoginSubState (0x083A7C14)
int      LoginSubState  = 0;
DWORD    LoginNextSubState  = 0;
DWORD    ExitGameTriggerFlag  = 0;
DWORD    ExitGameCountdown  = 0;
DWORD    InGameUiState  = 0;
DWORD    InGameUiStateNext  = 0;
DWORD    CameraWalkType  = 0;
DWORD    CameraNumber  = 0;
DWORD    ServerListCount  = 0;  // cantidad de servers del F4/02 (ReceiveServerList)
DWORD    ServerClickCooldown  = 0;
// ConnectionCheckEnable es char en el binario original (flag "connection check enable"
// leído como byte en Game_MainLoop). No declararlo DWORD: `ConnectionCheckEnable = 1`
// escribiría 4 bytes y pisaría InitLogIn/4a/4b (c49 es el init flag de
// Scene_Login).
char     ConnectionCheckEnable  = 0;
char     InitLogIn  = 0;
char     InitSceneLoading  = 0;
// IDA: DAT_083a7c4b
char     CharSelectSceneInitialized = 0;
char     InitMainScene  = 0;
char     CharSelectWarningFlag  = 0;
DWORD    RenderWatchdogCount  = 0;
DWORD    TestTime  = 0; // IDA: TestTime (0x083A7C54)
DWORD    DisconnectDialogShown  = 0;
DWORD    ExitCountdownLogLabel  = 0;  // chat-log label for exit-countdown message (empty id)
DWORD    ServerListLogText  = 0;
DWORD    ServerListLogLabel  = 0;
DWORD    ServerConnectLogLabelA  = 0;
DWORD    ServerConnectLogLabelB  = 0;
DWORD    PostLoginLogLabelA  = 0;
DWORD    PostLoginLogLabelB  = 0;
DWORD    EnterWorldLogText  = 0;
DWORD    EnterWorldLogLabel  = 0;
DWORD    CharSelectLogText  = 0;
DWORD    CharSelectLogLabel  = 0;
DWORD    ScreenshotLogLabelA  = 0;
DWORD    ScreenshotLogLabelB  = 0;
char*    g_BitmapsRaw = ResourceTables::TextureBase(); // IDA: Bitmaps, base estable; páginas bajo demanda.
DWORD    m_dwUsedTextureMemory  = 0;
DWORD    RandomTimeSeed  = 0;

// Input / player name buffers

// ── Additional globals needed by Game/Scene files ─────────────────────────────
// Padding canario de 64 bytes alrededor de GuildInputEnable..d72 (ChatMode/IME/
// DigitOnly): esos flags aparecían pisados con 0xFF cada frame por un desborde
// vecino nunca localizado. Los canarios absorben el desborde si cae cerca de
// d70/d71/d72 (si siguen en 0xCC, el escritor está más lejos).
char     g_PadBeforeChatMode[64] = { static_cast<char>(0xCC) };
char     GuildInputEnable  = 0;
char     ImeModeFlag  = 0;
char     GoldInputEnable  = 0;
char     g_PadAfterChatMode[64]  = { static_cast<char>(0xCC) };
DWORD    ActiveInputIndex  = 0;
DWORD    EditFlag  = 0;
DWORD    InputCooldownTicks  = 0;
DWORD    MoveDebounceCounter  = 0;
DWORD    DontMoveFlag  = 0;
float   FacingLastWorldTime  = 0.0f;
float   FacingDelta  = 0.0f;
BYTE     InputFieldFlags[40]  = {0};
// InputPasswordMaskFlag es InputFieldFlags[1] en el binario original (flag per-slot de
// obfuscación: 0=plain, 1=password-mask, 2=hybrid). Separarlo en su propio
// global hacía que el init escribiera a otra address y Chat_DrawField siempre
// leyera 0 → password se mostraba en texto plano.
char&    InputPasswordMaskFlag = reinterpret_cast<char&>(InputFieldFlags[1]);

DWORD    TargetX  = 0;
DWORD    TargetY  = 0;
DWORD    ItemKey  = 0;
// Historial de chat @ 0x07E113E4 — anillo de 5 entradas x 256 bytes.
// Lo confirma el propio binario: el walker de RenderChatInput termina en
// 0x07E118E4, y 0x7E118E4 - 0x7E113E4 = 0x500 = 5 * 256.
// Chat_InputTick (flechas arriba/abajo) hace `memcpy((char*)&ChatHistoryBuffer +
// idx * 0x100, ...)`: tiene que ser el anillo completo.
char     ChatHistoryBuffer[5 * 256] = {};
// FacingAnglePacket already defined at line ~470

DWORD    MouseOnWindowFlag  = 0;
BYTE     InputTextLengths[40]  = {0};
// InputLength1 no es una variable aparte: es `InputLength[1]` (bytes +4..+7 de
// InputTextLengths), un macro en globals.h que proyecta dentro del array.
// Multi-slot input buffer (IDA: InputText[10][256] @ 0x07db8710).
// Slot 0 = chat / username, slot 1 = whisper-target / password.
// PasswordInputBuffer is a #define alias for slot 1 in globals.h.
char     InputTextBuffers[10][256] = {{0}};
DWORD    HoverTargetType  = 0;

// Server-config globals (poblados por los opcodes 0xDD/DE/DF).
// gPrintPlayer.MaxCharacterLevel del DLL source mapea a g_MaxCharacterLevel.
// Usado por RenderCharacterInfoWindow "Nivel: %d / %d".  Default 400 = cap
// vanilla 0.97k hasta que el server mande PMSG_CHARACTER_MAX_LEVEL_RECV.
extern "C" {
    DWORD    g_MaxCharacterLevel  = 400;
    WORD     g_CharDeleteMaxLevel = 10;
    BYTE     g_CharCreationEnable = 1;
}

void    *CharacterAttributePtr  = NULL;

char     SavedUsernameBuf[12] = {0};

// LegacyConnectStatusStr / LegacyCharNameSendBuf / LegacyCharNameConfBuf son GlobalText[470] / [472] / [473]
// (ver el bloque de alias al final de globals.h).
DWORD    ServerSelectRenderFlag  = 0;
// LegacyServerGroupStr / LegacyServerChanStr son GlobalText[563] / [564] (ver el bloque de alias
// al final de globals.h).

// Model data table base + entity vtable
// (ModelsBase and WorldTime are defined above in their original sections)
// ── BoneVertex pool ──────────────────────────────────────────────────────────
// Transformed-vertex buffer used by Sprite_DrawBone (Skeleton_Transform), BMD_DrawMesh
// and related paths. Layout: [mesh * 15000 + vert] * float[3] = 12 bytes stride.
// Capacity: 32 mesh/frame slots × 15000 verts × 12 B = 5.76 MB.
// BoneVertBufBase is the base (slot 0, vert 0). BoneVertOutBase is a Ghidra label at
// +8 B (the output start used by Skeleton_Transform's pfOut). Both resolve via macros
// (see globals.h) to DWORD lvalues at the correct offsets within this buffer.
char     g_BoneVertexBuf[32 * 15000 * 12] = {0};  // 5,760,000 bytes
char     GuildNameCacheA[0x50] = {0};
char     GuildNameCacheB[0x50] = {0};
char     GuildNameCacheC[0x50] = {0};

// GuildInfoTable, EntitySlotCount, Hero, FrustrumY/228 — defined above

// Temp bone position buffers
DWORD    TempBoneMatrixA[12] = {0};
DWORD    TempBoneMatrixB[12] = {0};
DWORD    TempBoneMatrixBufferC[12] = {0};
DWORD    MoveJointTempMat[12] = {0};

// Animation distance / frequency constants

// Teleport-anim pool — 100 slots × 0x70 bytes; see globals.h note (was 1 byte
// causing heap corruption in Entity_TeleportAnim).
DamagePoint DamagePointPool[kDamagePointCount] = {};

// Character/effect update pool — 1002 slots × 444 bytes = 0x6c660 (matches
// binario original 0x07c85890..0x07cf1ef0). CreateSprite (Effect_Spawn) necesita
// el pool real para spawnear (glow +9, wing FX, weapon glows, lightning crackles).
char     EffectPoolArray[kSpritePoolSize * OBJECT_STRIDE] = {0};
// LegacySigilPoolOff4 = mismo pool, offset +4 (Sound_Queue.cpp / Sigil_RenderAll).
// Lo dejamos como referencia al int en pool[4..7] para compartir storage.
int&     LegacySigilPoolOff4 = *reinterpret_cast<int*>(&EffectPoolArray[4]);

// Math / animation constants
float   TeleportZOffset = 140.0f;
DWORD    TeleportZOffsetBits = 0;
// ConvertXScale — X-scale constant (1/640); definido arriba con valor correcto.
// Esta definición fue 0.0f por error; reemplazada por la inicialización en la zona de video-scale.
float   TerrainHalfWidthNear = 540.0f;
float   TerrainHalfWidthFar = 1190.0f;

// Server select input
DWORD    ServerLocalSelect = 0;

// Char menu UI builder (RenderHelpWindow)
int      HelpWindowMode = 0;
int      MenuF1ItemType = 0;
char     TextListStorage[3000] = {};  // 30 slots * 100 bytes
int      TextListColor[30] = {};  // TextListColor - 30 slots, igual que lpString
int      TextBold[30] = {};  // TextBold      - 30 slots, igual que lpString
// LegacyClassNameA / LegacyClassInfoA / LegacyClassNameB / LegacyClassInfoB / LegacyClassNameC son
// GlobalText[120] / [121] / [140] / [141] / [160] (ver el bloque de alias al final
// de globals.h).
std::uintptr_t ItemAttributeBase = 0; // Base de atributos primarios de items.
// El backup de ItemAttributeBase está en Render/Render_Frame.cpp y no acá: junto a
// ItemAttributeBase lo pisaba el mismo escritor (dos ints consecutivos).
// IDA: FontHeight (0x07D78080) vive en CFont.
// SkillHelpReqR0C2/534/53c/540 pasaron a ser macros sobre CharStatsTable (ver globals.h):
// en el binario son COLUMNAS de la misma tabla, no globals sueltos.
// UI text strings
char     UiTextStr00[] = "";
char     UiTextStr01[] = "";
char     UiTextStr02[] = "";
char     UiTextStr03[] = "";
char     UiTextStr04[] = "";
char     UiTextStr05[] = "";
char     UiTextStr06[] = "";
char     UiTextStr07[] = "";
char     UiTextStr08[] = "";
char     UiTextStr09[] = "";
char     UiTextStr10[] = "";
char     UiTextStr11[] = "";
// RenderItemInfo string constants
// Los SIETE son "\n" en el binario (leídos en 0x0055A4E0, 0x0055A4E4, 0x0055A570,
// 0x0055A5F4, 0x0055A5F0, 0x0055A5FC, 0x0055A640) — líneas separadoras de MEDIA
// altura, que es lo que cuenta SkipNum (SkipNum).  No dejarlas vacías:
// DrawItemInfoBox corta el conteo en la primera línea vacía.
char     FormatNewlineAfterPrice[] = "\n";  // 0x0055A4E4 — separador tras la linea de precio
char     FormatNewlineAfterName[] = "\n";  // 0x0055A570 — separador tras el nombre del item
char     FormatNewlineItemName[] = "\n";  // 0x0055A4E0 — separador de media altura (slot 0)
char     FormatNewlineStatsHeader[] = "\n";  // 0x0055A5F4 — separador de media altura
char     FormatNewlineRenderItem[] = "\n";  // 0x0055A5F0 — separador de media altura (RenderItemInfo)
char     FormatNewlineRepairInfo[] = "\n";  // 0x0055A5FC — separador de media altura (RenderRepairInfo)
char     FormatNewlineRepairFinal[] = "\n";  // 0x0055A640 — separador de media altura (RenderRepairInfo, final)
char     FmtSxSxSStr[] = "";    // s__s__s format
char     SecondaryStatsStr[] = "";    // secondary stats line
// LegacyItemLevelFmt (item level line format) es GlobalText[238] (ver el bloque de alias
// al final de globals.h).

// Weather particle system (MoveLeaves): WeatherParticleBase is the +0x20 alias
// of SkillEffectPool, declared in globals.h; it has no standalone storage.
DWORD    RainYOffset  = 0;
DWORD    RainZCounter  = 0;
int     RainWindSpeed  = 0;
float   AlphaFadeStep  = 0.05f;
float   WeatherFlyMaxDist2  = 200000.0f;
float   WeatherAngleFlip  = 3.2f;
float   RainSinFreq  = 0.0005f;

// Mouse hover tick (Mouse_UpdateHoverTargets)
DWORD    SelectXF  = 0;
DWORD    SelectYF  = 0;
float    TerrainPickPosX  = 0.0f;
float    TerrainPickPosY  = 0.0f;
float    TerrainPickPosZ  = 0.0f;
int      CursorFrameIndex  = 0;
// Hover targets: -1 = none. Mouse_Hover (Mouse_UpdateHoverTargets) resets each frame to -1
// before priority-probing. En login/char-select Mouse_Hover no corre, así que
// el valor inicial debe ser -1 para que RenderCursor (Cursor_Render) muestre
// el cursor arrow por defecto en vez del item-cursor (bitmap 5 = mano abierta).
int      SelectedItem       = -1;  // DAT_00559c48
int      SelectedNpc        = -1;  // DAT_00559c4c
int      SelectedCharacter  = -1;  // DAT_00559c50
int      SelectedOperate    = -1;  // DAT_00559c54
int      Attacking  = -1; // IDA: Attacking (0x00559C58)
// m_bAutoAttack default = 1 (enabled), como en IDA. Per IDA Mouse_Hover
// (sub_4B0310:85), si !m_bAutoAttack el hover-target (DAT_00559c50 /
// SelectedCharacter) se resetea a -1 cada frame ANTES de que el click handler lo
// lea → el click sobre un mob cae al handler de click en el suelo.
char     m_bAutoAttack  = 1;
int      QKeyItemType  = 0;
int      WKeyItemType  = 0;
int      EKeyItemType  = 0;
int      HoverAttackTarget  = 0;
int      HoverTargetIndex  = 0;
DWORD    MouseUpdateTimeMax  = 0;
// MouseRButton already defined at line ~652
DWORD    MouseRButtonPush  = 0;
// Lista `Operates` (0x083A2370): objetos interactuables del mundo -- sillas,
// bancos, barandas y los orbes de Noria.  200 entradas de 12 bytes:
//   [0] activo (byte)   [2] puntero al objeto (DWORD)
// La llena `sub_4FF580` desde CreateObject y la lee el picker `sub_4B0240`.
// `Operates` de IDA (0x083A2378) es el campo [2] de la entrada 0 -- ver el
// alias en globals.h.
char     OperateEntryPool[0x960]  = {};   // 200 x 0xc (0x083A2370..0x083A2CD0)
// Pool de boids (fish/butterfly/bird flocking) @ 0x083a2e90.
// Particle_PathUpdate itera 10 entries × stride 0x1bc = 0x1180 bytes.
char     FishObjectPool[10 * OBJECT_STRIDE] = {};
// SpecialObjPtrTable es ahora un alias dentro de OperateEntryPool (ver globals.h).

// Server select + char menu new globals
char     ServerIpString[64] = {};
DWORD    ServerSelectLogLabel = 0;

// Sprite entity pool (RenderParticles)
DWORD    SpriteEntityPoolBase = 0;
float   SpriteSheetColStep = 0.25f;
float   EntitySinPeriod = 0.02f;   // Entity_UpdateRender sin period scale (case 0x15d)
float   EntitySinBias = 0.65f;   // Entity_UpdateRender sin bias
float   EntitySinAmplitude = 0.35f;   // Entity_UpdateRender sin amplitude
float   AnimSpeedA = -0.02f;
float   AnimSpeedB = 0.02f;

// UI_InGameMenu state machine (UI_InGameMenu)
DWORD    DialogTextId  = 0;
DWORD    DialogPageNum  = 0;
char     DialogPageMax  = 0;
DWORD    DialogAnswerCountA  = 0;
DWORD    ChaosMixCategory  = 0;
// LegacyDlgBtnRectX — ahora macro dentro de DialogButtonRects (dialog button rects)
DWORD    MessageBoxLineCount  = 0;
// see comment near LegacyMsgBoxLine1 — sized as 7×0x26 message-box-custom buffer.
char     MessageBoxCustomBuf[7 * 0x26] = {0};
// LegacyNoCharNameStr (fallback char name string) es GlobalText[609] (ver el bloque de
// alias al final de globals.h).
char     Fmt_IntSpStr[] = "%d %s";
DWORD    g_iCurrentDialogScriptStorage  = 0;
int      InputGold  = 0;
DWORD    ShopContextA  = 0;
DWORD    ShopContextB  = 0;
DWORD    ShopContextC  = 0;
char     MultiSelectResultA  = 0;
DWORD    InventoryItemSelectState  = 0;
DWORD    m_nMyTradeWait  = 0;
DWORD    StorageGoldFlag  = 0;
DWORD    ShopItemSelectionC  = 0;

// ChatDisplayToggle = byte_559BF1 = toggle "Ver chat on/off" (tecla F2).
// Default IDA = 1 (verificado: byte en 0x559BF1 = 0x01). Con 0, el chat normal
// (canal 3) se descarta en DOS lugares:
//   - ChatLB_AddText:    `else if (kind == 3) return;`  → ni se agregaba a la lista
//   - ChatLB_renderLine: `if (!ChatDisplayToggle && msgType==3) return 0;` → no se dibujaba
DWORD    ChatDisplayToggle  = 1;
DWORD    ChatFirstVisibleSlot  = 0;
char     LastSentChatBuf[0x100] = {};

// Chat ring buffer (UI_RenderChatLogOverlay renderer, UIChatLogWindow_AddText writer).
// WhisperActiveFlag, WhisperMsgType y WhisperPosX son ALIASES a offsets 0x0B / 0x10C /
// 0x114 del slot 0 dentro de este mismo buffer en el binario original (Ghidra los
// recuperó como globales independientes): son macros en globals.h que resuelven
// al byte/DWORD real del buffer, para que writer y reader vean la misma memoria.
char     WhisperBubblePool[0x77 * 0x118]  = {0};
DWORD    PageUpDebounce  = 0;
DWORD    PageDownDebounce  = 0;
DWORD    UpArrowDebounce  = 0;
DWORD    DownArrowDebounce  = 0;
DWORD    BKeyDebounce  = 0;
DWORD    RKeyDebounce  = 0;
char     ItemSelectMenuFlag  = 0;
DWORD    TooltipPosX  = 0;
DWORD    TooltipPosY  = 0;
// Macro hotkey table — 10 slots × 0x100 bytes.
// OpenMacro escribe a [0x07e0ffc8 .. 0x07e109c8] = 2560 bytes.
char     MacroText[10 * 0x100] = {};
char     PlayerNameString  = 0;
DWORD    NameUtilityPtrA  = 0;
DWORD    NameUtilityPtrB  = 0;
char     TextInputBuffer  = 0;
// WhisperCommandText / PvpCommandText / GmCommandText son GlobalText[264] / [265] / [260].
// Chat command parser name buffers (Chat_ValidateCommandName): LegacyChatNameBuf1 /
// LegacyChatNameBuf2 / LegacyChatNameBuf3 / LegacyChatNameBuf4 / LegacyChatNameBuf5 / LegacyChatNameBuf6 /
// WhisperTargetText / PvpToggleMessage son GlobalText[258] / [259] / [256] / [254] / [248] /
// [249] / [267] / [268]. (Ver el bloque de alias al final de globals.h.)
DWORD    WhisperBlockFlag  = 0;
char     BKeyToggleGuard  = 0;
DWORD    lpDefault_00583d88 = 0;
// LegacyChatLogMsg es GlobalText[593] (ver el bloque de alias al final de globals.h).
DWORD    ChaosMixItems  = 0;
char     RepairEnableFlag  = 0;   // RepairEnable_0

// ── Chat / UI countdown timers ────────────────────────────────────────────────
int      NoticeTime  = 300;
int      ChatScrollTimer  = 0x96;
// PeriodicNoticeText = byte_7E11DD0: buffer de TEXTO del aviso periódico.
// MoveNotices (IDA 0x47FCB0) hace cada 300 frames `CreateNotice(byte_7E11DD0, 0)`,
// y CreateNotice hace `lstrlenA` + `strcpy` sobre él (hasta 256 bytes).
// Tiene que ser un buffer propio (no un char suelto). Zero-init → aviso vacío (no
// se dibuja) hasta que se porte el handler que lo llena (probablemente el F3/E6
// periódico del server, que trae los textos de evento tipo "Devil Square").
char     PeriodicNoticeText[256] = {0};
// NoticeTextArg = strText (0x07E11DD8) y NoticeTitleArg = byte_7E11DDC: los dos
// argumentos de la llamada periodica de Chat_TickMessageTimer (0x480950).
// En TODO el binario cada uno tiene UN SOLO xref, que es justamente esa
// lectura: nadie los escribe nunca, o sea son cadenas vacias permanentes.
// Eso es a proposito -- ver la nota en Chat_TickMessageTimer.
//
// Tienen que ser BUFFERS, no un char suelto: se pasan como `const char*` y se
// recorren con strlen, asi que un unico byte no garantiza terminador propio y
// la lectura se mete en el global que el linker haya puesto al lado (igual que
// PeriodicNoticeText).  Los tamanos son los huecos reales del binario: dd8..ddc =
// 4 bytes, ddc..de8 = 12.
char     NoticeTextArg[4]  = {0};
char     NoticeTitleArg[12] = {0};
// Event NPC admission limits.  Populated by the server's C1:8E / C1:8F
// packets; the arrays mirror the original client layout used by
// RenderEventWindow (0x004F3C50).
int      m_iDevilSquareLimitLevel[4][2] = {};
int      m_iBloodCastleLimitLevel[12][2] = {};
// Chat ring buffers
// SystemChatBuffer: el loop de IDA en UI_RenderNotices recorre 6 slots × 0x108 stride.
char     SystemChatBuffer[6 * 0x108]  = {0};   // system chat buffer (6 slots × 0x108)
// NoticeSlotFlags was the flag byte alias inside slot 0 (+0x104). Now resolves
// to SystemChatBuffer[0x104] via macro in globals.h.
int      SystemChatCount  = 0;
int      ChatLogCount  = 0;
// Text-extent work vars
// GLOBAL PARTIDO EN DOS (2026-10-03, issue #75): en el binario esto es UN
// `SIZE TextSize` (0x07E113D0).  El port lo habia partido en dos globals, y
// `UI_RenderInputField` llama GetTextExtentPointA con `(LPSIZE)&lpsz_07e113d0`,
// o sea escribe 8 bytes desde ahi.  El linker los coloco al reves
// (TextExtentCy en 0x...108 y lpsz_07e113d0 en 0x...10C), asi que el `cy`
// caia sobre el global SIGUIENTE -- `WhisperID_Num`, que quedaba con la altura
// del texto y luego indexaba WhisperRegistID (10 entradas) fuera de rango.
// Pasaba en cada render del campo de login: de ahi que el cliente muriera
// siempre en el primer frame con el panel de credenciales.
SIZE     g_TextExtent07E113D0 = {0, 0};

int&    TextExtentCy  = *reinterpret_cast<int*>(&g_TextExtent07E113D0.cy);
DWORD    CaretBlinkCounter  = 0;
// Tabla de 10 punteros del buffer de composición del IME: WinMain la escribe con
// `slot * 4` (slot clampeado a 0..9) y Chat.cpp la lee como `LPCSTR*`.
char     DbcsHighBytes[10 * 4] = {0};
// String constants
char     PlayerListMsgFmt  = 0;
char     FmtSDashSStr  = 0;
char     PlayerListAltFmtStr  = 0;

// ── Guild system (opcodes 0x90-0x99) ─────────────────────────────────────────
// InventoryOpened — defined above (char)
// CharacterOpened — defined above (char)
// GoldenArcherOpenType — defined above (DWORD)
int      GoldenArcherItemCount  = 0;
// StorageGoldFlag — defined above (DWORD)
BYTE     GoldenArcherLuckyNumberText[64] = {};
char     GoldenArcherLuckyNumberTicket  = 0;
// LegacyGuildCreateMsg1 / LegacyGuildCreateMsg2 / LegacyGuildCreateMsg3 / LegacyGuildCreateMsg4 / LegacyGuildCreateMsg5 /
// LegacyGuildJoinOkMsg son GlobalText[677] / [678] / [686] / [687] / [679] / [850]
// (ver el bloque de alias al final de globals.h).
char     Fmt_PartyJoinError = 0;
// LegacyGuildReqOkMsg es GlobalText[643] (ver el bloque de alias al final de globals.h).
char     Fmt_PartyRequestError = 0;
DWORD    GoldenArcherLuckyNumber = 0;
WORD     GuildTargetTileY  = 0;
// InputTextMax — defined above (int&)
// InputEnable  — defined above (DWORD)
// InputNumber  — defined above (DWORD)
// Guild UI state
// MessageBoxLineCount  — defined above (DWORD)
// MessageBoxCustomBuf  — defined above (char[7 * 0x26])
DWORD    DialogButtonRects[10] = {};
// InGameUiState  — defined above (DWORD)
// InGameUiStateNext  — defined above (DWORD)
int      GuildMemberCount  = 0;
int      GuildUiAuxParam  = 0;
// Tabla de miembros de guild (UI_GuildLegacy): array de registros de 0x18 bytes:
//   +0x00 name[10]  (DWORD+DWORD+WORD)   +0x0C, +0x10, +0x14  DWORDs
// `GuildMemberList_Set` copia `count * 0x18` bytes desde el paquete y el render
// lee `base + iMod*0x18`.  El hueco real en el binario va de 0x083A7AF8 al
// siguiente global conocido (0x083A7C00) = 0x108 bytes = 11 entradas.
BYTE     GuildMemberTable[GUILD_MEMBER_TABLE_BYTES] = {0};
// LegacyGuildPanelTitle es GlobalText[647] (ver el bloque de alias al final de globals.h).
// LegacyGuildColHeaders / LegacyGuildColHeadLast son GlobalText[680] / [685] (ver el bloque de alias
// al final de globals.h).
char *   PTR_DAT_005618a0  = nullptr;
char     GuildWinTitleNoGuild[300] = {};
char     GuildWinSubtitleNoGuild[300] = {};
char     GuildWinTitle[300] = {};
char     GuildWinSubtitle[300] = {};
char     Fmt_ScoreTotal  = 0;
char     Fmt_OpponentDeaths  = 0;
char     Fmt_OwnKills  = 0;

// ── Pre-compile missing globals (found by diff scan) ─────────────────────────
float    FloatZero = 0.0f;
char     DebugLogDataBlock[256]  = {};   // debug log format/data block
char     g_tex_ext_mode  = 1;         // compressed-assets flag (alias: g_tex_ext_mode); 1=Data mode (plain Data\ folder, no pak)
DWORD    LastFpsTickMs = 0;         // FPS tick timer (DWORD milliseconds)
char     ButterflyObjectPool[10 * OBJECT_STRIDE]  = {};  // Butterfles OBJECT array (10 entries × 0x1BC stride = 0x1158 bytes)
// Float aliases for view/projection matrix — MUST alias the array slots
// (see CameraMatrix[12] above). Definidos como referencias para compartir
// memoria con CameraMatrix[3/7/11] y evitar stomping.
float&   CameraTranslateX = *reinterpret_cast<float*>(&CameraMatrix[3]);
float&   CameraTranslateY = *reinterpret_cast<float*>(&CameraMatrix[7]);
float&   CameraTranslateZ = *reinterpret_cast<float*>(&CameraMatrix[11]);
// _CameraRayOriginX..428c — float aliases sobre el mismo array DWORD que escribe
// Camera_MouseRay (ver CameraRayOriginX_arr arriba): las lecturas vía _DAT_xxx y
// las escrituras vía DAT_xxx (DWORD) tienen que ver la misma memoria.
float&   _CameraRayOriginX = *reinterpret_cast<float*>(&CameraRayOriginX_arr[0]); // _CameraRayOriginX
float&   _CameraRayOriginY = *reinterpret_cast<float*>(&CameraRayOriginX_arr[1]); // _CameraRayOriginY
float&   _CameraRayOriginZ = *reinterpret_cast<float*>(&CameraRayOriginX_arr[2]); // _CameraRayOriginZ
float    PerspectiveScaleX = 0.0f;
float    PerspectiveScaleY = 0.0f;
float    ScreenCenterYFlip = 0.0f;  // projection center Y
// Bone transform scratch buffer (3x4 row-major matrix, 12 floats / 48 bytes).
// Must be CONTIGUOUS — AngleMatrix/BMD_Animation write all 12 floats starting
// at &RootBoneMatrix assuming array layout. Defining the three DAT_ names as
// separate floats would let MSVC scatter them anywhere → memory corruption.
// Instead: one backing array, individual names as references at offsets 0/5/10.
float    RootBoneMatrixStorage[12] = {0};
float&   RootBoneMatrix  = RootBoneMatrixStorage[0];   // matrix[0][0] @ +0x00
float&  GfxMatrix11  = RootBoneMatrixStorage[5];   // matrix[1][1] @ +0x14
float&  GfxMatrix22  = RootBoneMatrixStorage[10];  // matrix[2][2] @ +0x28
// ── Bone normal-intensity pool ───────────────────────────────────────────────
// Per-normal scalar intensity used for per-vertex lighting.
// Layout: [mesh * 15000 + normal] * float = 4 bytes stride.
// Capacity: 32 mesh slots × 15000 normals × 4 B = 1.92 MB.
// IntensityTransformBuf resolves via macro (globals.h) to a DWORD lvalue at offset 0.
char     g_BoneNormalBuf[32 * 15000 *  4] = {0};  // 1,920,000 bytes
// BoneVertOutBase is an alias label 8 bytes into g_BoneVertexBuf (see globals.h).
// CharMenu_Build sub-function globals
int      SkipNum  = 0;
char     SkillReqMetFmt[64] = {};
char     SkillReqNotMetFmt[64] = {};
// Tabla de requisitos de stats @ 0x07E91528 — 12 filas x 10 ints (480 bytes).
// sub_4C2E20 escribe `dword_7E91528[10*i]`, `dword_7E91530[10*i]`, etc. (paso de
// fila = 40 bytes) y sub_4C2C10 la recorre con `v9 += 10`.
int      CharStatsTable[12 * 10] = {};
// LegacySkillDescTable es GlobalText[161] (ver el bloque de alias al final de globals.h).
int      CharMenuBuiltClassId  = -1;

// UI_StatsPanel (RenderErrorMessage) globals
float   EntityPanelBtnWidth = 35.0f;
float   StatsPanelXOffset = 213.0f;
// m_dwTextColor — defined above (DWORD); using 0xffffffff as initial value there
// SetBackgroundTextColor — defined above (DWORD)
// LegacyInputTextWidth — defined above (DWORD)
// m_bAutoAttack — defined above (char)
char     m_bWhisperSound  = 0;
// LegacyClassNameTable es un alias de GlobalText (ver globals.h): se recorre con
// `&LegacyClassNameTable + i * 300` y sus dos lectores usan índices ~601-607 (nombres
// de clase).
// LegacyLoginAccountFmt es GlobalText[397] (ver el bloque de alias al final de globals.h).
char     PartyRequestFormatA[300] = {};
char     SecondPasswordErrorFormat  = 0;
char     PartyRequestFormatB[300] = {};
// LegacyDuelOfferFmt es GlobalText[429] (ver el bloque de alias al final de globals.h).
char     TradeDuelRequestLine1  = 0;
char     TradeAcceptText  = 0;
char     DuelAcceptText  = 0;
char     VendorItemNameList  = 0;
char     NpcVendorInfoLineA  = 0;
char     NpcVendorInfoLineB  = 0;
char     NpcEntry0LineA  = 0;
char     NpcEntry0LineB  = 0;
// DialogButton0Width — ahora macro dentro de DialogButtonRects (dialog button rects)
char     DialogAnswerBuffer[10][1][38] = {};   // g_lpszDialogAnswer (10 x 38 = 380)
// DialogPageNum — defined above (DWORD)
// DialogPageMax — defined above (char)
// DialogAnswerCountA — defined above (DWORD)
// MouseLButtonPush — defined above (DWORD)
// InvCursorBoundXMin — defined above (float)
// Options submenu (0x96) toggle labels — rendered by UI_StatsPanel RenderErrorMessage
// L152-175. El render llama crt_sprintf(buf, Txt_AutoAttackOn/Txt_WhisperOn...) sin argumentos, por
// lo que el string debe ser literal (sin %s). Los 4 slots corresponden a los
// toggles m_bAutoAttack (m_bAutoAttack) y m_bWhisperSound (m_bWhisperSound), NO
// son master-sound/master-music toggles:
//   m_bAutoAttack    — auto-ataque en combate (Combat.cpp / Player_InputTick)
//   m_bWhisperSound  — notificación sonora de whisper (Font_Text.cpp config bits)
char     Txt_AutoAttackOn[32]  = "Auto-ataque : On";
char     Txt_AutoAttackOff[32] = "Auto-ataque : Off";
char     Txt_WhisperOn[32]  = "Susurros : On";
char     Txt_WhisperOff[32] = "Susurros : Off";
char     StatsPanelLabel[32] = {};
// In the original binary these are 300-byte slots inside a GlobalText[] pool
// loaded from Data/Local/Text.bmd by sub_479830 (@0x00479830).
// Los placeholders en español son fallback — OpenTextData() los sobrescribe
// con GlobalText[381..385,388] al arrancar (ver src/Local/Text_Data.cpp).
// Layout: stride 300 (0x12c).
//
// Orden real verificado en UI_StatsPanel.cpp RenderErrorMessage state 0x6e:
//   07d45ba0 → SIEMPRE button 0 (Y=0x41 center)           = "Salir del juego"
//   07d45ccc → button 1 en charselect/ingame (Y=0x5f)     = "Ir a otro servidor"
//   07d45df8 → button 2 en ingame (Y=0x7d)                = "Ir a otro personaje"
//   07d46050 → button {3/2/1} ingame/charsel/login (Y variable) + título 0x96 = "Opciones"
//   07d45f24 → SIEMPRE último (final Y=0x7d/0x9b/0xb9)    = "Cancelar"
//   07d463d4 → estado 0x96 slot 3 (Y=0x7d)                = "Cancelar" (close submenu)
char     DlgText6E_0[300] = "Salir del juego";         // 0x6e btn 0 (Exit)
char     DlgText6E_1[300] = "Ir a otro servidor";      // 0x6e btn 1 ingame/charsel
char     DlgText6E_2[300] = "Ir a otro personaje";     // 0x6e btn 2 ingame
char     DlgText6E_3[300] = "Cancelar";                // 0x6e último (Close)
char     DlgText6E_4[300] = "Opciones";                // 0x6e btn Options + título 0x96
char     DlgText96_3[300] = "Cancelar";                // 0x96 submenu close
char     DlgText7A[300] = {};
char     DlgText7B[300] = {};
char     DlgText73_0[300] = {};
char     DlgText73_1[300] = {};
char     DlgText36_0[300] = {};
char     DlgText36_1[300] = {};
char     DlgText37[300] = {};
char     DlgText72_1[300] = {};
char     DlgText84_0[300] = {};
char     DlgText84_1[300] = {};
char     DlgText8A[300] = {};
char     DlgText71[300] = {};
char     DlgText64[300] = {};
char     DlgText65[300] = {};
char     DlgText1A_0[300] = {};
char     DlgText1A_1[300] = {};
char     DlgText15[300] = {};
char     DlgText1B[300] = {};
char     DlgText1C[300] = {};
char     DlgText1D[300] = {};
char     DlgText1E[300] = {};
char     DlgText1F[300] = {};
char     DlgText20[300] = {};
char     DlgText16[300] = {};
char     DlgText17[300] = {};
char     DlgText18[300] = {};
char     DlgText19[300] = {};
char     DlgText74_0[300] = {};
char     DlgText74_1[300] = {};
char     DlgText74_2[300] = {};
char     DlgText75[300] = {};
char     DlgText76[300] = {};
char     DlgText7F[300] = {};
char     DlgText7E_0[300] = {};
char     DlgText7E_1[300] = {};
char     DlgText25[300] = {};
char     DlgText85[300] = {};
char     DlgText26[300] = {};
char     DlgText3A_0[300] = {};
char     DlgText3A_3[300] = {};
char     DlgText86[300] = {};
char     DlgText87[300] = {};
char     DlgText88[300] = {};
char     DlgText98_0[300] = {};
char     DlgText98_1[300] = {};
char     DlgText89[300] = {};
char     DlgText22[300] = {};
char     DlgText23[300] = {};
char     DlgText24[300] = {};
char     DlgText8F[300] = {};

// ── GL_State cached state ─────────────────────────────────────────────────────
int     BlendModeCache = 0;
char    AlphaTestEnableCache = 0;
char    Texture2DEnableCache = 0;
int     ScreenshotActiveFlag = 0;
int     ScreenshotCounter = 0;
// ── Float constants ───────────────────────────────────────────────────────────
float   FloatZeroB = 0.0f;
float   WeatherMaxAltitude = 400.0f;
float   Math_DegreesToRadians = 0.017453292f;  // π/180
float   EulerHalfFactor = 0.5f;          // half-angle factor for EulerToQuat (EulerToQuat).
                                       // IDA sub_4FA1D0 shows literal `a1[k] * 0.5` — quaternion
                                       // half-angle. Input Euler angles are already in RADIANS
                                       // (AngleMatrix path uses Math_DegreesToRadians=π/180, different const).
                                       // No usar π/180 acá: deja toda rotación de hueso ≈ 0.
float   QuatOne = 1.0f;          // 1.0 (quaternion normalization)
float   HalfPi = 1.5707963f;    // π/2 (SLERP degenerate)
float   SlerpEpsilon = 0.001f;        // SLERP near-parallel epsilon
// ── Effect pool ───────────────────────────────────────────────────────────────
// Effect pool — Effect_TickAll itera 200 slots × 0x1bc bytes.
char    EffectPoolBase[kEffectPoolSize * OBJECT_STRIDE] = {};   // IDA: 200 slots: (0x07B27150-0x07B11670)/0x1bc (IDA bound unk_7B27178 = poolEnd+40)
// ── Monster_Data string literals ─────────────────────────────────────────────
char    Path_MonsterSetBase2Txt[] = "Data2/MonsterSetBase2.txt";
char    Dir_Data2Monster[] = "Data2/Monster/";
char    Smd_SwordBasic[] = "swordbasic.smd";
char    MonsterSmdName147 = 0;
char    MonsterSmdName148 = 0;
char    MonsterSmdName149 = 0;
char    MonsterSmdName14A = 0;
char    MonsterSmdName14B = 0;
char    MonsterSmdName14C = 0;
FILE   *BmdFileHandle = nullptr;
char    FopenModeRb[] = "rb";  // binary read mode string at 0x005580ac
// bBuxCode @ 0x00558090 — la clave XOR de 3 bytes de BuxConvert_1 (0x401120),
// la que descifra Quest.bmd.  Leida del binario: FC CF AB — la misma que usa
// BuxConvert_0 (BuxConvertXorKeyA), pero es otra copia en otra direccion.
// BuxConvert_1 indexa `(&bBuxCode)[i % 3]`, así que tienen que ser 3 bytes
// reales: con un escalar el script de quests queda sin descifrar.
// IDA: bBuxCode (0x00558090)
char    bBuxCode[3] = { (char)0xFC, (char)0xCF, (char)0xAB };
char    TextParserTokenString[256] = {}; // DAT_07CF1EF0 — GetToken buffer (0x47A1F0)
char    SentinelEndString = 0;
int     MonsterSpawnSlotIndex = 0;
char    Dir_DataMonster[] = "Data/Monster/";
char    Dir_Monster[] = "Monster";
char    Dir_MonsterSlash[] = "Monster/";
// ── Monster sound filename strings ───────────────────────────────────────────
char    MonsterSoundFileName125 = 0;  char    MonsterSoundFileName124 = 0;
char    MonsterSoundFileName123 = 0;  char    MonsterSoundFileName122 = 0;
char    MonsterSoundFileName121 = 0;  char    MonsterSoundFileName120 = 0;
char    MonsterSoundFileName119 = 0;  char    MonsterSoundFileName118 = 0;
char    MonsterSoundFileName117 = 0;  char    MonsterSoundFileName116 = 0;
char    MonsterSoundFileName115 = 0;  char    MonsterSoundFileName114 = 0;
char    MonsterSoundFileName113 = 0;  char    MonsterSoundFileName112 = 0;
char    MonsterSoundFileName111 = 0;  char    MonsterSoundFileName110 = 0;
char    MonsterSoundFileName109 = 0;  char    MonsterSoundFileName108 = 0;
char    MonsterSoundFileName107 = 0;  char    MonsterSoundFileName106 = 0;
char    MonsterSoundFileName105 = 0;  char    MonsterSoundFileName104 = 0;
char    MonsterSoundFileName103 = 0;  char    MonsterSoundFileName102 = 0;
char    MonsterSoundFileName101 = 0;  char    MonsterSoundFileName100 = 0;
char    MonsterSoundFileName99 = 0;  char    MonsterSoundFileName98 = 0;
char    MonsterSoundFileName97 = 0;  char    MonsterSoundFileName96 = 0;
char    MonsterSoundFileName95 = 0;  char    MonsterSoundFileName94 = 0;
char    MonsterSoundFileName93 = 0;  char    MonsterSoundFileName92 = 0;
char    MonsterSoundFileName91 = 0;  char    MonsterSoundFileName90 = 0;
char    MonsterSoundFileName89 = 0;  char    MonsterSoundFileName88 = 0;
char    MonsterSoundFileName87 = 0;  char    MonsterSoundFileName86 = 0;
char    MonsterSoundFileName85 = 0;  char    MonsterSoundFileName84 = 0;
char    MonsterSoundFileName83 = 0;  char    MonsterSoundFileName82 = 0;
char    MonsterSoundFileName81 = 0;  char    MonsterSoundFileName80 = 0;
char    MonsterSoundFileName79 = 0;  char    MonsterSoundFileName78 = 0;
char    MonsterSoundFileName77 = 0;  char    MonsterSoundFileName76 = 0;
char    MonsterSoundFileName75 = 0;  char    MonsterSoundFileName74 = 0;
char    MonsterSoundFileName73 = 0;  char    MonsterSoundFileName72 = 0;
char    MonsterSoundFileName71 = 0;  char    MonsterSoundFileName70 = 0;
char    MonsterSoundFileName69 = 0;  char    MonsterSoundFileName68 = 0;
char    MonsterSoundFileName67 = 0;  char    MonsterSoundFileName66 = 0;
char    MonsterSoundFileName65 = 0;  char    MonsterSoundFileName64 = 0;
char    MonsterSoundFileName63 = 0;  char    MonsterSoundFileName62 = 0;
char    MonsterSoundFileName58 = 0;  char    MonsterSoundFileName57 = 0;
char    MonsterSoundFileName56 = 0;  char    MonsterSoundFileName55 = 0;
char    MonsterSoundFileName61 = 0;  char    MonsterSoundFileName60 = 0;
char    MonsterSoundFileName59 = 0;  char    MonsterSoundFileName54 = 0;
char    MonsterSoundFileName53 = 0;  char    MonsterSoundFileName52 = 0;
char    MonsterSoundFileName51 = 0;  char    MonsterSoundFileName50 = 0;
char    MonsterSoundFileName49 = 0;  char    MonsterSoundFileName48 = 0;
char    MonsterSoundFileName47 = 0;  char    MonsterSoundFileName46 = 0;
char    MonsterSoundFileName45 = 0;  char    MonsterSoundFileName44 = 0;
char    MonsterSoundFileName43 = 0;  char    MonsterSoundFileName42 = 0;
char    MonsterSoundFileName41 = 0;  char    MonsterSoundFileName40 = 0;
char    MonsterSoundFileName39 = 0;  char    MonsterSoundFileName38 = 0;
char    MonsterSoundFileName37 = 0;  char    MonsterSoundFileName36 = 0;
char    MonsterSoundFileName35 = 0;  char    MonsterSoundFileName34 = 0;
char    MonsterSoundFileName33 = 0;  char    MonsterSoundFileName32 = 0;
char    MonsterSoundFileName31 = 0;  char    MonsterSoundFileName30 = 0;
char    MonsterSoundFileName29 = 0;  char    MonsterSoundFileName28 = 0;
char    MonsterSoundFileName27 = 0;  char    MonsterSoundFileName26 = 0;
char    MonsterSoundFileName25 = 0;  char    MonsterSoundFileName24 = 0;
char    MonsterSoundFileName23 = 0;  char    MonsterSoundFileName22 = 0;
char    MonsterSoundFileName21 = 0;  char    MonsterSoundFileName20 = 0;
char    MonsterSoundFileName19 = 0;  char    MonsterSoundFileName18 = 0;
char    MonsterSoundFileName17 = 0;  char    MonsterSoundFileName16 = 0;
char    MonsterSoundFileName15 = 0;  char    MonsterSoundFileName14 = 0;
char    MonsterSoundFileName13 = 0;  char    MonsterSoundFileName12 = 0;
char    MonsterSoundFileName11 = 0;  char    MonsterSoundFileName10 = 0;
char    MonsterSoundFileName09 = 0;  char    MonsterSoundFileName08 = 0;
char    MonsterSoundFileName07 = 0;  char    MonsterSoundFileName06 = 0;
char    MonsterSoundFileName05 = 0;  char    MonsterSoundFileName04 = 0;
char    MonsterSoundFileName03 = 0;  char    MonsterSoundFileName02 = 0;
char    MonsterSoundFileName01 = 0;  char    MonsterSoundFileName00 = 0;
// ── Monster s_ named sound strings ───────────────────────────────────────────
char    Wav_Iron1[] = "Data/Sound/iron1.wav";
char    Wav_IronAttack1[] = "Data/Sound/iron_attack1.wav";
char    Wav_Jaikan1[] = "Data/Sound/jaikan1.wav";
char    Wav_Jaikan2[] = "Data/Sound/jaikan2.wav";
char    Wav_JaikanAttack1[] = "Data/Sound/jaikan_attack1.wav";
char    Wav_JaikanAttack2[] = "Data/Sound/jaikan_attack2.wav";
char    Wav_JaikanDie[] = "Data/Sound/jaikan_die.wav";
char    Img_MonsterBv01_2[] = "Monster/bv01_2.jpg";
char    Img_MonsterBv02_2[] = "Monster/bv02_2.jpg";
char    Wav_Blood1[] = "Data/Sound/blood1.wav";
char    Wav_BloodAttack1[] = "Data/Sound/blood_attack1.wav";
char    Wav_BloodAttack2[] = "Data/Sound/blood_attack2.wav";
char    Wav_BloodDie[] = "Data/Sound/blood_die.wav";
char    Wav_Death1[] = "Data/Sound/death1.wav";
char    Wav_DeathAttack1[] = "Data/Sound/death_attack1.wav";
char    Wav_DeathDie[] = "Data/Sound/death_die.wav";
char    Wav_Mutant1[] = "Data/Sound/mutant1.wav";
char    Wav_Mutant2[] = "Data/Sound/mutant2.wav";
char    Wav_MutantAttack1[] = "Data/Sound/mutant_attack1.wav";
char    Wav_OrcArcherAttack1[] = "Data/Sound/mOrcArcherAttack1.wav";
char    Wav_OrcCapAttack1[] = "Data/Sound/mOrcCapAttack1.wav";
char    Wav_CursedKing1[] = "Data/Sound/mCursedKing1.wav";
char    Wav_CursedKing2[] = "Data/Sound/mCursedKing2.wav";
char    Wav_CursedKingDie1[] = "Data/Sound/mCursedKingDie1.wav";
char    Img_MonsterIui02[] = "Monster/iui02.tga";
char    Img_MonsterIui03[] = "Monster/iui03.tga";
char    Wav_Molt1[] = "Data/Sound/mMolt1.wav";
char    Wav_MoltAttack1[] = "Data/Sound/mMoltAttack1.wav";
char    Wav_MoltDie[] = "Data/Sound/mMoltDie.wav";
char    Wav_MegaCrust1[] = "Data/Sound/mMegaCrust1.wav";
char    Wav_MegaCrustAttack1[] = "Data/Sound/mMegaCrustAttack1.wav";
char    Wav_MegaCrustDie[] = "Data/Sound/mMegaCrustDie.wav";
char    Wav_AlquamosAttack1[] = "Data/Sound/mAlquamosAttack1.wav";
char    Wav_AlquamosDie[] = "Data/Sound/mAlquamosDie.wav";
char    Wav_Rainner1[] = "Data/Sound/mRainner1.wav";
char    Wav_RainnerAttack1[] = "Data/Sound/mRainnerAttack1.wav";
char    Wav_RainnerDie[] = "Data/Sound/mRainnerDie.wav";
char    Wav_Phantom1[] = "Data/Sound/mPhantom1.wav";
char    Wav_PhantomAttack1[] = "Data/Sound/mPhantomAttack1.wav";
char    Wav_PhantomDie[] = "Data/Sound/mPhantomDie.wav";
char    Wav_Drakan1[] = "Data/Sound/mDrakan1.wav";
char    Wav_DrakanAttack1[] = "Data/Sound/mDrakanAttack1.wav";
char    Wav_DrakanDie[] = "Data/Sound/mDrakanDie.wav";
char    Wav_Phoenix1[] = "Data/Sound/mPhoenix1.wav";
char    Wav_PhoenixAttack1[] = "Data/Sound/mPhoenixAttack1.wav";
char    Wav_MagicSkull[] = "Data/Sound/mMagicSkull.wav";
char    Wav_BullDie[] = "Data/Sound/mBullDie.wav";
char    Wav_BlackSkullDie[] = "Data/Sound/mBlackSkullDie.wav";
char    Wav_BlackSkullAttack[] = "Data/Sound/mBlackSkullAttack.wav";
char    Wav_GhaintOrgerDie[] = "Data/Sound/mGhaintOrgerDie.wav";
char    Wav_RedSkull[] = "Data/Sound/mRedSkull.wav";
char    Wav_RedSkullDie[] = "Data/Sound/mRedSkullDie.wav";
char    Wav_RedSkullAttack[] = "Data/Sound/mRedSkullAttack.wav";

// ── CreateEffect float constants ────────────────────────────────────────────
float   AngleDeg180 = 180.0f;
float   DegToRad = 0.0174532924f;
float   DeathFrameMax = 9.0f;  // death particle anim frame upper bound
float   EffectAngleRadA = 2.0f;
float   EffectAngleRadB = 2.0f;
// ── Weather particle pool (40 × 0x1bc = 0x4560 bytes) ────────────────────────
// Backing buffer único: Weather_Update recorre los slots 1..39 con stride 0x1bc.
// Ver globals.h para los macros de field accessors.
alignas(16) char g_WeatherSlotPool[40 * OBJECT_STRIDE] = {0};
// ── Weather float constants ───────────────────────────────────────────────────
float   WeatherMinAltitude = 200.0f;
float   AngleDeg360 = 360.0f;
float   WeatherCeiling = 300.0f;
float   WeatherNearDist = 600.0f;
float   WeatherDist1000 = 1000.0f;
float   WeatherDespawnDist = 1500.0f;
float   WeatherOrbitFreq = 0.0003f;

// ── Camera / viewport globals ─────────────────────────────────────────────────
// SetActionObject (0x4FA5C0) / MoveObject_Special (0x4FA5F0): animacion de
// derrumbe de la puerta del evento.  En el binario los cuatro arrancan en -1
// (bytes .data en 0x0055A7B0: FF FF FF FF x3 + 00 00 80 BF) y los guards de
// MoveObject_Special son comparaciones CON SIGNO (`unk_55A7B4 < 0`).
// No inicializarlos en 0: los `< 0` nunca se cumplirían y en el primer frame de
// Lorencia (World == 0 == ActionObjWorldId) cualquier objeto de tipo 0 entraría al
// bloque de derrumbe.
int     ActionObjCollapseType   = -1;   // tipo de objeto que se derrumba
int     ActionObjWorldId   = -1;   // World en el que aplica
int     ActionObjFrameCount   = -1;   // contador de frames (20 = arranca)
float   ViewportHeightF  = -1.0f;// acumulador de velocidad angular

// ── GL_State cache ────────────────────────────────────────────────────────────
char    CullFaceEnableCache   = 0;
char    DepthTestEnableCache   = 0;
float   RayCastClosestT   = 0.0f; // Map_InitRayCast running closest-hit ray-t (audit #7)
DWORD   DepthMaskCache   = 0;

// ── Fog / projection globals ──────────────────────────────────────────────────
float   FogStartDistance   = 0.0f;
DWORD   FogColor   = 0;

// ── Perspective constant ──────────────────────────────────────────────────────
float   PerspectiveFarScale  = 1.4f;

// ── Joint pool ────────────────────────────────────────────────────────────────
// Joint pool — Joint_TickAll itera 500 slots × 0x9d8 bytes (~1.2MB).
char    JointPoolBase[kJointPoolSize * JOINT_STRIDE] = {};   // IDA: 500 slots: (0x07C5AB30-0x07B27150)/0x9d8 = 1260000/2520 (IDA ItemDrop_Render: 0x7B27B08..0x7C5B4E8, ancla +0x9B8)

// ── Joint_Create float constants ─────────────────────────────────────────────
float   NegDegToRad  = -0.0174532924f;

// ── Timer globals ─────────────────────────────────────────────────────────────
DWORD   FpsFrameCountB   = 0;
// IDA: DAT_05826df0
DWORD   FrameTimeCurrentMs   = 0;
float   MsToSeconds  = 0.001f;  // 1/1000 ms-to-s
float   FpsWindowSec  = 5.0f;    // 5.0 second FPS window
// DeltaTimeScale — defined above (float)
DWORD   FpsWindowStartMs   = 0;
float   DeltaT  = 0.0f;    // delta time per frame
// IDA: DAT_05826dfc
DWORD   FrameTimePreviousMs   = 0;
float   FPS  = 0.0f;    // smoothed FPS value

// IDA: m_MusicOnOff (0x055C9E3C) y Mp3FileName (0x055C9D04): CSound.

// ── Sound_DS3D globals ────────────────────────────────────────────────────────
DWORD   DS3DState   = 0;

// ── Net_Connect globals ───────────────────────────────────────────────────────
// IDA: First (0x055CA15C)
DWORD   First   = 0;
char    Err_FailedToConnect[] = "Failed to connect.";

// ── Net_PacketSession globals ─────────────────────────────────────────────────
// LegacyInvGridKey was a single DWORD; FILL_GRID(&LegacyInvGridKey) and the
// Net_PacketSession reset loop both write 0x1100 bytes into it (= 64 slots ×
// 0x44 stride matching the IDA bound 0x7ea9548 - 0x7ea8448 = 0x1100). Sized
// properly to avoid heap corruption when in-game inventory grids fill.
// LegacyInvGridKey: alias de OffsetInventoryItems.Key (globals.h)
// LegacyWarehouseKey: alias del Key del pool del baul (globals.h)
// LegacyMixItemsKey: alias de OffsetMixItems.Key (globals.h)
DWORD   TradeConfirmLatch   = 0;
// LegacyTradeItems: alias de OffsetTradeItems (globals.h)
// MarkColor[16]: `CreateGuildMark` (0x4F0100) escribe los 16 colores y
// `RenderGuildMark` (0x4F02F0) indexa `MarkColor[p5]` con p5 en 0..15.
// El hueco hasta TradeItemList es de 68 bytes, asi que los 16 entran.
DWORD   GuildMarkColors[16] = {0};  // MarkColor[16] — paleta de la marca (ARGB)
BYTE    TradeItemList[0x880] = {0};
// LegacyInventoryKey: alias de Inventory.Key (globals.h)
// LegacyTradeKey: alias de OffsetTradeItems.Key (globals.h)
// TradeItemKeyBase: alias de TradeItemList.Key (globals.h)
DWORD   ChatFocusIdA   = 0;
DWORD   InventorySpecialModeFlag   = 0;

// ── Entity_Init globals ───────────────────────────────────────────────────────
char    BinaryReadModeStr[] = "rb";  // binary read mode string at 0x00559b74
char    Fmt_DualHpSlots[] = "%4d/%4d[0] %4d/%4d[1]";

// ── Scene_CharPreview globals ─────────────────────────────────────────────────
char    PickedItemOptionByte   = 0;  // picked item durability/option byte
char    PickedItemDurExtra   = 0;
// Buffer del item que abrio el dialogo de ShowCheckBox(153) -- el click derecho
// sobre el item 431 (Fruit) le hace memcpy de un ITEM entero (0x44 bytes).
// Estaba declarado como UN DWORD, asi que el memcpy desbordaba 64 bytes sobre
// el BSS vecino y los campos que el dialogo lee (+4 Level, +9 slot, +0x1b
// Option1) caian en globals distintos: PreviewItemLevel quedaba siempre en 0 y el
// cartel decia "Ene" para cualquier fruta.  Rango del binario:
// 0x07EA5240..0x07EA5283 (0x44), justo hasta InventoryStartY.
BYTE    PreviewItemData[0x44] = { 0 };

// ── Effect_Tick globals ───────────────────────────────────────────────────────
// Fade-effect pool — Effect_TickFade itera 40 slots × 0x1bc bytes.
// DAT_07c74ec8 es el anclaje +0x60 de g_PlayerRenderPool, no otro pool.
// Flare effect pool — Effect_TickFlare itera 63 slots × 0x70 bytes.
// IDA: Pointers (0x07C82CD0), 100 slots de 0x70 compartidos por alta/tick/draw.
char    g_PointerPool[100 * 0x70] = {};
// Spark-effect pool — Effect_TickSpark itera 100 slots × 0x70 bytes.
char    SparkEffectPool[100 * 0x70] = {};

// ── Terrain_Light globals ─────────────────────────────────────────────────────
DWORD   LightMapEnableFlag   = 0;
// cb60c/cb610 and 0828b60c/610 are NOT separate globals — in the
// original binary they're the 2nd/3rd DWORDs of slot 0 of cb608/0828b608.
// Code uses `(&PrimaryTerrainLightG)[iVar2*3]` to access slot iVar2's 2nd field, and
// `(char*)&PrimaryTerrainLightG + iVar2*12` to write to it.  We promote them to
// macros that project into the actual buffer.  See globals.h.

// ── Scene_Resources string literals ──────────────────────────────────────────
// Originally at .rdata in the binary; their content must match the actual
// asset file basenames on disk or Scene_LoadAccountResources / ...CharSelectResources
// construct malformed paths (e.g. "Data\\Object1\\01.bmd" instead of "Ship01.bmd").
// Ship/Logo/Face are BMD basenames used by AccessModel;
// las cuatro entradas SMD (fondos / caras de la escena de login) sólo las consume
// OpenModel, que en este port no carga SMD: los nombres quedan como referencia.
char    ModelBaseNameShip[8]   = "Ship";          // → Data\Object1\Ship01.bmd (login ship)
char    ModelBaseNameLogo[8]   = "Logo";          // → Data\Logo\Logo0N.bmd   (login logos 1..4)
char    ModelBaseNameFace[8]   = "Face";          // → Data\Logo\Face0N.bmd   (char-select faces)
char    SmdNameBackground[32]  = "background.smd";  // SMD basename, slot 0xa0 (stub)
char    SmdNameBackground2[32]  = "background2.smd"; // SMD basename, slot 0xa1 (stub)
char    SmdNameSwordsmanFace[32]  = "swordsman_face.smd"; // SMD basename, slot 0xad (stub)
char    SmdNameWizardFace[32]  = "wizard_face.smd";    // SMD basename, slot 0xaa (stub)
char    Img_LogoWebzenlogo[]         = "Logo/Webzenlogo.jpg";
char    Img_LogoTitle[]              = "Logo/Title.jpg";
char    Img_GfxInterface[] = "Interface/GFx/Interface.jpg";
char    Img_GfxInterface2[]         = "Interface/GFx/Interface2.jpg";
char    Img_DataInterface[]                   = "Data/Interface.jpg";
char    Img_DataInterface2[]                  = "Data/Interface2.jpg";
char    Img_Object1[]                  = "Object1/Object1.jpg";
char    Bmd_Warrior[]                          = "warrior.bmd";
char    Bmd_Main[]                             = "main.bmd";
char    Bmd_Fairy[]                            = "fairy.bmd";

// ── Scene_Resources strings ───────────────────────────────────────────────────
char    Img_LogoAccountNew[]       = "Logo\\0Account_new.tga";
char    Img_LogoOnButton[]         = "Logo\\0On_Botton.jpg";
char    Img_LogoOnButton2[]        = "Logo\\0On_Botton2.jpg";
char    Img_LogoTextBox[]          = "Logo\\0Text_Box.jpg";
char    Img_LogoNewAccount01[]     = "Logo\\0New_Account01.tga";
char    Img_LogoNewAccount02[]     = "Logo\\0New_Account02.tga";
char    Img_LogoBox[]               = "Logo\\0Box.jpg";
char    Img_ProgressBack[] = "Interface\\Progress_Back.jpg";
char    Img_Progress[]      = "Interface\\Progress.jpg";
char    Dir_Data2Object1[]              = "Data2\\Object1\\";
char    Smd_Ship[]                    = "ship.smd";
char    Dir_Data2Logo[]                 = "Data2\\Logo\\";
char    Smd_Mu[]                      = "mu.smd";
char    Smd_Sun[]                     = "sun.smd";
char    Dir_DataObject1[]               = "Data\\Object1\\";
char    Dir_DataLogo[]                  = "Data\\Logo\\";
char    Dir_Object1[]                    = "Object1\\";
char    Dir_Logo[]                       = "Logo\\";
char    Img_LogoInterface01[]        = "Logo\\Interface01.tga";
char    Img_LogoInterface02[]        = "Logo\\Interface02.tga";
char    Img_LogoInterface03[]        = "Logo\\Interface03.tga";
char    Img_LogoInterface04[]        = "Logo\\Interface04.tga";
char    Img_NewCharacter201[]   = "Logo\\New_Character201.tga";
char    Img_NewCharacter202[]   = "Logo\\New_Character202.jpg";
char    Img_Delete01[]           = "Logo\\Delete01.tga";
char    Img_Delete02[]           = "Logo\\Delete02.jpg";
char    Img_Ok01[]               = "Logo\\Ok01.tga";
char    Img_Ok02[]               = "Logo\\Ok02.jpg";
char    Img_NewCharacter01[]    = "Logo\\New_Character01.tga";
char    Img_NewCharacter02[]    = "Logo\\New_Character02.tga";
char    Img_NewCharacterCancel[] = "Logo\\New_Character_Cancel.jpg";
char    Img_NewCharacterOk[]   = "Logo\\New_Character_Ok.jpg";
char    Img_NewCharacter001[]   = "Logo\\New_Character001.jpg";
char    Img_NewCharacter002[]   = "Logo\\New_Character002.jpg";
char    Img_NewCharacter003[]   = "Logo\\New_Character003.jpg";
char    Img_NewCharacter004[]   = "Logo\\New_Character004.jpg";
char    Smd_Main[]                    = "main.smd";
char    Smd_Warrior[]                 = "warrior.smd";
char    Smd_Fairy[]                   = "fairy.smd";

// ── Additional misc globals ───────────────────────────────────────────────────
// LastFpsTickMsBits — same address as LastFpsTickMs above; alias defined in globals.h
// Tabla de huesos del brillo de Alquamos (MonsterID 69, RenderCharacter case 'E').
// Leida del binario original en 0x0055984C: 0a 12 25 26 33 34 3a 3b 42.
// Estaba en ceros y nadie la poblaba, asi que las 9 chispas nacian todas
// sobre el hueso 0 en vez de repartirse por el cuerpo.
// Unico consumidor: RenderCharacter (0x456B86).
BYTE    GlitterBoneIndexTable[9] = { 10, 18, 37, 38, 51, 52, 58, 59, 66 };
// LegacyChatBubblePool (el pool de burbujas de chat proyectado a +40) es un macro: ver
// ChatBubblePool más abajo.
// Key-state table para PressKey (PressKey / Key_IsJustPressed).
// La función indexa como `*(DWORD*)((char*)&KeyState + vkey*4)`, o sea
// 256 entradas DWORD (1024 bytes) — una por código VK. En IDA es una tabla
// al símbolo dword_7E118EC. Tiene que ser la tabla completa: cualquier vkey>=1
// indexa más allá de la primera entrada (ESC=27 → +108 bytes).
DWORD   KeyState[256] = {0};
DWORD   F1KeyDebounce   = 0;
DWORD   F2KeyDebounce   = 0;
DWORD   F3KeyDebounce   = 0;
DWORD   F4KeyDebounce   = 0;
DWORD   ChatBlinkCounter   = 0;
DWORD   InventoryStartY   = 0;
DWORD   InventoryStartX   = 0;
DWORD   ItemRenderMode   = 0;
// AmbientParticleAnchor ya no es un array propio: es el ancla del walker de
// Ambient_ParticleUpdate dentro de FishObjectPool (ver globals.h).
float   ClothWindKey  = 0.0f;   // IDA: flt_590AF0, magnitud del viento de la tela

// IDA: g_PhysicsManager (0x083A4338) -- CPhysicsManager de 16 bytes
// (vtable, cantidad, cabeza, cola).  En el binario lo construye el
// inicializador estatico sub_5133F0 con sub_409AD0; aca, el ctor de un
// objeto estatico de este archivo.
DWORD g_PhysicsManager[4] = { 0, 0, 0, 0 };
namespace {
struct PhysicsManagerInit {
    PhysicsManagerInit() { CPhysicsManager_Ctor(g_PhysicsManager); }
} s_PhysicsManagerInit;
}
float   ClothWindSin   = 0.0f;
float   ClothWindCos   = 0.0f;
float   ClothWindZ   = 0.0f;  // cloth wind Z (flt_590AFC; el binario nunca lo escribe)
float   ClothGravityG  = 9.8f;      // gravedad   (flt_559068, leído del binario)
float   ClothFixedDt  = 0.0025f;   // dt fijo    (flt_55906C, leído del binario)
DWORD   TextOutMode = 0;
// Strings reservados de sub_513570 (name-filter). En el binario original son
// patrones bloqueados (espacio, DBCS coreano, punto). No dejarlos en 0: con string
// vacío FindText(nombre,"") devuelve 1 y TODO nombre se rechaza con "palabras
// restringidas". Valores reales de IDA (bytes little-endian): 0x561740=" ",
// 0x561744="\xA1\xA1", 0x561748=".", 0x56174c="\xA1\xA4", 0x561750="\xA1\xAD".
DWORD   ChatFindTextSpace   = 0x20;      // " "
DWORD   ChatFindTextDbcsA1A1   = 0xA1A1;    // "\xA1\xA1" (DBCS)
DWORD   ChatFindTextPeriod   = 0x2E;      // "."
DWORD   ChatFindTextDbcsA1A4   = 0xA4A1;    // "\xA1\xA4" (DBCS)
char    Txt_WebzenCaps[]  = "WEBZEN";
char    Txt_WebzenCamel[]  = "WebZen";
char    Txt_Webzen_A[]  = "Webzen";
DWORD   ChatFindTextDbcsA1AD   = 0xADA1;    // "\xA1\xAD" (DBCS)
char    Txt_Webzen_B[]  = "Webzen";

// ── Chat.cpp missing globals ──────────────────────────────────────────────────
char    InputFieldByte2    = 0;
char    InputFieldBufferByte4    = 0;
DWORD   PinDataBase    = 0;   // PIN data base for char-select second-password
// Word-filter table (banned chat keywords), 1000 entries × 20 bytes = 20000 bytes.
// Loaded at boot by OpenFilterFile("Data/Local/Filter.bmd") — 20000 bytes XOR'd
// with BuxConvert_0 (3-byte key FC CF AB), preceded by a 4-byte ring checksum
// (seed 0x7cfa00, magic 15997).  Empty first byte terminates the valid range.
char    WordFilterTable[20000] = {};
// GlobalText[] — 1000 × 300 byte localized string table, loaded at boot from
// Data/Local/Text.bmd by OpenTextData().  Each row holds one null-terminated
// entry; row index matches the original binary's string IDs.
char    GlobalText[GLOBALTEXT_ROWS][300] = {};   // ver nota en globals.h
// LegacyChatFilterWords/5dc = GlobalText[457]/[458] — ahora macros en globals.h (name-filter).
DWORD   ChatBufferExtended    = 0;
int     NameFilterCount    = 0;         // command table A count (name-filter)
int     WordFilterCount    = 0;         // command table B count (word-filter)
// Name-filter table (banned character names), 1000 entries × 20 bytes = 20000 bytes.
// Loaded at boot by OpenNameFilterFile("Data/Local/FilterName.bmd") — seed 0x578200,
// magic 11201, same BuxConvert_0 XOR cipher.
char    NameFilterTable[20000] = {};
float   LightningSpeed   = 2.5f;     // lightning speed constant

char    lpSubBlock_005592d0[4]    = "\\"; // path separator (DAT_005592d0; sin usos en el código compilado)

// ── Misc.cpp / Entity gravity globals ────────────────────────────────────────
float   GravityMinClamp   = -0.2f;   // gravity min clamp
// Valores leídos del binario:
// 0x5527D0 = 40 C0 00 00 → 6.0f  y  0x552A28 = C1 20 00 00 → -10.0f.
float   ItemVelDecay   = 6.0f;    // MoveItems: decaimiento de la velocidad Z por frame


// ── Terrain map globals (OpenTerrainMapping, OpenObjectsEnc, CreateTerrain) ────────────
// La celda original 0x083A0218 pertenece a g_ObjectBucketGrid[0].
float   HeightNormScale = 0.00392156886f; // height scale

// ── Terrain tile pick / normal buffer globals ─────────────────────────────────
// Estos son ALIASES dentro de g_TilePickBuf[12] declarado arriba para garantizar
// que los 12 floats están contiguos (consumidos como vec3 corners por
// glVertex3fv y por las cross-product helpers).
DWORD   TilePickResultFlag  = 0;   // tile pick result flag
DWORD   TargetSelectMode  = 0;   // combat target select sub-mode

// ── Model slot index globals ──────────────────────────────────────────────────
DWORD   TextureBegin  = 0;
DWORD   TextureCurrent  = 0;

// ── Terrain culling globals ───────────────────────────────────────────────────
short   TerrainHorizonClip = 0;
DWORD   TerrainViewScale = 0;
DWORD   TerrainLodScale = 0;
// MsToSeconds already defined above (terrain: reuse same address)

// ── Model loader format strings ───────────────────────────────────────────────
char    Fmt_BmdName[]    = "%s.bmd";
char    Fmt_BmdNamePadZero[] = "%s0%d.bmd";
char    Fmt_BmdNameNum[]  = "%s%d.bmd";

// ── Item/Skill/Gate data loader globals ──────────────────────────────────────
BYTE*   ItemAttributeShadowTable = nullptr;    // Base de la tabla sombra de atributos de items.
BYTE*   SkillShadowTable = nullptr;    // Base de la tabla sombra de skills.
BYTE*   SkillTableBase = nullptr;    // Base de la tabla de skills reservada al inicio.
GATE_ATTRIBUTE* GateAttribute = nullptr; // Base de la tabla de gates reservada al inicio.
// dialog data array — original ocupaba 0x07cf5608..0x07d27608 (0x32000 bytes).
// Declaramos un buffer real de ese tamaño en DWORDs para que &DialogScriptTable apunte a memoria válida.
DWORD   DialogScriptTable[(DIALOG_SCRIPT_COUNT * 0x400) / 4] = {};   // g_DialogScript (200 x 0x400 = 200 KB)
int     EditMonsterNumber    = 0;       // NPC name count (EditMonsterNumber)
// MonsterScript / LegacyMonsterScript / LegacyMonsterScript1 son la MISMA tabla en 0x07CF2000
// (verificado en IDA: getMonsterName lee `MonsterScript` = 0x07CF2000, y
// NPCName_Load escribe `&LegacyMonsterScript` = 0x07CF2000).  Antes estaban declarados
// como TRES globals de 1 byte separados → el loader escribia en uno y
// getMonsterName leia de otro → nombres de NPC/mob vacios.  Ahora es UNA tabla
// real: N entradas × 0x36 (Type[0], Name[1..32], Level, Attribute).
// Definida abajo como MonsterScript[]; LegacyMonsterScript/2001 son macros a ella.
DWORD   ItemBmdChecksumSeed    = 0x01c5e200;  // item BMD checksum seed A (literal = su propia dirección original)
DWORD   SkillBmdChecksumSeed    = 0x00b43000;  // skill BMD checksum seed B
// BuxConvert_0 indexa (&BuxConvertXorKeyA)[i % 3]: tiene que ser un array de 3 bytes,
// no un escalar (si no, el XOR toma bytes vecinos y rompe el decode de Text.bmd /
// Filter.bmd / Dialog.bmd).
char    BuxConvertXorKeyA[3] = { (char)0xFC, (char)0xCF, (char)0xAB };
// Error message format strings
char    Err_FileNotExist[] = "%s - File not exist.";
char    Err_FileCorrupted[] = "%s - File corrupted.";
// File open mode strings
char    FopenModeWb[] = "wb";  // binary write mode at 0x005597d4

// ── SecondPassword Screen2 entity list globals ────────────────────────────────
// LegacyParty: alias de Party (globals.h)
DWORD   PartyStartXStorage  = 0;   // Screen2 panel origin X
DWORD   PartyStartYStorage  = 0;   // Screen2 panel origin Y

// ── SecondPassword UI sub-handler globals ─────────────────────────────────────
DWORD   PinDialogOriginX  = 0;   // SecondPassword dialog origin X (pixel)
DWORD   PinDialogOriginY  = 0;   // SecondPassword dialog origin Y (pixel)
DWORD   MixState  = 0;   // MixState (0x07EAA140)
DWORD   PinCheckboxState  = 0;   // SecondPassword checkbox/toggle state
DWORD   RepairEnable  = 0; // IDA: RepairEnable (0x07EAA138)
DWORD   TradeInventoryStartX  = 0;   // SecondPassword alt-panel origin X
DWORD   TradeInventoryStartY  = 0;   // SecondPassword alt-panel origin Y
// LegacyPinWrongMsg (error string – case 0 wrong PIN) es GlobalText[580] (ver el bloque
// de alias al final de globals.h).
char    UiMessageLabelA  = 0;   // UI message label A
// LegacyAuthFailMsg (error string – auth fail) es GlobalText[581] (ver el bloque de
// alias al final de globals.h).
char    UiMessageLabelB  = 0;   // UI message label B
// LegacyPinErrorMsg (error string – case 0xfffffff8/0xfffffffe) es GlobalText[600] (ver
// el bloque de alias al final de globals.h).
char    UiMessageLabelC  = 0;   // UI message label C
int     ItemAttrMaxWidth  = 0;   // auth mode param A
int     ItemAttrMaxHeight  = 0;   // auth mode param B

// ── SecondPassword Screen5/6/7 additional globals ────────────────────────────
DWORD   EventEntranceHitTestMode  = 0;   // EventEntrance_HitTest mode
char    GuildMarkColorIdx  = 0;   // SecondPassword selected grid index
// Buffers del editor de creacion de GUILD (no "PIN entry"); el codigo los recorre
// como arrays:
//   RenderGuildCreation lee `mark[gx + gy*8]` con gx,gy en 0..7 -> 64 bytes.
// Layout del binario (contiguo, verificado contra el vecino PreviewItemData que
// deja 75 bytes de espacio):
//   0x7EA51EC  GuildName[8]   (IDA lo lee como 2 DWORDs: +0 y +4)
//   0x7EA51F5  GuildMark[64]  (1 byte por celda de la grilla 8x8)
char    GuildName[33] = {0};   // GuildName (0.97.40: 32 bytes + NUL)
char    GuildMarkGrid[64] = {0};   // GuildMark (grilla 8x8)
float  Screen5BtnXMax  = 425.0f; // Screen5 button X upper bound
float  Screen5BtnHeight  = 33.0f; // Screen5 button height
float  Screen5BtnYBase  = 210.0f; // Screen5 button Y base
// LegacyEventNoItemSlot ("no item in slot") y LegacyEventInvalidSlot ("invalid slot") son
// GlobalText[896] / [854] (ver el bloque de alias al final de globals.h).
short   SecPassLevelCheckB  = 0;   // second-password level check B
int     LevelThreshArrayBase  = 0;   // level threshold array base
int     LevelThreshArrayUpper  = 0;   // level threshold array upper
// LegacyDevilSquareLimitMin / LegacyDevilSquareLimitMax -> macros sobre m_iDevilSquareLimitLevel (globals.h)
// int     LegacyDevilSquareLimitMin  = 0;   // level range lower array
// int     LegacyDevilSquareLimitMax  = 0;   // level range upper array
// LegacyTradeItems — declared above as DWORD
// LegacyWarehouseItems -> macro-alias de OffsetWarehouseItems (globals.h).

// ── BMD_DrawMesh / BMD_DrawBoneSlot_Anim buffers ─────────────────────────────
// LightTransform RGB buffer (float R,G,B per vertex). Stride 12 B per vertex.
// Capacity: 32 mesh slots × 15000 verts × 12 B = 5.76 MB.
// Backed by g_BoneLightBuf; BoneLightData is a 1-byte-typed lvalue macro
// (globals.h) so that &BoneLightData + k yields byte-level arithmetic, matching
// the Ghidra-decompiled accesses (&BoneLightData + iVar2 * 12, + meshIdx * 180000).
char    g_BoneLightBuf[32 * 15000 * 12] = {0};  // 5,760,000 bytes
// Chrome UV scratch (per-mesh, {U,V} pairs indexed by vertex, up to 15000 verts).
// Written + read within the same mesh by BMD_DrawMesh chrome pre-loop / tri loop.
float   g_ChromeUVBuf[15000 * 2] = {0};   // 120,000 bytes
// Transformed-normal buffer for chrome env-map (3 floats/normal, 180000 B/mesh,
// 32 mesh slots — parallel to g_BoneVertexBuf). Written by Skeleton_Transform's normal
// loop, read by BMD_DrawMesh chrome pre-loop. Sin esto, las normales quedaban en
// cero → el env-map chrome colapsaba a un texel → armas/glow chrome como barra.
char    g_BoneChromeNormalBuf[32 * 15000 * 12] = {0};  // 5,760,000 bytes
float  AlphaChannelThreshold  = 0.99f; // alpha threshold (alpha < this → use alpha channel path)
float  ChromeUScale  = 0.00024f; // chrome U scale factor
float  VertexWaveFreq  = 0.007f; // sin period scale for vertex deformation
float  VertexWaveAmplitude  = 28.0f; // sin amplitude for vertex deformation

// ── MoveEffect constants ──────────────────────────────────────
float  MoveEffectConst27  = 27.0f;
float  AngleDeg270  = 270.0f;
float  TwoPiOver3  = 2.0943952f;
// Los tres son DOUBLES de 8 bytes en el binario (no float).
// Bytes reales (ida_get_bytes 0x5529D8, 32): la region intercala
//   0x5529D8 double 12.5 | 0x5529E0 float 260.0 | 0x5529E4 padding
//   0x5529E8 double 1/180 | 0x5529F0 double PI
// Los usa MoveEffect case 244 (Rageful Blow): PI*(1/180) para el seno del arco del
// arma y el test `v356 != 12.5` para que el arma suba y baje.
double DoubleConst12p5  = 12.5;
double DoubleInv180  = 0.005555555555555556;
double DoublePi  = 3.141592;
float  MoveJointTrigScale1  = 70.0f;  // MoveJoint trig scale (mode 1)
float  MoveJointAngleScale  = 0.18f;  // MoveJoint particle angle scale
float  MoveJointFadeA  = 0.6666667f;  // MoveJoint color fade rate A
float  MoveJointFadeB  = 0.7692308f;  // MoveJoint color fade rate B
float  MoveJointTrigFreqA  = 0.03065f;  // MoveJoint trig freq A
float  MoveJointTrigScaleB  = 0.024f;  // MoveJoint trig scale B
float  MoveJointHpScale  = 0.025f;  // MoveJoint HP-bar scale factor
float  RingOrbitScaleX  = 0.0613f;  // Ring_ComputeOrbit ring trig scale X
float  RingOrbitScaleY  = 0.048f;  // Ring_ComputeOrbit ring trig scale Y
float  RingOrbitScaleZ  = 0.1113f;  // Ring_ComputeOrbit ring trig scale Z
// Tabla de HUESOS del aura de Swell Life: dato del binario que el port dejo
// en ceros.  No son escalas -- MoveEffect los pasa como 'Scale' a
// Particle_Spawn(0x47e, ..., sub 4, Scale, owner), que los guarda en el
// campo +0x40 del slot, y MoveParticles los usa como indice de hueso para
// re-anclar la particula al dueño en cada tick.
// Leida de 0x00559B78: 19 1a 1b 14 22 23 24 00.
// Unico consumidor: MoveEffect (0x0046A3D1), que la lee en las DOS direcciones:
// hacia adelante desde 0x559B78 y hacia ATRAS desde 0x559B7F
//   IDA: Scalex = *((std::uint8_t *)&unk_559B7F - v227);
// Por eso MoveEffectBufOff7 no es un array aparte sino un alias al offset 7 de esta
// misma tabla (mismo patron que PrimaryTerrainLightG). Con dos arrays sueltos el indice
// negativo leia fuera de rango.
char   ParticleEffectIdTable[8] = { 25, 26, 27, 20, 34, 35, 36, 0 };

// ── Map_LoadObjectModels (0x0050c4d0) ─────────────────────────────────────────
FILE*  ParserFileHandle       = nullptr;  // DAT_083A40FC — file handle for custom-map object list
char   ParserTokenString[256] = {};       // DAT_083A3FF4 — token read buffer
char   LorenciaModelsLoadedFlag       = 0;            // Lorencia models-loaded flag
// Icarus water-tile name table — 32 entries × 0x38 bytes (= 0x700 bytes total).
// Was 1 byte → Scene_Objects.cpp case 7 (Icarus map) wrote 32 filename strings
// past the global → heap corruption on map enter. IDA bound: 0x83a91d8 - 0x83a8ad8 = 0x700.
char   IcarusWaterTileNameTable[32 * 0x38] = {0};

// ── BMD_SkinUpdate (0x0040b630) ──────────────────────────────────────────────
int    SkinnedVertexCount       = 0;            // skinned vertex count
float  BoneWorldVtxBase       = 0.0f;         // bone world-vertex array base
float  SrcVertexPosTable       = 0.0f;         // source vertex position array base
short  ParentBoneIdxTable       = 0;            // parent-bone index array base
float  GlobalMeshScale      = 1.0f;         // global mesh scale factor
float  BoneWorldMatBase       = 0.0f;         // bone world-position matrix base
float  BoneWorldPosX       = 0.0f;         // bone world-pos X
float  BoneWorldPosY       = 0.0f;         // bone world-pos Y
float  BoneWorldPosZ       = 0.0f;         // bone world-pos Z
int    TriangleCount       = 0;            // triangle count
BYTE   TriangleMatNameTable       = 0;            // material name array base (stride 0x20)
float  TriangleVertDataTable       = 0.0f;         // triangle vertex data base
short  UvIndexCount       = 0;            // UV index count per mesh group
short  NormalIndexCount       = 0;            // normal index count per mesh group
short  VertexIndexCount       = 0;            // vertex index count per mesh group
short  MaterialGroupCount       = 0;            // material group count accumulator
BYTE   MaterialNameTable       = 0;            // material name table base
float  PackedVertexBuffer       = 0.0f;         // packed vertex buffer base
short  VertexIndexTable       = 0;            // vertex index table base
short  UvIndexTable       = 0;            // UV index table base
short  UvLinkedVertTable       = 0;            // UV name/linked-vert table base
float  UvCoordDataTable       = 0.0f;         // UV coordinate data base
float  UvExtraZChannel       = 0.0f;         // UV extra/Z channel base
short  UvLinkedVertIndex       = 0;            // UV linked vertex index table base
float  NormalDataTable       = 0.0f;         // normal data base
float  NormalYChannel       = 0.0f;         // normal Y channel base
short  NormalIndexTable       = 0;            // normal index table base

// ── Parse_NextToken (0x0050e2c0) ─────────────────────────────────────────────
int    ParserCurrentToken = 0;             // DAT_083A40F4 — last token type code
float  ParserTokenNumber  = 0.0f;          // DAT_083A40F8 — last numeric token value

// ── RenderObjectScreen (0x004e13a0) ──────────────────────────────────────────
float  ObjectSelect_Angle      = 0.0f;         // item render: rotation X offset
float  ItemRenderRotY      = 0.0f;         // item render: rotation Y
float  ItemRenderRotZ      = 0.0f;         // item render: rotation Z
short  ObjectSelect_Type      = 0;            // item render: resolved model type index
float  ObjectSelect_HeadAngle      = 0.0f;         // item render: HeadAngle[0]
int    ObjectSelect_AnimationFrame      = 0;            // item render: state flag A
int    ObjectSelect_PriorAnimationFrame      = 0;            // item render: state flag B
short  ObjectSelect_PriorAction       = 0;            // item render: state flag C

// ── Filter / name-filter system globals ──────────────────────────────────────
DWORD  WordFilterChecksumSeed       = 0x007cfa00;  // word-filter BMD checksum seed (literal)
DWORD  NameFilterChecksumSeed       = 0x00578200;  // name-filter BMD checksum seed (literal)
char   ExitErrorMsgBuf[256] = {};  // fatal-error message string (shown by ExitProgram)

// ── BuxConvert_1 XOR key and misc ───────────────────────────────────────────────
BYTE   TerrainOverlayGate       = 1;    // unk_55A76C — gate de la 2da pasada del terreno
                                  // (TerrainFlag=2, la capa de billboards de
                                  // pasto/arena que se mueve con el viento).
                                  // En el binario vale 1 (ida_get_bytes
                                  // 0x0055A76C -> 01 00 00 00) aunque nadie
                                  // lo escribe (un solo xref, la lectura en
                                  // RenderTerrain): con 0 el overlay no se
                                  // dibuja en ningun mapa.
// bBuxCode de BuxConvert_1 (0x004F6EB0) -- la copia que usa OpenTerrainAttribute.
//
// En el binario esta direccion vale FC CF AB (ida_get_bytes 0x0055A770), igual
// que las otras dos copias.  ACA VA EN CERO A PROPOSITO, y hay que dejarlo asi:
// nuestro OpenTerrainAttribute carga `Data/<mundo>/Terrain%d.att`, que viene SIN
// cifrar (empieza con 00 FF FF, el header ya descifrado), y no el EncTerrain%d.att
// cifrado -- ese mide 131076 bytes y usa otro formato, ver Scene_OpenWorld.cpp.
// Con la clave real el XOR convertiria el archivo en claro en basura y la
// validacion del header lo rechazaria: terreno sin atributos, o sea sin zonas
// seguras, sin colisiones y sin agua.
//
// Tienen que ser tres bytes propios (la funcion
// indexa [i % 3]); en cero el XOR queda neutro.
// IDA: bBuxCode (0x0055A770)
BYTE   BuxConvertXorKeyB[3]    = { 0, 0, 0 };

// ── SkillAttribute table ──────────────────────────────────────────────────────
// WorldMapType (HeroTile) ya está definido como DWORD más arriba.
_SkillAttrEntry SkillAttribute = {};   // skill attribute table base @ 0x07D29D20
LPVOID CharExtraBmdBuffer       = nullptr;   // character extra BMD heap buffer
// LegacyPointerPoolBase es el inicio de g_PointerPool.
char   WaterWaveBufferA       = 0;        // water wave buffer A
// Grass-wind / water-wave ping-pong buffer: sub_4F98C0 (setup) y sub_4F9A30
// (smoothing) escriben `&GrassWindWaveBuffer + 0x40000*toggle` → 2 buffers de 0x40000
// (256×256 DWORDs c/u) = 0x80000; RenderTerrain lo usa cada frame.
char   GrassWindWaveBuffer[0x80000] = {};    // grass-wind/water-wave double buffer
char   TextureExtTgaStr[8]    = "OZT";   // OpenTGA extension suffix (Data mode: .tga→.OZT)
DWORD  MapLoadReadDesc       = 0;    // Map_Load block-read descriptor

// ── Small-function batch globals ─────────────────────────────────────────────
BYTE   QuestTableBase[0x1C900] = {0};  // Quest table base — see globals.h
DWORD  SoundDeviceCtx       = 0;       // Sound device context
DWORD  ChatListBoxNextId       = 0;       // RefCount / tick counter

int    Vtbl_ObjectBase   = 0;
int    ClothNodeVtable   = 0;
int    Vtbl_StreamBase   = 0;
int    Vtbl_Quest   = 0;
int    Vtbl_CWaveFile   = 0;
int    Vtbl_CErrorReport   = 0;
int    Vtbl_HashTable   = 0;
int    Vtbl_ChatListBox   = 0;
int    Vtbl_StreamDeinit   = 0;
int    Vtbl_GameGuard   = 0;

// IDA off_XXXX vtable/pointer-literal symbols, renamed by port.py to DAT_xxxxxxxx.
// (parallel to PTR_LAB_/PTR_FUN_ above; port.py uses the DAT_ form)
DWORD  ClothBodyVTable = 0;

// _PartyKey already defined above (line ~532)
HANDLE hEvent             = NULL;
LPVOID lpParameter        = NULL;
char   g_GameGuardGameName[64] = "Mu";  // IDA: aMu (0x0055910C)
uintptr_t g_csQuest       = 0;

BYTE   m_byMatchType      = 0;
int    m_iMatchTimeMax    = 0;
int    m_iMatchTime       = 0;   // 0x00559CCC  tiempo restante del evento
int    m_iMaxKillMonster  = 0;
int    m_iKillMonster     = 0;

BOOL   g_EnableSound      = FALSE;
bool   g_Enable3DSound    = false;
LPDIRECTSOUND           g_lpDS            = NULL;
LPDIRECTSOUND3DLISTENER g_lpDS3DListener  = NULL;
void*  wavefile           = NULL;
DWORD  g_dwBufferBytes    = 0;
// DirectSound buffer tables
int                    MaxBufferChannel[420] = {};
int                    BufferChannel[420]    = {};
bool                   Enable3DSound[420]    = {};
int                    SoundLoadCount         = 0;
char                   BufferName[420][0x40] = {};
LPDIRECTSOUNDBUFFER    g_lpDSBuffer[420][4]   = {};
LPDIRECTSOUND3DBUFFER  g_lpDS3DBuffer[420][4] = {};
const OBJECT*          Object3DSound[420][4]  = {};

// Tabla de nombres NPC/mob: 512 entradas × 0x36 bytes (ver nota en 0x07CF2000).
BYTE   MonsterScript[512 * 0x36] = {};
char   WhisperRegistID[10][41] = {};   // IDA: WhisperRegistID (0x07DB9310); 0.97.40: nombres de 40 bytes
int    WhisperID_Num = 0;              // IDA: WhisperID_Num (0x07E11DB0)

// LegacyRenderPoolOff16 — see globals.h. Defined as macro into g_RenderPool.
// (The standalone declaration was a single int that backed nothing — MoveBlurs
//  walked past it. Now the pool array is the storage and the macro projects.)
// Butterfles is now a macro alias for ButterflyObjectPool (same address 0x083a1218)

// ── Batch 3 globals ──────────────────────────────────────────────────────────
void  *RbTreeSentinel       = NULL;   // RB-tree sentinel (NIL)
float  CameraAngle[3]     = {0};
float  CameraPosition[3]  = {0};
// InventoryOpened/CharacterOpened/etc. are now #define aliases for
// InventoryOpened/116/etc. — see globals.h. Storage is the byte-sized DAT_
// globals defined above (lines ~745-753).
FILE  *SMDFile             = NULL;
// lpDefault_00583d88 already defined above (line ~1014)
DWORD  CodecStateA       = 0;
// MapLoadReadDesc already defined above (line ~1922)

// Missing globals (compilation fixes)
// DAT_07c82cf4 es el anclaje +0x24 de g_PointerPool.
DWORD  JpegNaturalOrderTable       = 0;
// m_dwTextColor / m_dwBackColor NO son globals separados: en IDA son EXACTAMENTE
// 0x559c78 / 0x559c80 (= m_dwTextColor / SetBackgroundTextColor). Verificado por disasm
// (sub_40D610 @0x40D734: `mov [0x559c78], 0xffff9664`, y sub_480980 idéntico).
// Por eso son macros (globals.h) que apuntan al global real: todo el código que
// setea m_dwTextColor (HUD_Pass1/2/3, ChatListBox render) tiene que llegar al
// global que lee el render de texto (CUIRenderText_RenderText, lee m_dwTextColor).
// g_lpszMessageBoxCustom es un alias de MessageBoxCustomBuf (ver globals.h).
// m_hFontDC y las fuentes viven en CFont (Core/Font.h).

// Batch 18 — InitGame / ReceiveChat globals
DWORD  EnableUse          = 0;
int    SendGetItem       = -1; // IDA: SendGetItem (0x07E11998)
// MoveDebounceCounter already defined above (line ~805)
int    LegacySoccerTime       = 0;
int    LegacySoccerObserver       = 0;
int    LegacySelectedNpc       = -1;
int    LegacySelectedOperate       = -1;
// DownArrowDebounce already defined above (line ~994)
int    LegacySelectedItem       = -1;
// UpArrowDebounce already defined above (line ~993)
// AutoAttackFlag: alias de m_bAutoAttack (m_bAutoAttack), ver globals.h
// MenuF1ItemType already defined above (line ~873)
// InputCooldownTicks already defined above (line ~804)
BYTE   UiAlphaByte       = 0xFF;
short  MutoNumber0       = -1;
short  MutoNumber1       = -1;
short  MutoNumber2       = -1;
int    LegacySceneFlag       = 0;

// Batch 19 — SendCheck globals
DWORD  LegacyGameServerConnected       = 0;
// IDA: g_byPacketSerialSend (0x07DB8600)
BYTE   g_byPacketSerialSend       = 0; // IDA: g_byPacketSerialSend (0x07DB8600)
BYTE   SendCheckFirstCallFlag       = 0;
DWORD  SendCheckFirstCallTick       = 0;

// Batch 20 — OpenNpc, MoveCamera, RenderEquipment3D, RenderItems3D, Send_ActionRequest, LookAtTarget
// Only truly new globals:
float  EquipSlotRingX      = 55.0f;  // equip pendant X offset
float  EquipSlotYTop      = 46.0f;  // equip slot Y offset (helm row)
float  EquipSlotXBody      = 75.0f;  // equip slot X offset (pants/gloves/boots col)
float  EquipSlotYMid      = 89.0f;  // equip slot Y offset (mid row)
float  EquipSlotYBottom      = 152.0f;  // equip slot Y offset (ring/boots row)
float  EquipSlotXRight      = 134.0f;  // equip slot X offset (weapon_L/ring_R col)
float  ArrowCountOffsetX      = 13.0f;  // arrow count X offset
float  CurrentCameraWalkDelta[6]    = {0};   // CurrentCameraWalkDelta[6]
float  CameraLerpSpeed      = 0.166666672f;  // lerp speed (camera smoothing)
void*  g_LoginSceneObjects[9] = {0}; // sky, ship1, wave1, ship2, wave2, ship3, wave3, banner, sun

// Batch 22 — AttackStage, CreateArrow
// g_iLimitAttackTime @ 0x00559858 — umbral del contador de ataque (c+757) que
// gatea TODA la seccion de skills de MoveCharacter (0x449900 L840):
//     if (*(BYTE*)(c + 757) >= g_iLimitAttackTime) { ...skills..., hit sounds }
// El valor inicial del .data del binario es 15 (leido: 0f 00 00 00); despues lo
// reescribe AttackStage (0x448930) con 5 o 15 segun el skill.
// Estaba en 0, con lo cual el gate era `0 >= 0` = siempre cierto y el bloque
// corria en CADA frame para CADA entidad: en char-select eso disparaba cientos
// de PlayBuffer(rand()%7+50) = el ruido de golpes (eBlow/eShortBlow).
int    g_iLimitAttackTime       = 15;    // g_iLimitAttackTime
// IDA: CurrentSkill (0x05826D10)
DWORD  CurrentSkill       = 0;
DWORD  WarriorSkill43Tick       = 0;     // UseSkillWarrior 43 activation tick
float  SwordTrailRadius      = 1400.0f;  // sin/cos offset multiplier (sword trail radius)
float  SlashProjRadius      = 145.0f;  // sin/cos offset multiplier (slash projectile)
float  ComboAnimOffset      = 0.54f;  // combo animation offset constant

// Batch 21 — RenderSkillIcon, CheckMixRecipe, MoveObjects, CreateArrows, CollisionDetectLineToMesh
DWORD  VisibleObjectCount       = 0;     // visible object counter (MoveObjects)
DWORD  MixSocketFlag       = 0;     // mix recipe: socket flag byte
DWORD  WingMixFlagBits       = 0;     // mix recipe: wing socket option value
int    MixValueScore       = 0;     // mix recipe: combined value score (capped at 0x5a)
char   LastHotkeyChar       = 0;     // last rendered hotkey char
float  ArrowAngleOffsetSmall      = 7.5f;  // arrow angle offset (5-way shot small)
float  ArrowAngleOffsetLarge      = 22.5f;  // arrow angle offset (5-way shot large)
float  SkillHotkeyNumY      = 26.0f;  // RenderNumber2D Y offset

// ── UseSkillWizard dependencies ──────────────────────────────────────────────
DWORD  MovementSkillTarget       = 0;     // MovementSkillTarget (index into CharactersClient)
DWORD  SelectedSkillSlot       = 0;     // skill slot index (current selected skill slot)
char   GmNameCheck1[8]    = "webzen";  // GM name string (anti-impersonation check 1)
char   GmNameCheck2[8]    = "webzen";  // GM name string (anti-impersonation check 2)

// ── SkillElf dependencies ────────────────────────────────────────────────────
char   GmNameCheckString       = 0;     // GM name check string (part of "webzen" pattern)
char   ChatLogWidgetId       = 0;     // chat log widget ID string (for AddText)
// NotEnoughManaText ("Not enough mana") es GlobalText[474] (ver el bloque de alias al
// final de globals.h).

// ── MoveParticles camera shake globals ──────────────────────────────────────
// IDA MoveParticles 0x477090 walks contiguous floats. Separate scalars let
// velocity[1] overwrite EarthQuake in Release (hardware watchpoint confirmed).
float g_ParticleDriftPosition[3] = {};
float g_ParticleDriftVelocity[2] = {};

// ── MoveParticles float constants ───────────────────────────────────────────
float  ParticleScaleDeltaA      = 0.09f;
float  ParticleScaleDeltaB      = 0.07f;
// AnimSpeedA — already defined above (line ~964)
float  ParticleWaveFreqA      = 0.87266463f;
float  ParticleWaveFreqB      = 0.0872664601f;
// AspectRatioScale — already defined above (line ~141)
float  ParticleWaveFreqC      = 0.17453292f;
float  ParticleColorDecayA      = 0.9090909f;
float  ParticleGreenFactorA      = 0.6145098f;
float  ParticleRedFactorA      = 0.980392158f;
float  ParticleDampA      = 0.42f;
float  ParticleLightA      = 0.0416666679f;
float  ParticleBlueFactorB      = 0.333f;
float  ParticleGreenFactorB      = 0.572f;
float  ParticleRedFactorB      = 0.725f;
float  ParticleWaveFreqD      = 0.00174532924f;
float  ParticleWaveFreqE      = 0.0002f;
float  ParticleBlueFactorC      = 0.3137255f;
float  ParticleGreenFactorC      = 0.394901961f;
float  ParticleRedFactorC      = 0.470588237f;
float  ParticleLightB      = 0.125f;
float  ParticleVelYAdd      = 6.4f;
float  ParticleDampB      = 1.05f;
float  ParticleLightC      = 0.2f;
float  ParticleDampC      = 0.98f;
float  ParticleDriftScale      = 0.000400000019f;
float  ParticleColorDecayB      = 0.7407407f;
float  ParticleScaleDecay      = 0.0001f;
float  ParticleGravScale      = 0.095f;
float  ParticleDampD      = 0.85f;
float  ParticleDampGeneral      = 0.95f;
float  ShakeClampMin      = -1.7f;
float  ShakeClampMax      = 1.7f;
float  ShakeBounce      = -0.6f;
float  ShakeRandomScale      = 0.0006f;

// Additional math/render constants
float  RadToDeg      = 57.29578f;  // 180/pi (radians to degrees)
float  TerrainTileSize      = 64.0f;      // terrain tile size

// CheckArrow chat string globals (runtime-initialized by resource loader)
char   ArrowsRequiredMsg       = 0;
char   BoltsRequiredMsg       = 0;
char   AutoSwapChatIdA       = 0;
char   AutoSwapChatIdB       = 0;
char   EmptyChatMessage       = 0;

// Skill selection
char   SkillModeFlag       = 0;

// ── Pool de burbujas de chat (CreateChat 0x481BA0 / MoveChat 0x4821A0) ───────
// En el binario: base `unk_7E016F8`, stride 596 (0x254), fin `unk_7E0FFC8`.
//   (0x7E0FFC8 - 0x7E016F8) / 596 = **100 slots**.
// `unk_7E01720` NO es otro pool: es base + 40 (el campo timer1), que es donde
// MoveChat arranca su walk. Por eso ahora es una macro (ver globals.h).
alignas(8) char ChatBubblePool[100 * (0x254 + sizeof(void*) - 4)] = {};   // CHAT: Owner es un puntero (x64: 0x258)
// Guild mark bracket strings (initialized by resource loader)
char   GuildMarkPrefixChar       = 0;  // guild mark prefix string "["
short  GuildMarkSuffixChar       = 0;  // guild mark suffix 2-byte "]"
char   GuildMarkSuffixTrailChar       = 0;  // guild mark suffix trailing

// ── HUD render globals (Phase-2 port) ────────────────────────────────────────
int    PartyNumber           = 0;
int    PartyKey              = 0;
BYTE   Party[2048]           = {0};
int    EnableGuildWar        = 0;
int    EnableSoccer          = 0;
int    GuildWarIndex         = -1;
int    HeroSoccerTeam        = 0;
int    GuildWarScore[2]      = {0, 0};
char   GuildWarName[80]      = {0};
char   SoccerTeamName[2][80] = {{0}, {0}};

// LegacyBubblePoolOff22c tampoco es un buffer aparte: 0x7E01924 = 0x7E016F8 + 0x22C (campo
// disp1 del slot 0 del pool de burbujas). CreateChat escribe en ChatBubblePool y
// RenderBooleans lee acá: es una macro sobre el pool único (ver globals.h).

// g_hFont es ahora un alias de DAT_055ca0xx (ver globals.h).
// FontHeight vive ahora en la direccion que le corresponde (0x07D78080), mas
// arriba en este archivo.  Aca habia una SEGUNDA variable con el mismo nombre
// inicializada en 14: WinMain escribia una y el render leia la otra.
SIZE   TextSize              = {0, 0};

// dword_55C9BC8 hash-table state lives in g_HashTableCtx[4] (line ~438);
// macros MAIN_HASH_CLASS/cc/d0/d4 alias it.  Do NOT re-define here.

void  *CharacterMachine      = nullptr;
// CharacterAttribute is a #define alias of CharacterAttributePtr — no storage here.

// HudTextVisibleFlag is already defined at line ~676 as char (matches header).

// Scratch de coordenadas de los paneles Party / GuildCreation. No usar
// Inventory[32] para esto: es el slot 0 del overlay del pool de la TIENDA.
int g_PartyPanelScratchX = 0, g_PartyPanelScratchY = 0;
int g_GuildCreatorScratchX = 0, g_GuildCreatorScratchY = 0;


