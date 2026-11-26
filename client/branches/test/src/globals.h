#pragma once
#include "Entity/EntityTypes.h"
#include "Render/DamagePointTypes.h"
#include "Item/ItemDefines.h"
#include "Scene/SceneObjectCell.h"
#include "Render/EffectPools.h"
struct GATE_ATTRIBUTE;
struct OBJECT;
// globals.h — extern declarations for all Ghidra DAT_ globals.
// Los tipos se infieren del contexto; el prefijo _DAT_ = alias float de la misma dirección.
// NO incluir este header directo — entra vía stdafx.h.

// Reinterpret a DWORD global as float (for globals that have no _DAT_ float alias).
// Uso: Ff(DAT_xxx) para lecturas/escrituras float; Fi(DAT_xxx) para lecturas int.
#ifndef Ff
#define Ff(sym)  (*(float*)&(sym))
#define Fi(sym)  (*(int*)&(sym))
#endif

// ── Low-address constants (x87 FPU/hardware, Ghidra artifacts) ────────────────
// Aparecen cerca de la dirección 0 — se acceden como float vía el alias _DAT_.
extern float  GuardPageScratch_10;  // legacy null-page scratch (0x10 mapped RW in original client); only ever zeroed  // _DAT_00000010
extern float  GuardPageScratch_14;  // legacy null-page scratch (0x14 mapped RW in original client); only ever zeroed  // _DAT_00000014
extern float  GuardPageScratch_18;  // legacy null-page scratch (0x18 mapped RW in original client); only ever zeroed  // _DAT_00000018
extern float  GuardPageScratch_1C;  // legacy null-page scratch (0x1C mapped RW in original client); only ever zeroed  // _DAT_0000001c
extern float  GuardPageScratch_20;  // legacy null-page scratch (0x20 mapped RW in original client); only ever zeroed  // _DAT_00000020
extern float  GuardPageScratch_24;  // legacy null-page scratch (0x24 mapped RW in original client); only ever zeroed  // _DAT_00000024

// ── Game math / render constants (0x005524xx – 0x00552dxx) ────────────────────
// Casi todos se acceden como float vía el alias _DAT_; en algunos también hay acceso entero.
// Kept float-literal storage (referenced by address in tunable tables):
extern float  ImpulseDriftVertB;  // particle impulse vertical drift factor B (ParticleImpulses)  // _DAT_005524f4
extern float  ImpulseDriftVertA;  // particle impulse vertical drift factor A (ParticleImpulses)  // _DAT_005526e4
extern float  FadeAlphaFactor;  // fade-alpha factor (EffectFadePresets)  // _DAT_005529d4
extern float  ImpulseDriftHorizA;  // particle impulse horizontal drift factor (ParticleImpulses)  // _DAT_005528b4
extern float  ImpulseScaleBaseB;  // particle impulse scale base (ParticleImpulses)  // _DAT_0055256c
extern float  FragmentGravityA;  // fragment gravity preset A (EffectBouncePresets)  // _DAT_00552540
extern float  FragmentGravityB;  // fragment gravity preset B (EffectBouncePresets)  // _DAT_00552650
extern float  PuffDriftFactor;  // puff particle drift factor (ParticleLinearMotions)  // _DAT_005529c8
extern float  SmokeDriftFactor;  // smoke particle drift factor (ParticleLinearMotions)  // _DAT_005529c0
extern float  FragmentBounceKick;  // fragment bounce kick (EffectBouncePresets)  // _DAT_00552a14
extern float  GridToWorldScale;   // grid-to-world scale factor  // _DAT_005524f0
extern DWORD   GridToWorldScaleBits;  // DAT_005524f0
extern float  SinFreqWorldTime;  // _DAT_00552500
extern DWORD   SinFreqWorldTimeBits;  // DAT_00552500
extern float  HalfScale;  // _DAT_00552504
extern DWORD   HalfScaleBits;  // DAT_00552504
extern float  ClothGridUvScale;   // cloth grid uv scale factor  // _DAT_00552538
extern DWORD   ClothGridUvScaleBits;  // DAT_00552538
extern float  PartyHpRowHeight;   // Party HP bar row height in pixels (SecondPassword_HoverCheck)  // _DAT_005527d4
extern float  SpringEdgeThreshold;   // spring horizontal-vs-vertical angle threshold  // _DAT_00552560
extern DWORD   SpringEdgeThresholdBits;  // DAT_00552560
extern DWORD   FloatZeroBBits;  // DAT_00552580
extern float  EffectScaleUnit;  // _DAT_00552594
extern DWORD   EffectScaleUnitBits;  // DAT_00552594
extern float  ConvertYScale;   // 1/480 — Y scale constant  // _DAT_00552838
extern DWORD   ConvertYScaleBits;  // DAT_00552838
extern float  ConvertXScale;   // 1/640 — X scale constant  // _DAT_0055283c
extern DWORD   ConvertXScaleBits;  // DAT_0055283c
extern DWORD   WeatherMaxAltitudeBits;  // DAT_00552850
extern float  FloatEpsilon;  // _DAT_00552868
extern DWORD   FloatEpsilonBits;  // DAT_00552868
extern float  AlphaFadeStep;   // alpha fade rate (weather particle)  // _DAT_00552874
extern float  CatmullWeightA;   // Catmull-Rom weight t=0.25 (substep 1 X bias)  // _DAT_0055287c
extern float  CatmullWeightB;   // Catmull-Rom weight (substep 0 main)  // _DAT_00552884
extern float  CatmullWeightC;   // Catmull-Rom weight (substep 0/2 cross)  // _DAT_00552888
extern float  CatmullWeightD;   // Catmull-Rom weight (substep 0/1 lookahead)  // _DAT_0055288c
extern DWORD   AngleDeg360Bits;  // DAT_0055286c
extern DWORD   AlphaFadeStepBits;  // DAT_00552874
extern float  DeltaTimeScale;   // movement speed scale (facing dt factor)  // _DAT_00552890
extern float  DegToRad2;  // _DAT_005528b0
extern DWORD   DegToRad2Bits;  // DAT_005528b0
extern DWORD   SpriteSheetColStepBits;  // DAT_005528dc
extern float  ZBobFreq;   // Z-bob amplitude / fsin input  // _DAT_005528e0
extern DWORD   ZBobFreqBits;  // DAT_005528e0
extern float  MoveJointZOffset;  // MoveJoint Z-offset constant  // _DAT_005528fc
extern float  GemScaleStep;   // particle scale constant (gem/bead effects)  // _DAT_00552910
extern DWORD   GemScaleStepBits;  // DAT_00552910
extern float  SpinFreqTime;   // PathFinder spin-speed scale / Entity_Reset spin  // _DAT_0055291c
extern DWORD   SpinFreqTimeBits;  // DAT_0055291c
extern float  BmdAnimFreq;   // BMD anim frame scale  // _DAT_00552920
extern DWORD   BmdAnimFreqBits;  // DAT_00552920
extern float  ParticleZOffset;   // particle Z offset constant  // _DAT_0055293c
extern DWORD   ParticleZOffsetBits;  // DAT_0055293c
extern float  JointLerpStep;  // _DAT_00552a00
extern DWORD   JointLerpStepBits;  // DAT_00552a00
extern float  WeatherFlyMaxDist2;   // butterfly/bubble max distance squared  // _DAT_00552a38
extern float  WeatherAngleFlip;   // PI constant for angle flip (360°)  // _DAT_00552a3c
extern float  RainSinFreq;   // secondary fsin frequency (rain Y offset)  // _DAT_00552a40
extern float  WindRotScale;   // particle rotation scale (wind effect)  // _DAT_00552abc
extern DWORD   WindRotScaleBits;  // DAT_00552abc
extern float  BigSmokeSizeScale;   // particle size scale (large smoke/wind)  // _DAT_00552ac0
extern DWORD   BigSmokeSizeScaleBits;  // DAT_00552ac0
extern float  AspectRatioScale;   // aspect ratio correction (float view of AspectRatioScaleBits)  // _DAT_00552adc
extern DWORD   AspectRatioScaleBits;  // DAT_00552adc
extern DWORD   HeightNormScaleBits;  // DAT_00552b70
extern float  FacingDtMax;   // facing delta threshold (max dt before skipping)  // _DAT_00552b6c
extern float  ScrollWaveFreq;  // _DAT_00552b88
extern DWORD   ScrollWaveFreqBits;  // DAT_00552b88
extern float  ItemSpinSpeed;   // item spin speed (frames-to-degrees multiplier)  // _DAT_00552c00
extern float  InvCursorBoundXMax;  // _DAT_00552c24
extern DWORD   InvCursorBoundXMaxBits;  // DAT_00552c24
extern float  InvCursorBoundYMax;  // _DAT_00552ca8
extern DWORD   InvCursorBoundYMaxBits;  // DAT_00552ca8
extern float  InvCursorBoundXMin;  // _DAT_00552cac
extern DWORD   InvCursorBoundXMinBits;  // DAT_00552cac
extern DWORD   ObjectScaleFactor;  // DAT_00552cb0
extern float  HalfDegToRad;   // PI/180 = 0.017453292f  (float view of HalfDegToRadBits)  // _DAT_00552cc4
extern DWORD   HalfDegToRadBits;  // DAT_00552cc4
extern float  WorldToBucketGrid;   // 1/1600 (world→object-bucket-grid factor)  // _DAT_00552d20
extern DWORD   WorldToBucketGridBits;  // DAT_00552d20
extern float  IntroLogoOffsetY;  // _DAT_00552d38
extern DWORD   IntroLogoOffsetYBits;  // DAT_00552d38
extern float  IntroLogoFactorY;  // _DAT_00552d3c
extern DWORD   IntroLogoFactorYBits;  // DAT_00552d3c
extern float  ServerSelectX;  // _DAT_00552d48
extern DWORD   ServerSelectXBits;  // DAT_00552d48
extern float  ServerSelectRowStep;  // _DAT_00552d4c
extern DWORD   ServerSelectRowStepBits;  // DAT_00552d4c
extern DWORD   GameGuardOkStatus;  // DAT_005538a0

// ── Entity / render constants (0x005590xx – 0x00559dxx) ───────────────────────
extern BYTE    PacketXorKey16[16];   // 16-byte XOR key table (Crypto/anti-tamper) — original name LegacyPacketXorKey16
extern float  ClothGravityScalar;   // Verlet physics damping/gravity scalar  // _DAT_00559070
extern DWORD   ClothGravityScalarBits;  // DAT_00559070
extern DWORD   g_bUseChatListBox; // IDA: g_bUseChatListBox (0x005590AC)
// Layout de los 3 botones popup del ChatListBox — flt_5590B0/B4/B8 en IDA.
// Leídos del binario: 295.0 / 417.0 / 18.0 (ver comentario en globals.cpp).
extern float   ChatListBox_TabButtonsX;       // DAT_005590b0 — X del primer botón
extern float   ChatListBox_TabButtonsY;       // DAT_005590b4 — Y de los tres
extern float   ChatListBox_TabButtonSpacing;  // DAT_005590b8 — separación horizontal
extern BYTE    Version[5]; // IDA: Version (0x0055961C)
extern BYTE    Serial[16]; // IDA: Serial (0x00559624)
extern DWORD   PacketXorKey3; // IDA: PacketXorKey3 (0x00559678)
extern DWORD   CachedShopSlotIndex;  // DAT_00559684
extern char    PathFinderDebugString;    // PathFinder debug string (first char; take &)  // DAT_005597a0
extern DWORD   RenderSubModeFlag;  // DAT_005597c4
extern float  EntityLodScale;   // LOD scale: 1.0/1.2/1.4  // _DAT_005597c8
extern DWORD   EntityLodScaleBits;  // DAT_005597c8
extern DWORD   MonsterNameA;  // DAT_0055987c
extern DWORD   MonsterNameB;  // DAT_005599b0
extern DWORD   MonsterNameC;  // DAT_005599e0
extern DWORD   WhisperMuteFlag;   // IDA: m_bBlockWhisper (0x00559BF0), interruptor de susurros (F3)  // DAT_00559bf0
extern int    RainWindSpeed;   // wind speed result (fsin * tick, *10)  // _DAT_00559b9c
extern DWORD   MouseUpdateTimeMax;   // movement cooldown threshold  // DAT_00559bec
// Hover/selection state.  Names follow the 5.2 reference only where the
// 0.97k behaviour and lifetime match; the DAT address is kept for IDA work.
extern int     SelectedItem;       // DAT_00559c48 — item on ground (-1 = none)
extern int     SelectedNpc;        // DAT_00559c4c — NPC/shop entity (-1 = none)
extern int     SelectedCharacter;  // DAT_00559c50 — monster/player (-1 = none)
extern int     SelectedOperate;    // DAT_00559c54 — special world object (-1 = none)
extern int     Attacking; // IDA: Attacking (0x00559C58) -- -1 = no esta atacando. NO es un "hover target"
extern char    m_bAutoAttack;   // IDA: m_bAutoAttack (0x00559C5C)
extern int     QKeyItemType;   // QKey: tipo de item de la tecla Q (NO arma equipada; F3/30 le suma 448)  // DAT_00559c60
extern int     WKeyItemType;   // WKey: tipo de item de la tecla W  // DAT_00559c64
extern int     EKeyItemType;   // EKey: tipo de item de la tecla E  // DAT_00559c68
extern int     HoverAttackTarget;   // hover attack target (entity index copy)  // DAT_00559c70
extern DWORD   m_dwTextColor;  // DAT_00559c78
// 0x00559C7C — IDA `SetTextColor_0` (color del prefijo/guild en sub_47F360).
// Una sola memoria: el alias existe porque parte del port lo nombra DAT_ y
// otra parte SetTextColor_0.  Ver el comentario en globals.cpp.
#ifdef __cplusplus
extern "C" DWORD SetTextColor_0;  // guild-prefix text color (0xFFFFFFFF default)
#else
extern DWORD SetTextColor_0;  // guild-prefix text color (0xFFFFFFFF default)
#endif
#define LegacySetTextColor0 SetTextColor_0  // DAT_00559c7c
extern DWORD   SetBackgroundTextColor;   // IDA: DAT_00559c80 (0x00559C80)
extern char    GmAdminNameFilterStr[];  // GM/admin name substring (Entity_FindNearby filter string)  // DAT_00559b80
extern DWORD   InputEnable; // IDA: InputEnable (0x00559C84)  // DAT_00559c84
extern DWORD   InputNumber;   // IDA: DAT_00559c88 (0x00559C88)
extern DWORD   LegacyInputTextWidth;  // DAT_00559c8c
extern DWORD   ActiveInputSlot;  // input-slot selector (ClearInput writes -1; password prompt writes 1)  // DAT_00559c90
extern int     _InputTextMaxArr[8];  // per-slot input length limit in bytes (0x00559c94 array)
// 0.97.40: tope en grapheme clusters por slot (0 = sólo el tope en bytes de
// _InputTextMaxArr). ClearInput lo vuelve a 0 en los slots que limpia.
extern int     InputTextMaxClusters[10];  // per-slot grapheme-cluster limit (port addition)
inline void Input_SetLimit(int slot, int bytes, int clusters)
{
    _InputTextMaxArr[slot] = bytes;
    InputTextMaxClusters[slot] = clusters;
}
extern int&    InputTextMax;  // alias -> _InputTextMaxArr[0] (active input max length)
extern int&    InputTextMaxB;  // _DAT_00559c98
extern DWORD&  InputTextMaxBBits;  // DAT_00559c98
extern DWORD   ChatDisplayToggle;   // chat/entity flag (allows type 3 show)  // DAT_00559bf1
extern DWORD   ChatHistoryIndex;   // chat history index (0..4)  // DAT_00559cc4
extern DWORD   ChatFirstVisibleSlot;   // entity table iteration offset  // DAT_00559ce0
// Alias de los tres globals del panel de evento (ver m_iMatchTime mas abajo).
#define LegacyMatchTime  (*(DWORD*)&m_iMatchTime)  // DAT_00559ccc
#define LegacyMaxKillMonster  (*(DWORD*)&m_iMaxKillMonster)  // DAT_00559cd0
#define LegacyKillMonster  (*(DWORD*)&m_iKillMonster)  // DAT_00559cd4
extern DWORD   ImeOldSentMode;  // DAT_00559cd8
extern int     HoverTargetIndex;   // hover target index copy (char-select slot or NPC)  // DAT_00559ce8
extern DWORD   BuffNameFmt;  // DAT_00559ef0
extern DWORD   BuffStatFmt2;  // DAT_00559ef4
extern DWORD   BuffStatFmt3;  // DAT_00559ef8
extern char    MultiSelectResultA;   // multi-select item result slot A  // DAT_00559f5e
extern char    StoragePinOkFlag;  // DAT_00559f5f
extern DWORD   ChaosMixInfo[4];   // chaos-mix info (ReceiveTalk sub 3)  // DAT_0055a3e8
extern int     EventType;         // EventWindow type (ReceiveTalk sub 4/6)
extern int     g_PartyPanelScratchX, g_PartyPanelScratchY;  // party panel draw origin X/Y (scratch; cannot reuse Inventory[32])
extern int     g_GuildCreatorScratchX, g_GuildCreatorScratchY;   // 1 = talk 0x30 enviado, server puede tener Interface.use=1
extern "C" { extern int g_bServerDivisionEnable; extern int g_bServerDivisionAccept; }  // ReceiveTalk sub 5
extern DWORD   HoveredSkillIndex;  // DAT_0055a3e4
extern BYTE    TerrainOverlayGate;    // unk_55A76C — gate de la 2da pasada del terreno  // DAT_0055a76c
                                // (capa de billboards de pasto/arena).  Constante
                                // de .data: vale 1 en el binario.
extern BYTE    BuxConvertXorKeyB[3]; // clave XOR de BuxConvert_1 (0x004F6EB0) -- en CERO a proposito, ver globals.cpp  // DAT_0055a770
extern DWORD   FrustrumBoundMaxX_1;   // IDA: DAT_0055a774 (0x0055A774)
extern DWORD   FrustrumBoundMaxY_1;   // IDA: DAT_0055a778 (0x0055A778)
extern DWORD   FrustrumBoundMaxX_2;   // IDA: DAT_0055a77c (0x0055A77C)
extern DWORD   FrustrumBoundMaxY_2;   // IDA: DAT_0055a780 (0x0055A780)
extern char    Fmt_FileNotFound[];  // "%s file not found" -- missing-file error format  // DAT_0055a784
extern DWORD   TerrainFileSuffix;  // DAT_0055a798
extern char    DataPathPrefix[];  // "Data\\"  path prefix  // DAT_0055a79c
extern char    Data2PathPrefix[];  // "Data2\\" path prefix  // DAT_0055a7a4
// IDA: World (0x0055A7AC): gMapManager.GetCurrentMap().
extern float  CharSelectResetScratch;  // write-only scratch zeroed at char-select reset (original role unknown)  // _DAT_0055a7c0
// ── Game loop / scene state (0x005615xx – 0x005617xx) ─────────────────────────
// Nota: SceneFlag, g_hWnd y g_hDC se declaran en stdafx.h.
extern DWORD   CameraViewNearStorage;  // DAT_0056154c
extern DWORD   CameraViewFarStorage;  // DAT_00561550
extern DWORD   CameraFOVStorage;  // DAT_00561554
extern float   CameraDistanceTarget;   // DAT_005616B4: MoveMainCamera smoothing target
extern float   CameraDistance;         // DAT_083A45D0: current MoveMainCamera distance
extern DWORD   BoundTextureSlotCache;  // DAT_00561574
extern char   *szServerIpAddress; // IDA: szServerIpAddress (0x005615B8)
extern WORD    g_ServerPort; // IDA: g_ServerPort (0x005615BC)

// ── ConnectServer flow ──────────────────────────────────────────
// SceneFlag (in stdafx.h)
// g_lpszMp3 (IDA 0x005615C4, rutas de los BGM): ver CMapManager (Game/MapManager.cpp).
// g_iCurrentDialogScriptStorage @ 0x005615DC ES `g_iCurrentDialogScript` — el índice del diálogo
// de quest activo.  Lo escriben CSQuest::ShowDialogText (0x4017E7) y sub_51D840,
// y lo leen sub_401AF0 y UI_InGameMenu.  La etiqueta vieja ("login/scene misc
// flag") era una misidentificación.
// `g_iCurrentDialogScript` es un alias de este global: writer y reader tienen que
// ver la misma memoria.
extern DWORD   g_iCurrentDialogScriptStorage;  // DAT_005615dc
#define g_iCurrentDialogScript  (*(int*)&g_iCurrentDialogScriptStorage)
extern DWORD   DeleteCharSlotIdx;  // DAT_005615e0
extern DWORD   CurrentCameraCount;   // fade timer  // DAT_005615e8
// CameraWalk[7][6] contiguous waypoint table (see globals.cpp for layout).
extern float   CameraWalkTable[42];  // camera waypoint table: 7 waypoints x 6 floats (pos + angles)  // DAT_005615ec
extern DWORD&  CameraWalk0PosX;   // alias → CameraWalk[0]  (wp0.pos.x)  // DAT_005615ec
extern DWORD&  CameraWalk0PosY;   // alias → CameraWalk[1]  // DAT_005615f0
extern DWORD&  CameraWalk0PosZ;   // alias → CameraWalk[2]  // DAT_005615f4
extern DWORD&  CameraWalk0Pitch;   // alias → CameraWalk[3]  // DAT_005615f8
extern DWORD&  CameraWalk0Yaw;   // alias → CameraWalk[4]  // DAT_005615fc
extern DWORD&  CameraWalk0Roll;   // alias → CameraWalk[5]  // DAT_00561600
extern DWORD&  CameraWalk5PosX;   // alias → CameraWalk[30] (wp5.pos.x)  // DAT_00561664
extern DWORD&  CameraWalk5PosY;   // alias → CameraWalk[31]  // DAT_00561668
extern DWORD&  CameraWalk5PosZ;   // alias → CameraWalk[32]  // DAT_0056166c
extern DWORD&  CameraWalk5Pitch;   // alias → CameraWalk[33]  // DAT_00561670
extern DWORD&  CameraWalk5Yaw;   // alias → CameraWalk[34]  // DAT_00561674
extern DWORD&  CameraWalk5Roll;   // alias → CameraWalk[35]  // DAT_00561678
extern DWORD   ServerSelectHi;   // IDA: DAT_00561694 (0x00561694)
extern DWORD   ServerSelectLo;   // IDA: DAT_00561698 (0x00561698)
extern DWORD   LoginErrorFlag;  // DAT_005616a0
extern int     AnimatedDialogY;   // dialog Y (signed; ver globals.cpp)  // DAT_005616a4
extern int     AnimatedCameraX;   // camera X (signed; ver globals.cpp)  // DAT_005616a8
extern DWORD   SelectedHero; // IDA: SelectedHero (0x005616AC)  // DAT_005616ac
extern DWORD   SelectedCharConfirm;  // DAT_005616b0
extern DWORD   TimeRemain; // IDA: TimeRemain (0x005616B8)
extern DWORD   IntroDebugString;  // DAT_005617a0
extern char    CharSelectNameFormat[];   // "%s" format string used by Scene_CharSelect labels  // DAT_00561a30
extern DWORD   TickFormatStub;  // empty-string format stub used by the per-tick sprintf (original string at DAT_00561b04 lost)  // DAT_00561b04
extern char    TextureExtJpgStr[]; // "OZJ" JPEG extension suffix  // DAT_00561b70
extern char    TextureExtTgaStr[];  // OpenTGA extension suffix used in non-Data2 path (e.g. ".tga")  // DAT_00561ba8

// Literales de string (datos de sólo lectura, etiquetas de Ghidra)
extern char    Img_LocalWebzenlogoJpg[];  // "Local/Webzenlogo.jpg" -- intro WebZen logo splash (OpenJPG, Scene_Intro)  // DAT_00561774
extern char    Img_LocalEveryoneJpg[];  // "Local/Everyone.jpg" -- intro "Everyone" splash (OpenJPG, Scene_Intro)  // DAT_0056178c
extern char    Img_LocalLoading01Jpg[];  // "Local/Loading01.jpg" -- loading screen background (OpenJPG, Scene_Loading)  // DAT_00561a88
extern char    Img_LocalLoading02Jpg[];  // "Local/Loading02.jpg" -- loading screen background (OpenJPG, Scene_Loading)  // DAT_00561a9c
extern char    Img_LocalLoading03Jpg[];  // "Local/Loading03.jpg" -- loading screen background (OpenJPG, Scene_Loading)  // DAT_00561ab0
extern char    Host_ConnectMuonlineCoKr[];  // "connect.muonline.co.kr" -- default server hostname (config default for szServerIpAddress)  // DAT_005615b8
extern char    Dir_Dialog[];  // "Dialog" -- directory prefix  // DAT_005595e0
extern char    Err_HashTableFullGetIndex[];  // "Hash table full!!! GetIndex" -- CErrorReport diagnostic message  // DAT_00558108
extern char    Err_HashTableFullInsert[];  // "Hash table full!!! Insert" -- CErrorReport diagnostic message  // DAT_005580e8
extern char    Txt_MacroTime[];  // "Macro_Time" -- macro timer label  // DAT_00559f30
extern char    Txt_Dash[];  // " - " -- dash separator text  // DAT_00559f44
extern char    Txt_DashPadR[];  // " -  " -- dash separator text (right-padded)  // DAT_00559f3c
extern char    Txt_DashPadL[];  // "  - " -- dash separator text (left-padded)  // DAT_00559f4c
extern char    Fmt_IntSlashInt[];  // "%d/%d" -- HUD numeric pair format  // DAT_00559f00
extern char    Fmt_HpSlashMaxHp[];  // "%d / %d" -- HUD HP / MaxHP format  // DAT_00559f1c
extern char    Fmt_NameSpLevel[];  // "%s %d" -- character name + level label (Scene_CharSelect)  // DAT_00561a34
extern char    MsgAccountBlocked[128];  // "Account is blocked." -- blocked-account message (char-select; content differs from old note "Cannot create...")  // DAT_07d49c14
extern char    MsgContactOperator[128];  // "Please contact the operator." -- blocked-account follow-up message (char-select)  // DAT_07d49d40
extern char    ServerInfoLine1[64];  // server info line 1 (char-select server details)  // DAT_00561a3c
extern char    ServerInfoLine2[64];  // server info line 2 (char-select server details)  // DAT_00561a58
extern char    ServerInfoLine3[64];  // server info line 3 (char-select server details)  // DAT_00561a68
extern char    Fmt_ServerRowNonPvp_A[];  // "%s%d Non PVP %s" -- server-list row format (non-PVP server)  // DAT_0056192c
extern char    Fmt_ServerRowNonPvp_B[];  // "%s%d Non PVP %s" -- server-list row format (non-PVP server)  // DAT_00561940
extern char    Fmt_ServerRowNonPvp_C[];  // "%s%d Non PVP %s" -- server-list row format (non-PVP server)  // DAT_00561954
extern char    Fmt_ServerRow_A[];  // "%s%d %s" -- server-list row format  // DAT_0056192c
extern char    Fmt_ServerRow_B[];  // "%s%d %s" -- server-list row format  // DAT_00561940
extern char    Fmt_ServerRow_C[];  // "%s%d %s" -- server-list row format  // DAT_00561954
extern char    Fmt_ServerRow_D[];  // "%s%d %s" -- server-list row format  // DAT_00561968
extern char    Fmt_ServerRow_E[];  // "%s%d %s" -- server-list row format  // DAT_00561974
extern char    Fmt_ServerRow_F[];  // "%s%d %s" -- server-list row format  // DAT_00561980
extern char    Fmt_ServerRowFull_A[];  // "%s%s" -- server-list row format (full server)  // DAT_00561914
extern char    Fmt_ServerRowFull_B[];  // "%s%s" -- server-list row format (full server)  // DAT_0056191c
extern char    Fmt_ServerRowFull_C[];  // "%s%s" -- server-list row format (full server)  // DAT_00561924

// ── Misc game globals (0x00583dxx – 0x00590xxx) ───────────────────────────────
// g_csQuest @ 0x00583D8C ES `g_csQuest` — el PUNTERO al objeto CSQuest, no
// el objeto.  Lo confirman los xrefs: ReceiveQuestHistory / State / Result
// referencian 0x583D8C y el decompile los muestra como `g_csQuest`; el objeto
// es `unk_567500` (= QuestTableBase), que es a donde apunta.
// Todos los consumidores lo usan como puntero (`(uintptr_t)g_csQuest +
// 0x1c87f`), por eso es un alias de g_csQuest y no un objeto propio.
// g_EnableSound (0x0590AC8) - sound master flag - see declaration below in Sound section. — see declaration below in Sound section.
// LegacyEnable3DSound = g_Enable3DSound (3D audio flag)    — see declaration below in Sound section.
// LegacySoundLoadCount = SoundLoadCount                     — see declaration below in Sound section.
// LegacyLpDS = g_lpDS (IDirectSound*)             — see declaration below in Sound section.
// LegacyWavefile = wavefile (CWaveFile* being loaded) — see declaration below in Sound section.
#define LegacyEnable3DSound  g_Enable3DSound  // DAT_00590ac9
#define LegacySoundLoadCount  SoundLoadCount  // DAT_00590acc
#define LegacyLpDS  g_lpDS  // DAT_00590ad0
#define LegacyWavefile  wavefile  // DAT_00590ad8
// LegacyGdwBufferBytes = g_dwBufferBytes (current WAV data chunk size).
#define LegacyGdwBufferBytes  g_dwBufferBytes  // DAT_00583da8

// ── Engine / Window handles (0x055c9xxx – 0x055caxxx) ─────────────────────────
// Nota: g_hWnd y g_hDC viven en CWindow; el DC del texto (DAT_055c9fec) en CFont.
extern char    SavedUsernameBuf[12];  // 12-byte saved username buffer (loaded/stored by Config_Load; originally noted as version-string buffer)  // DAT_055c9ba0
extern DWORD   LegacyEnableSound;   // g_EnableSound  // DAT_055c9b40
extern DWORD   SoundChannelIndexOffset;   // sound channel index offset  // DAT_055c9b60
extern DWORD   ScreenRateXBits;  // DAT_055c9b70
extern DWORD   ScreenRateYBits;  // DAT_055c9b74
extern DWORD   ChatFocusIdB;  // DAT_055c9b80
extern char    ConfigLoginVersion[12]; // IDA: m_ExeVersion — config.ini [LOGIN] Version string
// Contexto de la ofuscación por HashTable — buffer contiguo (el binario original tiene el
// 4 fields physically contiguous at 0x055c9bc8..0x055c9bd4). Callers do
// `&MAIN_HASH_CLASS + 3 * sizeof(void*)` esperando la capacidad; las macros superponen el buffer para que
// holds. See globals.cpp for init.
extern uintptr_t g_HashTableCtx[4];  // anti-tamper hash-table state: [0] vtable, [1] values, [2] keys, [3] capacity
#define MAIN_HASH_CLASS  (g_HashTableCtx[0])   // vtable pointer
#define HashTableValues  (g_HashTableCtx[1])   // values array  // DAT_055c9bcc
#define HashTableKeys  (g_HashTableCtx[2])   // keys array  // DAT_055c9bd0
#define HashTableCapacity  (g_HashTableCtx[3])   // capacity  // DAT_055c9bd4
extern BYTE*   HashTableBaseAlloc;  // DAT_055c9be0
extern float  HashTableResizeScatter;  // random even value written during hash-table rebuild (write-only)  // _DAT_055c9be4
extern BYTE*   HashValuesAlloc;  // DAT_055c9be8
extern BYTE*   HashKeysAlloc;  // DAT_055c9bec
extern DWORD   ErrorReportCtx;  // DAT_055c9bf0
extern DWORD   LocaleStringPtr;  // DAT_055c9d00
extern DWORD   ServerAddressStr;  // DAT_055c9e04
extern std::uintptr_t ItemAttrTableBase; // Reserva de la tabla primaria de atributos de items.  // DAT_055c9e44
extern std::uintptr_t EntityArrayAlloc;  // DAT_055c9e48
// RandomTable @ 0x055C9E58 — 100 enteros (`rand() % 360`) que siembra WinMain
// (0x41E8A0 L522-525).  Sólo lo usa Entity_Render (0x5038E0) para repartir en
// círculo las monedas del montón de Zen del suelo.
// Tiene que ser el array completo y estar sembrado: si no, las monedas se apilan
// en un mismo punto.
extern int     RandomTable[100];  // DAT_055c9e58
extern BYTE*   ChatHistoryWidget;   // objeto del historial de chat; layout pendiente de B2  // DAT_055c9ff0
extern BYTE*   WhisperChatWidget;   // objeto de susurros; layout pendiente de B2  // DAT_055c9ff4
extern uintptr_t UiRenderTextObj;  // DAT_055c9ff8
// DAT_055ca004 = g_hDC — vive en CWindow (gWindow.GetHdc()).
extern DWORD   g_hRC;  // DAT_055ca008
extern DWORD   AppQuitFlag;  // DAT_055ca018
extern char    g_IME_Active;  // DAT_055ca019
extern DWORD   g_FatalError;  // DAT_055ca020
extern DWORD   g_iNoMouseTime;   // IDA: DAT_055ca028 (0x055CA028)
extern char    g_TextBufNearFull;  // DAT_055ca031
extern DWORD   g_DBCS_Counter;  // DAT_055ca034
extern DWORD   EnterKeyPressedFlag;  // DAT_055ca038
extern DWORD   g_MouseX_at_down;  // DAT_055ca03c
extern DWORD   g_MouseY_at_down;  // DAT_055ca040
extern DWORD   LoginTokenBuf;  // DAT_055ca050
// Socket context struct (contiguous buffer, ~0x4030 bytes). Callers of
// CWsctlc_Startup/Net_Connect/Net_Disconnect pass the base address directly
// (originalmente el literal 0x55ca160). Los campos se superponen al buffer:
extern char    SocketClient[0x260000]; // IDA: SocketClient (0x055CA160)
#define SocketClientIsGame  (*(DWORD*)(SocketClient + 4))    // IDA: SocketClient.m_bGame (0x055CA164)
#define SocketClientSocket  (*(SOCKET*)(SocketClient + 8))   // IDA: SocketClient.m_socket (0x055CA168)
#define SocketClientSendBuffer  ((char*)(SocketClient + 0xC))    // IDA: SocketClient.m_SendBuf (0x055CA16C)
// El envío y FD_WRITE deben consultar el mismo contador dentro de CWsctlc.
#define SocketClientSendBufferLength (*(DWORD*)(SocketClient + 0x200C)) // IDA: 0x055CC16C
extern DWORD   SocketClientLogPrint;         // IDA: SocketClient.m_LogPrint (0x055CE174)

// ── Network / login state (0x05826xxx) ────────────────────────────────────────
extern DWORD   DecryptedCharName;  // DAT_05826bdc
extern DWORD   LegacyTradeRequester;  // DAT_05826c00
extern DWORD   LegacyTradeAccepted;  // DAT_05826c04
extern DWORD   SoccerTime; // IDA: SoccerTime (0x05826C08)  // DAT_05826c08
                               // evento; lo escriben InitGame y el F3/22.
extern DWORD   SimpleModulusXorKeys[4];   // CSimpleModulus XOR-deobfuscate key table  // DAT_00562e48
extern DWORD   g_SimpleModulusCS[17]; // IDA: g_SimpleModulusCS (0x05826C10)
extern DWORD   g_SimpleModulusSC[17]; // IDA: g_SimpleModulusSC (0x05826C58)
extern DWORD   m_nTempMyTradeGold; // IDA: m_nTempMyTradeGold (0x05826C9C)
extern DWORD   HeroIndex; // IDA: HeroIndex (0x05826CA0)
extern DWORD   HeroKey; // IDA: HeroKey (0x05826CAC)
extern DWORD   CurrentProtocolState; // IDA: CurrentProtocolState (0x05826CB0)  // DAT_05826cb0
extern char    ChatWhisperID[41]; // IDA: ChatWhisperID (0x05826CB4); 0.97.40: 40 bytes + NUL
extern DWORD   ServerInfoField1;  // DAT_05826cc0
extern DWORD   ServerInfoField2;  // DAT_05826cc8
extern char    ServerInfoField3;  // DAT_05826cc9
extern char    LogInID[16]; // IDA: DAT_05826CD4 (0x05826CD4)
extern DWORD   g_bGameServerConnected; // IDA: g_bGameServerConnected (0x05826CF0)
extern DWORD   g_dwLatestMagicTick; // IDA: g_dwLatestMagicTick (0x05826CF4)
extern DWORD   LogIn; // IDA: DAT_05826CF8 (0x05826CF8)
extern DWORD   ChatTime; // IDA: ChatTime (0x05826D08)
// Teleport es 0x05826D14 (ida_xrefs_to: ReceiveTeleport, Attack, CheckGate,
// Skills_PacketHandler, sub_482BE0, sub_4D23B0...).
// LegacyTeleport es un alias para que Attack escriba el mismo global que leen los
// demás (si no, el gate del Town Portal no se entera del teleport en curso).
#define LegacyTeleport Teleport  // DAT_05826d04
extern char    LastSentChatBuf[0x100]; // last-sent chat message buffer (rate-limit compare); 0.97.40: hasta 200 bytes  // DAT_05826adc
extern char    Teleport; // IDA: Teleport (0x05826D14)
                               // 1=gate/teleport en curso.  La etiqueta vieja
                               // ("NPC script keepalive") era falsa.
extern DWORD   BuyCost; // IDA: BuyCost (0x05826D18)
                               // lo escribe ProtocolCore y lo lee sub_4D23B0.
extern DWORD   EnableUse;      // IDA: EnableUse (0x05826D1C) — cooldown de equipar/usar item;  // DAT_05826d1c
                               // lo ponen en 10 los usos y lo resetean las respuestas del server
extern DWORD   DeleteCharResult;  // DAT_05826d20
extern DWORD   SummonLife; // IDA: SummonLife (0x05826D24)  // DAT_05826d24
// AttackPlayer (IDA @0x05826D28): indice de slot del ULTIMO atacante.  Lo
// escriben ReceiveAction (act 100/101), Skills_PacketHandler y
// ReceiveMagicPosition; lo lee ReceiveAttackDamage para el destello de
// bloqueo (efecto 259).
extern int     AttackPlayer;  // slot index of the last attacker (IDA @0x05826D28)
// MixState (0x07EAA140): 0 = inactivo, 1 = mezclando (esperando al server),
// 2..50 = animacion de la caja de Chaos, que `RenderItemsBoxes` avanza un paso
// por frame hasta 51.  Ver la nota de HUD_Pass4.cpp.
// Cuenta regresiva de evento (0x07E11D8C / 0x07E11D90).  Definidos en
// Render/HUD_Pass4.cpp; los escribe StartMatchCountDown (opcode 0x92).
extern "C" { extern int m_iMatchCountDownType; extern DWORD m_dwMatchCountDownStart; }
                               // invocada; lo alias-ea HUD_Pass2.cpp.
extern DWORD   LegacyTradeActive;  // DAT_05826d30
extern char    LegacyDuelMode;  // DAT_05826d31
extern char    LegacyTradeParam;  // DAT_05826d32
extern char    SoccerObserver; // IDA: SoccerObserver (0x05826D33)  // DAT_05826d33
extern DWORD   TradeResultLabels;  // DAT_05826dc8
struct PathContext;
extern PathContext* PathFindContext;  // DAT_05826df4
extern DWORD   FpsWindowStartTimeMs; // IDA: DAT_05826e04
// WorldTime se declara más abajo, con su comentario, en la sección "Entity / animation tick globals"
extern DWORD   FpsTimerInitialized;  // IDA: DAT_05826e0c
extern DWORD   FpsWindowFrameCount;  // DAT_05826e10
extern char    BoneQuaternionScratch[200 * 0x10];   // BoneQuaternion scratch (200 huesos x 16B)  // DAT_05826e18
// ModelsBase declared below in "Model data table" section

// ── Bone / skeleton data (0x06970xxx) ─────────────────────────────────────────
// Bone-matrix scratch pool (see globals.cpp). All DAT_06970XXX siblings alias
// slots en el mismo buffer, con stride 0x30 entre slots de hueso adyacentes.
// Los offsets de abajo se calculan relativos a 0x06970a9c (la etiqueta de Ghidra que
// fija el slot 0). Cada macro produce un lvalue DWORD, así que `&DAT_06970XXX` devuelve
// un puntero al inicio del slot y se mantiene la semántica vieja de leer/escribir DWORD.
extern char    g_BoneScratch[200 * 0x30];   // MAX_BONES=200 (más chico desborda sobre el preview char)
#define BoneScratchRoot   (*(DWORD*)(g_BoneScratch + 0x000))  // root bone slot  // DAT_06970a9c
#define BoneScratchSecA   (*(DWORD*)(g_BoneScratch + 0x030))  // DAT_06970acc
#define BoneScratchSlot060   (*(DWORD*)(g_BoneScratch + 0x060))  // DAT_06970afc
#define BoneScratchSecB   (*(DWORD*)(g_BoneScratch + 0x090))  // DAT_06970b2c
#define BoneScratchTertiary   (*(DWORD*)(g_BoneScratch + 0x0c0))  // DAT_06970b5c
#define BoneScratchQuaternary   (*(DWORD*)(g_BoneScratch + 0x120))  // DAT_06970bbc
#define EffectBoneD   (*(DWORD*)(g_BoneScratch + 0x150))  // DAT_06970bec
#define EffectFireBoneA   (*(DWORD*)(g_BoneScratch + 0x180))  // DAT_06970c1c
#define BoneScratchFireB   (*(DWORD*)(g_BoneScratch + 0x1b0))  // DAT_06970c4c
#define BoneScratchFireC   (*(DWORD*)(g_BoneScratch + 0x1e0))  // DAT_06970c7c
#define EffectFireBoneD   (*(DWORD*)(g_BoneScratch + 0x270))  // DAT_06970d0c
#define SpriteAnchorPosA   (*(DWORD*)(g_BoneScratch + 0x2d0))  // DAT_06970d6c
#define SpriteAnchorPosB   (*(DWORD*)(g_BoneScratch + 0x390))  // DAT_06970e2c
#define SpriteAnchorPosC   (*(DWORD*)(g_BoneScratch + 0x3f0))  // DAT_06970e8c
#define BoneScratchSlot450   (*(DWORD*)(g_BoneScratch + 0x450))  // DAT_06970eec
#define SpriteAnchorPosD   (*(DWORD*)(g_BoneScratch + 0x5d0))  // DAT_0697106c
#define EffectPortalBoneA   (*(DWORD*)(g_BoneScratch + 0x900))  // DAT_0697139c
#define BoneScratchPortalB   (*(DWORD*)(g_BoneScratch + 0xab0))  // DAT_0697154c
#define EffectPortalBoneC   (*(DWORD*)(g_BoneScratch + 0xae0))  // DAT_0697157c
#define EffectPortalBoneArr   (*(DWORD*)(g_BoneScratch + 0xb70))  // DAT_0697160c

// ── Preview character entity (0x07abf050) ─────────────────────────────────────
// CharSelPreviewEntity ES el struct de entidad del PREVIEW char de char-select (creado
// por CreateCharacterPointer con model 0xab). Es un entity struct COMPLETO (stride
// 0x394; el original lo espacia 0x580 hasta el array en 0x07abf5d0):
// CreateCharacterPointer escribe hasta +908. Los símbolos CharSelPreviewF00C …
// PreviewBodyOriginZ son CAMPOS de esta entidad (Ghidra los separó) y son macros que
// proyectan dentro del buffer. Ver globals.cpp.
extern char    CharSelPreviewEntity[0x580];  // DAT_07abf050
#define CharSelPreviewF00C (*(float*)(CharSelPreviewEntity + 0x00c))  // _DAT_07abf05c
#define  CharSelPreviewF00CBits (*(DWORD*)(CharSelPreviewEntity + 0x00c))  // DAT_07abf05c
#define CharSelCamOffsetX (*(float*)(CharSelPreviewEntity + 0x010))  // _DAT_07abf060
#define  CharSelCamOffsetXBits (*(DWORD*)(CharSelPreviewEntity + 0x010))  // DAT_07abf060
#define CharSelCamOffsetY (*(float*)(CharSelPreviewEntity + 0x014))  // _DAT_07abf064
#define  CharSelCamOffsetYBits (*(DWORD*)(CharSelPreviewEntity + 0x014))  // DAT_07abf064
#define CharSelCamOffsetZ (*(float*)(CharSelPreviewEntity + 0x018))  // _DAT_07abf068
#define  CharSelCamOffsetZBits (*(DWORD*)(CharSelPreviewEntity + 0x018))  // DAT_07abf068
#define CharSelPreviewF01C (*(float*)(CharSelPreviewEntity + 0x01c))  // _DAT_07abf06c
#define  CharSelPreviewF01CBits (*(DWORD*)(CharSelPreviewEntity + 0x01c))  // DAT_07abf06c
#define CharSelPreviewF020 (*(float*)(CharSelPreviewEntity + 0x020))  // _DAT_07abf070
#define  CharSelPreviewF020Bits (*(DWORD*)(CharSelPreviewEntity + 0x020))  // DAT_07abf070
#define  CharSelPreviewF084 (*(DWORD*)(CharSelPreviewEntity + 0x084))  // DAT_07abf0d4
#define CharSelColorR (*(float*)(CharSelPreviewEntity + 0x0e8))  // _DAT_07abf138
#define  CharSelColorRBits (*(DWORD*)(CharSelPreviewEntity + 0x0e8))  // DAT_07abf138
#define CharSelColorG (*(float*)(CharSelPreviewEntity + 0x0ec))  // _DAT_07abf13c
#define  CharSelColorGBits (*(DWORD*)(CharSelPreviewEntity + 0x0ec))  // DAT_07abf13c
#define CharSelColorB (*(float*)(CharSelPreviewEntity + 0x0f0))  // _DAT_07abf140
#define  CharSelColorBBits (*(DWORD*)(CharSelPreviewEntity + 0x0f0))  // DAT_07abf140
#define  CharSelPageSlot (*(DWORD*)(CharSelPreviewEntity + 0x1bc))  // DAT_07abf20c
extern char    ClassNameTable[];  // class name table (stride 300, 30 slots)  // DAT_07d2b494
#define PreviewBodyOriginX (*(float*)(CharSelPreviewEntity + 0x574))  // preview entity field (BodyOrigin x)  // _DAT_07abf5c4
#define PreviewBodyOriginY (*(float*)(CharSelPreviewEntity + 0x578))  // preview entity field (BodyOrigin y)  // _DAT_07abf5c8
#define PreviewBodyOriginZ (*(float*)(CharSelPreviewEntity + 0x57c))  // preview entity field (BodyOrigin z)  // _DAT_07abf5cc
extern std::uintptr_t CharactersClient; // IDA: CharactersClient (0x07ABF5D0)  // DAT_07abf5d0
extern int     VisibleEntityCount;   // visible entity counter (reset each frame)  // DAT_07abf5d4
extern char   *Hero; // IDA: Hero (0x07ABF5D8)  // DAT_07abf5d8
extern DWORD   EntitySlotCount;  // DAT_07abf5dc
extern float  RenderExtraFlag;   // extra flag (reset 0 each frame)  // _DAT_07abf5e8
extern DWORD   RenderExtraFlagBits;  // DAT_07abf5e8
extern char    SpritePoolBase[kParticlePoolSize * 0x70];   // particle pool — 3000 slots × 112B  // DAT_07abf5f0

// ── Skill effects pool (0x07b1xxxx – 0x07c7xxxx) ─────────────────────────────
extern DWORD   MapEffectPoolBase;  // DAT_07b11698
extern DWORD   ParticlePoolBase;  // DAT_07b116d0
extern DWORD   ItemDropPoolBase;  // DAT_07b27b08
extern unsigned char SkillEffectPool[200 * 0x70];   // SkillEffect pool: 200 slots × 0x70 bytes  // DAT_07c5ab3c
// El unk_7C5AB5C de IDA no es una segunda alocación: es el +0x20 dentro del
// pool de efectos 0x7C5AB3C. Weather_Update lo recorre con stride de 0x70 bytes.
// Mantenerlo como global separado desplaza ese recorrido al pool de blur.
// IDA MoveLeaves arranca en 0x7C5AB5C y SkillEffect_Render en 0x7C5AB3C, ambos
// sobre el MISMO pool cuyo slot base es 0x7C5AB30. Como SkillEffectPool[0] es el
// slot base en este arbol, el alias correcto es +0x2C (no +0x20): la diferencia
// 0x7C5AB5C - 0x7C5AB30 = 0x2C. Verificado con dos campos independientes —
// el tipo (i-40 == v0-8) y la posicion (i-28 == v0+4) caen en el mismo lugar.
#define WeatherParticleBase (*(DWORD*)(SkillEffectPool + 0x2c))  // DAT_07c5ab5c
// RenderCharacter (00456770) captures these bone transforms for action kind
// 0x4D; AttackEffect (00445230) consume los dos primeros en un tick posterior.
extern float   g_AttackEffectMatrix_04D[3][4];
extern float   g_AttackEffectMatrix_04D_Alt[3][4];
extern float   g_AttackEffectMatrix_04D_Aux[3][4];
// Pool de render de joints/trails/blur — ver globals.cpp. La base es el array
// `g_RenderPool`; JointRenderPool es el campo ancla +12 del slot[0].
extern char    g_RenderPool[100 * 0x2f0];  // legacy render pool, 100 slots x 0x2F0 (JointRenderPool = slot[0]+12 anchor)  // DAT_07c608a8
// Dueños de slot: los pools conservan el layout x86, con 4 bytes para el puntero.
extern uintptr_t g_ParticleOwner[kParticlePoolSize];  // owner object per particle slot (particle -> entity attribution)
extern uintptr_t g_TrailOwner[100];  // owner character per trail slot
extern uintptr_t g_OperateObjects[200];  // interactable object pointer table (cursor pick targets)
#define JointRenderPool  (*(DWORD*)(&g_RenderPool[12]))  // DAT_07c608b4
extern DWORD   WeatherIntensityTarget;  // DAT_07c74ae0
extern DWORD   WeatherIntensityCurrent;   // weather intensity target (interpolated)  // DAT_07c74ae4
extern DWORD   RainYOffset;   // rain Y offset (driven by fsin)  // DAT_07c74ae8
extern DWORD   RainZCounter;   // rain position counter (0..2000)  // DAT_07c74aec

// ── Player render pool ─────────────────────────────────────────────────────────
// El walker v1 de Player_Render arranca en PlayerRenderV1AnchorPtr y lee offsets NEGATIVOS
// hasta -0xEC. El pool REAL es g_PlayerRenderPool[100 × 0x1BC] cubriendo todos
// los slots; PlayerRenderV1AnchorPtr es un alias a offset +0xEC (donde v1 vive en cada slot).
extern char    g_PlayerRenderPool[100 * OBJECT_STRIDE];  // player/plane render pool, 100 slots x 0x1BC stride
extern DWORD*  PlayerRenderV1AnchorPtr;   // = g_PlayerRenderPool + 0xEC (v1 anchor)  // DAT_07c74f54

// ── g_CharData XOR-encoded block (0x07cfxxxx) ─────────────────────────────────
extern void   *CharacterAttributePtr;   // g_CharData sub-pointer (XOR-encoded)  // DAT_07cf1ff4
extern void   *CharacterMachinePtr;   // g_CharData pointer (XOR-encoded, 0x584 bytes)  // DAT_07cf1ffc

// ── Player input / targeting state (0x07e01xxx – 0x07e11xxx) ─────────────────
extern DWORD   TargetX;   // target grid X (from ray cast or hover click)  [IDA 0x07E016C0]
extern DWORD   TargetY;   // target grid Y  [IDA 0x07E016C4]
// 0x07E109C8 es `ItemKey` (confirmado por xrefs de IDA: lo escribe
// Player_InputTick al clickear un item del suelo y lo leen los 3 sitios de
// `Action` que arman el pickup).  NO es un indice de NPC — esa etiqueta
// vieja hizo que el camino de llegada releyera SelectedItem en vivo.
extern DWORD   ItemKey;  // DAT_07e109c8
extern DWORD   HoverTargetType;   // hover target entity type (from entity+2)  // DAT_07db8708
extern char    ChatHistoryBuffer[5 * 256];   // historial de chat: 5 entradas x 256B  // DAT_07e113e4

// ── UI / HUD data (0x07e1xxxx – 0x07eaxxxx) ──────────────────────────────────
extern DWORD  FacingAnglePacket;   // player facing angle (float, sent in movement packets)  // _DAT_07e118e4
extern DWORD   WorldMapType;   // world/map type  // DAT_07e118e8
extern char    GuildInputEnable;   // char-select init flag  [IDA 0x07E11D70]
extern char    ImeModeFlag;   // char-select flag B  // DAT_07e11d71
extern char    GoldInputEnable;   // char-select flag C  [IDA 0x07E11D72]
extern int     InputGold;   // gold / currency (checked vs 0x2faf081)  [IDA 0x07E11D74]
extern DWORD   ActiveInputIndex;   // InputIndex — indice del campo de input activo (lo rota el Tab)  // DAT_07e11d78
extern DWORD   MacroCooldown;   // MacroTime (0x07E11D7C) -- cooldown de las macros de chat  // DAT_07e11d7c
extern DWORD   InputCooldownTicks;   // per-frame cooldown tick counter  // DAT_07e11d1c
extern DWORD   MoveDebounceCounter;   // movement debounce step counter  // DAT_07e11d28
extern DWORD   EditFlag;   // frame counter (anti-tamper)  // DAT_07e11d30
extern int     CursorFrameIndex;   // cursor sprite size/frame index  // DAT_07e11d5c
extern DWORD   DontMoveFlag;   // terrain walk flag (0=walkable 1=blocked)  // DAT_07e11d64
extern float  FacingLastWorldTime;   // facing tick reference (last frame time)  // _DAT_07e11d4c
extern float  FacingDelta;   // facing dt (delta * speed scale)  // _DAT_07e11d50
extern BYTE    InputFieldFlags[40];   // world-enter counter (memset writes 40 bytes)  // DAT_07e113d8
extern char&   InputPasswordMaskFlag;   // alias de InputFieldFlags[1] (password-mask flag)  // DAT_07e113d9
extern DWORD   SkillNotifyType;  // DAT_07e11d8c
extern DWORD   SkillNotifyStartTick;  // DAT_07e11d90
extern DWORD   ImeOldConv;  // DAT_07e11d94
extern DWORD   ImeCurrConv;  // DAT_07e11d98
extern DWORD   BlinkFrameCounter;  // DAT_07e11da0
extern DWORD   PacketBufSlotCount;  // DAT_07e11db4
extern DWORD   MoveStepCounter;   // movement step count (must be >0x27 for facing packet)  // DAT_07e11db8
extern int     FacingAngle;   // last facing tick time  // DAT_07e11dbc
extern char    MoveLockFlag;   // movement lock flag B  // DAT_07e11dc0
extern DWORD   NpcDialogActiveFlag;   // NPC script: dialog-active flag  // DAT_07e11dc4
extern DWORD   NpcKeepaliveTick;   // NPC script: keepalive timer (GetTickCount at last send)  // DAT_07e11dc8
extern DWORD   TotalPacketSize;   // IDA: DAT_07e11dcc (0x07E11DCC)
extern DWORD   _PartyNumber;   // NPC script chat-log slot 0  // DAT_07e11e50
extern DWORD   _PartyKey;   // NPC script chat-log slot 1  // DAT_07e11e54
extern DWORD   NpcScriptChatLogSlot2;   // NPC script chat-log slot 2  // DAT_07e11e58
extern DWORD   NpcScriptChatLogSlot3;   // NPC script chat-log slot 3  // DAT_07e11e5c
extern DWORD   TargetInvSlot;   // target entity index (active selection)  // DAT_07e11e78
extern DWORD   PartyHPBarsTable;   // party HP bars (stride 0x24)  // DAT_07e11e98
// 0x07E11E9C = Party + 0x1C (indice de entidad del slot 0). Era un char suelto.
#define LegacyPartySlotArray (*(char*)(Party + 0x1C))  // DAT_07e11e9c
extern char    HudTextVisibleFlag;            // flag reset each login frame  // DAT_07e11d6e
extern DWORD   InventoryItemSelectState;            // UI inventory/item selection state  // DAT_07d552e4
extern DWORD   MouseOnWindowFlag;            // UI click/selection flag  // DAT_07d78094
extern BYTE    InputTextLengths[40];         // username field length (memset writes 40 bytes)  // DAT_07d780a8
// InputLength[1] — el largo del campo 1 (destino del susurro).
//
// No es un DWORD aparte: `WM_CHAR` lee el largo como `((DWORD*)InputTextLengths)[slot]`,
// o sea los bytes +4..+7 del array. En el binario las dos cosas son la misma
// memoria (0x07D780A8 es InputLength[10] y 0x07D780AC es su elemento 1), y los
// sitios que escriben el largo del susurro (historial de flechas, tab-complete,
// click derecho sobre un jugador, cierre del menu) tienen que llegar al array.
// El buffer companero (PasswordInputBuffer) es igualmente un alias del slot 1.
#define InputLength1   (*(DWORD*)&InputTextLengths[4])  // DAT_07d780ac
// IDA: InputText[10][256] @ 0x07db8710. Slot 0 = username/chat, slot 1 =
// contraseña/destino-del-susurro. PasswordInputBuffer es el alias +0x100 del slot 1 que usan
// los caminos de login/susurro; el mismo storage respalda a RenderInputText.
extern char    InputTextBuffers[10][256];    // input slot table (10 × 256 bytes)  // DAT_07db8710
#define PasswordInputBuffer   (InputTextBuffers[1]) // alias: +0x100 = slot 1  // DAT_07db8810
// extern char    LegacyConnectStatusStr[256];       // connecting status string buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyCharNameSendBuf[256];       // char name send buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyCharNameConfBuf[256];       // char name confirm buffer   // -> alias a GlobalText, ver el final del archivo
extern DWORD   ServerSelectRenderFlag;            // server select render flag  // DAT_07d52c38
// extern char    LegacyServerGroupStr[256];       // server group name buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyServerChanStr[256];       // server channel name buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyUsernameDispBuf[256];       // username display buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyPasswordDispBuf[256];       // password display buffer   // -> alias a GlobalText, ver el final del archivo
extern char    BtnTextOk[128];  // "OK" -- login panel OK button text  // DAT_07d4aed4
extern char    BtnTextExit[128];  // "Exit" -- login panel exit/cancel button text  // DAT_07d4b000
// extern char    LegacyCharNameFmt[128];       // char name format string   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyVersionStr1[128];       // version string 1   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyVersionStr2[128];       // version string 2   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyVersionStr3Fmt[128];       // version string 3 format   // -> alias a GlobalText, ver el final del archivo
extern char    MsgConnecting[128];  // "Connecting..." -- connect-status string (Ghidra split of the LegacyConnectStatusStr block)  // DAT_07d4c518
extern DWORD   GroundItemArray;  // DAT_07e127f8
constexpr int GROUND_ITEM_STRIDE = 0x48 + OBJECT_STRIDE;  // cabecera + OBJECT
extern unsigned char GroundItemPool[1000 * GROUND_ITEM_STRIDE];   // GroundItem pool: 1000 × 0x204 bytes  // DAT_07e12840
extern DWORD   PlayerSpritePoolBase;   // sprite render loop base (stride 0x204)  // DAT_07e12945
extern char    PickedItemData[0x44];      // pPickedItem — sizeof(ITEM) = 0x44  // DAT_07e91350
extern "C" DWORD   ItemPickedFlag;  // DAT_07e91388
extern byte    InvGridSelCol;   // UI grid selected column (byte, Item_ReturnPickedItem)  // DAT_07e9138e
extern byte    InvGridSelRow;   // UI grid selected row (byte, Item_ReturnPickedItem)  // DAT_07e9138f
extern short   PinShuffleDigits[10];   // IDA: word_7E91394  // DAT_07e91394
extern char    MaskedFieldPrefix;       // IDA: byte_7EAA1A4  // DAT_07eaa1a4
extern DWORD   ScreenBannerText;  // DAT_07e913a8
extern DWORD   CharSelectClearScratch;  // write-only scratch cleared at char-select entry (alongside ScreenBannerText)  // DAT_07e91428
extern DWORD   GuildListStartYStorage;  // DAT_07e91784
extern DWORD   GuildListStartXStorage;  // DAT_07e91788
extern DWORD   TradeSlotEntityIds;  // DAT_07e919b8
// Tabla de nombres stride 80 (guild members / buffs). NO es un DWORD: los
// consumidores (HUD_Pass2, HUD_Pass3, SecondPassword, SMD_Parser y el
// handler 0x52 de Net_Process) la recorren con `&GuildInfoTable + N*80`.
extern char    GuildInfoTable[0x13C30];  // DAT_07e919bc
// Pools de índices de nombres de personaje — cada uno es un buffer de 0x880 bytes (32 entradas × 0x44).
// Lo recorre CloseInventoryRelatedWindows (CharPreview_Reset) con stride 0x44.
// LegacyInventory: alias de Inventory (ver el bloque de alias de pools mas abajo).
extern DWORD   SourceInvSlot;  // DAT_07ea5b18
extern DWORD   HudBtn1PosX;  // DAT_07ea5b1c
extern DWORD   HudBtn1PosY;  // DAT_07ea5b20
extern DWORD   LegacyInventoryItems;  // DAT_07ea8410
extern DWORD   LegacyInventoryLevels;  // DAT_07ea8414
// unk_7EA9504 / unk_7EA9328 NO son un buffer aparte: son dos posiciones dentro
// del inventario real (OffsetInventoryItems @ 0x07EA8410).  Ver la derivación
// completa en globals.cpp, junto a la definición de los punteros.
extern int    *p_DAT_07ea9504_;   // &OffsetInventoryItems[63*0x44+56] (slot 63, Key)
extern int    *p_DAT_07ea9328_;   // &OffsetInventoryItems[56*0x44+56] (slot 56, Key)
#define InventoryGridEnd  (*p_DAT_07ea9504_)
#define InventoryGridBase  (*p_DAT_07ea9328_)
extern BYTE*   InventoryBasePtr;  // DAT_07ea9800
extern BYTE*   g_ItemMoveSourcePool;  // item drag source buffer pointer (Item_Move)
extern BYTE*   g_ItemMoveTargetPool;  // item drag target buffer pointer (Item_Move)
extern DWORD   ShopContextA;   // NPC shop context A  // DAT_07ea9804
extern DWORD   ShopContextB;   // NPC shop context B  // DAT_07ea9808
extern DWORD   ShopContextC;   // NPC shop context C  // DAT_07ea980c
extern DWORD   EquipTargetSlot;  // DAT_07ea9810
extern char    PinBytesPart0Bits[16];   // IDA: dword_7EA9814 — texto tipeado en el teclado del PIN  // DAT_07ea9814
// Alias dentro de ese buffer (el binario los aborda como globales sueltos):
#define PinBytes4to7   (*(unsigned int*)&PinBytesPart0Bits[4])  // DAT_07ea9818
#define PinBytes8to9   (*(unsigned short*)&PinBytesPart0Bits[8])  // DAT_07ea981c
#define PinByte10   (PinBytesPart0Bits[10])  // DAT_07ea981e
#define PinFirstPassCopy   (*(unsigned int*)&PinBytesPart0Bits[11])   // copia del PIN de la 1ra pasada  // DAT_07ea981f
// PinDigitByte1/2/3 (DAT_07ea9815/16/17, bytes dentro de PinBytesPart0Bits) eran externs
// sin usos: eliminados en la pasada de muertos.

extern DWORD   CharacterInfoStartXStorage;   // Screen3 panel origin X  // DAT_07ea982c
extern DWORD   CharacterInfoStartYStorage;   // Screen3 panel origin Y  // DAT_07ea9830
extern char    TradePartnerName[41];    // IDA: nombre remoto de Trade (0.97.40: 40 bytes del paquete + NUL)  // DAT_07ea9834
extern char    PinResetFlag;  // DAT_07ea983e
extern DWORD   TradePartnerKey;  // DAT_07eaa0d0
extern DWORD   GuildInviteKey;  // DAT_07eaa0d8
// 0x07EAA0E0 = PartyNumber. Era una copia que nadie escribia.
extern DWORD   PartyInviteKey;  // DAT_07eaa0e4
extern DWORD   TradeOtherGold;  // DAT_07eaa0f0
extern DWORD   TradeMyGold;  // DAT_07eaa0f4
extern int     RepairGoldTotal;   // CharData_RecalcDurability accumulator (count of items needing repair)  // DAT_07eaa0f8
extern DWORD   m_bYourConfirm;   // IDA: DAT_07eaa0fc (0x07EAA0FC)
extern char    m_bMyConfirm;   // IDA: DAT_07eaa0fd (0x07EAA0FD)
extern int     TradeYourWait;       // IDA: m_nYourTradeWait
extern int     TradeMyWait;         // IDA: m_nMyTradeWait
extern DWORD   TradeRemoteGuildKey; // IDA: DAT_00559F54 durante Trade
extern WORD    TradeRemoteLevel;    // IDA: DAT_07EAA0EC durante Trade
extern DWORD   m_nMyTradeWait;   // shop/item selection A  [IDA 0x07EAA104]
extern DWORD   StorageGoldFlag;   // shop/item selection B  [IDA 0x07EAA108]
extern char    GuildOpened;  // DAT_07eaa114
extern char    PartyOpened;  // DAT_07eaa115
extern char    CharacterOpened;  // DAT_07eaa116
extern char    InventoryOpened;  // DAT_07eaa117
extern char    ShopOpened;  // DAT_07eaa118
extern char    WarehouseOpened;  // DAT_07eaa119
extern char    ChaosMixOpened;  // DAT_07eaa11a
extern char    TradeOpened;  // DAT_07eaa11b
extern char    EventWindowOpened;  // DAT_07eaa11c
extern DWORD   GuildCreatorOpened;  // DAT_07eaa124
extern "C" DWORD   GoldenArcherOpenType;   // IDA: DAT_07eaa128 (0x07EAA128)
extern "C" DWORD   ItemActionMode;  // DAT_07eaa13c
extern DWORD   MixType;   // guild join/select current selection index  [IDA 0x07EAA16C]
extern DWORD   g_iKeyPadEnable;  // DAT_07eaa144
extern DWORD   ShopItemSelectionC;   // shop/item selection C  // DAT_07eaa148
extern DWORD   PinInputModeState;  // DAT_07eaa14c
extern DWORD   ServerContextId;  // DAT_07eaa150
extern DWORD   TextLineCount;  // DAT_07eaa154
extern uintptr_t CheckInventoryPtr;  // DAT_07eaa160
extern char    ItemMoveInFlightFlag;   // char-select packet-sent flag  // DAT_07eaa165
extern DWORD   MixRecipeLevel;   // TextureEnable (GL texture state cache)  // DAT_07eaa168
extern char    PinInputActiveFlag;  // DAT_07eaa179
extern char    InventoryDropErrorText;   // inventory drop error message ID string  // DAT_07eaa190
// Frustum world corners: 5 corners × 3 floats = 15 DWORDs at 0x07eab1b0
extern float   FrustrumVertex;   // corner 0 X (apex)  [IDA 0x07EAB1B0]
extern float   FrustrumCorner0Y;   // corner 0 Y  // DAT_07eab1b4
extern float   FrustrumCorner0Z;   // corner 0 Z  // DAT_07eab1b8
extern float   FrustrumCorner1X;   // corner 1 X (top-left)  // DAT_07eab1bc
extern float   FrustrumCorner1Y;   // corner 1 Y  // DAT_07eab1c0
extern float   FrustrumCorner1Z;   // corner 1 Z  // DAT_07eab1c4
extern float   FrustrumCorner2X;   // corner 2 X (top-right)  // DAT_07eab1c8
extern float   FrustrumCorner2Y;   // corner 2 Y  // DAT_07eab1cc
extern float   FrustrumCorner2Z;   // corner 2 Z  // DAT_07eab1d0
extern float   FrustrumCorner3X;   // corner 3 X (bot-right)  // DAT_07eab1d4
extern float   FrustrumCorner3Y;   // corner 3 Y  // DAT_07eab1d8
extern float   FrustrumCorner3Z;   // corner 3 Z  // DAT_07eab1dc
extern float   FrustrumCorner4X;   // corner 4 X (bot-left)  // DAT_07eab1e0
extern float   FrustrumCorner4Y;   // corner 4 Y  // DAT_07eab1e4
extern float   FrustrumCorner4Z;   // corner 4 Z  // DAT_07eab1e8
extern DWORD   TerrainTileIdxTL;  // DAT_07eab1ec
extern DWORD   TerrainTileIdxTR;  // DAT_07eab1f0
extern DWORD   TerrainTileIdxBL;  // DAT_07eab1f4
extern DWORD   TerrainTileIdxBR;  // DAT_07eab1f8
extern float   WaterWaveHeightTable[256 * 256];  // water-wave heights (row*256+col floats)  // DAT_07eab200
                               // PrimaryTerrainLightStorage @0x081CB608). Global muerto, sin
                               // usos — no reintroducirlo.
extern DWORD   FrustrumFaceD;   // IDA: DAT_07eeb200 (0x07EEB200)
extern DWORD   FrustrumPlane1Dist;  // DAT_07eeb204
extern DWORD   FrustrumPlane2Dist;  // DAT_07eeb208
extern DWORD   FrustrumPlane3Dist;  // DAT_07eeb20c
extern DWORD   FrustrumPlane4Dist;  // DAT_07eeb210
extern float   WaterMove;      // WaterMove — terrain water UV scroll offset (RenderTerrain)  // DAT_07eeb214
extern float   FrustrumY[4];   // frustum quad Y[4] (per TestFrustrum2D)  [IDA 0x07EEB218]
extern float   FrustrumX[4];   // frustum quad X[4]  [IDA 0x07EEB228]
extern float   TerrainLightAmbientBuf[256 * 256 * 3];   // TerrainLight RGB ambient buffer (256x256 RGB floats)  // DAT_07eeb238
extern DWORD   TerrainUvX;  // DAT_07feb238
extern DWORD   TerrainUvY;  // DAT_07feb23c
extern float   g_TerrainTexCoord[8];   // TerrainTextureCoord[4][2] — UVs per tile face (RenderTerrainFace/FaceTexture)
// Tile pick corners buffer (12 floats contiguous, 4 vec3 corners)
extern float   g_TilePickBuf[12];  // 4 vec3 tile-pick corners for hover highlight (Mouse_Hover)
#define TilePickMinX  (g_TilePickBuf[0])  // DAT_07feb258
#define TilePickMinY  (g_TilePickBuf[1])  // DAT_07feb25c
#define TilePickBufZ  (g_TilePickBuf[2])  // DAT_07feb260
#define TilePickCornerBX (g_TilePickBuf[3])  // _DAT_07feb264
#define TilePickCornerBY (g_TilePickBuf[4])  // _DAT_07feb268
#define TilePickCornerBZ (g_TilePickBuf[5])  // _DAT_07feb26c
#define TilePickCornerCX (g_TilePickBuf[6])  // _DAT_07feb270
#define TilePickCornerCY (g_TilePickBuf[7])  // _DAT_07feb274
#define TilePickCornerCZ (g_TilePickBuf[8])  // _DAT_07feb278
#define TilePickCornerDX (g_TilePickBuf[9])  // _DAT_07feb27c
#define TilePickCornerDY (g_TilePickBuf[10])  // _DAT_07feb280
#define TilePickCornerDZ (g_TilePickBuf[11])  // _DAT_07feb284
extern float   TerrainNormalTable[256 * 256 * 3];   // TerrainNormal[256*256][3] vertex normals  // DAT_07feb288

// ── Large game data arrays (0x080xxxxx – 0x083bxxxx) ─────────────────────────
extern DWORD   SelectXF;   // cursor screen X (float)  // DAT_080ab288
extern DWORD   SelectYF;   // cursor screen Y (float)  // DAT_080ab28c
extern unsigned char  TerrainMappingLayer2[0x10000];   // TileTex2[256*256] tile texture index 2  [IDA 0x080AB2B4]
extern unsigned char  TerrainMappingLayer1[0x10000];   // TileTex1[256*256] tile texture index 1  [IDA 0x080BB2B4]
extern float   BackTerrainHeight[0x10000];          // BackTerrainHeight[256*256] tile height  // DAT_080cb2cc
extern float   TerrainNoiseTable[0x10000];          // TerrainNoise[256*256] random noise init  // DAT_0810b2cc
extern DWORD   WaterRenderDisableFlag;  // DAT_0814b2dc
// Ambient terrain-object table — sub_4F7060 (Terrain_SpawnAmbientObjects) walks 100 records de
// stride 8 leyendo campos en [-5..+1].  Backing zeroado con prefijo de 5 bytes
// para que las lecturas [-5] queden in-bounds; vacío en nuestro build (el loader
// de objetos .obj no está wireado) → el loop no spawnea nada.
// &LegacyTerrainObjTable == &g_TerrainObjTable[5].
extern BYTE    g_TerrainObjTable[0x328];  // ambient terrain-object table (sub_4F7060); &LegacyTerrainObjTable = &[5]
#define LegacyTerrainObjTable  (g_TerrainObjTable[5])  // DAT_081cb2ed
// Buffer de iluminación por tile, vivo — dimensionado para 256×256 tiles × 3
// floats. Las macros de abajo proyectan PrimaryTerrainLightG/610 en el 2do/3er DWORD del
// slot, como el layout contiguo del binario original.
extern float   PrimaryTerrainLightStorage[256 * 256 * 3];  // DAT_081cb608
extern float   TerrainLightData[256 * 256 * 3];   // TerrainLightData[256*256][3] post-normal lighting  // DAT_0828b608
// cb60c/cb610 son el 2do/3er DWORD del slot 0 de cb608 (y análogamente para
// 0828b608). El código los accede como `(&DAT_*60c)[iVar2*3]` (lee el 2do campo
// del slot iVar2) o `(char*)&DAT_*60c + iVar2*12` (escribe en el 2do campo).
#define PrimaryTerrainLightG   (*(DWORD*)((char*)&PrimaryTerrainLightStorage[0] + 4))  // DAT_081cb60c
#define PrimaryTerrainLightB   (*(DWORD*)((char*)&PrimaryTerrainLightStorage[0] + 8))  // DAT_081cb610
#define TerrainLightDataG   (*(DWORD*)((char*)&TerrainLightData[0] + 4))  // DAT_0828b60c
#define TerrainLightDataB   (*(DWORD*)((char*)&TerrainLightData[0] + 8))  // DAT_0828b610
extern float   TerrainMappingAlpha[0x10000];          // TerrainMappingAlpha[256*256] — blend factor layer1↔layer2 (IDA/DLL; NO es height)  [IDA 0x0834B608]
extern float   FrustrumFaceNormal;  // frustum plane 0 normal X  [IDA 0x0838B7C4]
extern float   FrustrumPlane0NormalY;  // frustum plane 0 normal Y  // DAT_0838b7c8
extern float   FrustrumPlane0NormalZ;  // frustum plane 0 normal Z  // DAT_0838b7cc
extern float   FrustrumPlane1NormalX;  // frustum plane 1 normal X  // DAT_0838b7d0
extern float   FrustrumPlane1NormalY;  // frustum plane 1 normal Y  // DAT_0838b7d4
extern float   FrustrumPlane1NormalZ;  // frustum plane 1 normal Z  // DAT_0838b7d8
extern float   FrustrumPlane2NormalX;  // frustum plane 2 normal X  // DAT_0838b7dc
extern float   FrustrumPlane2NormalY;  // frustum plane 2 normal Y  // DAT_0838b7e0
extern float   FrustrumPlane2NormalZ;  // frustum plane 2 normal Z  // DAT_0838b7e4
extern float   FrustrumPlane3NormalX;  // frustum plane 3 normal X  // DAT_0838b7e8
extern float   FrustrumPlane3NormalY;  // frustum plane 3 normal Y  // DAT_0838b7ec
extern float   FrustrumPlane3NormalZ;  // frustum plane 3 normal Z  // DAT_0838b7f0
extern float   FrustrumPlane4NormalX;  // frustum plane 4 (near) normal X  // DAT_0838b7f4
extern float   FrustrumPlane4NormalY;  // frustum plane 4 (near) normal Y  // DAT_0838b7f8
extern float   FrustrumPlane4NormalZ;  // frustum plane 4 (near) normal Z  // DAT_0838b7fc
extern unsigned char BmpHeaderBuffer[1080];   // BMPHeader (BITMAPFILEHEADER + DIB + palette)  // DAT_0838b800
extern DWORD   TerrainFlag;    // TerrainFlag (RenderTerrain pass selector: 0=base, 2=alpha overlay)  // DAT_0838bc44
extern char    TerrainWallFlags[];  // char array (monster name/flag table)  // DAT_0838bc70
extern DWORD   TerrainLightBufToggle;    // terrain-light double-buffer toggle (RenderTerrain ^=1 cada frame)  // DAT_0839bc88
extern DWORD   TerrainNoDrawFlag;  // DAT_0839bc86
extern DWORD   WaterTextureNumber;  // DAT_0839bc8c
extern DWORD   FrustrumBoundMinX_1;   // IDA: DAT_0839bc90 (0x0839BC90)
extern DWORD   FrustrumBoundMinY_1;   // IDA: DAT_0839bc94 (0x0839BC94)
extern DWORD   FrustrumBoundMinX_2;   // IDA: DAT_0839bc98 (0x0839BC98)
extern DWORD   FrustrumBoundMinY_2;   // IDA: DAT_0839bc9c (0x0839BC9C)
extern DWORD   RenderPoolA;  // DAT_0839be18

// ── Scene / state machine vars (0x083a0xxx – 0x083bxxxx) ─────────────────────
extern char    OperateEntryPool[0x960];   // Entity render-state pool (stride 0xc, 128 slots, 0x960 bytes)  // DAT_083a2370
#define TableEndMarker OperateEntryPool[0x960-1]  // end-marker alias  // DAT_083a2cd0
extern char    FishObjectPool[10 * OBJECT_STRIDE];   // Boids pool: 10 entries × 0x1bc bytes  // DAT_083a2e90
// `Operates` de IDA = 0x083A2378 = OperateEntryPool + 8, o sea el campo [2] (puntero
// al objeto) de la ENTRADA 0 de la lista de objetos interactuables.  Los
// consumidores lo indexan `Operates[3 * SelectedOperate]`, que salta de entrada
// en entrada (3 DWORDs = los 12 bytes de stride).
//
// No es un DWORD suelto: `((int*)&SpecialObjPtrTable)[i*3]` tiene que caer dentro de
// la lista (con un global aparte, SelectedOperate == 0 da `*(short*)(0 + 2)` →
// crash).
#define SpecialObjPtrTable  (*(DWORD*)&OperateEntryPool[8])  // DAT_083a2378
extern float  EarthQuakeStrength;  // _DAT_083a0210
extern DWORD   EarthQuakeStrengthBits;  // DAT_083a0210
// Grilla 16x16. El contrato x86 conserva los 0x1000 bytes del binario.
// Los consumidores usan celdas y punteros nativos, nunca strides de DWORD.
extern SceneObjectCell g_ObjectBucketGrid[256];  // spatial bucket grid for scene objects (256 cells)
extern DWORD   NpcVisiblePool;  // DAT_083a1378
extern DWORD   RenderPoolB;  // DAT_083a2e92
extern DWORD   WeatherModeState;  // DAT_083a3ff0
extern char    InGameModeFlag;   // Scene_Intro: normal-path flag  // DAT_083a410c
// MouseRayEndPointStorage — 3-DWORD storage para mouse-ray endpoint (Camera_MouseRay
// escribe out_ray[0..2]). MouseRayEndPoint es referencia al [0].
extern DWORD   MouseRayEndPointStorage[3];  // 3-DWORD storage for the mouse-pick ray endpoint (Camera_MouseRay)
extern DWORD&  MouseRayEndPoint;  // DAT_083a4110
extern DWORD   MouseLButtonPush;  // MouseLButtonPush flag — VALORES VÁLIDOS: {0,1}  // DAT_083a4124
// Helper de defensa: usar IsClickPushed() en vez de truthy-check `&& MouseLButtonPush`.
// Algún stub stompea MouseLButtonPush con un valor wild (0x4397355A observado) que
// pasaría como verdadero y dispara phantom clicks. La igualdad estricta == 1
// suprime el síntoma mientras buscamos al culpable.
static inline bool IsClickPushed() { return MouseLButtonPush == 1; }
extern float   TerrainPickPosX;   // cursor billboard screen X  // DAT_083a4130
extern float   TerrainPickPosY;   // cursor billboard screen Y  // DAT_083a4134
extern float   TerrainPickPosZ;   // terrain pick Z result  // DAT_083a4138
extern DWORD   MouseLButtonPop;  // DAT_083a413c
// CameraMatrix es un buffer de 48 bytes (matrix 3×4 floats) — ver globals.cpp.
// CameraTranslateXBits / CameraTranslateYBits / CameraTranslateZBits son referencias a índices 3/7/11
// del mismo array; comparten memoria para que GL_GetModelViewMatrix (que escribe 12
// DWORDs) no desborde sobre globales vecinos.
extern DWORD   CameraMatrix[12];   // IDA: DAT_083a4140 (0x083A4140)
extern DWORD&  CameraTranslateXBits;  // DAT_083a414c
extern DWORD&  CameraTranslateYBits;  // DAT_083a415c
extern DWORD&  CameraTranslateZBits;  // DAT_083a416c
extern char    GrabFileName[0x104];   // screenshot name buf (26 chars+)  [IDA 0x083A4174]
extern DWORD   MouseY;  // DAT_083a4278
extern DWORD   MouseX;  // DAT_083a427c
extern DWORD   OpenglWindowWidth;   // viewport width (pixels)  [IDA 0x083A4280]
// CameraRayOriginX_arr — 3-DWORD storage para camera world position (escrito por
// Camera_MouseRay vía Vector_InverseRotate(...&CameraRayOriginX)). _CameraRayOriginX..428c son
// aliases float al mismo storage.
extern DWORD   CameraRayOriginX_arr[3];  // 3-DWORD storage for camera world position used by the pick ray
extern DWORD&  CameraRayOriginX;       // DAT_083a4284 _CameraRayOriginXBits
extern DWORD&  CameraRayOriginY;       // DAT_083a4288 _CameraRayOriginYBits
extern DWORD&  CameraRayOriginZ;       // DAT_083a428c _CameraRayOriginZBits
extern char    MouseDblClickFlag;  // DAT_083a4299
extern DWORD   ViewportCenterX;        // DAT_083A429C
extern DWORD   ViewportCenterY;        // DAT_083A42A0
extern DWORD   PerspectiveScaleXBits;  // DAT_083a42a4
extern DWORD   PerspectiveScaleYBits;  // DAT_083a42a8
extern DWORD   MouseRButton;   // UI flag: mouse button state  // DAT_083a42ac
extern DWORD   OpenglWindowHeight;   // viewport height (pixels)  [IDA 0x083A42B0]
// CameraAngle/CameraPosition contiguous — DAT_ names are aliases (see globals.cpp)
extern DWORD&  LegacyCameraAngle0;   // alias → CameraAngle[0]  // DAT_083a42b8
extern DWORD&  LegacyCameraAngle1;   // alias → CameraAngle[1]  // DAT_083a42bc
extern DWORD&  LegacyCameraAngle2;   // alias → CameraAngle[2]  // DAT_083a42c0
extern DWORD   MouseLButton;  // DAT_083a42c4
extern DWORD   OpenglWindowX;   // viewport x offset  [IDA 0x083A42C8]
extern DWORD   OpenglWindowY;   // viewport y offset  [IDA 0x083A42CC]
extern DWORD   MouseRButtonPush;   // UI flag: right-click / context menu  [IDA 0x083A42D0]
extern float&  CameraPositionX;  // alias → CameraPosition[0]  // _DAT_083a42d4
extern DWORD&  CameraPositionXBits;   // alias → CameraPosition[0] (DWORD view)  // DAT_083a42d4
extern float&  CameraPositionY;  // alias → CameraPosition[1]  // _DAT_083a42d8
extern DWORD&  CameraPositionYBits;   // alias → CameraPosition[1] (DWORD view)  // DAT_083a42d8
extern float&  CameraPositionZ;  // alias → CameraPosition[2]  // _DAT_083a42dc
extern DWORD&  CameraPositionZBits;   // alias → CameraPosition[2] (DWORD view)  // DAT_083a42dc
extern char    CameraTopViewEnabled;   // DAT_083A42E9: top-view / force-cull-bypass flag
extern char    FogEnable;   // reset to 0 each frame in Scene_Loading  // DAT_083a42ea
extern char    AutoDropTriggerFlag;   // auto-drop trigger flag (inventory)  // DAT_083a42eb
extern DWORD   ScreenshotModeFlag;  // DAT_083a42ec
// LegacyDlgBtnRectX  — macro alias dentro de DialogButtonRects (ver bloque de dialog button rects)
extern DWORD   LoginRetryCount;  // DAT_083a4320
// MessageBoxLineCount @ 0x083A4324 ES `g_iNumLineMessageBoxCustom` — la cantidad de
// lineas que SeparateTextIntoLines dejo en el cuadro de dialogo.
// Confirmado en el disassembly de CSQuest::ShowDialogText 0x401806:
// `a3 24 43 3a 08` = `mov [0x083A4324], eax` justo despues del
// SeparateTextIntoLines.  La etiqueta vieja ("shop item list display count")
// era una misidentificacion.
extern DWORD   MessageBoxLineCount;  // DAT_083a4324
#define g_iNumLineMessageBoxCustom  (*(int*)&MessageBoxLineCount)
extern DWORD   SelectedServerCode;  // DAT_083a4328
// CurrentCameraPosition[3] contiguous (see globals.cpp).
extern float   CurrentCameraPosition[3];  // current camera world position (X, Y, Z)
extern DWORD&  LegacyCurrentCamPos0;   // alias → CurrentCameraPosition[0]  // DAT_083a432c
extern DWORD&  LegacyCurrentCamPos1;   // alias → CurrentCameraPosition[1]  // DAT_083a4330
extern float&  CurrentCameraPosZ;  // alias → CurrentCameraPosition[2]  // _DAT_083a4334
extern DWORD&  CurrentCameraPosZBits;   // alias → CurrentCameraPosition[2] (DWORD view)  // DAT_083a4334
extern char    MessageBoxCustomBuf[7 * 0x26];   // g_lpszMessageBoxCustom — 7×38-byte dialog buffer  // DAT_083a44c4
extern float   LoginBgAnimValue;   // login background animation angle (Scene_Dispatch)  // DAT_083a45d4
extern char    ServerListBuffer[0x3600];   // server list buffer (stride 0x21e, 24 entries); overlaps sub-symbols below  // DAT_083a45d8
#define ServerEntryActiveFlag   (*(DWORD*)(ServerListBuffer + 0x14))   // offset 0x083a45ec  // DAT_083a45ec
#define ServerEntryGroupFlag   (*(char *)(ServerListBuffer + 0x15))   // offset 0x083a45ed  // DAT_083a45ed
#define ServerChannelId   (*(DWORD*)(ServerListBuffer + 0x2c))   // offset 0x083a4604  // DAT_083a4604
#define ServerChannelLoad   (*(WORD *)(ServerListBuffer + 0x2e))   // offset 0x083a4606  // DAT_083a4606
// Name-field aliases usados por ReceiveServerList (F4/02). Ambos caen dentro
// del array ServerListBuffer[0x3600]: slot 23 (0x30B2) y slot 24 (0x32D0).
#define DisplaySlot23Name   (ServerListBuffer + 0x30B2)   // name field of display slot 23  // DAT_083a768a
#define DisplaySlot24Name   (ServerListBuffer + 0x32D0)   // name field of display slot 24 (event)  // DAT_083a78a8
// Local server-name table poblada por F4/04 (custom list); stride 300 por grupo.
extern char    ServerNameTable[24 * 300];  // DAT_07d52c34

// Login/scene init flags (byte-sized)
extern DWORD   ConnectStartTick;   // IDA: unk_83A7AC8 (0x083A7AC8), GetTickCount al empezar a conectar  // DAT_083a7ac8
extern char    PinUiHeightParam;  // DAT_083a7acc
// CurrentCameraAngle[3] contiguous (see globals.cpp).
extern float   CurrentCameraAngle[3];  // current camera angles (pitch/yaw/roll), written from CameraWalkTable
extern float&  LegacyCurrentCamAngle0;   // alias → CurrentCameraAngle[0]  // DAT_083a7ad0
extern float&  LegacyCurrentCamAngle1;   // alias → CurrentCameraAngle[1]  // DAT_083a7ad4
extern float&  LegacyCurrentCamAngle2;   // alias → CurrentCameraAngle[2]  // DAT_083a7ad8
extern DWORD   CameraWalkCut;   // fade-in flag (1 = fading in)  // DAT_083a7af4
// ── Object_TickUpdate / MoveObjects (sub_4FDC00): gate del port completo de IDA ──────
// El port completo vive en Scene/Scene_ObjectUpdate.cpp (sin #if); esta macro
// sólo desactiva la versión mínima de Scene/Scene_ObjectLegacy.cpp (#ifndef).
#define IDA_PORT_004FDC00 1

// ── AttackEffect (0x00445230): gate del port completo de IDA ──────────────────
// El port fiel vive en Combat/Combat_AttackEffect.cpp (sin #if, con sus shims
// anti-tamper locales); esta macro desactiva la versión parcial de
// Combat/Combat_LegacyAttackEffects.cpp (`#if !defined(IDA_PORT_00445230)`).
#define IDA_PORT_00445230 1
extern DWORD   MoveSceneFrame; // IDA: MoveSceneFrame (0x083A7C00)
extern DWORD   DialogTextId;   // in-game UI context A  // DAT_083a7c04
extern DWORD   DialogPageNum;   // in-game UI context B  // DAT_083a7c08
extern char    DialogPageMax;   // in-game UI flag byte  // DAT_083a7c09
// DialogAnswerCountA @ 0x083A7C0C ES `g_iNumAnswer`.  Disassembly 0x40182E:
// `89 35 0c 7c 3a 08` = `mov [0x083A7C0C], esi`.
extern DWORD   DialogAnswerCountA;  // DAT_083a7c0c
#define g_iNumAnswer  (*(int*)&DialogAnswerCountA)
extern DWORD   EnableMainRender;  // DAT_083a7c10
extern int     LoginSubState; // IDA: LoginSubState (0x083A7C14)  // DAT_083a7c14
extern DWORD   LoginNextSubState;  // DAT_083a7c18
extern DWORD   ExitGameTriggerFlag;  // DAT_083a7c1c
extern DWORD   ExitGameCountdown;  // DAT_083a7c20
extern DWORD   InGameUiState;   // secondary in-game UI state machine  // DAT_083a7c24
extern DWORD   InGameUiStateNext;   // queued next UI state (applied at end of tick)  // DAT_083a7c28
// Chaos Machine category selected by the local ErrorMessage=143 modal:
// 0 = general, 1 = Chaos Weapon.  It is not a server recipe/type byte.
extern DWORD   ChaosMixCategory;  // DAT_083a7c2c
extern DWORD   CameraWalkType;  // DAT_083a7c38
extern DWORD   CameraNumber;  // DAT_083a7c3c
extern DWORD   ServerListCount;  // DAT_083a7c40
extern DWORD   ServerClickCooldown;  // DAT_083a7c44
extern char    ConnectionCheckEnable;   // connection-check-enable flag (byte, NO DWORD)  // DAT_083a7c48
extern char    InitLogIn;   // Scene_Login init flag  // DAT_083a7c49
extern char    InitSceneLoading;   // Scene_Loading init flag  // DAT_083a7c4a
extern char    CharSelectSceneInitialized; // IDA: DAT_083a7c4b
extern char    InitMainScene;  // DAT_083a7c4c
extern char    CharSelectWarningFlag;  // DAT_083a7c4d
extern DWORD   RenderWatchdogCount;  // DAT_083a7c50
extern DWORD   TestTime; // IDA: TestTime (0x083A7C54)
extern DWORD   DisconnectDialogShown;  // DAT_083a7c58
extern DWORD   ExitCountdownLogLabel;   // chat-log label for exit-countdown  // DAT_083a7c5c
extern char    ServerIpString[64];  // server IP string buffer (filled on server select)  // DAT_083a7c64
extern DWORD   ServerSelectLogLabel;  // DAT_083a7c68
extern DWORD   ServerListLogText;  // DAT_083a7c6c
extern DWORD   ServerListLogLabel;  // DAT_083a7c70
extern DWORD   ServerConnectLogLabelA;  // DAT_083a7c74
extern DWORD   ServerConnectLogLabelB;  // DAT_083a7c78
extern DWORD   PostLoginLogLabelA;  // DAT_083a7c7c
extern DWORD   PostLoginLogLabelB;  // DAT_083a7c80
extern DWORD   EnterWorldLogText;  // DAT_083a7c84
extern DWORD   EnterWorldLogLabel;  // DAT_083a7c88
extern DWORD   CharSelectLogText;  // DAT_083a7c8c
extern DWORD   CharSelectLogLabel;  // DAT_083a7c90
extern DWORD   ScreenshotLogLabelA;  // DAT_083a7c94
extern DWORD   ScreenshotLogLabelB;  // DAT_083a7c98
// ── Bitmaps table (contiguous, 1450 entries × 0x38 stride = 0x13D30 bytes) ───
// DESVIACION: tabla de capacidad dinámica y base estable; el catálogo no
// recicla sus slots al cambiar de mapa.
// Original address range: 0x083A7CA0 .. 0x083BB9D0
// Slot layout: +0x00 active, +0x20 width(f), +0x24 height(f), +0x28 bpp(b),
//              +0x2C gl_handle(u), +0x30 ref_count(b), +0x34 pixels(ptr)
extern char*   g_BitmapsRaw;  // raw bitmap/texture table base (TEXTURE_STRIDE per slot)
constexpr int TEXTURE_STRIDE = sizeof(void*) == 4 ? 0x38 : 0x40;   // IDA: BITMAP_t, 0x38 bytes
constexpr int TEXTURE_STRIDE_DWORDS = TEXTURE_STRIDE / 4;

// Accesores retrocompatibles al slot[0] — el código usa (&DAT_xxx)[offset]
// para indexar la tabla contigua con aritmética de punteros.
#define LegacyBitmapTable  (*(DWORD*) &g_BitmapsRaw[0x00])  // DAT_083a7ca0
#define BitmapsWidth  (*(DWORD*) &g_BitmapsRaw[0x20])  // DAT_083a7cc0
#define BitmapsHeight  (*(DWORD*) &g_BitmapsRaw[0x24])  // DAT_083a7cc4
#define BitmapsBpp  (*(char*)  &g_BitmapsRaw[0x28])  // DAT_083a7cc8
#define BitmapsGlHandle  (*(DWORD*) &g_BitmapsRaw[0x2C])  // DAT_083a7ccc
#define BitmapsRefCount  (*(char*)  &g_BitmapsRaw[0x30])  // DAT_083a7cd0
extern DWORD   m_dwUsedTextureMemory;   // IDA: DAT_083bb9d0 (0x083BB9D0)
extern DWORD   RandomTimeSeed;  // DAT_083bbb74

// ── Named pipe handle (0x00563b58) ────────────────────────────────────────────

// ── Model data table (0x05828d58, stride 0xbc per entity_type) ────────────────
extern std::uintptr_t ModelsBase; // Base de Models; los desplazamientos siguen expresados en bytes.  // DAT_05828d58
extern void*   ModelMemoryPool;   // model memory pool base (allocated once in Model_LoadItems)  // DAT_06f42a58
// BoneVertex pool (see globals.cpp). Backed by g_BoneVertexBuf.
// BoneVertBufBase = base del slot 0; &BoneVertBufBase es DWORD*, así que el código existente
// like (float*)(&BoneVertBufBase + vertIdx * 3) does k*12-byte stride correctly
// (DWORD=4 × 3 = 12 B per vertex). (char*)&BoneVertBufBase yields a byte pointer.
extern char    g_BoneVertexBuf[32 * 15000 * 12];  // skinned-vertex position buffer (32 bones x 15000 x 12)
#define BoneVertBufBase  (*(DWORD*)(g_BoneVertexBuf + 0))  // DAT_0584621c
// BoneVertOutBase es una etiqueta de Ghidra 8 bytes adentro del mismo buffer (el inicio de pfOut
// de Skeleton_Transform: pfDst = pfOut - 2 = &BoneVertBufBase + local_64). Las escrituras abarcan 12 bytes desde pfDst.
#define BoneVertOutBase  (*(DWORD*)(g_BoneVertexBuf + 8))  // DAT_05846224

// ── BMD bounding-box scratch arrays (BMD_CreateBoundingBox — BMD_ComputeBounds) ────────
// Scratch de BMD_CreateBoundingBox — una entrada por hueso (ver globals.cpp).
// Los companions +4/+8 son macros que proyectan dentro del slot 0 de su base,
// igual que PrimaryTerrainLightG, para que `(char*)&DAT_x + n` y `((float*)&DAT_x)[i]`
// sigan valiendo lo mismo que en el binario.
extern short   BoneVertexCounts[200];      // contador de vertices por hueso  // DAT_077d87fc
extern float   BoneBBoxMax[200*3];    // bbox max, 3 floats por hueso  // DAT_05827a98
extern float   BoneBBoxMin[200*3];    // bbox min, 3 floats por hueso  // DAT_06f42a5c

// ── UI name-list panel data (ShowCheckBox) ────────────────────────────────────
// DialogButton1Rect  — macro alias dentro de DialogButtonRects (ver bloque de dialog button rects)
// LegacyMsgBoxLine1: alias por macro que proyecta en el offset +0x26 (line[1]) dentro
// del buffer MessageBoxCustomBuf (g_lpszMessageBoxCustom).
#define LegacyMsgBoxLine1   (MessageBoxCustomBuf[0x26])  // DAT_083a44ea
extern byte    ItemClassNameFormatPtrA;       // item class name format ptr A  // DAT_005618b8
extern byte    ItemClassNameFormatPtrB;       // item class name format ptr B  // DAT_005618bc
extern byte    ItemClassNameFormatPtrC;       // item class name format ptr C  // DAT_005618c0
extern byte    ItemClassNameFormatPtrD;       // item class name format ptr D  // DAT_005618c4
extern char    Fmt_PaddedName[];  // " %s " -- padded-name format  // DAT_005618c8

// ── Entity / animation tick globals ───────────────────────────────────────────
extern float   WorldTime; // IDA: WorldTime (0x05826E08)
// LegacyDuelMode — declared above
extern char    SoccerObserver;   // SoccerObserver (IDA @0x05826D33) - lo escriben  // DAT_05826d33
                               // InitGame y el F3/23, y lo lee RenderCharacter.
                               // La etiqueta vieja ("guild name flag B") era falsa.
extern char    GuildNameCacheA[0x50];  // player guild name cache A (wide->char cache; 4-byte shop-name key aliased as g_active_shop_name)  // DAT_05826bfc
extern char    GuildNameCacheB[0x50];  // player guild name cache B (wide->char cache)  // DAT_05826cc0
extern char    GuildNameCacheC[0x50];  // player guild name cache C (wide->char cache)  // DAT_05826cc9

// GuildInfoTable — base del array de info de guild (indexado por slot de party) — declarado arriba como DWORD
// EntitySlotCount — player/entity slot count — declared above as DWORD
// Hero — entity_array[0] base pointer — declared above as char*
// FrustrumY/228 — login camera Y/X screen-pos — declared above as DWORD

// ── Buffers temporales de posición de huesos (usados en el traspaso de animación) ──
extern DWORD   TempBoneMatrixA[12]; // temp bone matrix buffer A  // DAT_07abf444
extern DWORD   MoveJointTempMat[12]; // IDA flt_7ABF474 (usado por MoveJoint subtipo 0xC)  // DAT_07abf474
extern DWORD   TempBoneMatrixB[12]; // temp bone matrix buffer B  // DAT_07abf3e4
extern DWORD   TempBoneMatrixBufferC[12]; // temp bone matrix buffer C  // DAT_07abf414

// ── Animation distance / frequency constants ───────────────────────────────────

// ── Teleport-anim pool (0x07c80110, stride 0x70, 100 slots) ──────────────────
// Sized via IDA bound: (0x7c82cd0 - 0x7c80110) / 0x70 = 0x2BC0 / 0x70 = 100.
// Entity_TeleportAnim recorre los 100 slots escribiendo 0x70 bytes por paso: tiene
// que ser el array completo.
extern DamagePoint DamagePointPool[kDamagePointCount];   // active-flag at +0; slot stride 0x70  // DAT_07c80110

// ── Character/effect update pool (0x07c85890, stride 0x1BC, 1002 slots) ──────
extern char    EffectPoolArray[kSpritePoolSize * OBJECT_STRIDE];  // pool array (was 1 byte → AUTO-SKIP)  // DAT_07c85890
extern int&    LegacySigilPoolOff4;   // alias = pool[+4] (Sound_Queue / Sigil_RenderAll)  // DAT_07c85894

// ── Math / animation constants ────────────────────────────────────────────────
extern float  TeleportZOffset;   // teleport-anim height offset  // _DAT_00552958
extern DWORD   TeleportZOffsetBits;  // DAT_00552958
// ConvertXScale extern declarado arriba (1/640, X scale constant)
extern float  TerrainHalfWidthNear;   // login camera X-rotation speed  // _DAT_00552cb8
extern float  TerrainHalfWidthFar;   // login camera Y-rotation speed  // _DAT_00552cbc

// Input / player name buffers

// ── Sprite entity pool (RenderParticles — decoration/item-drop sprite render) ────
extern DWORD   SpriteEntityPoolBase;       // sprite entity pool base (stride 0x70, limit 0x7b116b3)  // DAT_07abf634
extern float  SpriteSheetColStep;       // UV step per sprite-sheet column (1/N cols)  // _DAT_005528dc
extern float  EntitySinPeriod;       // Entity_UpdateRender sin period scale (0x15d)  // _DAT_00552944
extern float  EntitySinBias;       // Entity_UpdateRender sin bias (0x15d)  // _DAT_00552948
extern float  EntitySinAmplitude;       // Entity_UpdateRender sin amplitude (0x15d)  // _DAT_0055294c
extern float  AnimSpeedA;       // anim speed scale constant A  // _DAT_00552acc
extern float  AnimSpeedB;       // anim speed scale constant B  // _DAT_00552914

// ── Server select input (CServerSelWin_UpdateWhileActive) ───────────────────────────────────────
extern DWORD   ServerLocalSelect;        // selected channel (port % 0x14 + 1)  [IDA 0x0056169C]

// ── Char menu UI builder (RenderHelpWindow) ───────────────────────────────────────
extern int     HelpWindowMode;        // UI mode: 1=class-list-A, 2=class-list-B, 3=stats  // DAT_07e11d20
extern int     MenuF1ItemType;        // character class/subtype ID (range 0..0x1FF)  // DAT_07e11d24
extern char    TextListStorage[];  // text list buffer, 30 slots x 100 bytes (aliased as TextList)  // DAT_07e90798
extern int     TextListColor[30];    // TextListColor @0x07E91708 - color por linea  // DAT_07e91708
extern int     TextBold[30];    // TextBold      @0x07EA7B10 - negrita por linea  // DAT_07ea7b10
// extern char    LegacyClassNameA;        // class name table A (first entry base)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyClassInfoA;        // class info list A (stride 300, limit 0x7d34134)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyClassNameB;        // class name table B (first entry base)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyClassInfoB;        // class info list B (stride 300, limit 0x7d358a4)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyClassNameC;        // class name / info buffer C   // -> alias a GlobalText, ver el final del archivo
extern "C" std::uintptr_t ItemAttributeBase; // Base desplazada de atributos de items, filas de 64 bytes.  // DAT_07d78068
// ── Posición del item que se está arrastrando ───────────────────────────────
// En IDA esto vive en `Inventory[32].Type` (el pool de shop/trade-in,
// slot 32, campo Type en offset 0) — lo usan sub_47D410 (preview de stats) y el
// render del footprint durante el drag.
//
// NO usar ItemRenderMode para esto: en el binario esa dirección se escribe SOLO
// como byte (`mov byte ptr [7EA9844], bl` en 0x4D2586 y `..., 0` en 0x4D1D8B) y
// se lee únicamente en Scene_MapTick, como 4º parámetro (bSell) de
// RenderItemInfo/RenderRepairInfo: un índice de slot != 0 haría que el tooltip
// muestre el PRECIO DE VENTA fuera de una tienda.
extern "C" BYTE Inventory[];  // player inventory item pool (stride 0x44 per item)
#define ItemPickedPos  (*(int*)&Inventory[32 * 68])   // Inventory[32].Type

// Base de la tabla ItemAttribute.  Devuelve 0 si todavia no se cargo, para
// que los callers no computen attrBase = type*0x40 + basura.
static __forceinline std::uintptr_t ItemAttribute_Base(void) {
    const std::uintptr_t p = ItemAttributeBase;
    const std::uintptr_t allocation = ItemAttrTableBase;
    // Reserva: (ITEM_MAX_EX + 0x400) filas de 64 bytes, con base
    // desplazada hasta 0x3ff filas. No asumir un mapa virtual de Windows.
    if (!allocation || p < allocation || p - allocation > 0x400u * 64u ||
        (p - allocation) % 64u != 0) return 0;
    return p;
}
// Fila `p` de la tabla ItemAttribute (base + type*64), validada contra la
// tabla cargada y no contra rangos de direcciones: en Linux x86 el heap puede
// quedar por encima de 2 GB.
static __forceinline bool ItemAttribute_IsRow(uintptr_t p) {
    const uintptr_t base = ItemAttribute_Base();
    return base && p >= base && (p - base) % 64u == 0 && (p - base) / 64u < (uintptr_t)ITEM_MAX_EX;
}
// SkillHelpReqR0C2/534/53c/540 — columnas 2/3/5/6 de la fila 0 de CharStatsTable
// (ver la declaracion de la tabla mas abajo).
#define SkillHelpReqR0C2   (CharStatsTable[2])  // DAT_07e91530
#define ItemStatValue3   (CharStatsTable[3])  // DAT_07e91534
#define ItemStatValue4   (CharStatsTable[4])  // DAT_07e91538
#define ItemStatValue5   (CharStatsTable[5])  // DAT_07e9153c
#define ItemStatValue6   (CharStatsTable[6])  // DAT_07e91540
#define ItemStatValue7   (CharStatsTable[7])  // DAT_07e91544
#define ItemStatValue8   (CharStatsTable[8])  // DAT_07e91548
#define ItemStatValue9   (CharStatsTable[9])  // DAT_07e9154c
// Strings de formato de la UI (datos de sólo lectura en el binario)
extern char    UiTextStr00[];  // DAT_0055a408
extern char    UiTextStr01[];  // DAT_0055a40c
extern char    UiTextStr02[];  // DAT_0055a410
extern char    UiTextStr03[];  // DAT_0055a414
extern char    UiTextStr04[];  // DAT_0055a418
extern char    UiTextStr05[];  // DAT_0055a41c
extern char    UiTextStr06[];  // DAT_0055a420
extern char    UiTextStr07[];  // DAT_0055a424
extern char    UiTextStr08[];  // DAT_0055a428
extern char    UiTextStr09[];  // DAT_0055a42c
extern char    UiTextStr10[];  // DAT_0055a430
extern char    UiTextStr11[];  // DAT_0055a434
// RenderItemInfo string constants
extern char    FormatNewlineItemName[];   // item name format string  // DAT_0055a4e0
extern char    FormatNewlineStatsHeader[];   // item stats header format string  // DAT_0055a5f4
extern char    FormatNewlineAfterPrice[];   // salto de linea — separador tras el precio  // DAT_0055a4e4
extern char    FormatNewlineAfterName[];   // salto de linea — separador tras el nombre  // DAT_0055a570
extern char    FormatNewlineRenderItem[];   // salto de linea — separador de media altura  // DAT_0055a5f0
extern char    FormatNewlineRepairInfo[];   // salto de linea — separador de media altura  // DAT_0055a5fc
extern char    FormatNewlineRepairFinal[];   // salto de linea — separador de media altura  // DAT_0055a640
extern char    FmtSxSxSStr[];   // s__s__s format  // DAT_0055a608
extern char    SecondaryStatsStr[];   // secondary stats line  // DAT_0055a630
// extern char    LegacyItemLevelFmt[];   // item level line format   // -> alias a GlobalText, ver el final del archivo

// ── Misc low-address globals ──────────────────────────────────────────────────
// (07abf06, 07d29e5, 07eaa11 — byte flags or array references)

// ── Chat ring buffer (UI_RenderChatLogOverlay renderer / UIChatLogWindow_AddText) ──────
// Único buffer real: 0x77 entries × 0x118 bytes stride.
// Per-slot layout:
//   +0x000..+0x00A  sender name (11 bytes)
//   +0x00B..+0x10B  message text (257 bytes)
//   +0x10C          type/channel DWORD (0..5)
//   +0x114          cached text-extent cx (LONG)
// WhisperActiveFlag, WhisperMsgType, WhisperPosX en el binario original son ALIASES
// dentro de este buffer a los offsets 0x0B / 0x10C / 0x114 del slot 0 (Ghidra
// los recuperó como globales independientes); acá son macros para que writer y
// reader vean la misma memoria.
extern char    WhisperBubblePool[0x77 * 0x118];  // DAT_07df9380
// Alias del primer byte del campo msg (slot 0). Las fórmulas
// `&WhisperActiveFlag + slot*0x118` del decompile de Ghidra resuelven al byte
// correcto gracias a que éste es un lvalue dentro del buffer real.
#define WhisperActiveFlag (WhisperBubblePool[0x0B])  // DAT_07df938b
// Alias del DWORD type/channel (slot 0). Mantiene el tipo int para que los
// accesos `(&WhisperMsgType)[slot*0x46]` del decompile avancen 0x118 bytes por
// slot (0x46 DWORDs = 280 bytes).
#define WhisperMsgType (*(int*)&WhisperBubblePool[0x10C])  // DAT_07df948c
// Alias del LONG cached cx (slot 0). Los call-sites usan
// `*(LONG*)(&WhisperPosX + slot*0x118)` — ese cast de pointer-arith a char*
// se hace en el call-site, acá sólo exponemos el DWORD base.
#define WhisperPosX (*(int*)&WhisperBubblePool[0x114])  // DAT_07df9494

// Key debounce for scroll/nav
extern DWORD   PageUpDebounce;           // PageUp debounce  // DAT_07e11970
extern DWORD   PageDownDebounce;           // PageDown debounce  // DAT_07e11974
extern DWORD   UpArrowDebounce;           // Up-arrow debounce  // DAT_07e11984
extern DWORD   DownArrowDebounce;           // Down-arrow debounce  // DAT_07e1198c
extern DWORD   BKeyDebounce;           // 'B' key debounce  // DAT_07e119f4
extern DWORD   RKeyDebounce;           // 'R' key debounce  // DAT_07e11a34

// Item/hotkey assignment UI
extern char    ItemSelectMenuFlag;           // item-selection toggle (char)  // DAT_07db870c
extern DWORD   TooltipPosX;           // UI cursor / hotkey X position  // DAT_07ea840c
extern DWORD   TooltipPosY;           // UI cursor / hotkey Y position  // DAT_07ea8408

// Chat input channel buffers (9 channels × 0x100 bytes)
extern char    MacroText[10 * 0x100];   // macro hotkey table: 10 slots × 256 bytes  [IDA 0x07E0FFC8]
// Fila 9 de la tabla de arriba (0x07E0FFC8 + 0x900 = 0x07E108C8), no un global
// aparte: es el buffer del destinatario de susurro y lo llenan Auth_XorEncode,
// strcmp/strlen/memcpy (hasta 255 bytes).
#define WhisperSendBuffer   (MacroText[0x900])  // DAT_07e108c8

// Player name (for chat name-match / tab-complete)
extern char    PlayerNameString;           // player name string (char array)  // DAT_005592dc
extern DWORD   NameUtilityPtrA;           // name utility ptr A  // DAT_005592d8
extern DWORD   NameUtilityPtrB;           // name utility ptr B  // DAT_005592d4

// Text input / command buffers
extern char    TextInputBuffer;           // current in-game text input buffer (char array)  // DAT_07d5391c
// extern char    WhisperCommandText;           // "/whisper" command string   // -> alias a GlobalText, ver el final del archivo
// extern char    PvpCommandText;           // "/pvp" command string   // -> alias a GlobalText, ver el final del archivo
// extern char    GmCommandText;           // GM command string   // -> alias a GlobalText, ver el final del archivo
// Chat command parser name buffers (Chat_ValidateCommandName)
// extern char    LegacyChatNameBuf1;           // player name buffer 1 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyChatNameBuf2;           // player name buffer 2 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyChatNameBuf3;           // player name buffer 3 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyChatNameBuf4;           // player name buffer 4 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyChatNameBuf5;           // player name buffer 5 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyChatNameBuf6;           // player name buffer 6 (chat cmd parser)   // -> alias a GlobalText, ver el final del archivo
// extern char    WhisperTargetText;           // whisper target display buffer   // -> alias a GlobalText, ver el final del archivo
// extern char    PvpToggleMessage;           // pvp-toggle result message buffer   // -> alias a GlobalText, ver el final del archivo
extern DWORD   WhisperBlockFlag;           // command-result flag (1=whisper, 0=pvp-off)  // DAT_07e11dac

// B-key toggle guards
// 0x07EAA130 ES g_bServerDivisionEnable (confirmado por xrefs de IDA:
// ReceiveTalk, SendMove, sub_492F10, Chat_InputTick, GetScreenWidth,
// RenderServerDivision).  ServerDivisionOpened es un alias: el writer
// (Net_Process, ReceiveTalk sub 5) y los readers tienen que ver el mismo byte.
// El byte 1 de ese int es 0x07EAA131 (en el port, PinCheckboxState es un global aparte).
#define ServerDivisionOpened   (*(char*)&g_bServerDivisionEnable)
extern char    BKeyToggleGuard;           // guard for B-key toggle enable  // DAT_07eaa132
extern char    RepairEnableFlag;           // RepairEnable_0  // DAT_07eaa134

// Screenshot / error dialog context
extern DWORD   lpDefault_00583d88;     // default context for UIChatLogWindow_AddText
// extern char    LegacyChatLogMsg;           // error/info message for UIChatLogWindow_AddText   // -> alias a GlobalText, ver el final del archivo

// Connection-check context
extern DWORD   ChaosMixItems;           // connection context for Connection_Check  // DAT_07ea9848

// DAT_05826ceb LegacyPacketSerialSend — rolling packet counter byte (declared above near 0x05826xxx block)

// ── Timers de cuenta regresiva de chat/UI (los usan Sound_Queue.cpp y UI/Chat.cpp) ──
extern int     NoticeTime;           // system-message scroll timer (reset to 300 on Chat_AddMessage)  // DAT_00559cdc
extern int     ChatScrollTimer;           // player-chat scroll timer   (reset to 0x96 on UI_ShowDialog)  // DAT_00559ce4
// Strings que Sound_Countdown1 / Sound_Countdown2 vuelven a mostrar al vencer el timer:
extern char    PeriodicNoticeText[256];      // byte_7E11DD0: texto del aviso periódico (MoveNotices/CreateNotice)  // DAT_07e11dd0
// Siempre vacios: nadie los escribe en todo el binario (un xref cada uno, la
// lectura de Chat_TickMessageTimer).  Son buffers y no char sueltos porque se
// leen como cadenas -- ver la nota en globals.cpp.
extern char    NoticeTextArg[4];        // strText          (arg2 de UIChatLogWindow_AddText)  // DAT_07e11dd8
extern char    NoticeTitleArg[12];       // byte_7E11DDC     (arg1 de UIChatLogWindow_AddText)  // DAT_07e11ddc

// ── Chat ring buffers ─────────────────────────────────────────────────────────
// System message buffer (UI_AddNotice / Chat_DrawMessages)
extern char    SystemChatBuffer[6 * 0x108];   // system chat ring buffer (6 slots × 0x108 bytes)  // DAT_07db80d8
#define        NoticeSlotFlags   (SystemChatBuffer[0x104])  // flag byte alias (slot 0 +0x104)  // DAT_07db81dc

// Server-config globals (poblados por los opcodes 0xDD/DE/DF).
extern "C" {
    extern DWORD g_MaxCharacterLevel;     // 0xDF — cap de level (default 400)
    extern WORD  g_CharDeleteMaxLevel;    // 0xDD — max level que permite delete
    extern BYTE  g_CharCreationEnable;    // 0xDE — permite crear chars
}
extern int     SystemChatCount;           // system message count (max 5, i.e. 6 entries 0..5)  // DAT_07e11d9c
// Player chat / notification buffer (UIChatLogWindow_AddText / UI_DrawPlayerList)
// Entry stride: 0x118 bytes (= 0x46 × sizeof(DWORD), Ghidra type convention)
// Layout per entry: sender[0..0xA] + msg[0xB..0x10B] + type[0x10C]
extern int     ChatLogCount;           // player chat entry count (max 0x77 = 119 entries)  // DAT_07e11da4

// ── Chat text-extent work variables (Chat_DrawField / Chat_DrawEntry) ─────────
extern SIZE    g_TextExtent07E113D0;   // IDA TextSize: one contiguous cx/cy pair
extern SIZE g_TextExtent07E113D0;     // IDA: TextSize (0x07E113D0)
extern int&   TextExtentCy;           // last GetTextExtentPointA result (cy)  // _DAT_07e113d4
extern DWORD   CaretBlinkCounter;           // char-index/step counter (draw-field loop)  // DAT_07e11d2c
extern char    DbcsHighBytes[10 * 4];   // 10 slots x 4 bytes; WinMain escribe con slot*4 (slot 0..9)  // DAT_07e11cec

// ── Chat string constants (displayed on login/cursor events) ──────────────────
extern char    PlayerListMsgFmt;           // player-list format: sender+space+msg fmt string  // DAT_00559d4c
extern char    FmtSDashSStr;           // s__s___s: " %s - %s" style format string  // DAT_00559d54
extern char    PlayerListAltFmtStr;           // player-list alternate format string  // DAT_00559d5c

// ── Guild system globals (opcodes 0x90-0x99) ──────────────────────────────────
// InventoryOpened — declared above as char
// CharacterOpened — declared above as char
// GoldenArcherOpenType — declared above as DWORD
extern int     GoldenArcherItemCount;           // g_shEventChipCount (IDA 0x07EAA12C)  [IDA 0x07EAA12C]
// StorageGoldFlag — guild UI flag — declared above as DWORD
extern BYTE    GoldenArcherLuckyNumberText[64];       // guild entity pool (zeroed on stage 3, 0x40 bytes)  [IDA 0x07EA97C0]
extern char    GoldenArcherLuckyNumberTicket;           // char-select flag D (set 1 when guild stage==3)  [IDA 0x07E11D73]
// Guild message string buffers (shown via ShowGuildMessage / CreateOkMessageBox):
// extern BYTE    LegacyGuildCreateMsg1;           // guild create result msg 1   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    LegacyGuildCreateMsg2;           // guild create result msg 2   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    LegacyGuildCreateMsg3;           // guild create result msg 3   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    LegacyGuildCreateMsg4;           // guild create result msg 4   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    LegacyGuildCreateMsg5;           // guild create result msg 5   // -> alias a GlobalText, ver el final del archivo
// extern BYTE    LegacyGuildJoinOkMsg;           // guild join OK message   // -> alias a GlobalText, ver el final del archivo
extern char    Fmt_PartyJoinError;  // party/guild join error format string (wsprintfA %d)  // DAT_07d68268
// extern BYTE    LegacyGuildReqOkMsg;           // guild request OK message   // -> alias a GlobalText, ver el final del archivo
extern char    Fmt_PartyRequestError;  // party/guild request error format string (wsprintfA %d)  // DAT_07d58fd4
// Posición objetivo del guild (la setean los opcodes 0x94 / 0x96):
extern DWORD   GoldenArcherLuckyNumber;          // guild target tile X (4-byte, overlapping)  [IDA 0x00559F58]
extern WORD    GuildTargetTileY;           // guild target tile Y (2-byte)  // DAT_00559f5c
// Campos del sub-estado de login (también los escribe la rama 3 del guild 0x94):
// InputTextMax (int&), InputEnable (= InputEnable) e InputNumber: declarados más arriba.
// ── Estado de la UI de guild (bloque 0x083a, usado por ShowGuildMessage / la lista de miembros) ──
// MessageBoxLineCount (DWORD) y MessageBoxCustomBuf (char[7 * 0x26]): declarados más arriba.
// ── Dialog button rects (0x083A42F8, 2 entradas × 5 ints = 0x28 bytes) ───────
// Layout por entrada: [0]=bitmapId-240 (1..4) [1]=x [2]=y [3]=width [4]=height.
// Escrito por CreateOkMessageBox/CreateDialogInterface/ShowCheckBox/sub_51D9E0/
// sub_51DA80 (todos hacen `memset(&unk_83A42F8, 0, 0x28)` -> la region es
// EXACTAMENTE 40 bytes) y leido por RenderErrorMessage (&unk_83A4304, o sea el
// campo width de la entrada 0, avanzando de a 5 ints) y por el hit-test de
// UI_InGameMenu (&unk_83A42FC = campo x).
//
// En el binario 42F8 / 42FC / 4304 / 430C son OFFSETS DENTRO DE ESTA MISMA REGION:
// por eso 42FC / 4304 / 430C son macros sobre DialogButtonRects y no globals propios.
extern DWORD   DialogButtonRects[10];       // UI panel descriptor array (set by guild funcs)  // DAT_083a42f8
#define LegacyDlgBtnRectX   (DialogButtonRects[1])                 // entry0.x  // DAT_083a42fc
#define DialogButton0Width   (*(int*)&DialogButtonRects[3])         // entry0.width  // DAT_083a4304
#define DialogButton1Rect   ((DWORD*)&DialogButtonRects[5])        // entry1 base  // DAT_083a430c
// InGameUiState  — declared above as DWORD
// InGameUiStateNext  — declared above as DWORD
extern int     GuildMemberCount;           // guild member count for UI  // DAT_083a7c30
extern int     GuildUiAuxParam;           // guild UI auxiliary param  // DAT_083a7c34


// ── Más declaraciones de globals definidos en globals.cpp ─────────────────────
extern char    LockInputStatus; // IDA: LockInputStatus (0x07E11D6F)  // DAT_07e11d6f
extern DWORD   WarriorSkill43Tick;              // UseSkillWarrior 43: tick de activacion  // DAT_07e11d84
extern char    AutoSwapChatIdA;  // DAT_07e11dec
extern char    AutoSwapChatIdB;  // DAT_07e11df0
extern int     g_WorldLoading;            // >0 mientras corre OpenWorld (ver WinMain WM_USER)
extern int     m_iDevilSquareLimitLevel[4][2];
extern int     m_iBloodCastleLimitLevel[12][2];

// Ranking de Devil Square / Blood Castle (opcode 0x93): 11 registros
// (0x083A7AF8..0x083A7C00 en el binario, de 0x18 bytes). Desde 0.97.40 cada
// registro es name[40], score, RewardExperience, RewardMoney = 52 bytes.
#define GUILD_MEMBER_STRIDE       52
#define GUILD_MEMBER_TABLE_BYTES  (11 * GUILD_MEMBER_STRIDE)
extern BYTE    GuildMemberTable[GUILD_MEMBER_TABLE_BYTES];  // DAT_083a7af8
#define Record0Score  (*(DWORD*)&GuildMemberTable[40])   // score del registro 0  // DAT_083a7b04
#define PlayerExperience  (*(DWORD*)&GuildMemberTable[44])   // experiencia  // DAT_083a7b08
#define PlayerZen  (*(DWORD*)&GuildMemberTable[48])   // zen  // DAT_083a7b0c
// Columnas de datos de miembro de guild (parte del mismo array de stride 0x18 que arranca en GuildMemberTable):

// Guild leaderboard UI strings (RenderMatchScore / GuildOverview_Render):
// extern char    LegacyGuildPanelTitle;           // guild panel title string ("Guild War Score" etc.)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyGuildColHeaders;           // column headers array (stride 300, ~5 entries, ends at 0x7d5bfe0)   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyGuildColHeadLast;           // last column header entry   // -> alias a GlobalText, ver el final del archivo
extern char    GuildWinTitleNoGuild[300];  // guild-win dialog title (no guild state)  // DAT_07d68bc8
extern char    GuildWinSubtitleNoGuild[300];  // guild-win dialog subtitle (no guild state)  // DAT_07d68cf4
extern char    GuildWinTitle[300];  // guild-win dialog title (with guild state)  // DAT_07d68970
extern char    GuildWinSubtitle[300];  // guild-win dialog subtitle (with guild state)  // DAT_07d68a9c
extern char    Fmt_ScoreTotal;  // score-total format string (guild duel panel)  // DAT_07d68e20
extern char    Fmt_OpponentDeaths;  // opponent deaths format string (guild duel panel)  // DAT_07d68f4c
extern char    Fmt_OwnKills;  // own kills format string (guild duel panel)  // DAT_07d69078

// ── Pre-compile missing globals (found by diff scan) ─────────────────────────

// Game math constants missing from 0x005524xx–0x00552dxx section:
extern float  FloatZero;   // float constant 0.0 (guard for division / degenerate-length check)

// Hash table / debug string (0x00558xxx):
extern char    DebugLogDataBlock[];  // debug log data block / format string  // DAT_00558128

// Compressed-assets flag (0x0055a7c4):
extern char    g_tex_ext_mode;   // 0=plain files, 1=compressed OZJ/OZT; alias: g_tex_ext_mode  // DAT_0055a7c4

// Misc.cpp / Entity gravity constants:
extern float   GravityMinClamp;  // _DAT_00552570
extern float   ItemVelDecay;  // _DAT_005527d0

// FPS timer (0x07e016f0) — DWORD millisecond timestamp, accessed via _DAT_ Ghidra alias:
extern DWORD   LastFpsTickMs;  // last FPS tick timestamp (timeGetTime value)  // _DAT_07e016f0

// Pool en 0x083a1218 (array Butterfles; lo recorren SMD_Parser.cpp y Entity_Render.cpp, entre otros):
extern char    ButterflyObjectPool[10 * OBJECT_STRIDE];   // Butterfles OBJECT array (10 entries × 0x1BC stride)  // DAT_083a1218

// Float aliases for view/projection matrix globals (0x083a4xxx):
// DWORD counterparts (CameraTranslateXBits etc.) are already declared above.
extern float& CameraTranslateX;   // alias → CameraMatrix[3]  // _DAT_083a414c
extern float& CameraTranslateY;   // alias → CameraMatrix[7]  // _DAT_083a415c
extern float& CameraTranslateZ;   // alias → CameraMatrix[11]  // _DAT_083a416c
extern float& _CameraRayOriginX;   // _CameraRayOriginX: camera position X alias  // _DAT_083a4284
extern float& _CameraRayOriginY;   // _CameraRayOriginY: camera position Y alias  // _DAT_083a4288
extern float& _CameraRayOriginZ;   // _CameraRayOriginZ: camera position Z alias  // _DAT_083a428c
extern float  PerspectiveScaleX;   // projection scale X (float alias of PerspectiveScaleXBits)  // _DAT_083a42a4
extern float  PerspectiveScaleY;   // projection scale Y (float alias of PerspectiveScaleYBits)  // _DAT_083a42a8
extern float  ScreenCenterYFlip;   // projection center Y (float)  [IDA 0x083A4294]

// Bone transform scratch buffer (0x06989xxx) — 3x4 row-major matrix (12 floats).
// Respaldado por RootBoneMatrixStorage (contiguo). RootBoneMatrix es la dirección de
// matrix[0]; los dos alias _DAT apuntan a los elementos [5] y [10] de la diagonal.
// El código que pasa `&RootBoneMatrix` como float* obtiene un puntero adentro del array,
// so (&RootBoneMatrix)[N] reaches matrix[N] correctly.
extern float   RootBoneMatrixStorage[12];  // 3x4 root-bone transform matrix (aliases: RootBoneMatrix, GfxMatrix11/22)
extern float&  RootBoneMatrix;   // alias → RootBoneMatrixStorage[0]
extern float& GfxMatrix11;   // alias → RootBoneMatrixStorage[5]  // _DAT_06989cb0
extern float& GfxMatrix22;   // alias → RootBoneMatrixStorage[10]  // _DAT_06989cc4

// Bone normal-intensity pool (see globals.cpp). Backed by g_BoneNormalBuf.
// &IntensityTransformBuf es DWORD*, así que (float*)(&IntensityTransformBuf + meshIdx * 15000)
// avanza k*4 bytes por float (correcto para un buffer escalar por normal).
extern char    g_BoneNormalBuf[32 * 15000 * 4];  // skinned-vertex normal buffer (32 bones x 15000 x 4)
#define IntensityTransformBuf  (*(DWORD*)(g_BoneNormalBuf + 0))  // DAT_077e298c

// BoneVertOutBase — declared above as #define aliasing g_BoneVertexBuf + 8.

// ── CharMenu_Build sub-function globals ──────────────────────────────────────
extern int     SkipNum;           // secondary text-entry counter (CharMenu_AppendSkillReq)  // DAT_07eaa158
extern char    SkillReqMetFmt[];         // skill-req format string A (met)  // DAT_0055a400
extern char    SkillReqNotMetFmt[];         // skill-req format string B (not met)  // DAT_0055a404
// Tabla de requisitos de stats @ 0x07E91528 — 12 filas x 10 ints.  Los simbolos
// SkillHelpReqR0C1/530/534/538/53c/540/544/548/54c son las COLUMNAS 1..9 de la fila 0
// (macros de arriba), no globals independientes.  Fila i, columna c =
// CharStatsTable[10*i + c].
extern int     CharStatsTable[12 * 10];  // DAT_07e91528
#define SkillHelpReqR0C1   (CharStatsTable[1])  // DAT_07e9152c
// extern char    LegacySkillDescTable;           // skill description string table base (slot 0, stride ~0x138)   // -> alias a GlobalText, ver el final del archivo
extern int     CharMenuBuiltClassId;           // class-data cache guard (last built class_id)  // DAT_00559fe0

// ── UI_StatsPanel (RenderErrorMessage) globals ─────────────────────────────────────
// Float step/scale constants (0x00552xxx)
extern float  EntityPanelBtnWidth;   // button width for entity panel  // _DAT_00552a2c
extern float  StatsPanelXOffset;   // panel X offset  // _DAT_00552d40
// Dialog text color control
extern DWORD   m_dwTextColor;   // current text color ABGR (0xffffffff = white)  // DAT_00559c78
// SetBackgroundTextColor — declared above as DWORD
// LegacyInputTextWidth — declared above as DWORD
// Toggle flags
extern char    m_bAutoAttack;   // IDA: m_bAutoAttack (0x00559C5C)
extern char    m_bWhisperSound;   // IDA: m_bWhisperSound (0x07E11D80) — aviso sonoro de susurros
// Entity/level data
// Format strings for numeric dialogs
// extern char    LegacyLoginAccountFmt;   // login account name format   // -> alias a GlobalText, ver el final del archivo
extern char    PartyRequestFormatA[300]; // party/trade request status format  // DAT_07d486fc
extern char    SecondPasswordErrorFormat;   // second-password error format  // DAT_07d48828
extern char    PartyRequestFormatB[300]; // party/trade request secondary format  // DAT_07d48f30
// extern char    LegacyDuelOfferFmt;   // duel offer sub-text format   // -> alias a GlobalText, ver el final del archivo
extern char    TradeDuelRequestLine1;   // trade/duel request line 1  // DAT_07d4950c
extern char    TradeAcceptText;   // trade accept text  // DAT_07d49638
extern char    DuelAcceptText;   // duel accept text  // DAT_07d49764
// Vendor/NPC name tables
extern char    VendorItemNameList;   // vendor item name list (stride 300, 2 entries)  // DAT_07d5fa78
extern char    NpcVendorInfoLineA;   // NPC/vendor info line A  // DAT_07d698ac
extern char    NpcVendorInfoLineB;   // NPC/vendor info line B  // DAT_07d699d8
extern char    NpcEntry0LineA;   // NPC entry 0 line A  // DAT_07d69b04
extern char    NpcEntry0LineB;   // NPC entry 0 line B  // DAT_07d69c30
// DialogButton0Width  — macro alias dentro de DialogButtonRects (ver bloque de dialog button rects)
// DialogAnswerBuffer @ 0x083A4348 ES `g_lpszDialogAnswer` — las 10 respuestas del
// dialogo, 38 (0x26) bytes cada una.  Disassembly 0x401812:
// `bf 48 43 3a 08` = `mov edi, 0x083A4348` seguido de `mov ecx, 5Fh; rep stosd`
// = 0x5F dwords = 380 bytes = 10 * 38, que fija el tamano real.
extern char    DialogAnswerBuffer[10][1][38];  // DAT_083a4348
#define g_lpszDialogAnswer  DialogAnswerBuffer
// Guild message decode state
// DialogPageNum — declared above as DWORD
extern char    DialogPageMax;   // guild dialog flag B  // DAT_083a7c09
// DialogAnswerCountA — declared above as DWORD
// Zona del cursor / hit-test (también se usa en otros paneles)
// MouseLButtonPush — declared above as DWORD
// Y del cursor de selección en la lista de servers (también se usa en el panel 0x96)
extern float  InvCursorBoundXMin;   // panel cursor bound Y-min  // _DAT_00552cac
// String literals at fixed addresses (Ghidra: s__s_xxx_address)
extern char    Txt_AutoAttackOn[];  // "Auto-ataque : On" -- auto-attack toggle chat label  // DAT_0056184c
extern char    Txt_AutoAttackOff[];  // "Auto-ataque : Off" -- auto-attack toggle chat label  // DAT_00561854
extern char    Txt_WhisperOn[];  // "Susurros : On" -- whisper toggle chat label  // DAT_0056185c
extern char    Txt_WhisperOff[];  // "Susurros : Off" -- whisper toggle chat label  // DAT_00561864
extern char    StatsPanelLabel[];  // stats-panel label buffer, 32 B (holds "Cancel" or empty)  // DAT_0056186c
// Error/message string table (in-game dialogs, ~0x07d4xxxx range)
extern char    DlgText6E_0[300];  // dialog string 0x6e/0 (menu title; GlobalText[381])  // DAT_07d45ba0
extern char    DlgText6E_1[300];  // dialog string 0x6e/1 (GlobalText[382])  // DAT_07d45ccc
extern char    DlgText6E_2[300];  // dialog string 0x6e/2 (GlobalText[383])  // DAT_07d45df8
extern char    DlgText6E_3[300];  // dialog string 0x6e/3 (GlobalText[384])  // DAT_07d45f24
extern char    DlgText6E_4[300];  // dialog string 0x6e/4 / 0x96 header (GlobalText[385])  // DAT_07d46050
extern char    DlgText96_3[300];  // dialog string 0x96/3 (GlobalText[388])  // DAT_07d463d4
extern char    DlgText7A[300];  // dialog string 0x7a (GlobalText[390])  // DAT_07d4662c
extern char    DlgText7B[300];  // dialog string 0x7b (GlobalText[391])  // DAT_07d46758
extern char    DlgText73_0[300];  // dialog string 0x73/line0 (GlobalText[392])  // DAT_07d46884
extern char    DlgText73_1[300];  // dialog string 0x73/line1 (GlobalText[393])  // DAT_07d469b0
extern char    DlgText36_0[300];  // dialog string 0x36/line0 (GlobalText[394])  // DAT_07d46adc
extern char    DlgText36_1[300];  // dialog string 0x36/line1 (GlobalText[395])  // DAT_07d46c08
extern char    DlgText37[300];  // dialog string 0x37 (GlobalText[396])  // DAT_07d46d34
extern char    DlgText72_1[300];  // dialog string 0x72/line1 (GlobalText[398])  // DAT_07d46f8c
extern char    DlgText84_0[300];  // dialog string 0x84/line0 (GlobalText[399])  // DAT_07d470b8
extern char    DlgText84_1[300];  // dialog string 0x84/line1 (GlobalText[400])  // DAT_07d471e4
extern char    DlgText8A[300];  // dialog string 0x8a/0x3a/0x90 (GlobalText[401])  // DAT_07d47310
extern char    DlgText71[300];  // dialog string 0x71 (GlobalText[402])  // DAT_07d4743c
extern char    DlgText64[300];  // dialog string 0x64 (GlobalText[403])  // DAT_07d47568
extern char    DlgText65[300];  // dialog string 0x65 (GlobalText[404])  // DAT_07d47694
extern char    DlgText1A_0[300];  // dialog string 0x1a/0x70/line0 (GlobalText[405])  // DAT_07d477c0
extern char    DlgText1A_1[300];  // dialog string 0x1a/0x70/line1 (GlobalText[406])  // DAT_07d478ec
extern char    DlgText15[300];  // dialog string 0x15/0x47 (GlobalText[407])  // DAT_07d47a18
extern char    DlgText1B[300];  // dialog string 0x1b (GlobalText[408])  // DAT_07d47b44
extern char    DlgText1C[300];  // dialog string 0x1c (GlobalText[409])  // DAT_07d47c70
extern char    DlgText1D[300];  // dialog string 0x1d (GlobalText[410])  // DAT_07d47d9c
extern char    DlgText1E[300];  // dialog string 0x1e (GlobalText[411])  // DAT_07d47ec8
extern char    DlgText1F[300];  // dialog string 0x1f (GlobalText[412])  // DAT_07d47ff4
extern char    DlgText20[300];  // dialog string 0x20 (GlobalText[413])  // DAT_07d48120
extern char    DlgText16[300];  // dialog string 0x16/0x3f/0x41/0x45 (GlobalText[414])  // DAT_07d4824c
extern char    DlgText17[300];  // dialog string 0x17 (GlobalText[415])  // DAT_07d48378
extern char    DlgText18[300];  // dialog string 0x18 (GlobalText[416])  // DAT_07d484a4
extern char    DlgText19[300];  // dialog string 0x19 (GlobalText[417])  // DAT_07d485d0
extern char    DlgText74_0[300];  // dialog string 0x74 case 0 (GlobalText[420])  // DAT_07d48954
extern char    DlgText74_1[300];  // dialog string 0x74 case 1 (GlobalText[421])  // DAT_07d48a80
extern char    DlgText74_2[300];  // dialog string 0x74 case 2 (GlobalText[422])  // DAT_07d48bac
extern char    DlgText75[300];  // dialog string 0x75 (GlobalText[423])  // DAT_07d48cd8
extern char    DlgText76[300];  // dialog string 0x76 (GlobalText[424])  // DAT_07d48e04
extern char    DlgText7F[300];  // dialog string 0x7f (GlobalText[426])  // DAT_07d4905c
extern char    DlgText7E_0[300];  // dialog string 0x7e/line0 (GlobalText[427])  // DAT_07d49188
extern char    DlgText7E_1[300];  // dialog string 0x7e/line1 (GlobalText[428])  // DAT_07d492b4
extern char    DlgText25[300];  // dialog string 0x25 (GlobalText[433])  // DAT_07d49890
extern char    DlgText85[300];  // dialog string 0x85 (GlobalText[434])  // DAT_07d499bc
extern char    DlgText26[300];  // dialog string 0x26 (GlobalText[435])  // DAT_07d49ae8
extern char    DlgText3A_0[300];  // dialog string 0x3a case 0 (GlobalText[438])  // DAT_07d49e6c
extern char    DlgText3A_3[300];  // dialog string 0x3a case 3 (GlobalText[439])  // DAT_07d49f98
extern char    DlgText86[300];  // dialog string 0x86 (GlobalText[440])  // DAT_07d4a0c4
extern char    DlgText87[300];  // dialog string 0x87 (GlobalText[441])  // DAT_07d4a1f0
extern char    DlgText88[300];  // dialog string 0x88 (GlobalText[442])  // DAT_07d4a31c
extern char    DlgText98_0[300];  // dialog string 0x98/line0 (GlobalText[443])  // DAT_07d4a448
extern char    DlgText98_1[300];  // dialog string 0x98/line1 (GlobalText[444])  // DAT_07d4a574
extern char    DlgText89[300];  // dialog string 0x89 (GlobalText[445])  // DAT_07d4a6a0
extern char    DlgText22[300];  // dialog string 0x22 (GlobalText[597])  // DAT_07d558c0
extern char    DlgText23[300];  // dialog string 0x23 (GlobalText[598])  // DAT_07d559ec
extern char    DlgText24[300];  // dialog string 0x24 (GlobalText[599])  // DAT_07d55b18
extern char    DlgText8F[300];  // dialog string 0x8f (guild membership; GlobalText[734])  // DAT_07d5f94c

// ── GL_State cached state globals ────────────────────────────────────────────
extern int     BlendModeCache;   // current blend mode (0=off 2=SrcOver 3=Add 4=SrcAlpha)  // DAT_083a412c
extern char    AlphaTestEnableCache;   // **AlphaTestEnable** cache (glEnable/Disable 0xBC0). El comentario viejo decia 'fog' y es FALSO: ver GL_SetAlphaTest/680.  // DAT_083a411d
extern char    Texture2DEnableCache;   // **TextureEnable** cache (glEnable/Disable 0xDE1 = GL_TEXTURE_2D). El comentario viejo decia 'depth test' y es FALSO.  // DAT_083a4125
extern int     ScreenshotActiveFlag;   // screenshot active flag  // DAT_083a42f4
extern int     ScreenshotCounter;   // screenshot counter (mod 10000)  // DAT_083a42f0
// ── Float constants (overlapping globals) ────────────────────────────────────
extern float   FloatZeroB;  // 0.0f guard constant  // _DAT_00552580
extern float   WeatherMaxAltitude;  // particle proximity threshold (squared)  // _DAT_00552850
extern float   Math_DegreesToRadians;  // degrees-to-radians (π/180 = 0.017453f)
extern float   EulerHalfFactor;  // half-angle factor 0.5 (used in EulerToQuat EulerToQuat; input already radians)  // _DAT_00552ce0
extern float   QuatOne;  // 1.0f (used in quaternion→matrix)  // _DAT_00552cf0
extern float   HalfPi;  // π/2 (used in SLERP degenerate case)  // _DAT_00552cf8
extern float   SlerpEpsilon;  // SLERP near-parallel threshold (~1e-3)  // _DAT_00552d00
// ── Effect pool ──────────────────────────────────────────────────────────────
extern char    EffectPoolBase[kEffectPoolSize * OBJECT_STRIDE];   // Effect pool: 200 × 0x1bc (IDA: 0x07B11670..0x07B27150)  // DAT_07b11670
// ── Monster_Data string literals ─────────────────────────────────────────────
extern char    Path_MonsterSetBase2Txt[];  // "Data2/MonsterSetBase2.txt" -- monster spawn table path  // DAT_00561530
extern char    Dir_Data2Monster[];  // "Data2/Monster/" -- directory prefix  // DAT_0055e06c
extern char    Smd_SwordBasic[];  // "swordbasic.smd" -- SMD model path  // DAT_0055e07c
extern char    MonsterSmdName147;  // DAT_0055dff0
extern char    MonsterSmdName148;  // DAT_0055df70
extern char    MonsterSmdName149;  // DAT_0055dee0
extern char    MonsterSmdName14A;  // DAT_0055dec0
extern char    MonsterSmdName14B;  // DAT_0055de84
extern char    MonsterSmdName14C;  // DAT_0055de10
extern FILE   *BmdFileHandle;   // file handle (MonsterSetBase2 parser)  // DAT_07d7806c
extern char    FopenModeRb[]; // "rb" fopen mode string  // DAT_005580ac
extern char    bBuxCode[3]; // IDA: bBuxCode (0x00558090)
extern char    TextParserTokenString[256]; // DAT_07CF1EF0 — TokenString output of
                                   // GetToken (0x47A1F0).  NO confundir con
                                   // ParserTokenString, que es el buffer del OTRO
                                   // tokenizer (Parse_NextToken, 0x50E2C0).
extern char    SentinelEndString;   // sentinel "END" string base  // DAT_00559088
extern int     MonsterSpawnSlotIndex;   // spawn slot index  // DAT_07d7807c
extern char    Dir_DataMonster[];  // "Data/Monster/" -- directory prefix  // DAT_0055ddf8
extern char    Dir_Monster[];  // "Monster" -- directory name  // DAT_0055de08
extern char    Dir_MonsterSlash[];  // "Monster/" -- directory prefix  // DAT_0055ddec
// ── Monster sound filename strings (DAT_0055d…) ──────────────────────────────
extern char    MonsterSoundFileName125;  extern char    MonsterSoundFileName124;  // DAT_0055ddd4 DAT_0055ddbc
extern char    MonsterSoundFileName123;  extern char    MonsterSoundFileName122;  // DAT_0055dda0 DAT_0055dd84
extern char    MonsterSoundFileName121;  extern char    MonsterSoundFileName120;  // DAT_0055dd68 DAT_0055dd50
extern char    MonsterSoundFileName119;  extern char    MonsterSoundFileName118;  // DAT_0055dd38 DAT_0055dd1c
extern char    MonsterSoundFileName117;  extern char    MonsterSoundFileName116;  // DAT_0055dd00 DAT_0055dce4
extern char    MonsterSoundFileName115;  extern char    MonsterSoundFileName114;  // DAT_0055dccc DAT_0055dcb0
extern char    MonsterSoundFileName113;  extern char    MonsterSoundFileName112;  // DAT_0055dc94 DAT_0055dc7c
extern char    MonsterSoundFileName111;  extern char    MonsterSoundFileName110;  // DAT_0055dc60 DAT_0055dc44
extern char    MonsterSoundFileName109;  extern char    MonsterSoundFileName108;  // DAT_0055dc24 DAT_0055dc04
extern char    MonsterSoundFileName107;  extern char    MonsterSoundFileName106;  // DAT_0055dbe4 DAT_0055dbcc
extern char    MonsterSoundFileName105;  extern char    MonsterSoundFileName104;  // DAT_0055dbb4 DAT_0055db98
extern char    MonsterSoundFileName103;  extern char    MonsterSoundFileName102;  // DAT_0055db7c DAT_0055db60
extern char    MonsterSoundFileName101;  extern char    MonsterSoundFileName100;  // DAT_0055db44 DAT_0055db28
extern char    MonsterSoundFileName99;  extern char    MonsterSoundFileName98;  // DAT_0055db08 DAT_0055dae8
extern char    MonsterSoundFileName97;  extern char    MonsterSoundFileName96;  // DAT_0055dac8 DAT_0055dab0
extern char    MonsterSoundFileName95;  extern char    MonsterSoundFileName94;  // DAT_0055da98 DAT_0055da7c
extern char    MonsterSoundFileName93;  extern char    MonsterSoundFileName92;  // DAT_0055da5c DAT_0055da3c
extern char    MonsterSoundFileName91;  extern char    MonsterSoundFileName90;  // DAT_0055da24 DAT_0055da0c
extern char    MonsterSoundFileName89;  extern char    MonsterSoundFileName88;  // DAT_0055d9f0 DAT_0055d9d4
extern char    MonsterSoundFileName87;  extern char    MonsterSoundFileName86;  // DAT_0055d9b8 DAT_0055d9a0
extern char    MonsterSoundFileName85;  extern char    MonsterSoundFileName84;  // DAT_0055d988 DAT_0055d96c
extern char    MonsterSoundFileName83;  extern char    MonsterSoundFileName82;  // DAT_0055d950 DAT_0055d934
extern char    MonsterSoundFileName81;  extern char    MonsterSoundFileName80;  // DAT_0055d91c DAT_0055d904
extern char    MonsterSoundFileName79;  extern char    MonsterSoundFileName78;  // DAT_0055d8e8 DAT_0055d8cc
extern char    MonsterSoundFileName77;  extern char    MonsterSoundFileName76;  // DAT_0055d8b0 DAT_0055d894
extern char    MonsterSoundFileName75;  extern char    MonsterSoundFileName74;  // DAT_0055d878 DAT_0055d858
extern char    MonsterSoundFileName73;  extern char    MonsterSoundFileName72;  // DAT_0055d844 DAT_0055d830
extern char    MonsterSoundFileName71;  extern char    MonsterSoundFileName70;  // DAT_0055d818 DAT_0055d800
extern char    MonsterSoundFileName69;  extern char    MonsterSoundFileName68;  // DAT_0055d7e8 DAT_0055d7cc
extern char    MonsterSoundFileName67;  extern char    MonsterSoundFileName66;  // DAT_0055d7b0 DAT_0055d794
extern char    MonsterSoundFileName65;  extern char    MonsterSoundFileName64;  // DAT_0055d778 DAT_0055d758
extern char    MonsterSoundFileName63;  extern char    MonsterSoundFileName62;  // DAT_0055d738 DAT_0055d718
extern char    MonsterSoundFileName58;  extern char    MonsterSoundFileName57;  // DAT_0055d6ac DAT_0055d694
extern char    MonsterSoundFileName56;  extern char    MonsterSoundFileName55;  // DAT_0055d678 DAT_0055d65c
extern char    MonsterSoundFileName61;  extern char    MonsterSoundFileName60;  // DAT_0055d6fc DAT_0055d6e0
extern char    MonsterSoundFileName59;  extern char    MonsterSoundFileName54;  // DAT_0055d6c4 DAT_0055d644
extern char    MonsterSoundFileName53;  extern char    MonsterSoundFileName52;  // DAT_0055d62c DAT_0055d610
extern char    MonsterSoundFileName51;  extern char    MonsterSoundFileName50;  // DAT_0055d5f4 DAT_0055d5d8
extern char    MonsterSoundFileName49;  extern char    MonsterSoundFileName48;  // DAT_0055d5bc DAT_0055d5a0
extern char    MonsterSoundFileName47;  extern char    MonsterSoundFileName46;  // DAT_0055d580 DAT_0055d560
extern char    MonsterSoundFileName45;  extern char    MonsterSoundFileName44;  // DAT_0055d540 DAT_0055d528
extern char    MonsterSoundFileName43;  extern char    MonsterSoundFileName42;  // DAT_0055d50c DAT_0055d4f0
extern char    MonsterSoundFileName41;  extern char    MonsterSoundFileName40;  // DAT_0055d4d8 DAT_0055d4c0
extern char    MonsterSoundFileName39;  extern char    MonsterSoundFileName38;  // DAT_0055d4a4 DAT_0055d488
extern char    MonsterSoundFileName37;  extern char    MonsterSoundFileName36;  // DAT_0055d46c DAT_0055d450
extern char    MonsterSoundFileName35;  extern char    MonsterSoundFileName34;  // DAT_0055d434 DAT_0055d414
extern char    MonsterSoundFileName33;  extern char    MonsterSoundFileName32;  // DAT_0055d3f4 DAT_0055d3d4
extern char    MonsterSoundFileName31;  extern char    MonsterSoundFileName30;  // DAT_0055d3bc DAT_0055d3a4
extern char    MonsterSoundFileName29;  extern char    MonsterSoundFileName28;  // DAT_0055d388 DAT_0055d36c
extern char    MonsterSoundFileName27;  extern char    MonsterSoundFileName26;  // DAT_0055d350 DAT_0055d338
extern char    MonsterSoundFileName25;  extern char    MonsterSoundFileName24;  // DAT_0055d320 DAT_0055d304
extern char    MonsterSoundFileName23;  extern char    MonsterSoundFileName22;  // DAT_0055d2e8 DAT_0055d2cc
extern char    MonsterSoundFileName21;  extern char    MonsterSoundFileName20;  // DAT_0055d2b0 DAT_0055d298
extern char    MonsterSoundFileName19;  extern char    MonsterSoundFileName18;  // DAT_0055d280 DAT_0055d264
extern char    MonsterSoundFileName17;  extern char    MonsterSoundFileName16;  // DAT_0055d24c DAT_0055d234
extern char    MonsterSoundFileName15;  extern char    MonsterSoundFileName14;  // DAT_0055d218 DAT_0055d1fc
extern char    MonsterSoundFileName13;  extern char    MonsterSoundFileName12;  // DAT_0055d1e0 DAT_0055d1c8
extern char    MonsterSoundFileName11;  extern char    MonsterSoundFileName10;  // DAT_0055d1b0 DAT_0055d194
extern char    MonsterSoundFileName09;  extern char    MonsterSoundFileName08;  // DAT_0055d178 DAT_0055d15c
extern char    MonsterSoundFileName07;  extern char    MonsterSoundFileName06;  // DAT_0055d144 DAT_0055d12c
extern char    MonsterSoundFileName05;  extern char    MonsterSoundFileName04;  // DAT_0055d114 DAT_0055d0f8
extern char    MonsterSoundFileName03;  extern char    MonsterSoundFileName02;  // DAT_0055d0dc DAT_0055d0c0
extern char    MonsterSoundFileName01;  extern char    MonsterSoundFileName00;  // DAT_0055d0a8 DAT_0055d08c
// ── Monster sound s_ named strings (0055cxxx range) ──────────────────────────
extern char    Wav_Iron1[];  // "Data/Sound/iron1.wav" -- sfx path (iron golem)  // DAT_0055d074
extern char    Wav_IronAttack1[];  // "Data/Sound/iron_attack1.wav" -- attack sfx path (iron golem)  // DAT_0055d058
extern char    Wav_Jaikan1[];  // "Data/Sound/jaikan1.wav" -- sfx path (Jaikan)  // DAT_0055d040
extern char    Wav_Jaikan2[];  // "Data/Sound/jaikan2.wav" -- sfx path (Jaikan)  // DAT_0055d028
extern char    Wav_JaikanAttack1[];  // "Data/Sound/jaikan_attack1.wav" -- attack sfx path (Jaikan)  // DAT_0055d008
extern char    Wav_JaikanAttack2[];  // "Data/Sound/jaikan_attack2.wav" -- attack sfx path (Jaikan)  // DAT_0055cfe8
extern char    Wav_JaikanDie[];  // "Data/Sound/jaikan_die.wav" -- death sfx path (Jaikan)  // DAT_0055cfcc
extern char    Img_MonsterBv01_2[];  // "Monster/bv01_2.jpg" -- monster image path  // DAT_0055cfb8
extern char    Img_MonsterBv02_2[];  // "Monster/bv02_2.jpg" -- monster image path  // DAT_0055cfa4
extern char    Wav_Blood1[];  // "Data/Sound/blood1.wav" -- sfx path (Blood soldier)  // DAT_0055cf8c
extern char    Wav_BloodAttack1[];  // "Data/Sound/blood_attack1.wav" -- attack sfx path (Blood soldier)  // DAT_0055cf6c
extern char    Wav_BloodAttack2[];  // "Data/Sound/blood_attack2.wav" -- attack sfx path (Blood soldier)  // DAT_0055cf4c
extern char    Wav_BloodDie[];  // "Data/Sound/blood_die.wav" -- death sfx path (Blood soldier)  // DAT_0055cf30
extern char    Wav_Death1[];  // "Data/Sound/death1.wav" -- sfx path (Death cow)  // DAT_0055cf18
extern char    Wav_DeathAttack1[];  // "Data/Sound/death_attack1.wav" -- attack sfx path (Death cow)  // DAT_0055cef8
extern char    Wav_DeathDie[];  // "Data/Sound/death_die.wav" -- death sfx path (Death cow)  // DAT_0055cedc
extern char    Wav_Mutant1[];  // "Data/Sound/mutant1.wav" -- sfx path (Mutant)  // DAT_0055cec4
extern char    Wav_Mutant2[];  // "Data/Sound/mutant2.wav" -- sfx path (Mutant)  // DAT_0055ceac
extern char    Wav_MutantAttack1[];  // "Data/Sound/mutant_attack1.wav" -- attack sfx path (Mutant)  // DAT_0055ce8c
extern char    Wav_OrcArcherAttack1[];  // "Data/Sound/mOrcArcherAttack1.wav" -- attack sfx path (Orc archer)  // DAT_0055ce68
extern char    Wav_OrcCapAttack1[];  // "Data/Sound/mOrcCapAttack1.wav" -- attack sfx path (Orc captain)  // DAT_0055ce48
extern char    Wav_CursedKing1[];  // "Data/Sound/mCursedKing1.wav" -- sfx path (Cursed King)  // DAT_0055ce2c
extern char    Wav_CursedKing2[];  // "Data/Sound/mCursedKing2.wav" -- sfx path (Cursed King)  // DAT_0055ce10
extern char    Wav_CursedKingDie1[];  // "Data/Sound/mCursedKingDie1.wav" -- death sfx path (Cursed King)  // DAT_0055cdf0
extern char    Img_MonsterIui02[];  // "Monster/iui02.tga" -- monster image path  // DAT_0055cddc
extern char    Img_MonsterIui03[];  // "Monster/iui03.tga" -- monster image path  // DAT_0055cdc8
extern char    Wav_Molt1[];  // "Data/Sound/mMolt1.wav" -- sfx path (Molt)  // DAT_0055cd50
extern char    Wav_MoltAttack1[];  // "Data/Sound/mMoltAttack1.wav" -- attack sfx path (Molt)  // DAT_0055cd34
extern char    Wav_MoltDie[];  // "Data/Sound/mMoltDie.wav" -- death sfx path (Molt)  // DAT_0055cd1c
extern char    Wav_MegaCrust1[];  // "Data/Sound/mMegaCrust1.wav" -- sfx path (MegaCrust)  // DAT_0055cdac
extern char    Wav_MegaCrustAttack1[];  // "Data/Sound/mMegaCrustAttack1.wav" -- attack sfx path (MegaCrust)  // DAT_0055cd88
extern char    Wav_MegaCrustDie[];  // "Data/Sound/mMegaCrustDie.wav" -- death sfx path (MegaCrust)  // DAT_0055cd68
extern char    Wav_AlquamosAttack1[];  // "Data/Sound/mAlquamosAttack1.wav" -- attack sfx path (Alquamos)  // DAT_0055ccfc
extern char    Wav_AlquamosDie[];  // "Data/Sound/mAlquamosDie.wav" -- death sfx path (Alquamos)  // DAT_0055cce0
extern char    Wav_Rainner1[];  // "Data/Sound/mRainner1.wav" -- sfx path (Rainner)  // DAT_0055ccc4
extern char    Wav_RainnerAttack1[];  // "Data/Sound/mRainnerAttack1.wav" -- attack sfx path (Rainner)  // DAT_0055cca4
extern char    Wav_RainnerDie[];  // "Data/Sound/mRainnerDie.wav" -- death sfx path (Rainner)  // DAT_0055cc88
extern char    Wav_Phantom1[];  // "Data/Sound/mPhantom1.wav" -- sfx path (Phantom knight)  // DAT_0055cc6c
extern char    Wav_PhantomAttack1[];  // "Data/Sound/mPhantomAttack1.wav" -- attack sfx path (Phantom knight)  // DAT_0055cc4c
extern char    Wav_PhantomDie[];  // "Data/Sound/mPhantomDie.wav" -- death sfx path (Phantom knight)  // DAT_0055cc30
extern char    Wav_Drakan1[];  // "Data/Sound/mDrakan1.wav" -- sfx path (Drakan)  // DAT_0055cc18
extern char    Wav_DrakanAttack1[];  // "Data/Sound/mDrakanAttack1.wav" -- attack sfx path (Drakan)  // DAT_0055cbf8
extern char    Wav_DrakanDie[];  // "Data/Sound/mDrakanDie.wav" -- death sfx path (Drakan)  // DAT_0055cbdc
extern char    Wav_Phoenix1[];  // "Data/Sound/mPhoenix1.wav" -- sfx path (Phoenix)  // DAT_0055cbc0
extern char    Wav_PhoenixAttack1[];  // "Data/Sound/mPhoenixAttack1.wav" -- attack sfx path (Phoenix)  // DAT_0055cba0
extern char    Wav_MagicSkull[];  // "Data/Sound/mMagicSkull.wav" -- sfx path (magic skull)  // DAT_0055cb84
extern char    Wav_BullDie[];  // "Data/Sound/mBullDie.wav" -- death sfx path (Bull fighter)  // DAT_0055cb6c
extern char    Wav_BlackSkullDie[];  // "Data/Sound/mBlackSkullDie.wav" -- death sfx path (Black knight)  // DAT_0055cb4c
extern char    Wav_BlackSkullAttack[];  // "Data/Sound/mBlackSkullAttack.wav" -- attack sfx path (Black knight)  // DAT_0055cb28
extern char    Wav_GhaintOrgerDie[];  // "Data/Sound/mGhaintOrgerDie.wav" -- death sfx path (Ghaint Orger)  // DAT_0055cb08
extern char    Wav_RedSkull[];  // "Data/Sound/mRedSkull.wav" -- sfx path (Red skeleton)  // DAT_0055caec
extern char    Wav_RedSkullDie[];  // "Data/Sound/mRedSkullDie.wav" -- death sfx path (Red skeleton)  // DAT_0055cad0
extern char    Wav_RedSkullAttack[];  // "Data/Sound/mRedSkullAttack.wav" -- attack sfx path (Red skeleton)  // DAT_0055cab0

// ── CreateEffect float constants ────────────────────────────────────────────
extern float   AngleDeg180;  // _DAT_005524ec
extern float   DegToRad;  // _DAT_0055253c
extern float   DeathFrameMax;  // death particle anim frame upper bound  // _DAT_00552830
extern float   EffectAngleRadA;  // _DAT_00552960
extern float   EffectAngleRadB;  // _DAT_00552968
// ── Weather particle pool (40 slots × 0x1bc bytes = 0x4560 bytes) ────────────
// Los 30+ globals DAT_0839bc?? son macros que indexan dentro de un único buffer
// contiguo: Weather_Update y Particle_PathUpdate los acceden con stride 0x1bc
// (slot stride) o 0x6f (int stride = 0x1bc/4).
extern char    g_WeatherSlotPool[40 * OBJECT_STRIDE];  // weather effect object pool (40 slots x OBJECT_STRIDE)
#define WeatherSlot0ActiveFlag  g_WeatherSlotPool[0x000]                              // slot[0] active flag (char)  // DAT_0839bcb0
#define WeatherSlot0Type  (*(short        *)&g_WeatherSlotPool[0x002])          // slot[0] type (short, stride 0xde × short)  // DAT_0839bcb2
#define WeatherModeFlag  g_WeatherSlotPool[0x004]                              // slot[0] mode flag (char)  // DAT_0839bcb4
#define WeatherSlot0AngSpeed  (*(unsigned int *)&g_WeatherSlotPool[0x00c])          // slot[0] angular speed (int/float)  // DAT_0839bcbc
#define WeatherSlot0PosX  (*(unsigned int *)&g_WeatherSlotPool[0x010])          // slot[0] pos_x (int/float)  // DAT_0839bcc0
#define WeatherSlot0PosY  (*(unsigned int *)&g_WeatherSlotPool[0x014])          // slot[0] pos_y (int/float)  // DAT_0839bcc4
#define WeatherSlot0PosZ  (*(unsigned int *)&g_WeatherSlotPool[0x018])          // slot[0] pos_z (int/float)  // DAT_0839bcc8
#define WeatherSlot0VelX  (*(unsigned int *)&g_WeatherSlotPool[0x01c])          // slot[0] vel_x (int/float)  // DAT_0839bccc
#define WeatherSlot0VelY  (*(unsigned int *)&g_WeatherSlotPool[0x020])          // slot[0] vel_y (int/float)  // DAT_0839bcd0
#define WeatherSlot0Phase  (*(unsigned int *)&g_WeatherSlotPool[0x024])          // slot[0] phase (int/float)  // DAT_0839bcd4
#define WeatherSlot0ColorR  (*(unsigned int *)&g_WeatherSlotPool[0x058])          // slot[0] color R or -1 (int)  // DAT_0839bd08
#define WeatherSlot0ColorG  (*(unsigned int *)&g_WeatherSlotPool[0x060])          // slot[0] color G/intensity (int)  // DAT_0839bd10
#define WeatherSlot0ColorB  (*(unsigned int *)&g_WeatherSlotPool[0x064])          // slot[0] color B (int)  // DAT_0839bd14
#define WeatherSlot0RotAngle  (*(unsigned int *)&g_WeatherSlotPool[0x080])          // slot[0] rotation angle (int/float)  // DAT_0839bd30
#define WeatherSlot0DirVec  g_WeatherSlotPool[0x090]                              // slot[0] direction vector base (3 floats)  // DAT_0839bd40
#define WeatherHeadPosX  g_WeatherSlotPool[0x0c0]                              // slot[0] cached head pos X (float byte addr)  // DAT_0839bd70
#define WeatherHeadPosY  g_WeatherSlotPool[0x0c4]                              // slot[0] cached head pos Y  // DAT_0839bd74
#define WeatherSlot0VelZ  g_WeatherSlotPool[0x0c8]                              // slot[0] velocity Z (float)  // DAT_0839bd78
#define WeatherSlot0Scale  (*(unsigned int *)&g_WeatherSlotPool[0x0cc])          // slot[0] scale (int/float)  // DAT_0839bd7c
#define WeatherSize  g_WeatherSlotPool[0x0d4]                              // slot[0] size (float byte addr)  // DAT_0839bd84
#define WeatherSlot0AltLimit  g_WeatherSlotPool[0x0d8]                              // slot[0] altitude limit (float byte addr)  // DAT_0839bd88
#define WeatherRenderFlag  (*(unsigned int *)&g_WeatherSlotPool[0x0dc])          // slot[0] render flag (int)  // DAT_0839bd8c
#define WeatherSlot0ColorRF  g_WeatherSlotPool[0x0e8]                              // slot[0] color R float byte addr  // DAT_0839bd98
#define WeatherSlot0ColorGF  g_WeatherSlotPool[0x0ec]                              // slot[0] color G float byte addr  // DAT_0839bd9c
#define WeatherSlot0ColorBF  g_WeatherSlotPool[0x0f0]                              // slot[0] color B float byte addr  // DAT_0839bda0
#define WeatherSlot0State  g_WeatherSlotPool[EntityOffset(0x104)]                              // slot[0] state byte (cloud life phase)  // DAT_0839bdb4
#define WeatherSlot0BlendMode  g_WeatherSlotPool[EntityOffset(0x105)]                              // slot[0] blend mode byte  // DAT_0839bdb5
#define WeatherSlotByte106  g_WeatherSlotPool[EntityOffset(0x106)]                              // slot[0] field at +0x106  // DAT_0839bdb6
#define WeatherSlot0RotF  g_WeatherSlotPool[EntityOffset(0x108)]                              // slot[0] rotation float byte addr  // DAT_0839bdb8
#define WeatherSlotByte10C  g_WeatherSlotPool[EntityOffset(0x10c)]                              // slot[0] field at +0x10c  // DAT_0839bdbc
#define WeatherSlotFlag161  g_WeatherSlotPool[EntityOffset(0x161)]                              // slot[0] byte field  // DAT_0839be11
#define WeatherSlot0Alpha  (*(unsigned int *)&g_WeatherSlotPool[EntityOffset(0x164)])          // slot[0] alpha fade (int/float)  // DAT_0839be14
// ── Weather float constants ───────────────────────────────────────────────────
extern float   WeatherMinAltitude;  // weather float constant  // _DAT_0055285c
extern float   AngleDeg360;  // weather float constant  // _DAT_0055286c
extern float   WeatherCeiling;  // weather z-offset constant  // _DAT_00552900
extern float   WeatherNearDist;  // weather float constant  // _DAT_00552ab4
extern float   WeatherDist1000;  // weather float constant  // _DAT_00552ab8
extern float   WeatherDespawnDist;  // weather float constant  // _DAT_00552d24
extern float   WeatherOrbitFreq;  // weather float constant  // _DAT_00552d28

// ── Camera / viewport globals ─────────────────────────────────────────────────
extern int     ActionObjCollapseType;   // SetActionObject: tipo de objeto que se derrumba  // DAT_0055a7b0
extern int     ActionObjWorldId;   // SetActionObject: World en el que aplica  // DAT_0055a7b4
extern int     ActionObjFrameCount;   // SetActionObject: contador de frames (20 = arranca)  // DAT_0055a7b8
extern float   ViewportHeightF;  // viewport height (float)  // _DAT_0055a7bc

// ── GL_State cache ────────────────────────────────────────────────────────────
extern char    CullFaceEnableCache;   // GL state cache byte  // DAT_083a411c
extern char    DepthTestEnableCache;   // GL state cache byte  // DAT_083a411e
extern float   RayCastClosestT;   // Map_InitRayCast running closest-hit ray-t (was DWORD; widened to match IDA flt_83A4120 — audit #7)  // DAT_083a4120
extern DWORD   DepthMaskCache;   // GL state cache dword  // DAT_083a42e8
#define ViewportHeightFBits ViewportHeightF  // DAT_0055a7bc

// ── Fog / projection globals ──────────────────────────────────────────────────
extern float   FogStartDistance;   // fog start distance  // DAT_00561558
extern DWORD   FogColor;   // fog color pointer / float start  [IDA 0x0056155C]

// ── Perspective constant ──────────────────────────────────────────────────────
extern float   PerspectiveFarScale;  // perspective scale constant  // _DAT_00552d34

// ── Joint pool ────────────────────────────────────────────────────────────────
// JOINT lleva un puntero en +0x40: los campos siguientes se corren en x64.
constexpr int JOINT_FIELD_SHIFT = (int)sizeof(void*) - 4;
constexpr int JOINT_STRIDE = 0x9D8 + JOINT_FIELD_SHIFT;
extern char    JointPoolBase[kJointPoolSize * JOINT_STRIDE];   // Joint pool: 500 × 0x9d8 (IDA: 0x07B27150..0x07C5AB30)  // DAT_07b27150

// ── Joint_Create float constants ─────────────────────────────────────────────
extern float   NegDegToRad;  // joint float constant  // _DAT_00552a44

// ── Timer globals ─────────────────────────────────────────────────────────────
extern DWORD   FpsFrameCountB;   // FPS counter B  // DAT_05826e14
extern DWORD   FrameTimeCurrentMs;   // IDA: DAT_05826df0
extern float   MsToSeconds;  // 1/1000 ms-to-s scale  // _DAT_005528a8
extern float   FpsWindowSec;  // 5.0 second FPS window  // _DAT_00552898
extern float   DeltaTimeScale;  // delta-time scale  // _DAT_00552890
extern DWORD   FpsWindowStartMs;   // FPS accumulator ring buffer  // DAT_05826e00
extern float   DeltaT;  // delta time per frame  [IDA 0x0055979C]
extern DWORD   FrameTimePreviousMs;  // IDA: DAT_05826dfc
extern float   FPS;  // smoothed FPS value  [IDA 0x05826DF8]

// ── Music.cpp globals ─────────────────────────────────────────────────────────
// IDA: DAT_055C9D04

// ── Sound_DS3D globals ────────────────────────────────────────────────────────
extern DWORD   DS3DState;   // DS3D state dword  // DAT_0058443c

// ── Net_Connect globals ───────────────────────────────────────────────────────
extern DWORD   First; // IDA: First (0x055CA15C)
extern char    Err_FailedToConnect[];  // "Failed to connect." -- connection failure message  // DAT_00559688

// ── Net_PacketSession globals ─────────────────────────────────────────────────
// Char-select entry pools — IDA layout:
//   LegacyWarehouseKey .. 0x07ea7b48 (= 8160 bytes = 116 slots × 0x44 stride)
//   LegacyMixItemsKey .. 0x07eaa100 (= 2176 bytes =  32 slots × 0x44 stride)
// Cada slot es el registro del panel de char-select. En nuestro build estaban
// declarados como DWORDs sueltos, lo que hacía que CloseInventoryRelatedWindows (CharPreview_Reset)
// recorriera mucho más allá del final, en memoria random → AV.
extern DWORD   TradeConfirmLatch;  // DAT_07eaa0e8
extern DWORD   GuildMarkColors[16];   // MarkColor[16] — paleta de la marca de guild (ARGB)  // DAT_07e11f34
extern BYTE    TradeItemList[0x880];  // DAT_07e11f78
// Alias de CAMPO sobre los pools de items (verificado en el desensamblado de
// CloseInventoryRelatedWindows 0x4CBD36-0x4CBD9C y los errores de
// ida_get_function): en el binario no son copias sino el mismo pool abordado
// desde otro campo.  Los bucles originales escriben Type en `ptr - 0x38` y
// Key en `ptr`, o sea DAT_x + 0x38 = Key del slot 0 (no son arrays propios).
//   0x07EA5298 Inventory              0x07EA52D0 Inventory.Key
//   0x07EA7B88 OffsetTradeItems       0x07EA7BC0 OffsetTradeItems.Key
//   0x07EA9880 OffsetMixItems.Key     0x07EA8448 OffsetInventoryItems.Key
//   0x07EA5B68 Key del pool de 0x07EA5B30 (baul; en IDA tambien la tienda)
//   0x07E11FB0 Key de word_7E11F78 (trade del otro jugador)
// Son lvalues de array: `&`, la aritmetica y el decay a BYTE* funcionan igual.
#define LegacyInventory   (*(BYTE(*)[0x880])(Inventory))  // DAT_07ea5298
#define LegacyInventoryKey   (*(BYTE(*)[0x880])(Inventory + 0x38))  // DAT_07ea52d0
#define LegacyTradeItems   (*(BYTE(*)[0x880])(OffsetTradeItems))  // DAT_07ea7b88
#define LegacyTradeKey   (*(BYTE(*)[0x880])(OffsetTradeItems + 0x38))  // DAT_07ea7bc0
#define LegacyMixItemsKey   (*(BYTE(*)[0x880])(OffsetMixItems + 0x38))  // DAT_07ea9880
#define LegacyInvGridKey   (*(BYTE(*)[0x1100])(OffsetInventoryItems + 0x38))  // DAT_07ea8448
#define LegacyWarehouseKey   (*(BYTE(*)[0x1FE0])(OffsetWarehouseItems + 0x38))  // DAT_07ea5b68
#define TradeItemKeyBase   (*(BYTE(*)[0x880])(TradeItemList + 0x38))  // DAT_07e11fb0
extern DWORD   ChatFocusIdA;  // DAT_055c9b7c
extern DWORD   InventorySpecialModeFlag;  // DAT_07eaa164

// ── Entity_Init globals ───────────────────────────────────────────────────────
extern char    BinaryReadModeStr[];  // "rb" fopen mode  // DAT_00559b74
extern char    Fmt_DualHpSlots[];  // "%4d/%4d[0] %4d/%4d[1]" -- dual-slot HP readout format  // DAT_00559b58

// ── Scene_CharPreview globals ─────────────────────────────────────────────────
// IDA: pPickedItem.Level (0x07E91354) — el campo +4 del item levantado, no un
// global aparte.  Como variable propia valía siempre 0 y los chequeos de
// apilado comparaban contra nivel 0.
#define PickedItemLevel (*(DWORD*)&PickedItemData[4])  // DAT_07e91354
extern char    PickedItemOptionByte;   // picked item durability/option byte  // DAT_07e9136a
extern char    PickedItemDurExtra;  // DAT_07e9136b
// Buffer del item del dialogo ShowCheckBox(153) -- 0x44 bytes, 0x07EA5240.
// PreviewItemLevel (Level) y PreviewItemOption1 (Option1) son campos DENTRO de el, no
// globals aparte: declararlos sueltos hacia que el memcpy del click derecho no
// los alcanzara nunca.  Mismo patron que PrimaryTerrainLightG / LegacyDlgBtnRectX.
extern BYTE    PreviewItemData[0x44];  // DAT_07ea5240
#define PreviewItemLevel   (*(DWORD*)(PreviewItemData + 4))  // DAT_07ea5244
#define PreviewItemOption1   (*(char*) (PreviewItemData + 0x1b))  // DAT_07ea525b

// ── Effect_Tick globals ───────────────────────────────────────────────────────
// IDA: MovePlanes usa g_PlayerRenderPool +0x60; no tiene otra asignación.
extern char    g_PointerPool[100 * 0x70]; // IDA: Pointers (0x07C82CD0).
extern char    SparkEffectPool[100 * 0x70];   // Spark-effect pool: 100 × 0x70  // DAT_07c80128

// ── Terrain_Light globals ─────────────────────────────────────────────────────
extern DWORD   LightMapEnableFlag;  // DAT_0839bc84
// cb60c / 0828b60c / cb610 / 0828b610 NO son globals separados —
// son el 2do/3er DWORD del slot 0 de cb608 / 0828b608. Las macros están
// definidas junto a las declaraciones de cb608 / 0828b608, más arriba en este header.

// ── Scene_Resources string literals ──────────────────────────────────────────
// Nombres base de BMD/SMD que consumen AccessModel / OpenModel en Scene_Resources.
extern char    ModelBaseNameShip[8];     // "Ship"  // DAT_0055e834
extern char    ModelBaseNameLogo[8];     // "Logo"  // DAT_005606ac
extern char    ModelBaseNameFace[8];     // "Face"  // DAT_005607c0
extern char    SmdNameBackground[32];    // SMD basename (stub)  // DAT_005606e8
extern char    SmdNameBackground2[32];    // SMD basename (stub)  // DAT_005606d0
extern char    SmdNameSwordsmanFace[32];    // SMD basename (stub)  // DAT_005607c8
extern char    SmdNameWizardFace[32];    // SMD basename (stub)  // DAT_0056085c
extern DWORD   MapLoadReadDesc;    // Map_Load block-read descriptor (format/context for crt_fprintf)  // DAT_00560694
extern char    Img_LogoWebzenlogo[];  // "Logo/Webzenlogo.jpg" -- texture image path (WebZen logo)  // DAT_005606b0
extern char    Img_LogoTitle[];  // "Logo/Title.jpg" -- texture image path (title screen)  // DAT_005606bc
extern char    Img_GfxInterface[];  // "Interface/GFx/Interface.jpg" -- texture image path (interface background)  // DAT_005607c4
extern char    Img_GfxInterface2[];  // "Interface/GFx/Interface2.jpg" -- texture image path (interface background)
extern char    Img_DataInterface[];  // "Data/Interface.jpg" -- texture image path
extern char    Img_DataInterface2[];  // "Data/Interface2.jpg" -- texture image path
extern char    Img_Object1[];  // "Object1/Object1.jpg" -- texture image path
extern char    Bmd_Warrior[];  // "warrior.bmd" -- BMD model path
extern char    Bmd_Main[];  // "main.bmd" -- BMD model path
extern char    Bmd_Fairy[];  // "fairy.bmd" -- BMD model path

// ── Scene_Resources strings ───────────────────────────────────────────────────
extern char    Img_LogoAccountNew[];  // "Logo\\0Account_new.tga" -- texture image path (new account)  // DAT_005607a8
extern char    Img_LogoOnButton[];  // "Logo\\0On_Botton.jpg" -- texture image path (login button)  // DAT_00560794
extern char    Img_LogoOnButton2[];  // "Logo\\0On_Botton2.jpg" -- texture image path (login button)  // DAT_0056077c
extern char    Img_LogoTextBox[];  // "Logo\\0Text_Box.jpg" -- texture image path (text box)  // DAT_00560768
extern char    Img_LogoNewAccount01[];  // "Logo\\0New_Account01.tga" -- texture image path (new account)  // DAT_00560750
extern char    Img_LogoNewAccount02[];  // "Logo\\0New_Account02.tga" -- texture image path (new account)  // DAT_00560738
extern char    Img_LogoBox[];  // "Logo\\0Box.jpg" -- texture image path (box frame)  // DAT_00560728
extern char    Img_ProgressBack[];  // "Interface\\Progress_Back.jpg" -- texture image path (progress bar bg)  // DAT_0056070c
extern char    Img_Progress[];  // "Interface\\Progress.jpg" -- texture image path (progress bar)  // DAT_005606f4
extern char    Dir_Data2Object1[];  // "Data2\\Object1\\" -- directory prefix  // DAT_0055f120
extern char    Smd_Ship[];  // "ship.smd" -- SMD model path  // DAT_0055ee54
extern char    Dir_Data2Logo[];  // "Data2\\Logo\\" -- directory prefix  // DAT_005606dc
extern char    Smd_Mu[];  // "mu.smd" -- SMD model path  // DAT_005606c8
extern char    Smd_Sun[];  // "sun.smd" -- SMD model path  // DAT_005606b4
extern char    Dir_DataObject1[];  // "Data\\Object1\\" -- directory prefix  // DAT_0055f360
extern char    Dir_DataLogo[];  // "Data\\Logo\\" -- directory prefix  // DAT_005606a0
extern char    Dir_Object1[];  // "Object1\\" -- directory prefix  // DAT_0055f354
extern char    Dir_Logo[];  // "Logo\\" -- directory prefix  // DAT_00560698
extern char    Img_LogoInterface01[];  // "Logo\\Interface01.tga" -- texture image path (interface)  // DAT_00560a4c
extern char    Img_LogoInterface02[];  // "Logo\\Interface02.tga" -- texture image path (interface)  // DAT_00560a34
extern char    Img_LogoInterface03[];  // "Logo\\Interface03.tga" -- texture image path (interface)  // DAT_00560a1c
extern char    Img_LogoInterface04[];  // "Logo\\Interface04.tga" -- texture image path (interface)  // DAT_00560a04
extern char    Img_NewCharacter201[];  // "Logo\\New_Character201.tga" -- texture image path (character create)  // DAT_005609e8
extern char    Img_NewCharacter202[];  // "Logo\\New_Character202.jpg" -- texture image path (character create)  // DAT_005609cc
extern char    Img_Delete01[];  // "Logo\\Delete01.tga" -- texture image path (delete button)  // DAT_005609b8
extern char    Img_Delete02[];  // "Logo\\Delete02.jpg" -- texture image path (delete button)  // DAT_005609a4
extern char    Img_Ok01[];  // "Logo\\Ok01.tga" -- texture image path (OK button)  // DAT_00560994
extern char    Img_Ok02[];  // "Logo\\Ok02.jpg" -- texture image path (OK button)  // DAT_00560984
extern char    Img_NewCharacter01[];  // "Logo\\New_Character01.tga" -- texture image path (character create)  // DAT_00560968
extern char    Img_NewCharacter02[];  // "Logo\\New_Character02.tga" -- texture image path (character create)  // DAT_0056094c
extern char    Img_NewCharacterCancel[];  // "Logo\\New_Character_Cancel.jpg" -- texture image path (cancel button)  // DAT_0056092c
extern char    Img_NewCharacterOk[];  // "Logo\\New_Character_Ok.jpg" -- texture image path (OK button)  // DAT_00560910
extern char    Img_NewCharacter001[];  // "Logo\\New_Character001.jpg" -- texture image path (character create)  // DAT_005608f4
extern char    Img_NewCharacter002[];  // "Logo\\New_Character002.jpg" -- texture image path (character create)  // DAT_005608d8
extern char    Img_NewCharacter003[];  // "Logo\\New_Character003.jpg" -- texture image path (character create)  // DAT_005608bc
extern char    Img_NewCharacter004[];  // "Logo\\New_Character004.jpg" -- texture image path (character create)  // DAT_005608a0
extern char    Smd_Main[];  // "main.smd" -- SMD model path  // DAT_00560894
extern char    Smd_Warrior[];  // "warrior.smd" -- SMD model path  // DAT_00560828
extern char    Smd_Fairy[];  // "fairy.smd" -- SMD model path  // DAT_0055c438

// ── Entity_Spawn ──────────────────────────────────────────────────────────────
// TerrainWallFlags redeclarado como array de char (no DWORD)
// (definido en globals.cpp como char TerrainWallFlags[])

// ── Additional misc globals ───────────────────────────────────────────────────
// LastFpsTickMsBits — misma dirección que LastFpsTickMs (ya declarado arriba)
#define LastFpsTickMsBits  LastFpsTickMs  // DAT_07e016f0
// LegacyChatBubblePool -> macro sobre el pool (ver abajo)   // Tooltip/bubble pool: 26 × 0x254 bytes
extern BYTE    GlitterBoneIndexTable[9];  // bone index table for glitter effect (Entity_UpdateRender)  // DAT_0055984c
extern DWORD   KeyState[256];  // key-state edge-trigger table  [IDA 0x07E118EC]
extern DWORD   F1KeyDebounce;  // DAT_07e11aac
extern DWORD   F2KeyDebounce;  // DAT_07e11ab0
extern DWORD   F3KeyDebounce;  // DAT_07e11ab4
extern DWORD   F4KeyDebounce;  // DAT_07e11ab8
extern DWORD   ChatBlinkCounter;  // DAT_07e11da8
extern DWORD   InventoryStartY;  // DAT_07ea5284
extern DWORD   InventoryStartX;  // DAT_07ea5288
extern DWORD   ItemRenderMode;  // DAT_07ea9844
// IDA Ambient_ParticleUpdate (0x00502320): el walker arranca en 0x083A2F78, pero
// cada slot empieza en `v1 - 58` (-0xE8) y el pool que le pasa a
// Particle_PathUpdate es &unk_83A2E90. O sea es la MISMA tabla que los boids,
// vista desde el ancla +0xE8. Como array propio, los campos con índice negativo
// del slot 0 caían 0xE8 bytes antes y pisaban g_PhysicsManager.
#define AmbientParticleAnchor   (FishObjectPool + 0xE8)  // DAT_083a2f78
extern float   ClothWindKey;  // _DAT_00590af0
extern DWORD   g_PhysicsManager[4];   // IDA: g_PhysicsManager (0x083A4338)
extern float   ClothWindSin;   // 3D sound sin component (updated by Sound_UpdateChannel3D_Tick)  // DAT_00590af4
extern float   ClothWindCos;   // 3D sound cos component (updated by Sound_UpdateChannel3D_Tick)  // DAT_00590af8
extern float   ClothWindZ;   // cloth wind Z  // DAT_00590afc
extern float   ClothGravityG;  // gravedad 9.8  // _DAT_00559068
extern float   ClothFixedDt;  // dt fijo 0.0025  // _DAT_0055906c
extern DWORD   TextOutMode;  // numeric TextOut mode passed to Font_CreateRenderer (not a pointer)  // DAT_055ca044
extern DWORD   ChatFindTextSpace;  // DAT_00561740
extern DWORD   ChatFindTextDbcsA1A1;  // DAT_00561744
extern DWORD   ChatFindTextPeriod;  // DAT_00561748
extern DWORD   ChatFindTextDbcsA1A4;  // DAT_0056174c
extern char    Txt_WebzenCaps[];  // "WEBZEN" -- WebZen brand text  // DAT_0056176c
extern char    Txt_WebzenCamel[];  // "WebZen" -- WebZen brand text  // DAT_0056175c
extern char    Txt_Webzen_A[];  // "Webzen" -- WebZen brand text  // DAT_00561764
extern DWORD   ChatFindTextDbcsA1AD;  // DAT_00561750
extern char    Txt_Webzen_B[];  // "Webzen" -- WebZen brand text  // DAT_00561754

// ── Chat.cpp missing globals ──────────────────────────────────────────────────
extern char    InputFieldByte2;         // input field buffer byte +2  // DAT_07db8714
extern char    InputFieldBufferByte4;         // input field buffer byte +4 (char)  // DAT_07db8716
extern DWORD   PinDataBase;         // input field buffer dword +8 (char-select PIN data base)  // DAT_07db8718
extern char    WordFilterTable[20000];  // word-filter table (1000 × 20 bytes, Filter.bmd)  // DAT_07d73104

// ── GlobalText[] localized string pool ────────────────────────────────────────
// 1000 slots × 300 bytes, los carga al arranque OpenTextData() desde
// Data/Local/Text.bmd (XOR con la clave de 3 bytes FC CF AB vía BuxConvert_0).
// Entries referenced across the codebase as strings like "Saldrás del juego
// en %d segundos." (index 380), error messages, UI labels, etc.
// GlobalText — el Text.bmd del 0.97k tiene EXACTAMENTE 1000 filas de 300 bytes
// (300000 bytes, verificado sobre el archivo).  Pero nuestro port heredo ~217
// indices DISTINTOS >= 1000 (hasta el 3305) de versiones posteriores, que con
// el array declarado [1000][300] eran LECTURAS FUERA DE RANGO: hasta ~690 KB
// pasado el final, entregadas a snprintf como si fueran cadenas.
// Se agranda el array a 4000 filas y se dejan las de arriba en cero: esas
// entradas devuelven cadena vacia y las rutinas de tooltip las saltean solas
// (todas chequean text[0]).  El loader sigue leyendo solo las 1000 reales.
#define GLOBALTEXT_ROWS_FILE   1000    // filas que trae el Text.bmd
#define GLOBALTEXT_ROWS        4000    // filas reservadas (el resto queda vacio)
extern char    GlobalText[GLOBALTEXT_ROWS][300];
void __cdecl OpenTextData(void);
int  __cdecl LoadTextData_Bin(const char *FileName);
// LegacyChatFilterWords/5dc son GlobalText[457]/[458] (name-filter blocked words, cargados
// de Text.bmd): `&LegacyChatFilterWords` = GlobalText[457].
// LegacyClassNameTable: base de la tabla de nombres de clase, recorrida con
// `&LegacyClassNameTable + i * 300`.  Es una fila de GlobalText -- sus dos lectores
// (UI_StatsPanel y SecondPassword) usan indices ~601-607, que en Text.bmd son
// los nombres de clase.
#define LegacyClassNameTable (GlobalText[0][0])  // DAT_07d29d24
#define LegacyChatFilterWords (GlobalText[457][0])  // DAT_07d4b4b0
#define GlobalText458 (GlobalText[458][0])  // DAT_07d4b5dc
extern DWORD   ChatBufferExtended;         // chat buffer extended  // DAT_07db8070
extern int     NameFilterCount;         // command table A count (Chat_DrawEntry)  // DAT_07d78074
extern int     WordFilterCount;         // command table B count (Chat_DrawEntry)  // DAT_07d78070
extern char    NameFilterTable[20000];  // name-filter table (1000 × 20 bytes, FilterName.bmd)  // DAT_07d27610
extern float   LightningSpeed;        // speed constant (Ambient_Particles lightning speed)  // _DAT_00552950

// ── Terrain map globals ───────────────────────────────────────────────────────
// La celda original 0x083A0218 pertenece a g_ObjectBucketGrid (ver arriba).
extern float   HeightNormScale;  // terrain height scale constant  // _DAT_00552b70

// ── Terrain tile pick / normal computation globals ────────────────────────────
// DAT_07feb284 (TilePickMinY..TilePickCornerDZBits são macros — ver g_TilePickBuf arriba)
extern DWORD   TilePickResultFlag;   // tile pick result flag  // DAT_07eab1fc
extern DWORD   TargetSelectMode;   // combat target select sub-mode  // DAT_07e11d44

// ── Model slot index globals ──────────────────────────────────────────────────
extern DWORD   TextureBegin;   // model slot active flag  [IDA 0x083A4104]
extern DWORD   TextureCurrent;   // model slot current index  [IDA 0x083A4108]

// ── Terrain culling globals ───────────────────────────────────────────────────
extern short   TerrainHorizonClip;  // terrain horizon/clip value  // _DAT_0838b60a
extern DWORD   TerrainViewScale;  // terrain view scale (float bits)  // _DAT_0838b614
extern DWORD   TerrainLodScale;  // terrain LOD scale (float bits)  // _DAT_0838b710
// MsToSeconds ya declarado arriba (1/1000, de ms a s, misma dirección)

// ── Model loader format strings ───────────────────────────────────────────────
extern char    Fmt_BmdName[];  // "%s.bmd" -- BMD resource name format  // DAT_0055a7f8
extern char    Fmt_BmdNamePadZero[];  // "%s0%d.bmd" -- BMD resource name format (zero-padded index)  // DAT_0055a7ec
extern char    Fmt_BmdNameNum[];  // "%s%d.bmd" -- BMD resource name format (indexed)  // DAT_0055a7e0

// ── Item/Skill/Gate data loader globals ──────────────────────────────────────
extern BYTE*   ItemAttributeShadowTable;       // Base de la tabla sombra de atributos de items.  // DAT_07cf1ff0
extern BYTE*   SkillShadowTable;       // Base de los registros sombra de skills.  // DAT_07cf1ff8
extern BYTE*   SkillTableBase;       // Base reservada para registros de skills de 0x28 bytes.  // DAT_07d29d20
extern GATE_ATTRIBUTE* GateAttribute; // IDA 0x07CF5600: base de 100 registros de nueve bytes
// ── g_DialogScript @ 0x07CF5608 — 200 entradas x 0x400 bytes ────────────────
// Layout verificado contra el disassembly de CSQuest::ShowDialogText (0x4017E0):
//   0x4017F1  shl eax, 0Ah            → stride de entrada = 0x400
//   0x4017F6  add eax, g_DialogScript → m_lpszText en +0x000
//   0x401834  cmp [ecx+0x07CF5734]    → m_iNumAnswer      en +0x12C
//   0x40184D  add eax, 0x07CF5788     → m_lpszAnswer      en +0x180, paso 0x40
// y contra los xrefs de sub_401AF0 / UI_InGameMenu para los otros dos campos:
//   0x07CF5738 = m_iLinkForAnswer   (+0x130)  — índice del diálogo siguiente
//   0x07CF5760 = m_iReturnForAnswer (+0x158)  — acción de la respuesta (1/2/3)
// Cierra exacto: 0x180 + 10*0x40 = 0x400, y 0x130 + 10*4 = 0x158 + 10*4 = 0x180.
#define DIALOG_SCRIPT_COUNT   200
struct DIALOG_SCRIPT {
    /*+0x000*/ char m_lpszText[300];
    /*+0x12C*/ int  m_iNumAnswer;
    /*+0x130*/ int  m_iLinkForAnswer[10];
    /*+0x158*/ int  m_iReturnForAnswer[10];
    /*+0x180*/ char m_lpszAnswer[10][0x40];
};
extern DWORD   DialogScriptTable[];     // base real de la tabla (DIALOG_SCRIPT_COUNT * 0x400)  // DAT_07cf5608
#define g_DialogScript  ((DIALOG_SCRIPT*)DialogScriptTable)
static_assert(sizeof(DIALOG_SCRIPT)                        == 0x400, "DIALOG_SCRIPT: stride de entrada");
static_assert(offsetof(DIALOG_SCRIPT, m_iNumAnswer)        == 0x12C, "DIALOG_SCRIPT: m_iNumAnswer");
static_assert(offsetof(DIALOG_SCRIPT, m_iLinkForAnswer)    == 0x130, "DIALOG_SCRIPT: m_iLinkForAnswer");
static_assert(offsetof(DIALOG_SCRIPT, m_iReturnForAnswer)  == 0x158, "DIALOG_SCRIPT: m_iReturnForAnswer");
static_assert(offsetof(DIALOG_SCRIPT, m_lpszAnswer)        == 0x180, "DIALOG_SCRIPT: m_lpszAnswer");
// Los cuatro DAT_ viejos son los campos de la ENTRADA 0, no globals aparte.
#define DialogAnswerCountB   (g_DialogScript[0].m_iNumAnswer)  // DAT_07cf5734
#define DialogAnswerLink0   (g_DialogScript[0].m_iLinkForAnswer[0])  // DAT_07cf5738
#define DialogAnswerReturn0   (g_DialogScript[0].m_iReturnForAnswer[0])  // DAT_07cf5760
#define DialogAnswerText0   (g_DialogScript[0].m_lpszAnswer[0][0])  // DAT_07cf5788
// extern char    LegacyNoCharNameStr;       // fallback char name string (no-char placeholder)   // -> alias a GlobalText, ver el final del archivo
extern char    Fmt_IntSpStr[];  // "%d %s" -- number + text format  // DAT_005580b0
extern int     EditMonsterNumber;       // NPC name count  [IDA 0x07D78078]
// LegacyMonsterScript / LegacyMonsterScript1: alias a MonsterScript (misma tabla 0x07CF2000).
// Ver nota en globals.cpp.  &LegacyMonsterScript = base de la tabla; LegacyMonsterScript1 =
// primer byte del campo Name de la entrada 0.
#define LegacyMonsterScript   (MonsterScript[0])  // DAT_07cf2000
#define LegacyMonsterScript1   (MonsterScript[1])  // DAT_07cf2001
extern DWORD   ItemBmdChecksumSeed;       // item/skill BMD checksum seed A  // DAT_01c5e200
extern DWORD   SkillBmdChecksumSeed;       // item/skill BMD checksum seed B  // DAT_00b43000
extern char    BuxConvertXorKeyA[3];    // XOR cipher key for BuxConvert_0 (0xFC,0xCF,0xAB)  // DAT_00559bb4
// Error message format strings
extern char    Err_FileNotExist[];  // "%s - File not exist." -- resource load error format  // DAT_00558094
extern char    Err_FileCorrupted[];  // "%s - File corrupted." -- resource load error format  // DAT_00559bd4
// File open modes
extern char    FopenModeWb[];  // "wb" (binary write mode for crt_fwrite callers)  // DAT_005597d4

// ── SecondPassword Screen2 entity list globals ────────────────────────────────
// 0x07E11E80 = Party (stride 0x24). Era un DWORD suelto.
#define LegacyParty (*(DWORD*)Party)  // DAT_07e11e80
extern DWORD   PartyStartXStorage;   // Screen2 panel origin X  // DAT_07ea5b24
extern DWORD   PartyStartYStorage;   // Screen2 panel origin Y  // DAT_07ea5b28

// ── SecondPassword UI sub-handler globals (ServerTransfer_HitTest / 004e9050 / 004eb5d0 / 004e6550) ──
extern DWORD   PinDialogOriginX;   // SecondPassword dialog origin X (pixel)  // DAT_07eaa0c8
extern DWORD   PinDialogOriginY;   // SecondPassword dialog origin Y (pixel)  // DAT_07eaa0cc
extern DWORD   MixState;   // MixState (ver el alias mas arriba). La etiqueta  // DAT_07eaa140
                               // vieja ("SecondPassword timeout") era falsa: IDA lo
                               // llama MixState y sus writers son ReceiveMix (0x4366C0),
                               // el envio del mix y ReceiveTalk.
extern "C" DWORD   PinCheckboxState;   // SecondPassword checkbox/toggle state  // DAT_07eaa131
extern DWORD   RepairEnable; // IDA: RepairEnable (0x07EAA138)  // DAT_07eaa138
extern DWORD   TradeInventoryStartX;   // SecondPassword alt-panel origin X  // DAT_07ea5290
extern DWORD   TradeInventoryStartY;   // SecondPassword alt-panel origin Y  // DAT_07ea528c
// Entradas de la tabla de strings que usa el switch de ChaosMix_HitTest:
// extern char    LegacyPinWrongMsg;   // error string for case 0 (second password wrong)   // -> alias a GlobalText, ver el final del archivo
extern char    UiMessageLabelA;   // UI message label A (UIChatLogWindow_AddText arg1)  // DAT_07eaa1a0
// extern char    LegacyAuthFailMsg;   // error string for auth-fail case   // -> alias a GlobalText, ver el final del archivo
extern char    UiMessageLabelB;   // UI message label B  // DAT_07eaa198
// extern char    LegacyPinErrorMsg;   // error string for case 0xfffffff8/0xfffffffe   // -> alias a GlobalText, ver el final del archivo
extern char    UiMessageLabelC;   // UI message label C  // DAT_07eaa19c
// ItemAttrMaxWidth / ItemAttrMaxHeight — iVar1/iVar3 defaults for Inventory_HasFreeSpace non-1/7/0xb/8 cases
extern int     ItemAttrMaxWidth;   // auth mode param A  // DAT_0055a3f8
extern int     ItemAttrMaxHeight;   // auth mode param B  // DAT_0055a3fc

// ── SecondPassword Screen5/6/7 additional globals ────────────────────────────
extern DWORD   EventEntranceHitTestMode;   // EventEntrance_HitTest mode (0=normal, 1=equip-select)  // DAT_07eaa120
extern "C" char    GuildMarkColorIdx;   // SecondPassword selected grid index (byte)  // DAT_07eaa0dc
// Buffers del editor de creacion de GUILD (no "PIN entry"): el render los recorre
// como arrays (64 bytes de mark). Ver la nota en globals.cpp.
extern char    GuildName[33];   // GuildName (0.97.40: 32 bytes + NUL)  // DAT_07ea51ec
#define NameFieldDword2 (*(DWORD*)&GuildName[4])   // 2do DWORD del nombre  // DAT_07ea51f0
extern "C" char    GuildMarkGrid[64];   // GuildMark (grilla 8x8, 1 byte por celda)  // DAT_07ea51f5
extern float  Screen5BtnXMax;   // Screen5 button X upper bound  // _DAT_00552c20
extern float  Screen5BtnHeight;   // Screen5 button height  // _DAT_00552c1c
extern float  Screen5BtnYBase;   // Screen5 button Y base  // _DAT_00552c28
// extern char    LegacyEventNoItemSlot;   // Error message: "no item in slot"   // -> alias a GlobalText, ver el final del archivo
// extern char    LegacyEventInvalidSlot;   // Error message: "invalid slot" (EventEntrance_HitTest)   // -> alias a GlobalText, ver el final del archivo
extern short   SecPassLevelCheckB;   // second-password level check B (short)  // DAT_00559f5a
extern int     LevelThreshArrayBase;   // level threshold array base (index by slot)  // DAT_00559f80
extern int     LevelThreshArrayUpper;   // level threshold array upper (index by slot)  // DAT_00559f84
// LegacyDevilSquareLimitMin / LegacyDevilSquareLimitMax SON m_iDevilSquareLimitLevel.
// Verificado con ida_get_function: m_iDevilSquareLimitLevel = 0x00559F60 y
// m_iBloodCastleLimitLevel = 0x00559F80 (32 bytes despues = 4 niveles x 2 int).
// El handler del 0x8E llena el array y `EventEntrance_HitTest` lee
// `(&LegacyDevilSquareLimitMin)[i*2]`: tienen que ser la misma memoria.
#define LegacyDevilSquareLimitMin   (m_iDevilSquareLimitLevel[0][0])  // DAT_00559f60
#define LegacyDevilSquareLimitMax   (m_iDevilSquareLimitLevel[0][1])  // DAT_00559f64
// LegacyTradeItems — declared above as DWORD
// LegacyWarehouseItems es la base del pool de warehouse (OffsetWarehouseItems).  Se
// mantiene como alias char de 1 byte para el código que hace aritmética de
// bytes sobre él, pero apunta a la memoria real del pool.
#define LegacyWarehouseItems  (*(char*)(OffsetWarehouseItems + 0))  // DAT_07ea5b30

// BMD_DrawMesh / BMD_DrawBoneSlot_Anim (BMD__RenderMesh / BMD__RenderMeshTranslate) buffers
// LightTransform pool (see globals.cpp). Backed by g_BoneLightBuf.
// BoneLightData es un lvalue char en el offset 0 del buffer; &BoneLightData + k avanza
// k bytes (aritmética de bytes) — lo exigen los accesos de BMD_DrawMesh.cpp.
extern char    g_BoneLightBuf[32 * 15000 * 12];  // per-vertex light accumulation buffer (32 x 15000 x 12)
#define BoneLightData  (*(char*)(g_BoneLightBuf + 0))  // DAT_060db65c
// Chrome UV scratch — backed by g_ChromeUVBuf. &ChromeUvData = &g_ChromeUVBuf[0]
// (float*). (&ChromeUvData)[i] = g_ChromeUVBuf[i]; +1 float = V slot (old d60).
extern float   g_ChromeUVBuf[15000 * 2];  // chrome-reflection UV buffer (15000 x 2)
#define ChromeUvData  (g_ChromeUVBuf[0])  // DAT_05828d5c
// Transformed-normal buffer — backed by g_BoneChromeNormalBuf. TransformedNormalBuf = base
// (destino de escritura de VectorRotate), LegacyBoneChromeNormalBuf = base+4 (lectura del pre-loop de chrome; pfVar8[-1]
// = normal.x en base, [0] = normal.y en base+4, [1] = normal.z en base+8).
extern char    g_BoneChromeNormalBuf[32 * 15000 * 12];  // chrome-effect normal buffer (32 x 15000 x 12)
#define TransformedNormalBuf  (*(char*)(g_BoneChromeNormalBuf + 0))  // DAT_06f433bc
#define LegacyBoneChromeNormalBuf  (*(float*)(g_BoneChromeNormalBuf + 4))  // DAT_06f433c0

// _DAT_ float constants missing from earlier declarations
extern float   AlphaChannelThreshold;  // alpha threshold (alpha < this → use alpha channel path)  // _DAT_00552544
extern float   ChromeUScale;  // chrome U scale factor  // _DAT_005528c0
extern float   VertexWaveFreq;  // sin period scale for vertex deformation (BMD__RenderMeshTranslate)  // _DAT_005528c4
extern float   VertexWaveAmplitude;  // sin amplitude for vertex deformation (BMD__RenderMeshTranslate)  // _DAT_00552644

// ── MoveEffect constants (0x005524a8–0x00552a20 range) ─────────
extern float   MoveEffectConst27;  // move effect float constant  // _DAT_005524a8
extern float   AngleDeg270;  // move effect float constant  // _DAT_00552864
extern float   TwoPiOver3;  // move effect float constant  // _DAT_005529a0
extern double  DoubleConst12p5;  // 12.5   (double de 8 bytes en el binario)  // _DAT_005529d8
extern double  DoubleInv180;  // 1/180  (double de 8 bytes en el binario)  // _DAT_005529e8
extern double  DoublePi;  // PI     (double de 8 bytes en el binario)  // _DAT_005529f0
extern float   MoveJointTrigScale1;  // MoveJoint trig scale (mode 1)  // _DAT_00552938
extern float   MoveJointAngleScale;  // MoveJoint particle angle scale  // _DAT_00552a54
extern float   MoveJointFadeA;  // MoveJoint color fade rate A  // _DAT_00552a8c
extern float   MoveJointFadeB;  // MoveJoint color fade rate B  // _DAT_00552a90
extern float   MoveJointTrigFreqA;  // MoveJoint trig freq A  // _DAT_00552a94
extern float   MoveJointTrigScaleB;  // MoveJoint trig scale B  // _DAT_00552a98
extern float   MoveJointHpScale;  // MoveJoint HP-bar scale factor  // _DAT_00552aa4
extern float   RingOrbitScaleX;  // Ring_ComputeOrbit ring trig scale X  // _DAT_00552a9c
extern float   RingOrbitScaleY;  // Ring_ComputeOrbit ring trig scale Y  // _DAT_00552aa0
extern float   RingOrbitScaleZ;  // Ring_ComputeOrbit ring trig scale Z  // _DAT_00552aa8
extern char    ParticleEffectIdTable[8];  // DAT_00559b78
#define MoveEffectBufOff7 (ParticleEffectIdTable + 7)   // alias: mismo buffer, offset 7  // DAT_00559b7f

// ── Map_LoadObjectModels (0x0050c4d0) ─────────────────────────────────────────
extern FILE*   ParserFileHandle;       // DAT_083A40FC — file handle for custom-map object list
extern char    ParserTokenString[256]; // DAT_083A3FF4 — ParseNextToken output buffer
extern int     ParserCurrentToken;     // DAT_083A40F4 — last token type code
extern float   ParserTokenNumber;      // DAT_083A40F8 — last numeric value
extern char    LorenciaModelsLoadedFlag;          // flag: Lorencia object models already loaded  // DAT_083a4100
extern char    IcarusWaterTileNameTable[32 * 0x38];  // Icarus water-tile name table (32 × 0x38)  // DAT_083a8ad8
// DAT_0055e714 MapObjPathFmt — format string for custom map object file path (zone index)
// FopenModeRb — declared above (file open mode char)
// InGameModeFlag, g_tex_ext_mode, World, ModelsBase — declared above

// ── BMD_SkinUpdate (0x0040b630) ──────────────────────────────────────────────
extern int     SkinnedVertexCount;          // skinned vertex count  // DAT_00590c10
extern float   BoneWorldVtxBase;          // bone output world-vertex array base   (stride 0x1b floats)  // DAT_055c4068
extern float   SrcVertexPosTable;          // source vertex position array          (stride 3 floats)  // DAT_00593008
extern short   ParentBoneIdxTable;          // parent-bone index array               (stride 0x11 shorts)  // DAT_00590c32
extern float   GlobalMeshScale;         // global mesh scale factor  // _DAT_0055259c
extern float   BoneWorldMatBase;          // bone world-position matrix array base (stride 0x6c bytes)  // DAT_055c4038
extern float   BoneWorldPosX;          // bone world-pos X component            (stride 0x1b)  // DAT_055c4098
extern float   BoneWorldPosY;          // bone world-pos Y component  // DAT_055c409c
extern float   BoneWorldPosZ;          // bone world-pos Z component  // DAT_055c40a0
extern int     TriangleCount;          // triangle count  // DAT_00593968
extern BYTE    TriangleMatNameTable;          // material name array base (stride 0x20, parallel to triangle data)  // DAT_0059396a
extern float   TriangleVertDataTable;          // triangle vertex data (9 floats/vert: bone,x,y,z,nx,ny,nz,u,v)  // DAT_00608c70
extern short   UvIndexCount;          // UV index count per mesh group  // DAT_0080a428
extern short   NormalIndexCount;          // normal index count per mesh group  // DAT_0080a42a
extern short   VertexIndexCount;          // vertex index count per mesh group  // DAT_0080a42c
extern short   MaterialGroupCount;          // material group count (build loop accumulator)  // DAT_008097a0
extern BYTE    MaterialNameTable;          // material name table base (stride 0x20 bytes per entry)  // DAT_008097a2
extern float   PackedVertexBuffer;          // packed vertex buffer (stride 0x31be9 floats per mesh group)  // DAT_0080a424
extern short   VertexIndexTable;          // vertex index table   (stride 0x637d2 per mesh group)  // DAT_00879588
extern short   UvIndexTable;          // UV index table       (same stride)  // DAT_00896a48
extern short   UvLinkedVertTable;          // UV name/linked-vert table  // DAT_00831530
extern float   UvCoordDataTable;          // UV coordinate data   (stride 0x31be9)  // DAT_00831534
extern float   UvExtraZChannel;          // UV extra/Z channel  // DAT_0083153c
extern short   UvLinkedVertIndex;          // UV linked vertex index table  // DAT_00831540
extern float   NormalDataTable;          // normal data          (stride 0x31be9)  // DAT_00862270
extern float   NormalYChannel;          // normal Y channel  // DAT_00862274
extern short   NormalIndexTable;          // normal index table   (stride 0x637d2)  // DAT_008b3f08

// ── RenderObjectScreen (0x004e13a0) ──────────────────────────────────────────
// Estos globals guardan la pose de render por frame del item/objeto que se está mostrando.
extern float   ObjectSelect_Angle;         // item render: rotation X offset  [IDA 0x07EA952C]
extern float   ItemRenderRotY;         // item render: rotation Y  // _DAT_07ea9530
extern float   ItemRenderRotZ;         // item render: rotation Z  // _DAT_07ea9534
extern short   ObjectSelect_Type;         // item render: resolved model type index  [IDA 0x07EA9512]
extern float   ObjectSelect_HeadAngle;         // item render: HeadAngle[0] (used by BMD_Animation)  [IDA 0x07EA9538]
extern int     ObjectSelect_AnimationFrame;         // item render: state flag A  [IDA 0x07EA9618]
extern int     ObjectSelect_PriorAnimationFrame;         // item render: state flag B  [IDA 0x07EA961C]
extern short   ObjectSelect_PriorAction;          // item render: state flag C  [IDA 0x07EA9616]

// ── Filter / name-filter system globals ──────────────────────────────────────
extern DWORD   WordFilterChecksumSeed;    // word-filter BMD checksum seed  // DAT_007cfa00
extern DWORD   NameFilterChecksumSeed;    // name-filter BMD checksum seed  // DAT_00578200
extern char    ExitErrorMsgBuf[];  // fatal-error message buffer shown by ExitProgram/ShowError (256 B)  // DAT_07d2aa08

// ── HeroTile — terrain tile type under the hero character ─────────────────────
// WorldMapType ya declarado como DWORD cerca de la línea 546 (tipo de mundo/mapa)
#define HeroTile ((int)WorldMapType)
// World ya está declarado arriba.
#ifndef World

// ── Pools de items y atributos de terreno ────────────────────────────────────
// Centralizadas acá (un único `extern` por global) para que mover funciones entre
// módulos no rompa la visibilidad de las globals.
//
// Los cuatro pools de items son grids de slots ITEM (stride 0x44). Ojo: su
// indice de celda es `slot - 12`; los 12 wear slots NO viven aca sino en
// `CharacterMachine + 536 + 68*slot`.
extern "C" BYTE  OffsetInventoryItems[];  // legacy inventory grid base (stride 0x44)
extern "C" BYTE  OffsetTradeItems[];  // legacy trade window grid base (stride 0x44)
extern "C" BYTE  OffsetWarehouseItems[];  // legacy warehouse grid base (stride 0x44)
extern "C" BYTE  OffsetMixItems[];  // legacy chaos-mix grid base (stride 0x44)
// TerrainWall = &TerrainWallFlags (256x256 flags de tile: bit0 safezone, bit1
// ocupado por entidad, bit3 no-caminable).
extern unsigned char* TerrainWall;  // per-tile wall/attribute flags, 256x256 (NOMOVE/NOGROUND bits)

#endif

// ── SkillAttribute @ 0x07D29D20 — 64 entradas × 40 bytes ─────────────────────
// WinMain reserva 0xA00 (= 2560) bytes y OpenSkillScript / el anti-tamper copian
// de a 0x28 (40) bytes por entrada, así que la stride es 40 — no 300.  Los
// consumidores indexan por BYTE: `&SkillAttribute[8 * (5*Level + 150)]`
// (= entrada 30+Level), con el nombre en el offset 0 de la entrada.
struct _SkillAttrEntry { char Raw[2560]; };
extern _SkillAttrEntry SkillAttribute;  // base of table @ 0x07D29D20

// ── CharactersClient extra BMD heap buffer ────────────────────────────────────
extern LPVOID  CharExtraBmdBuffer;   // extra BMD data heap block for character models  // DAT_07abf164

// ── FloatingLabel pool ────────────────────────────────────────────────────────
#define LegacyPointerPoolBase (g_PointerPool[0]) // IDA: base Pointers (0x07C82CD0).  // DAT_07c82cd0

// ── Water wave simulation buffers ─────────────────────────────────────────────
extern char    WaterWaveBufferA;   // water wave buffer A (256×256 ints, 2 banks × 0x40000 bytes)  // DAT_0814b6e0
extern char    GrassWindWaveBuffer[0x80000];   // grass-wind/water-wave double buffer (2×0x40000, sub_4F98C0/4F9A30)  // DAT_0814b2e0

// ── Small-function batch globals ─────────────────────────────────────────────
// Base de la tabla de quests (el objeto CSQuest al que apunta g_csQuest) — IDA la
// trata como un buffer de ~0x1C900 bytes y los accesos llegan hasta cerca del final
// (p.ej. el flag del panel de quest en +0x1C87F que lee GetScreenWidth). Por eso es
// un array de BYTE del tamaño real y no un DWORD.
extern BYTE    QuestTableBase[0x1C900];   // Quest table base (was DWORD)  // DAT_00567500
extern DWORD   SoundDeviceCtx;       // Sound device context  // DAT_00590b00
extern DWORD   ChatListBoxNextId;       // RefCount / tick counter  // DAT_055c9b78

extern int     Vtbl_ObjectBase;  // base-object vtable storage (set by Obj_SetVtable00552460)  // PTR_LAB_00552460
extern int     ClothNodeVtable;  // cloth-node vtable = {0x00408680}  // PTR_LAB_005524e8
extern int     Vtbl_StreamBase;  // stream-base vtable (StreamBase_SetVtableA/B)  // PTR_LAB_005527e4
extern int     Vtbl_Quest;  // quest system vtable (Quest_SetVtable)  // PTR_LAB_005524b8
extern int     Vtbl_CWaveFile;  // CWaveFile vtable  // PTR_FUN_005524c0
extern int     Vtbl_CErrorReport;  // CErrorReport vtable  // PTR_FUN_005524c4
extern int     Vtbl_HashTable;  // hash-table class vtable (intermediate + final)  // PTR_FUN_005524d8
extern int     Vtbl_ChatListBox;  // ChatListBox engine-object vtable (legacy IDA label: StreamBase2)  // PTR_LAB_00552810
extern int     Vtbl_StreamDeinit;  // stream deinit vtable  // PTR_FUN_005527e0
extern int     Vtbl_GameGuard;  // GameGuard class vtable  // PTR_FUN_0055389c

// Símbolos de vtable/literal-puntero off_XXXX de IDA, renombrados por port.py a DAT_xxxxxxxx.
// (en paralelo a los PTR_LAB_/PTR_FUN_ de arriba; port.py usa la forma DAT_)
extern DWORD   ClothBodyVTable;  // DAT_00552508

extern DWORD   _PartyNumber;       // _PartyNumber  // DAT_07e11e50
extern DWORD   _PartyKey;       // _PartyKey  // DAT_07e11e54

extern HANDLE  hEvent;                  // GameGuard sync event
extern LPVOID  lpParameter;             // IDA: lpParameter (0x083BBAE8) — contexto de
                                        // GameGuard (0x34C bytes) que aloca el pre-init;
                                        // != 0 significa "ya inicializado".
extern char    g_GameGuardGameName[];   // IDA: aMu (0x0055910C) — el nombre de juego que
                                        // recibe PreInitNPGameMon: "Mu". NO es un nombre de
                                        // ventana: el cliente no crea ninguna ventana de
                                        // GameGuard.

extern uintptr_t g_csQuest;      // Dirección del objeto CSQuest (0 = inactivo).  // DAT_00583d8c

// Match/DevilSquare info
extern BYTE    m_byMatchType;  // current event match type (Blood Castle etc.)
extern int     m_iMatchTimeMax;  // event match time limit
extern int     m_iMatchTime;      // 0x00559CCC (writers: SetMatchInfo / clearMatchInfo)
extern int     m_iMaxKillMonster;  // event kill limit (max kills)
extern int     m_iKillMonster;  // current event kill count

// Sound system (DirectSound8 pipeline — companion-project layout, 420 slots)
extern BOOL    g_EnableSound;                         // g_EnableSound — master on/off  // DAT_00590ac8
extern bool    g_Enable3DSound;                       // LegacyEnable3DSound — 3D audio enabled
extern LPDIRECTSOUND          g_lpDS;                 // LegacyLpDS — main device
extern LPDIRECTSOUND3DLISTENER g_lpDS3DListener;      // primary-buffer listener
extern void*   wavefile;                              // LegacyWavefile — CWaveFile* currently being loaded
extern DWORD   g_dwBufferBytes;                       // LegacyGdwBufferBytes — last WAV data chunk size

// Tablas por slot (stride 4, 420 entradas cada una — contiguas en la sección .data original):
extern int     MaxBufferChannel[420];                 // 0x0058E1C4 — # active channels (>0 means "loaded")
extern int     BufferChannel[420];                    // rotating channel index (0..MaxBufferChannel-1)
extern bool    Enable3DSound[420];                    // 0x00590924 — per-slot 3D flag
extern int     SoundLoadCount;                        // 0x00590ACC — running count of loaded sounds
extern char    BufferName[420][0x40];                 // per-slot filename (64 bytes)

extern LPDIRECTSOUNDBUFFER   g_lpDSBuffer  [420][4];  // 0x0058C780 — secondary buffers (up to 4 instances per slot)
extern LPDIRECTSOUND3DBUFFER g_lpDS3DBuffer[420][4];  // 0x0058EEE4 — parallel 3D-buffer array
extern const OBJECT* Object3DSound[420][4];          // Entidad ligada a cada canal de audio 3D.

// Music

// Entity data
extern BYTE    MonsterScript[512 * 0x36];  // NPC/mob name table (stride 0x36; Type[0], Name[1..32])
extern char    WhisperRegistID[10][41]; // IDA: WhisperRegistID (0x07DB9310) — anillo de 10 nombres (0.97.40: 40 bytes + NUL)
extern int     WhisperID_Num;           // IDA: WhisperID_Num (0x07E11DB0) — proxima fila del anillo

// Blur/trail system
// LegacyRenderPoolOff16 — campo int en el +16 del slot[0] de g_RenderPool (ver arriba).
#define LegacyRenderPoolOff16  (*(int*)(&g_RenderPool[16]))  // DAT_07c608b8

// Objetos mariposa/bicho — alias de ButterflyObjectPool (misma dirección 0x083a1218 en el original)
#define Butterfles  (*(DWORD*)ButterflyObjectPool)

// ── Batch 3 globals ──────────────────────────────────────────────────────────
// BST / std::map sentinel
extern void   *RbTreeSentinel;       // RB-tree sentinel node (NIL)  // DAT_055c9b98

// Camera
extern float   CameraAngle[3];     // DAT_083a42e0 (3 floats)
extern float   CameraPosition[3];  // CameraPositionXBits (3 floats)

// Quest/NPC window
// g_bEventChipDialogEnable es 0x07EAA128 (GoldenArcherOpenType): alias, para que
// el 0x94 (writer) y el panel del Golden Archer (reader) vean el mismo global.
#define _g_bEventChipDialogEnable (*(int*)&GoldenArcherOpenType)

// Los flags de los paneles de UI aliasan los bytes reales DAT_07eaa11x (per el
// Offsets.h del proyecto companion de IDA). Los flags reales están en
// CharacterOpened..11c (de un byte), no en 0x07e5ba84/88 (misidentificación de la
// época de Ghidra). Usamos #define para que tanto el código de toggle portado de
// IDA (escribe InventoryOpened) como los gates de render del HUD (leen
// `if (InventoryOpened)`) peguen en el mismo byte de memoria.

// Inventory/Trade panel origin coords. Mismo patrón de unificación que los flags
// *Opened de arriba — IDA escribe/lee vía InventoryStartY..5290 y los pases de render
// del HUD usan los nombres de C++: tienen que ser la misma memoria.

// SMD parser
extern FILE   *SMDFile;             // DAT_0055c0a0

// Codec / JPEG internal refs (function pointers stored as addresses)
// lpDefault_00583d88 already declared above (line ~1036) as DWORD
extern DWORD   CodecStateA;       // codec state A  // DAT_00563e00
// MapLoadReadDesc already declared above (line ~1616) as DWORD

// Missing globals (compilation fixes)
// IDA: RenderTerrainAlphaBitmaps usa g_PointerPool +0x24 y 100 slots.
extern DWORD   JpegNaturalOrderTable;       // JPEG natural order table  // DAT_0055339c
// m_dwTextColor / m_dwBackColor SON m_dwTextColor / SetBackgroundTextColor (mismo global en IDA:
// 0x559c78 / 0x559c80, verificado por disasm de sub_40D610 y sub_480980). El render de
// texto (CUIRenderText_RenderText) lee m_dwTextColor; sin esta unificación los seteos de m_dwTextColor
// (HUD passes + ChatListBox) se perdían → texto blanco. m_dwTextColor/80 declarados arriba.
#define m_dwBackColor  SetBackgroundTextColor
// g_iNumAnswer — alias de DialogAnswerCountA (ver arriba).
// g_iNumLineMessageBoxCustom — alias de MessageBoxLineCount (ver arriba).
// g_lpszMessageBoxCustom — alias de MessageBoxCustomBuf (el buffer real de 7 lineas x
// 0x26): los writers (ShowDialogText, CreateOkMessageBox) llenan MessageBoxCustomBuf y
// los readers (sub_402FF0) leen por este nombre.
#define g_lpszMessageBoxCustom  ((char (*)[0x26])MessageBoxCustomBuf)
// g_iCurrentDialogScript — alias de g_iCurrentDialogScriptStorage (ver arriba).
// g_lpszDialogAnswer — alias de DialogAnswerBuffer (ver arriba).
// m_hFontDC (IDA 0x055C9FEC) y los bits del DIB del texto (0x055C9E4C) viven en
// CFont: gFont.GetTextDC() / gFont.GetTextBits().
// Las tres fuentes (IDA g_hFont 0x055CA00C, g_hFontBold 0x055CA010, g_hFontBig
// 0x055CA014) viven en CFont: gFont.GetFont(FONT_NORMAL / FONT_BOLD / FONT_BIG).

// Batch 18 — InitGame / ReceiveChat globals
// EnableUse declarado arriba junto a EnableUse.
extern int     SendGetItem; // IDA: SendGetItem (0x07E11998)  // DAT_07e11998
// MoveDebounceCounter already declared above (line ~554) as DWORD
extern int     LegacySoccerTime;       // SoccerTime  // DAT_07e11e10
extern int     LegacySoccerObserver;       // SoccerObserver  // DAT_07e11e14
extern int     LegacySelectedNpc;       // SelectedNpc  // DAT_07e11994
extern int     LegacySelectedOperate;       // SelectedOperate  // DAT_07e11990
// DownArrowDebounce already declared above (line ~1003) as DWORD
extern int     LegacySelectedItem;       // SelectedItem  // DAT_07e11988
// UpArrowDebounce already declared above (line ~1002) as DWORD
// m_bAutoAttack vive en 0x00559C5C (m_bAutoAttack, IDA InitGame L39).  El port
// tenia ademas AutoAttackFlag: el toggle del menu y el hover escribian uno y el
// combate / F3/30 leian el otro.  Ahora es un alias.
#define AutoAttackFlag   m_bAutoAttack  // DAT_07e11e18
extern int     MenuF1ItemType;       // _CheckInventory  // DAT_07e11d24
// InputCooldownTicks already declared above (line ~553) as DWORD
extern BYTE    UiAlphaByte;       // UI alpha/state byte  // DAT_00559c6d
// g_shEventChipCount es 0x07EAA12C (GoldenArcherItemCount); mismo caso que el de arriba.
#define EventChipCount (*(short*)&GoldenArcherItemCount)  // DAT_07e11e1c
extern short   MutoNumber0;       // g_shMutoNumber[0]  // DAT_07e11e20
extern short   MutoNumber1;       // g_shMutoNumber[1]  // DAT_07e11e22
extern short   MutoNumber2;       // g_shMutoNumber[2]  // DAT_07e11e24
extern int     LegacySceneFlag;       // SceneFlag  // DAT_07e11980

// Batch 19 — SendCheck globals
extern DWORD   LegacyGameServerConnected;       // g_bGameServerConnected  // DAT_07e11d10
extern BYTE    g_byPacketSerialSend; // IDA: g_byPacketSerialSend (0x07DB8600)
extern BYTE    SendCheckFirstCallFlag;       // SendCheck first-call flag  // DAT_05826cfc
extern DWORD   SendCheckFirstCallTick;       // SendCheck first-call tickCount  // DAT_05826d00

// Batch 20 — OpenNpc, MoveCamera, RenderEquipment3D, RenderItems3D, Send_ActionRequest, LookAtTarget
// Sólo los globals realmente nuevos (los que no estaban declarados arriba):
extern float   EquipSlotRingX;      // equip pendant X offset  // _DAT_005524a0
extern float   EquipSlotYTop;      // equip slot Y offset (helm row)  // _DAT_00552c18
extern float   EquipSlotXBody;      // equip slot X offset (pants/gloves/boots col)  // _DAT_00552c10
extern float   EquipSlotYMid;      // equip slot Y offset (mid row)  // _DAT_00552c0c
extern float   EquipSlotYBottom;      // equip slot Y offset (ring/boots row)  // _DAT_00552c08
extern float   EquipSlotXRight;      // equip slot X offset (weapon_L/ring_R col)  // _DAT_00552c04
extern float   ArrowCountOffsetX;      // arrow count X offset  // _DAT_005527dc
extern float   CurrentCameraWalkDelta[];     // CurrentCameraWalkDelta[6] (pos delta + angle delta)  // DAT_083a7adc
extern float   CameraLerpSpeed;      // lerp speed (camera smoothing)  // _DAT_00552a30
extern void*   g_LoginSceneObjects[9]; // exact CreateObject pointers for login bg: sky, 3 ships, 3 waves, banner, sun

// Batch 22 — AttackStage, CreateArrow
extern int     g_iLimitAttackTime;       // g_iLimitAttackTime (attack frame limit counter)  [IDA 0x00559858]
extern DWORD   CurrentSkill; // IDA: CurrentSkill (0x05826D10)
extern float   SwordTrailRadius;      // sin/cos offset multiplier (sword trail radius)  // _DAT_00552904
extern float   SlashProjRadius;      // sin/cos offset multiplier (slash projectile)  // _DAT_005528f8
extern float   ComboAnimOffset;      // combo animation offset constant  // _DAT_005528f4

// Batch 21 — RenderSkillIcon, CheckMixRecipe, MoveObjects, CreateArrows, CollisionDetectLineToMesh
extern DWORD   VisibleObjectCount;       // visible object counter (MoveObjects)  // DAT_083a3fec
extern DWORD   MixSocketFlag;       // mix recipe: socket flag byte  // DAT_07eaa178
extern DWORD   WingMixFlagBits;       // mix recipe: wing socket option value  // DAT_07eaa170
extern int     MixValueScore;       // mix recipe: combined value score (capped at 0x5a)  // DAT_07eaa174
extern char    LastHotkeyChar;       // last rendered hotkey char  // DAT_00559c6c
extern float   ArrowAngleOffsetSmall;      // arrow angle offset (5-way shot small)  // _DAT_00552ab0
extern float   ArrowAngleOffsetLarge;      // arrow angle offset (5-way shot large)  // _DAT_00552584
extern float   SkillHotkeyNumY;      // RenderNumber2D Y offset  // _DAT_00552a4c

// ── UseSkillWizard dependencies ──────────────────────────────────────────────
extern DWORD   MovementSkillTarget;       // MovementSkillTarget (index into CharactersClient)  [IDA 0x07D780A0]
extern DWORD   SelectedSkillSlot;       // skill slot index (current selected skill slot)  // DAT_07d7809c
extern char    GmNameCheck1[8];    // GM name string "webzen" (anti-impersonation check 1)  // DAT_00559d94
extern char    GmNameCheck2[8];    // GM name string "webzen" (anti-impersonation check 2)  // DAT_00559d9c

// ── SkillElf dependencies ────────────────────────────────────────────────────
extern char    GmNameCheckString;       // GM name check string (part of "webzen" pattern)  // DAT_00559db4
extern char    ChatLogWidgetId;       // chat log widget ID string (for AddText)  // DAT_07e11dfc
// extern char    NotEnoughManaText;       // "Not enough mana" message string   // -> alias a GlobalText, ver el final del archivo

// MoveParticles drift vectors, independent of EarthQuake (0x083A0210).
// IDA 0x47709A..0x477193 indexes the first two components of each vector.
extern float g_ParticleDriftPosition[3]; // 0x07C800F8..0x07C80103
extern float g_ParticleDriftVelocity[2]; // 0x07C80104..0x07C8010B
#define LegacyParticleDriftPosX (g_ParticleDriftPosition[0])  // DAT_07c800f8
#define LegacyParticleDriftPosY (g_ParticleDriftPosition[1])  // DAT_07c800fc
#define ParticleDriftPositionZ (g_ParticleDriftPosition[2])  // DAT_07c80100
#define LegacyParticleDriftVelX (g_ParticleDriftVelocity[0])  // DAT_07c80104
#define LegacyParticleDriftVelY (g_ParticleDriftVelocity[1])  // DAT_07c80108

// ── MoveParticles float constants ────────────────────────────────────────────
extern float  ParticleScaleDeltaA;       // particle scale delta (type 0x565 sub!=0)  // _DAT_00552ac4
extern float  ParticleScaleDeltaB;       // particle scale delta (type 0x565 sub==0)  // _DAT_00552ac8
// AnimSpeedA — already declared above (line ~958)
extern float  ParticleWaveFreqA;       // sin frequency (type 0x599)  // _DAT_00552ad0
extern float  ParticleWaveFreqB;       // sin frequency (type 0x4cf)  // _DAT_00552ad8
// AspectRatioScale — already declared above (line ~151)
extern float  ParticleWaveFreqC;       // sin frequency (type 0x4ce/0x4d0/0x4d2)  // _DAT_00552ae0
extern float  ParticleColorDecayA;       // color decay (type 0x4e1 sub 4)  // _DAT_00552ae8
extern float  ParticleGreenFactorA;       // green channel factor (type 0x4c4 sub 9)  // _DAT_00552aec
extern float  ParticleRedFactorA;       // red channel factor (type 0x4c4 sub 9)  // _DAT_00552af0
extern float  ParticleDampA;       // velocity damping (type 0x4c4 sub 8)  // _DAT_00552af4
extern float  ParticleLightA;       // lifetime-to-light factor (type 0x4c4 sub 8)  // _DAT_00552af8
extern float  ParticleBlueFactorB;       // blue channel factor (type 0x4c4 sub 7)  // _DAT_00552afc
extern float  ParticleGreenFactorB;       // green channel factor (type 0x4c4 sub 7)  // _DAT_00552b00
extern float  ParticleRedFactorB;       // red channel factor (type 0x4c4 sub 7)  // _DAT_00552b04
extern float  ParticleWaveFreqD;       // sin frequency (type 0x4c4 sub 6)  // _DAT_00552b08
extern float  ParticleWaveFreqE;       // sin frequency (type 0x4f4/0x4c4 sub 6)  // _DAT_00552b0c
extern float  ParticleBlueFactorC;       // blue tint factor (type 0x4c4 sub 4)  // _DAT_00552b10
extern float  ParticleGreenFactorC;       // green tint factor (type 0x4c4 sub 4)  // _DAT_00552b14
extern float  ParticleRedFactorC;       // red tint factor (type 0x4c4 sub 4)  // _DAT_00552b18
extern float  ParticleLightB;       // lifetime-to-light factor (type 0x4c4 sub 0/3/4/7)  // _DAT_00552b1c
extern float  ParticleVelYAdd;       // velocity Y adjust (type 0x4b0 sub 1/5)  // _DAT_00552b20
extern float  ParticleDampB;       // velocity damping (type 0x4ac)  // _DAT_00552b24
extern float  ParticleLightC;       // lifetime-to-light factor (type 0x4a6/0x4d2/0x498)  // _DAT_00552b28
extern float  ParticleDampC;       // velocity damping (type 0x4ab/0x47e)  // _DAT_00552b2c
extern float  ParticleDriftScale;       // random offset scale (type 0x47e)  // _DAT_00552b30
extern float  ParticleColorDecayB;       // color decay (type 0x47e sub 4)  // _DAT_00552b34
extern float  ParticleScaleDecay;       // scale decay factor (type 0x47e sub 4)  // _DAT_00552b38
extern float  ParticleGravScale;       // gravity random scale (type 0x47e sub 4)  // _DAT_00552b3c
extern float  ParticleDampD;       // velocity damping (type 0x47e sub 3)  // _DAT_00552b40
extern float  ParticleDampGeneral;       // velocity damping general (multiple types)  // _DAT_00552b44
extern float  ShakeClampMin;       // camera shake clamp lower  // _DAT_00552b48
extern float  ShakeClampMax;       // camera shake clamp upper  // _DAT_00552b4c
extern float  ShakeBounce;       // camera shake bounce / terrain bounce factor  // _DAT_00552b50
extern float  ShakeRandomScale;       // camera shake random scale  // _DAT_00552b54

// ── Additional math/render constants (added for CreateAngle/Terrain_RenderTexturedObject) ──
extern float  RadToDeg;       // 57.29578f = 180/pi (radians to degrees)  // _DAT_00552860
extern float  TerrainTileSize;       // 64.0f (terrain tile size)  // _DAT_00552cb4
// RadToDegBits (DAT_00552860) y TerrainTileSizeBits (DAT_00552cb4) eran twins Bits
// sin usos: eliminados en la pasada de muertos.

// ── CheckArrow / CharSelect chat message string globals ──
extern char    ArrowsRequiredMsg;        // arrow-required error string ID  // DAT_07e11df4
extern char    BoltsRequiredMsg;        // bolt-required error string ID  // DAT_07e11df8
extern char    EmptyChatMessage;        // empty chat message string  // DAT_07d3c348

// ── Skill selection globals ──
extern char    SkillModeFlag;        // skill mode flag (0=direct, nonzero=from attribute)  // DAT_07d78098
extern DWORD   InputEnable;          // IDA: InputEnable (0x00559C84)  // DAT_00559c84

// ── CharSelect packet globals ──
extern DWORD   SelectedHero;         // IDA: SelectedHero (0x005616AC)  // DAT_005616ac

// ── Chat bubble pool ──
// Pool base = 0x07E016F8; stride 0x254 per entry; ~96 slots.
// Per-slot layout:
//   +0x000 char[24]  ID (sender name)
//   +0x018 char[12]  guild mark string  ("[GuildName]")
//   +0x024 char      color index
//   +0x025 char      guild team byte
//   +0x028 int       timer1 (tick counter for line 1)
//   +0x02c char[256] text line 1
//   +0x12c char[256] text line 2
//   +0x22c int       display timer 1
//   +0x230 int       display timer 2
//   +0x234 DWORD     owner CHARACTER* pointer
extern char    ChatBubblePool[100 * (0x254 + sizeof(void*) - 4)];   // pool de burbujas: 100 CHAT (596 B en x86)  // DAT_07e016f8
// unk_7E01720 = base + 40 (campo timer1). MoveChat arranca su walk ahi.
#define LegacyChatBubblePool  (&ChatBubblePool[40])  // DAT_07e01720
extern char    GuildMarkPrefixChar;        // guild mark prefix string (e.g. "[")  // DAT_00559d60
extern short   GuildMarkSuffixChar;        // guild mark suffix 2-byte (e.g. "]")  // DAT_00559d64
extern char    GuildMarkSuffixTrailChar;        // guild mark suffix trailing byte  // DAT_00559d66

// ── HUD render globals (Phase-2 port) ────────────────────────────────────────
// Para el port de RenderPartyHP / RenderMainFrameWindow / RenderBooleans /
// Render_HotbarItems3D from IDA.
//
// Estado de party / soccer / guild war — binding mínimo para que los renderers
// sigan funcionando antes de que se porten los sistemas completos de party/guild.
// Los valores por defecto en 0 hacen que todos esos caminos tomen sus ramas de
// salida temprana, así que el juego corre igual con los ports incompletos.
extern int     PartyNumber;          // 0x07EAA0E0 — count of valid party slots  // DAT_07eaa0e0
extern int     PartyKey;             // IDA: PartyKey — reiniciado por InitPartyList
extern BYTE    Party[2048];          // 0x07E11E80 — Party struct array (stride 36, header 24)
extern int     EnableGuildWar;       // GW active flag
extern int     EnableSoccer;         // DAT_05826d2c — modo fútbol de guerra de guild
extern int     GuildWarIndex;        // DAT_05826d34 — fila de tabla de marks de guild enemiga
extern int     HeroSoccerTeam;       // 0/1 — hero's soccer team index
extern int     GuildWarScore[2];     // [self, enemy]
extern char    GuildWarName[80];     // enemy guild name
extern char    SoccerTeamName[2][80];// team names

// Float-text floating-numbers entry pool (RenderBooleans iterator).
// Layout per entry (596 bytes = 149 dwords):
//   +0x000 (DWORD)  tipo/flag (se lee como `*((int*)v0 - 2)` y `*((int*)v0 - 131)`)
//   +0x008 (DWORD)  owner ptr (entity*)
//   +0x010 (3*float) world position fallback
//   +0x224 (DWORD)  extent de texto cx (lo setea sub_480C60)
//   +0x228 (DWORD)  text-extent cy
//   +0x22C..       text strings: at +44, +300, +24, +base
//   +0x22C (int)   countdown-1
//   +0x230 (int)   countdown-2
// Pool spans 0x07E01924..0x07E101F4 (~60 KB, ~100 entries).
#define LegacyBubblePoolOff22c  (&ChatBubblePool[0x22C])   // = base + disp1 (mismo pool)  // DAT_07e01924

// Globals de fuente / medición de texto que consumen sub_480C60 y el HUD
// renderers (HFONT object handles + DC + computed dimensions).
// FontHeight (0x07D78080): gFont.GetHeight().
extern SIZE    TextSize;             // shared scratch SIZE for text extent

// Anti-tamper hash-table state — already declared above as g_HashTableCtx[4]
// con las macros MAIN_HASH_CLASS/cc/d0/d4. Sólo referenciarlos; NO volver a hacerles extern.

// CharacterMachine — the encrypted player attribute buffer (size 0x584).
// Los desencripta in situ el bloque anti-tamper interno de RenderMainFrameWindow.
extern void   *CharacterMachine;     // pointer to encrypted CHARACTER struct
// CharacterAttribute aliases CharacterAttributePtr (already declared at line ~569).
#define CharacterAttribute  CharacterAttributePtr

// HudTextVisibleFlag (flag de UI sucia) ya está declarado en la línea 620 como `char`.


// ─── Filas de GlobalText que el port habia partido en globals sueltos ────────
// El pool de textos vive en `GlobalText[1000][300]` con base
// 0x07D29D24 (verificado: 0x07D4B4B0 == GlobalText[457]).  Estos simbolos caen
// EXACTAMENTE en multiplos de 300 desde esa base, o sea son filas del pool, no
// buffers propios.
//
// Mismo patron que PrimaryTerrainLightG: un macro que proyecta dentro del array real,
// asi `&DAT_x` sigue siendo un `char*` a la fila.
#define LegacyClassNameA       (GlobalText[120][0])  // DAT_07d329c4
#define LegacyClassInfoA       (GlobalText[121][0])  // DAT_07d32af0
#define LegacyClassNameB       (GlobalText[140][0])  // DAT_07d34134
#define LegacyClassInfoB       (GlobalText[141][0])  // DAT_07d34260
#define LegacyClassNameC       (GlobalText[160][0])  // DAT_07d358a4
#define LegacySkillDescTable       (GlobalText[161][0])  // DAT_07d359d0
#define LegacyItemLevelFmt       (GlobalText[238][0])  // DAT_07d3b40c
#define LegacyChatNameBuf5       (GlobalText[248][0])  // DAT_07d3bfc4
#define LegacyChatNameBuf6       (GlobalText[249][0])  // DAT_07d3c0f0
#define LegacyChatNameBuf4       (GlobalText[254][0])  // DAT_07d3c6cc
#define LegacyChatNameBuf3       (GlobalText[256][0])  // DAT_07d3c924
#define LegacyChatNameBuf1       (GlobalText[258][0])  // DAT_07d3cb7c
#define LegacyChatNameBuf2       (GlobalText[259][0])  // DAT_07d3cca8
#define GmCommandText       (GlobalText[260][0])  // DAT_07d3cdd4
#define WhisperCommandText       (GlobalText[264][0])  // DAT_07d3d284
#define PvpCommandText       (GlobalText[265][0])  // DAT_07d3d3b0
#define WhisperTargetText       (GlobalText[267][0])  // DAT_07d3d608
#define PvpToggleMessage       (GlobalText[268][0])  // DAT_07d3d734
#define LegacyLoginAccountFmt       (GlobalText[397][0])  // DAT_07d46e60
#define LegacyDuelOfferFmt       (GlobalText[429][0])  // DAT_07d493e0
#define LegacyUsernameDispBuf       (GlobalText[450][0])  // DAT_07d4ac7c
#define LegacyPasswordDispBuf       (GlobalText[451][0])  // DAT_07d4ada8
#define LegacyVersionStr1       (GlobalText[454][0])  // DAT_07d4b12c
#define LegacyVersionStr2       (GlobalText[455][0])  // DAT_07d4b258
#define LegacyVersionStr3Fmt       (GlobalText[456][0])  // DAT_07d4b384
#define LegacyCharNameFmt       (GlobalText[459][0])  // DAT_07d4b708
#define LegacyConnectStatusStr       (GlobalText[470][0])  // DAT_07d4c3ec
#define LegacyCharNameSendBuf       (GlobalText[472][0])  // DAT_07d4c644
#define LegacyCharNameConfBuf       (GlobalText[473][0])  // DAT_07d4c770
#define NotEnoughManaText       (GlobalText[474][0])  // DAT_07d4c89c
#define LegacyServerGroupStr       (GlobalText[563][0])  // DAT_07d530e8
#define LegacyServerChanStr       (GlobalText[564][0])  // DAT_07d53214
#define LegacyPinWrongMsg       (GlobalText[580][0])  // DAT_07d544d4
#define LegacyAuthFailMsg       (GlobalText[581][0])  // DAT_07d54600
#define LegacyChatLogMsg       (GlobalText[593][0])  // DAT_07d55410
#define LegacyPinErrorMsg       (GlobalText[600][0])  // DAT_07d55c44
#define LegacyNoCharNameStr       (GlobalText[609][0])  // DAT_07d566d0
#define LegacyGuildReqOkMsg       (GlobalText[643][0])  // DAT_07d58ea8
#define LegacyGuildPanelTitle       (GlobalText[647][0])  // DAT_07d59358
#define LegacyGuildCreateMsg1       (GlobalText[677][0])  // DAT_07d5b680
#define LegacyGuildCreateMsg2       (GlobalText[678][0])  // DAT_07d5b7ac
#define LegacyGuildCreateMsg5       (GlobalText[679][0])  // DAT_07d5b8d8
#define LegacyGuildColHeaders       (GlobalText[680][0])  // DAT_07d5ba04
#define LegacyGuildColHeadLast       (GlobalText[685][0])  // DAT_07d5bfe0
#define LegacyGuildCreateMsg3       (GlobalText[686][0])  // DAT_07d5c10c
#define LegacyGuildCreateMsg4       (GlobalText[687][0])  // DAT_07d5c238
#define LegacyGuildJoinOkMsg       (GlobalText[850][0])  // DAT_07d6813c
#define LegacyEventInvalidSlot       (GlobalText[854][0])  // DAT_07d685ec
#define LegacyEventNoItemSlot       (GlobalText[896][0])  // DAT_07d6b724
