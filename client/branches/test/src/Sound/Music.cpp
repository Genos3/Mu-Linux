// Music.cpp — elección del tema de fondo.
//
// IDA: PlayMp3 (0x00412890) y StopMp3 (0x004127F0). Comparan el nombre recibido con
// el tema en curso (m_CurrentTrack) y respetan m_MusicEnabled (Config.ini
// [Sound] EnableMusic, o el registro) y Destroy (AppQuitFlag, el cliente se
// está cerrando).
//
// DESVIACION: el binario reproducía con un proceso externo, MuPlayer.exe
// (WinExec para empezar, WM_CLOSE para cortar). Acá lo hace CSound dentro del
// cliente (Sound/SoundManager.h). El tema suena una vez y no se repite mientras
// siga siendo el mismo, igual que antes.

#include "stdafx.h"
#include "Platform/Text.h"
#include "Sound/SoundManager.h"

// IDA: StopMp3 (0x004127F0) — corta el tema si `Name` es el que está sonando.
void CSound::StopTrack(const char* Name, int bEnforce)
{
    if (Name == NULL) return;   // guard del port: la tabla de nombres puede venir vacía

    if ((m_MusicEnabled || bEnforce) && m_CurrentTrack[0] && strcmp(Name, m_CurrentTrack) == 0)
    {
        StopMusic();
        m_CurrentTrack[0] = 0;
    }
}

// IDA: PlayMp3 — empieza `Name`.
//   - mismo tema ya sonando  -> no hace nada
//   - otro tema sonando      -> lo corta y sale (el próximo frame arranca el nuevo)
//   - nada sonando           -> si el archivo existe, lo reproduce
void CSound::PlayTrack(const char* Name, int bEnforce)
{
    if (Name == NULL) return;   // guard del port: la tabla de nombres puede venir vacía

    if (AppQuitFlag != 0 || (!m_MusicEnabled && !bEnforce))
        return;
    // DESVIACION: música parada desde el menú de opciones (CSound).
    if (IsMusicStoppedByUser()) {
        m_CurrentTrack[0] = 0;
        return;
    }

    if (m_CurrentTrack[0])
    {
        if (strcmp(Name, m_CurrentTrack) == 0)
            return;

        StopMusic();
        m_CurrentTrack[0] = 0;
        return;
    }

    FILE* fp = crt_fopen(Name, FopenModeRb);
    if (fp == NULL) return;
    crt_fclose(fp);

    if (PlayMusic(Name))
        Platform::TextCopySecure(m_CurrentTrack, sizeof(m_CurrentTrack), Name);
}


// IDA: PlayMp3 (0x00412890) / StopMp3 (0x004127F0). Puentes para callers heredados.
void __cdecl Music_PlayTrack(const char* name, int enforce)
{
    gSound.PlayTrack(name, enforce);
}

void __cdecl Music_StopTrack(const char* name, int enforce)
{
    gSound.StopTrack(name, enforce);
}

// ListBox_HandleInput2 @ 0x00412180 (~66 lines) — ListBox_HandleInput2: identical structure to
// ListBox_HandleInput (key 7/0xC/0xD/0xE dispatch, scroll adjust, selection tracking).
// Separate vtable variant for a different list widget class.
int __fastcall ListBox_HandleInput2(int* param_1) {
    (void)param_1;
    return 0;
}

// Mem_Copy296 @ 0x004124D0 (24 bytes) — memcpy 0x4a dwords (296 bytes)
// Mem_Copy296 (IDA-activated)
void __cdecl Mem_Copy296(void *a1, const void *a2)
{
  if ( a1 )
  {
    qmemcpy(a1, a2, 0x128u);
  }
}

// GameGuard_ModuleDtor @ 0x004124F0 — GG module ~dtor
void __fastcall GameGuard_ModuleDtor(intptr_t ecx, int /*edx*/, BYTE param_1) {
    ListBox_Destructor_A((DWORD *)ecx);
    if (param_1 & 1) operator_delete((void *)ecx);
}

// ListBox_Destructor_A @ 0x00412510 (~45 lines) — ListBox_Destructor_A: clears item linked list
// via BST_UnlinkNode loop, frees list sentinel nodes, resets counts, then delegates to
// base class destructor (FUN_00410de0 + List_DestroyFreeSentinel). Sets vtable to PTR_FUN_00552668.
void __fastcall ListBox_Destructor_A(DWORD* param_1) {
    (void)param_1;
}

// GameGuard_Module2Dtor @ 0x004125F0 — GG module2 ~dtor
void __fastcall GameGuard_Module2Dtor(intptr_t ecx, int /*edx*/, BYTE param_1) {
    ListBox_Destructor_B((DWORD *)ecx);
    if (param_1 & 1) operator_delete((void *)ecx);
}

// ListBox_Destructor_B @ 0x00412610 (~45 lines) — ListBox_Destructor_B: same structure as
// ListBox_Destructor_A but sets vtable to PTR_FUN_00552760. Second list-box class variant.
void __fastcall ListBox_Destructor_B(DWORD* param_1) {
    (void)param_1;
}

// GameGuard_PreInitWrapper @ 0x00412700 (12 bytes) — NO es "string cleanup": es el wrapper que
// arranca GameGuard, `return PreInitNPGameMon("Mu")`.
//
// En el binario NO lo llama nadie desde codigo: su unico xref es de DATOS, desde la
// tabla de inicializadores dinamicos del CRT en 0x00558010. O sea corre ANTES de
// WinMain, via el thunk FUN_004126F0, que ademas registra el release con atexit.
//
// Nuestro build no replica esa tabla, asi que esta funcion queda sin callers y
// GameGuard nunca arranca, que es lo buscado.
void GameGuard_PreInitWrapper(void) { GameGuard_PreInitNPGameMon((BYTE *)&g_GameGuardGameName); }

// GameGuard_RegisterReleaseAtexit @ 0x00412710 (12 bytes) - IDA: Fn_00412710
// `crt_atexit(&GameGuard release @0x412720)`; neutralizado en este build.
void GameGuard_RegisterReleaseAtexit(void) {}

// PacketCipher_InitGlobal @ 0x00412780 (10 bytes) — cleanup hash class
void PacketCipher_InitGlobal(void) { PacketCipher_Initialize((void *)0x055ca0a0); }

// PacketCipher_RegisterStaticDtor @ 0x00412790 (12 bytes) - IDA: Fn_00412790
// `crt_atexit(&dtor de PacketCipher @0x55C9BC8)`; neutralizado en este build.
void PacketCipher_RegisterStaticDtor(void) {}

// CErrorReport_InitGlobal @ 0x004127C0 (10 bytes) — init error report
void CErrorReport_InitGlobal(void) { FUN_00405240_init((void *)0x055c9bf0); }

// CErrorReport_RegisterStaticDtor @ 0x004127D0 (12 bytes) - IDA: Fn_004127d0
// `crt_atexit(&dtor de CErrorReport @0x55C9BF0)`; neutralizado en este build.
void CErrorReport_RegisterStaticDtor(void) {}
