// Game_EnterWorldTick.cpp
// Game_EnterWorldTick @ 0x00521D80
//
// Character-select scene tick. Called every frame while SceneFlag==4.
// Drives the character list display, selection, and world-entry sequence.
//
// Entry guard: if CurrentProtocolState < 0x33 → return (not ready yet).
//
// Sub-states (LoginSubState):
//   0x14  — show char list (no free slot)
//   0x15  — show char list (free slot available)
//   0x16  — slide out (char chosen) → transition
//   0x17  — camera slide (equip view) → left
//   0x18  — slide out → send char-select 0xC1/0xF3 packet
//   0x19  — slide in → enter world (SceneFlag = 3)
//   0x1a  — camera slide right
//   0x1b  — slide in to Y=0xd
//   0x1c  — slide out to transition / back to char list
//
// Globals:
//   CharSelectSceneInitialized (IDA: DAT_083a7c4b) — one-time init guard
//   InitSceneLoading   — clear before Loading transition
//   SelectedHero   — selected character slot (-1 = none)
//   SelectedCharConfirm   — selected char confirmation (-1 = pending)
//   AnimatedDialogY   — animated dialog Y (lerped each frame)
//   AnimatedCameraX   — animated camera X offset (lerped each frame)
//   CharSelPreviewEntity   — character state struct for slot 0
//   CharSelPageSlot   — character page/slot counter
//   CharacterMachinePtr   — character data pointer
//   CharacterAttributePtr   — character name buffer
//   GuildOpened..  — anti-tamper counters
//   CurrentProtocolState   — server response code
//     0x33 = char list received
//     0x34 = char-select request sent
//     0x35-0x37 = char create/delete responses
//     0x39 = delete confirm
//     0x3a = enter world confirm
//   EnterKeyPressedFlag   — "select" button confirm flag

#include "stdafx.h"
#include "Net/NetText.h"
#include "Entity/EntityName.h"
#include "Text/TextCore.h"
#include "Platform/Text.h"
#include "Game/Game_EnterWorldTick.h"
#include "Net/Net.h"
#include "Net/MuEmu.h"

extern "C" void DbgLogPublic(const char*);

// ── CLICK-FLAG WATCHDOG ─────────────────────────────────────────────────────
// MouseLButtonPush (MouseLButtonPush) only toma valores {0,1} en el código fuente
// (WndProc lo settea a 1 en LBUTTONDOWN, todos los handlers lo limpian a 0).
// El runtime mostraba clk=1133983066 (0x4397355A) cada frame en char-select →
// phantom slot-clicks → state oscilando 0x18↔0x17.  La causa es algún stomp
// (probablemente buffer overflow en un struct adyacente).  Esta macro loggea
// cuándo aparece el valor wild para bisectar el corrupter, y la función
// IsClickPushed() filtra valores != 1 para suprimir phantom clicks mientras
// tanto.
static void Clk_Watch(const char* label)
{
    DWORD v = MouseLButtonPush;
    if (v != 0 && v != 1) {
        static unsigned s_count = 0;
        if (s_count < 60) {
            s_count++;
            char dbg[160];
            Platform::TextFormat(dbg, "CLK-CORRUPT @ %s: 0x%08X (decimal %u)", label, v, v);
            DbgLogPublic(dbg);
        }
        MouseLButtonPush = 0;  // clamp — only WndProc-set value 1 is trusted
    }
}
#define CLK_WATCH(LABEL) Clk_Watch(LABEL)
// IsClickPushed() ahora vive en globals.h para que todo módulo lo use.


// Paquete ya enmarcado (chain-XOR aplicado): lo manda CNetwork.
static void Pkt_Send(const BYTE* pkt_in, int len)
{
    gNetwork.SendRaw((const char*)pkt_in, len);
}

// Send the character-enter-world packet:
// [0xC1][len][0xF3] + username(10B, XOR) + padding(zeros, XOR) + slot_byte(XOR)
//
// El buffer arranca en 0 y el XOR encadena con el byte ANTERIOR ya codificado
// (`pkt[i] ^= pkt[i-1] ^ key[i]`), que es como lo descifra el server (XorData,
// recorriendo hacia atrás). Mismo patrón que gNetwork.Send.
static void Send_CharSelectPacket(void)
{
    // C1 LL F3 03 + name[40] + slot (0.97.40).
    const int NAME = (int)NetText::CharacterNameSize;
    const int LEN  = 4 + NAME + 1;
    BYTE pkt[64];
    memset(pkt, 0, sizeof(pkt));
    pkt[0] = 0xC1;
    pkt[1] = (BYTE)LEN;
    pkt[2] = 0xF3;
    pkt[3] = 0x03;

    NetText::ToServer(pkt + 4, NAME, (const char*)InputTextBuffers);

    // Char slot byte: CharSelPageSlot low nibble * 0x10 + high nibble
    pkt[4 + NAME] = (BYTE)((char)CharSelPageSlot * 0x10 + ((BYTE*)&CharSelPageSlot)[1]);

    // Forward chain XOR using PREVIOUS byte (matches server's reverse XorData).
    for (int i = 3; i < LEN; i++) {
        pkt[i] ^= pkt[i - 1] ^ CNetwork::XorKey[i & 0x1f];
    }

    // Pkt_Send aplica MuEmu byte-XOR + send (C1 path, no SerialModulus).
    Pkt_Send(pkt, LEN);
}

// Send the character-CREATE packet:
//   [0xC1][45][0xF3][0x01][name(40)][Class] — server PMSG_CHARACTER_CREATE_RECV.
// Class = page*16 (DB_CLASS_DW=0, DK=16, FE=32, MG=48) — la fórmula
// `page*0x10 + byte1` del IDA da ese valor.
static void Send_CharCreatePacket(void)
{
    const int NAME = (int)NetText::CharacterNameSize;
    const int LEN  = 4 + NAME + 1;
    BYTE pkt[64];
    memset(pkt, 0, sizeof(pkt));
    pkt[0] = 0xC1;
    pkt[1] = (BYTE)LEN;
    pkt[2] = 0xF3;
    pkt[3] = 0x01;      // sub 0x01 = CREATE

    NetText::ToServer(pkt + 4, NAME, (const char*)InputTextBuffers);   // typed char name (InputText[0])

    // Class byte: page*0x10 + byte1 → 0/16/32/48 = DW/DK/FE/MG.
    pkt[4 + NAME] = (BYTE)((char)CharSelPageSlot * 0x10 + ((BYTE*)&CharSelPageSlot)[1]);

    for (int i = 3; i < LEN; i++) {
        pkt[i] ^= pkt[i - 1] ^ CNetwork::XorKey[i & 0x1f];
    }
    Pkt_Send(pkt, LEN);
}

void Game_EnterWorldTick(void)
{
    // ── ENTRY GUARD ───────────────────────────────────────────────────────────
    if (CurrentProtocolState < 0x33) return;

    // Count free entity slots (first 5 entries in entity array)
    bool hasFreeSlot = false;
    {
        char* e = (char*)CharactersClient;
        for (int i = 0; i < 5; i++, e += CHARACTER_STRIDE) {
            if (*e == '\0') { hasFreeSlot = true; break; }
        }
    }

    // ── ONE-TIME INIT ─────────────────────────────────────────────────────────
    if (CharSelectSceneInitialized == '\0') {
        CharSelectSceneInitialized = 1;

        // Limpia flags de click heredados de la escena anterior: un doble click en
        // login/server-select deja MouseDblClickFlag=1 y en char-select dispararía la
        // entrada al mundo sin elegir slot. También se limpia MouseLButtonPush.
        MouseDblClickFlag = 0;   // double-click flag
        MouseLButtonPush = 0;   // single-click flag
        Clk_Watch("scene-init");

        Scene_LoadCharSelectResources(); // OpenCharacterSceneData (IDA)
        Clk_Watch("after-OpenCharacterSceneData");

        // Spawn background object 0xA4 at origin
        float pos3[3] = {0,0,0}, rot3[3] = {0,0,0};
        CreateObject(0xa4, pos3, rot3, 1.0f);

        CreateCharacterPointer((unsigned char*)&CharSelPreviewEntity, 0xab, 0, 0, 0.0f);
        CharSelPageSlot = (CharSelPageSlot & 0xffffff00) | 1;
        CharSelPreviewF084  = 0;

        SelectedHero  = -1;
        SelectedCharConfirm  = -1;
        DeleteCharSlotIdx  = -1;
        MouseOnWindowFlag  = 0;
        LoginNextSubState  = 0;
        AnimatedDialogY  = -100;

        if (!hasFreeSlot) {
            LoginSubState = 0x14;  // no free slot: char list only
        } else {
            LoginSubState = 0x15;  // free slot: can create char
            PlayBuffer(0x1b, 0, 0);
        }

        ClearInventory();

        // Anti-tamper: register CharacterMachinePtr in hash table
        {
            unsigned idx = HashTable_GetIndex(&MAIN_HASH_CLASS, CharacterMachinePtr);
            if (idx == 0xffffffff) {
                void* node = AntiTamper_HashNode();
                *((BYTE*)node + 0x584) = 1;
                HashTable_Insert(&MAIN_HASH_CLASS, node, CharacterMachinePtr);
            }
        }

        // Clear char data buffers
        {
            char* p = (char*)CharacterAttributePtr;
            for (int i = 0x56; i <= 0x6a; i++) p[i] = '\0';
            int* q = (int*)((char*)CharacterMachinePtr + 0x87 * 4);
            for (int i = 7; i >= 0; i--, q += 0x11) *q = 0;
        }

        // Re-register new entry
        {
            unsigned idx = HashTable_GetIndex(&MAIN_HASH_CLASS, CharacterMachinePtr);
            if (idx == 0xffffffff) {
                void* node = AntiTamper_HashNode();
                *((BYTE*)node + 0x584) = 1;
                HashTable_Insert(&MAIN_HASH_CLASS, node, CharacterMachinePtr);
            }
        }

        // Clear anti-tamper counters
        GuildOpened = 0; PartyOpened = 0;
        CharacterOpened = 0; InventoryOpened = 0;
        ShopOpened = 0; WarehouseOpened = 0;
        TradeOpened = 0;
        GuildCreatorOpened = 0; g_iKeyPadEnable = 0;
        PinInputModeState = 0;

        GuildInputEnable = 0; ImeModeFlag = 0; GoldInputEnable = 0;
        InputEnable = 1;
        ClearInput(1);
        ActiveInputIndex = 0;
        LegacyInputTextWidth = 0x5a;
        InputNumber = 1;

        // 120× widget draw
        for (int i = 0x78; i > 0; i--)
            UIChatLogWindow_AddText((const char*)&EnterWorldLogLabel, (const char*)&EnterWorldLogText, 0);

        // Clear entity local player ptr if null
        if (Hero == 0) {
            GuardPageScratch_10 = 0; GuardPageScratch_14 = 0; GuardPageScratch_18 = 0;
            GuardPageScratch_1C = 0; GuardPageScratch_20 = 0; GuardPageScratch_24 = 0;
        }

        // Set camera to char-select world position (slot 5 of CameraWalk).
        // CameraWalk5PosX..0056167b son aliases DWORD& sobre el storage float de
        // CameraWalkTable[30..35]: se leen como float para no convertir int→float.
        LegacyCurrentCamAngle0  = *(float*)&CameraWalk5Pitch;
        LegacyCurrentCamAngle1  = *(float*)&CameraWalk5Yaw;
        LegacyCurrentCamAngle2  = *(float*)&CameraWalk5Roll;
        CameraWalkType = 0;
        CameraNumber = 5;
        CurrentCameraCount = 0;
        LegacyCurrentCamPos0  = CameraWalk5PosX;     // DWORD&=DWORD ok
        LegacyCurrentCamPos1  = CameraWalk5PosY;     // DWORD&=DWORD ok
        CurrentCameraPosZ = *(float*)&CameraWalk5PosZ;

        CErrorReport_Write(&ErrorReportCtx, "> Character scene init success");
    }

    // ── PER-FRAME UPDATES ─────────────────────────────────────────────────────
    // Recargar sólo el fondo cuando cambia el aspecto, antes de simularlo.
    Scene_RefreshCharSelectBackdrop();
    CLK_WATCH("frame-start");
    // Este es el tick de char-select (Game_MainLoop state 4 → esta fn, pese al
    // nombre "EnterWorldTick"). IDA 0x521D80 L401 llama MoveBugs() acá, primero:
    // fade-in del alpha de las entidades "bug" y posición/animación de las
    // MONTURAS (Uniria bug=195 / Dinorant bug=267).
    { extern void __stdcall MoveBugs(void); MoveBugs(); }
    CLK_WATCH("after-MoveBugs");
    Object_MoveUpdate();         CLK_WATCH("after-Object_MoveUpdate");
    // MoveParticles decrementa el lifetime de cada particle del pool SpritePoolBase.
    // MEJORA DEL DLL (no esta en IDA): el binario solo llama MoveParticles en
    // 0x005223EE; el DLL (Patchs.cpp MoveParticles_MoveCharacterScene) agrega
    // MoveEffects + MoveJoints para que el efecto de las alas se vea en
    // char-select.  Sin el tick de joints los de vida 0 no morian nunca aca.
    Effect_TickAll();            // MoveEffects (0x0046B790)
    Joint_TickAll();             // MoveJoints  (0x004736E0)
    MoveParticles();        CLK_WATCH("after-MoveParticles");
    MoveCharacterClient((float*)&CharSelPreviewEntity); CLK_WATCH("after-MoveCharacterClient");

    // ── Per-character animation tick ──────────────────────────────────────────
    // En el binario original MoveCharactersClient (0x00455010) itera todas las
    // entidades y por cada slot activo llama MoveCharacterClient (0x00454FC0) →
    //   TestFrustrum2D + MoveMonsterClient + MoveCharacter + MoveCharacterVisual
    // donde MoveCharacter (0x00449900, 2773 lines) llama CharacterAnimation
    // (0x00448600) que avanza entity[+0x108] vía sub_440AA0.
    //
    // En nuestro port MoveCharacter es un stub vacío y MoveCharacter está sin
    // portar (es enorme, 90% es lógica de combate/mov in-game irrelevante para
    // char-select). Pero los chars en char-select necesitan que su frame de
    // animación avance cada tick para que la idle se vea animada.
    //
    // Solución mínima: aquí, por slot 0..7, llamar MoveCharacterVisual (escribe
    // model.action desde entity[+0x105]) seguido de CharacterAnimation (avanza
    // el frame). Sin MoveCharacterVisual el modelo lee acción 0 y todos los
    // chars correrían la misma anim.
    {
        for (int s = 0; s < 8; ++s) {
            intptr_t e = (intptr_t)CharactersClient + s * CHARACTER_STRIDE;
            if (*(char*)e != '\0') {
                MoveCharacterVisual(e);            // MoveCharacterVisual: model.action ← entity.action
                CharacterAnimation(e, e);   // advance entity[+0x108] (anim frame)
            }
        }
    } CLK_WATCH("after-CharAnim");
    Login_CameraUpdate();        CLK_WATCH("after-Login_CameraUpdate");
    // MoveCamera @ 0x0051E4E0 — actualiza CameraPosition/CameraAngle desde
    // CurrentCameraPosition/CurrentCameraAngle (lerp hacia wp5 = char-select).
    // Sin esta llamada la cámara queda anclada donde la dejó el login (~0,-1100,500)
    // y char-select se ve sin pitch correcto, los 5 personajes fuera de cuadro.
    // En IDA Game_EnterWorldTick (00521D80) llama MoveCamera() después de MoveParticles().
    MoveCamera();           CLK_WATCH("after-MoveCamera");

    // Delete entity if server confirmed (0x39 = delete ack)
    if (CurrentProtocolState == 0x39) {
        ((char*)CharactersClient)[DeleteCharSlotIdx * CHARACTER_STRIDE] = '\0';
    }

    int mouseX = MouseX;

    // ── CHARACTER SELECTION (no char selected yet) ────────────────────────────
    if (SelectedCharConfirm == -1) {
        GL_ResetState();  // render char list
        CLK_WATCH("after-GL_ResetState");

        int dialogY = AnimatedDialogY;

        // Click arrow → select char (0x11d-0x164, Y in [dialogY+0x4b, dialogY+0x64])
        if (hasFreeSlot &&
            mouseX > 0x11d && mouseX < 0x164 &&
            MouseY >= dialogY + 0x4b &&
            MouseY < dialogY + 0x64 &&
            IsClickPushed())
        {
            MouseLButtonPush = '\0';
            InGameUiState = InGameUiStateNext;
            InGameUiStateNext = 0;

            // Find first free entity slot
            int slotFound = SelectedCharConfirm;
            char* e = (char*)CharactersClient;
            for (int i = 0; i < 5; i++, e += CHARACTER_STRIDE) {
                if (*e == '\0') { slotFound = i; break; }
                slotFound = SelectedCharConfirm;
            }
            SelectedCharConfirm = slotFound;
            PlayBuffer(0x19, 0, 0);
            LoginSubState = 0x16;
            LoginNextSubState = 0x1b;
            PlayBuffer(0x1b, 0, 0);
            ClearInput(1);
            InputEnable = 1;
            InputNumber = 1;
            Input_SetLimit(0, (int)NetText::CharacterNameSize, (int)NetText::CharacterNameClusters);
        }

        // Update free slot flag after possible change
        hasFreeSlot = (SelectedHero != -1);

        // Click equip/select button (AnimatedCameraX+0x80..0xc6, Y 0x93-0xa7)
        if (hasFreeSlot &&
            mouseX >= (int)AnimatedCameraX + 0x80 - gWindow.GetRightShift() &&
            mouseX < (int)AnimatedCameraX + 0xc6 - gWindow.GetRightShift() &&
            MouseY > 0x93 && MouseY < 0xa7 &&
            IsClickPushed())
        {
            MouseLButtonPush = '\0';
            ushort lvl = *(ushort*)(CharactersClient + SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1be));
            BYTE   flags = ((BYTE*)CharactersClient)[SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1c0)];

            if (lvl < 40) {
                // Level too low error
                if (flags & 0x12) {
                    if (InGameUiState == 0) InGameUiState = 0x19; else InGameUiStateNext = 0x19;
                } else {
                    if (InGameUiState == 0) InGameUiState = 0x72; else InGameUiStateNext = 0x72;
                    ClearInput(1);
                    InputEnable = 1; InputNumber = 1;
                    InputTextMax = PinUiHeightParam;
                    InputFieldFlags[0] = 1;
                }
            } else {
                if (InGameUiState == 0) InGameUiState = 0x84; else InGameUiStateNext = 0x84;
            }
            PlayBuffer(0x19, 0, 0);
        }

        // Las flechas de cambio de clase van en el bloque `else if (SelectedCharConfirm != -1)`
        // (create/name view): en IDA el input del create-panel (flechas de clase,
        // OK/Cancel, nombre) corre cuando `SelectedCharConfirm >= 0`, no en la lista.

        // El bloque "back/delete/Enter" va en el `else if (SelectedCharConfirm != -1)` más
        // abajo (rama name-input), como en IDA (00521D80), no en la rama lista.

        // Entity slot click detection (5 slots)
        {
            int slotY = dialogY + 0x32;
            bool confirmed = false;
            int  selSlot   = SelectedHero;
            int  pendingB  = InGameUiStateNext;

            for (int i = 0; i < 5; i++, slotY += 0x14) {
                if (i == SelectedCharacter) {
                    if (MouseDblClickFlag != '\0') {
                        MouseDblClickFlag = '\0';
                        InGameUiStateNext = 0;
                        confirmed = true;
                        selSlot   = i;
                        pendingB  = 0;
                        SelectedHero = i;
                        InGameUiState = pendingB;
                    } else if (IsClickPushed()) {
                        // ── DIAGNOSTIC: capture state at slot-click trigger ──
                        MouseLButtonPush = '\0';
                        InGameUiStateNext = 0;
                        SelectedHero = i;
                        InGameUiState = pendingB;
                        PlayBuffer(0x19, 0, 0);
                        if (LoginSubState != 0x17 && LoginSubState != 0x15)
                            PlayBuffer(0x1b, 0, 0);
                        LoginSubState = 0x18;
                        LoginNextSubState = 0x17;
                        selSlot = SelectedHero;
                        pendingB = InGameUiStateNext;
                    }
                }
                pendingB = InGameUiStateNext;
            }

            // IDA original (00521D80) gates the dword_83A7C14 = 25 path with:
            //   if (!v103) goto LABEL_117;     // v103 = (SelectedHero != -1)
            //   if (!ErrorMessage && EnterKeyPressedFlag) v40 = 1;
            //   if (selectButtonClicked) { ... fall through to flag check }
            //   else { LABEL_117: if (!v40) goto LABEL_124; }
            //   // flag check → state = 25

            bool selectClicked = false;
            if (SelectedHero != -1) {
                // v40 = 1 path (line 545 IDA): confirm flag from button trigger
                if (hasFreeSlot && !confirmed) {
                    if (InGameUiState == 0 && EnterKeyPressedFlag != '\0') {
                        EnterKeyPressedFlag = '\0';
                        InGameUiStateNext = 0;
                        confirmed = true;
                        InGameUiState = pendingB;
                        PlayBuffer(0x19, 0, 0);
                    }
                }

                // Boton OK / entrar al juego (IDA 0x521D80 L556):
                //   MouseX >= 441 - AnimatedCameraX && MouseX < 441 - AnimatedCameraX + 70
                //   && MouseY >= 148 && MouseY < 167
                if (mouseX >= 441 - (int)AnimatedCameraX + gWindow.GetRightShift() &&
                    mouseX < 441 - (int)AnimatedCameraX + gWindow.GetRightShift() + 70 &&
                    MouseY >= 148 && MouseY < 167 &&
                    IsClickPushed())
                {
                    MouseLButtonPush = '\0';
                    InGameUiStateNext = 0;
                    InGameUiState = pendingB;
                    PlayBuffer(0x19, 0, 0);
                    selectClicked = true;
                }
            }

            // Flag check ONLY si v103 (char válido) Y (selectClick OR v40):
            // corresponde a IDA `if (!v40) goto LABEL_124` y precondición
            // SelectedHero != -1.  Sin esto, cada frame disparaba state=0x19.
            if (SelectedHero != -1 && (selectClicked || confirmed)) {
                BYTE flags = ((BYTE*)CharactersClient)[SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1c0)];
                // Precedence-fix: IDA `(flags & 1) == 0`, no `!flags & 1`.
                if ((flags & 1) == 0) {
                    LoginSubState = 0x19;
                    PlayBuffer(0x1b, 0, 0);
                } else {
                    if (InGameUiState == 0) InGameUiState = 0x85; else InGameUiStateNext = 0x85;
                }
            }
        }
    }
    // ── NAME-INPUT VIEW (slot reservado para nuevo char): b0 != -1 ────────────
    // IDA: `if (-1 < SelectedCharConfirm)`. Aquí Enter (EnterKeyPressedFlag) y los botones
    // back (mouseX 0xea..0x133) / OK (mouseX 0x14f..0x196) operan sobre el
    // diálogo de nombre del nuevo personaje.
    else if ((int)SelectedCharConfirm != -1) {
        int dialogY = AnimatedDialogY;

        // ── Class rotate arrows (create-panel) — IDA: corre con dword_5616B0 >= 0.
        // Fila superior (dialogY+196..215 = 0xc4..0xd7): flecha izq [0x16b,0x17f)=clase--,
        // flecha der [0x180,0x194)=clase++. Re-crea el preview char (tipo = clase+170).
        if (mouseX > 0x16a && mouseX < 0x17f &&
            MouseY >= dialogY + 0xc4 && MouseY < dialogY + 0xd7 &&
            IsClickPushed())
        {
            MouseLButtonPush = '\0';
            if ((char)CharSelPageSlot == '\0') {
                PlayBuffer(0x1a, 0, 0);
            } else {
                BYTE b = (BYTE)((char)CharSelPageSlot - 1);
                CharSelPageSlot = (CharSelPageSlot & 0xffffff00) | b;
                CreateCharacterPointer((unsigned char*)&CharSelPreviewEntity, b + 0xaa, 0, 0, 0.0f);
                CharSelPreviewF084 = 0;
                PlayBuffer(0x19, 0, 0);
            }
        }
        unsigned maxPage = 2;   // rotate limit: 3 si algún char >= lvl 220
        {
            char* e = (char*)CharactersClient;
            for (int i = 0; i < 5; i++, e += CHARACTER_STRIDE)
                if (*e != '\0' && *(ushort*)&((CHARACTER*)(uintptr_t)(e))->Level > 0xdb) { maxPage = 3; break; }
        }
        if (mouseX > 0x17f && mouseX < 0x194 &&
            MouseY >= dialogY + 0xc4 && MouseY < dialogY + 0xd7 &&
            IsClickPushed())
        {
            MouseLButtonPush = '\0';
            if ((CharSelPageSlot & 0xff) >= maxPage) {
                // Tope de rotacion: sonido de "bloqueado". Faltaba — IDA
                // Game_EnterWorldTick L635-638 tiene las dos ramas.
                PlayBuffer(0x1a, 0, 0);   // PlayBuffer(26)
            } else {
                BYTE b = (BYTE)((char)CharSelPageSlot + 1);
                CharSelPageSlot = (CharSelPageSlot & 0xffffff00) | b;
                CreateCharacterPointer((unsigned char*)&CharSelPreviewEntity, b + 0xaa, 0, 0, 0.0f);
                CharSelPreviewF084 = 0;
                PlayBuffer(0x19, 0, 0);
            }
        }

        bool backClick = (mouseX > 0xea && mouseX < 0x133 &&
                          MouseY >= dialogY + 0x15d &&
                          MouseY < dialogY + 0x172 &&
                          IsClickPushed());
        bool clearBtn = (EnterKeyPressedFlag != '\0');
        if (clearBtn) EnterKeyPressedFlag = '\0';

        // "Back" path — pure cancel back to slot list (IDA path A)
        if (backClick) {
            MouseLButtonPush = '\0';
            SelectedCharConfirm = -1;
            PlayBuffer(0x19, 0, 0);
            LoginSubState = 0x1c;
            LoginNextSubState = 0x15;
            PlayBuffer(0x1b, 0, 0);
        }
        else {
            // "OK button" hit-test (0x14f..0x196)
            bool okClick = (mouseX >= 0x14f && mouseX <= 0x196 &&
                            MouseY >= dialogY + 0x15d &&
                            MouseY < dialogY + 0x172 &&
                            IsClickPushed());
            if (okClick || clearBtn) {
                if (okClick) MouseLButtonPush = '\0';
                SelectedCharConfirm = -1;
                PlayBuffer(0x19, 0, 0);
                LoginSubState = 0x1c;
                LoginNextSubState = 0x15;
                PlayBuffer(0x1b, 0, 0);

                // Validate username length
                int ulen = (int)strlen((char*)InputTextBuffers);
                if (ulen < 4) {
                    if (InGameUiState == 0) InGameUiState = 0x7a; else InGameUiStateNext = 0x7a;
                } else {
                    if (Chat_ValidateInputCommand() == '\0' && !CheckSpecialText((const BYTE*)InputTextBuffers)) { // IDA: FUN_00406B30
                        LoginNextSubState = 0x1c;
                        InGameUiState = InGameUiStateNext;
                        InGameUiStateNext = 0;
                        CurrentProtocolState = 0x34;
                        Send_CharCreatePacket();   // F3/01 crear (no F3/03 select)
                    } else {
                        if (InGameUiState == 0) InGameUiState = 0x73; else InGameUiStateNext = 0x73;
                    }
                }
            }
        }
    }

    // ── DIAGNOSTIC: log substate transitions (only when state changes) ───────
    {
        static int s_lastSubState = -1;
        static int s_lastDialogY  = -999999;
        static int s_lastCamX     = -999999;
        if ((int)LoginSubState != s_lastSubState) {
            s_lastSubState = (int)LoginSubState;
        }
        // Also log dialog Y / camera X every ~32 frames if state is steady so we
        // can see if positions are bouncing without a state change.
        static int s_frameCtr = 0;
        if ((++s_frameCtr & 0x1f) == 0) {
            if ((int)AnimatedDialogY != s_lastDialogY || (int)AnimatedCameraX != s_lastCamX) {
                s_lastDialogY = (int)AnimatedDialogY;
                s_lastCamX    = (int)AnimatedCameraX;
            }
        }
    }

    // ── ANIMATION STATE MACHINE ───────────────────────────────────────────────
    switch (LoginSubState) {
    case 0x15:  // Ease dialog Y to 6
        AnimatedDialogY += (6 - AnimatedDialogY) / 3;
        break;

    case 0x16:  // Slide dialog off screen (down)
        AnimatedDialogY -= (AnimatedDialogY + 0x102) / 2;
        if (AnimatedDialogY < -0xff) {
            LoginSubState = LoginNextSubState;
            PlayBuffer(0x1b, 0, 0);
        }
        break;

    case 0x17:  // Camera slide left
    case 0x1a:  // Camera slide right
        {
            int d = (int)((unsigned long long)((long long)AnimatedCameraX * 0x55555555LL) >> 32) - AnimatedCameraX;
            AnimatedCameraX += (d >> 1) - (d >> 31);
        }
        break;

    case 0x18:  // Slide panel off, send char-select
        AnimatedCameraX -= (AnimatedCameraX + 0xca) / 2;
        if (AnimatedCameraX < -199) {
            LoginSubState = LoginNextSubState;
            PlayBuffer(0x1b, 0, 0);
            if (CurrentProtocolState == 0x3a) {
                int code = CurrentProtocolState;
                if (InGameUiState == 0) { InGameUiState = 0x3a; CurrentProtocolState = 0x33; }
                else                   { InGameUiStateNext = 0x3a; CurrentProtocolState = 0x33; }
            }
        }
        break;

    case 0x19:  // Slide off; on reach: copiar datos del personaje y pasar a Loading
        AnimatedCameraX -= (AnimatedCameraX + 0xca) / 2;
        if (AnimatedCameraX < -200) {
            int code = CurrentProtocolState;
            bool validCode = (code == 0x33 || code == 0x35 || code == 0x36 ||
                              code == 0x37 || code == 0x39 || code == 0x3a);
            if (validCode) {
                // Build and send world-enter packet with username + anti-tamper data
                {
                    // Copy char name from selected slot
                    // 0.97.40: CharacterMachine guarda 10 bytes; el nombre completo
                    // queda en EntityName::Selected().
                    const char* charName = EntityName::Get((const void*)(uintptr_t)(CharactersClient + SelectedHero * CHARACTER_STRIDE));
                    EntityName::SetSelected(charName);
                    TextEngine::CopyTruncated((char*)CharacterAttributePtr, 11, charName);
                    *(ushort*)((char*)CharacterAttributePtr + 0xe) = *(ushort*)(CharactersClient + SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1be));
                    ((char*)CharacterAttributePtr)[0xb] = ((char*)CharactersClient)[SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1bc)];
                    ((char*)CharacterAttributePtr)[0xc] = ((char*)CharactersClient)[SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1bd)];
                }
                // El F3/03 NO sale de aca: IDA 0x521D80 case 25 solo copia los
                // datos del personaje y pasa a Loading. Lo manda el init de
                // Game_CharSelectTick (IDA 0x524E30 L80-219).

                // Handle specific response codes
                if (code == 0x36 || code == 0x37) {
                    int r = code;
                    if (InGameUiState == 0) { InGameUiState = r; }
                    else { InGameUiStateNext = r; }
                }

                // ── WORLD ENTRY: copy char data → transition to Loading ──────
                // Va DENTRO de case 25, tras el Pkt_Send y el check de validCode (IDA
                // 0x00521D80:1150); fuera del switch correría cada frame.
                {
                    const char* charName = EntityName::Get((const void*)(uintptr_t)(CharactersClient + SelectedHero * CHARACTER_STRIDE));
                    char* charData = (char*)CharacterAttributePtr;
                    EntityName::SetSelected(charName);
                    TextEngine::CopyTruncated(charData, 11, charName);
                    *(ushort*)(charData + 0xe) = *(ushort*)(CharactersClient + SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1be));
                    charData[0xb] = ((char*)CharactersClient)[SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1bc)];
                    charData[0xc] = ((char*)CharactersClient)[SelectedHero * CHARACTER_STRIDE + EntityOffset(0x1bd)];
                }

                SceneFlag  = 3;  // SceneFlag = Loading
                InitSceneLoading  = 0;  // reset Scene_Loading init guard
                ReleaseCharacterSceneData(); // IDA: ReleaseCharacterSceneData (0x005102C0) — world loading kickoff
                Sound_StopBuffer(5);
            }
        }
        break;

    case 0x1b:  // Ease dialog to Y=0xd
        AnimatedDialogY += (0xd - AnimatedDialogY) / 3;
        break;

    case 0x1c:  // Slide off; transition to char list or create
        AnimatedDialogY -= (AnimatedDialogY + 0x192) / 2;
        if (AnimatedDialogY < -399) {
            int code = CurrentProtocolState;
            if (code == 0x35 || code == 0x36 || code == 0x37) {
                // Check if any slot is free
                bool anyFree = false;
                char* e = (char*)CharactersClient;
                for (int i = 0; i < 5; i++, e += CHARACTER_STRIDE)
                    if (*e == '\0') { anyFree = true; break; }

                if (anyFree) {
                    LoginSubState = 0x15;
                    PlayBuffer(0x1b, 0, 0);
                } else {
                    LoginSubState = 0x14;
                }

                // Propagate response code
                if (code == 0x36 || code == 0x37) {
                    int r = code;
                    if (CurrentProtocolState == 0) { InGameUiStateNext = r; InGameUiState = InGameUiStateNext; }
                    else if (InGameUiState == 0) { InGameUiState = r; }
                    else { InGameUiStateNext = r; }
                }
            } else if (LoginNextSubState == 0x15) {
                LoginSubState = 0x15;
            }
        }
        break;

    default:
        // Fall through — default animation stop
        break;
    }
}
