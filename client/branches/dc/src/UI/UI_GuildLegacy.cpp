// UI_GuildLegacy.cpp

#include "stdafx.h"
#include "Net/NetText.h"
#include "Entity/EntityName.h"
#include "Render/Gfx.h"
#include "Platform/Text.h"
void __fastcall FUN_0045aaa0_impl(void *_this, char flags);
void __cdecl    ClothNode_ScalarDtor(void *_this, char flags);
#include "globals.h"
#include "functions.h"

// -- Declaraciones de funciones definidas en otros modulos ------------------
// Cloth_Integrate esta en Physics/Cloth_Simulation.cpp y Cloth_Solve en
// Net/Crypto.cpp.
void __fastcall Cloth_Integrate(int*, float);
int  __cdecl    Cloth_Solve(DWORD *a1);

#include "Net/Net.h"

extern "C" BYTE OffsetInventoryItems[];
extern void __cdecl operator_delete(void* ptr);
extern void MapFileDecrypt(BYTE* buf, int size);

#ifndef qmemcpy
#define qmemcpy(dst,src,sz) memcpy((dst),(src),(size_t)(sz))
#endif
#ifndef delete__
#define delete__(p) operator_delete((unsigned char*)(p))
#endif
#ifndef __OFSUB__
#define __OFSUB__(x,y)       (0)
#endif

#ifndef LODWORD
#define LODWORD(x)           (*((DWORD*)&(x)))
#define HIDWORD(x)           (*(((DWORD*)&(x))+1))
#define SLOBYTE(x)           (*((char*)&(x)))
#define SLOWORD(x)           (*((short*)&(x)))
#define SLODWORD(x)          (*((int*)&(x)))
#endif
#ifndef LOBYTE
#define LOBYTE(x)            (*((unsigned char*)&(x)))
#define HIBYTE(x)            (*(((unsigned char*)&(x))+1))
#define LOWORD(x)            (*((unsigned short*)&(x)))
#define HIWORD(x)            (*(((unsigned short*)&(x))+1))
#endif


// ── sub_51DDF0 (0x0051DDF0) — tabla de puestos del evento ────────────────────
// NO es una lista de guild: es el ranking que sale al terminar Devil Square /
// Blood Castle.  Lo dispara `RenderErrorMessage` con `case 140: sub_51DDF0()`,
// tras `sub_51D9E0` (opcode 0x93) que deja ErrorMessage = 140 y copia las
// entradas a GuildMemberTable.
//
// Registro (= PMSG_DEVIL_SQUARE_SCORE del server, 0.97.40): +0 name[40] ·
// +40 score · +44 RewardExperience · +48 RewardMoney.
int __cdecl RenderMatchScore(void)
{
    // Columnas: puesto, personaje, puntos, experiencia, recompensa.
    static const int kCol[5] = { 219, 235, 287, 345, 383 };
    char buf[64];

    Gfx::Color3f(0.5f, 1.0f, 0.5f);
    UI_RenderText(229, 70, GlobalText[647], (LPSIZE)0, '\0', 0);
    Platform::TextFormat(buf, GlobalText[648], EntityName::Get(Hero));
    UI_RenderText(229, 86, buf, (LPSIZE)0, '\0', 0);

    // Encabezados 680..684.  El original SALTEA el 681 ("Personaje") y le suma
    // 10 px al indice 2; es asi en el binario.
    Gfx::Color3f(0.5f, 0.5f, 1.0f);
    for (int i = 0; i < 5; ++i) {
        if (i == 1) continue;
        UI_RenderText(kCol[i] + (i == 2 ? 10 : 0), 110, GlobalText[680 + i],
                      (LPSIZE)0, '\0', 0);
    }

    int count = GuildMemberCount;
    const int kMaxRows = GUILD_MEMBER_TABLE_BYTES / GUILD_MEMBER_STRIDE;
    if (count > kMaxRows) count = kMaxRows;
    if (count <= 0) return GuildMemberCount;

    int y = 126;
    Gfx::Color3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < count; ++i) {
        // El registro 0 del buffer es el del propio jugador y va ULTIMO: por eso
        // el original indexa con (i+1) % count.
        const BYTE *e = &GuildMemberTable[GUILD_MEMBER_STRIDE * ((i + 1) % count)];

        if (i == count - 1) {
            Gfx::Color3f(0.9f, 0.9f, 0.0f);
            UI_RenderText(219, 286, GlobalText[685], (LPSIZE)0, '\0', 0);
            y = 302;
            Platform::TextFormat(buf, "%2d", GuildUiAuxParam);
        } else {
            Platform::TextFormat(buf, "%2d", i + 1);
        }
        m_dwTextColor = 0xFFFFFFFFu;
        UI_RenderText(kCol[0], y, buf, (LPSIZE)0, '\0', 0);

        char name[NetText::CharacterNameSize + 1];
        NetText::FromServer(name, sizeof(name), e, NetText::CharacterNameSize);
        UI_RenderText(kCol[1], y, name, (LPSIZE)0, '\0', 0);

        const int NAME = (int)NetText::CharacterNameSize;
        m_dwTextColor = 0xFFFFD2D2u;
        Platform::TextFormat(buf, "%10d", *(const int *)(e + NAME));
        UI_RenderText(kCol[2], y, buf, (LPSIZE)0, '\0', 0);

        m_dwTextColor = 0xFFD2FFD2u;
        Platform::TextFormat(buf, "%6d", *(const int *)(e + NAME + 4));
        UI_RenderText(kCol[3], y, buf, (LPSIZE)0, '\0', 0);

        m_dwTextColor = 0xFFD2D2FFu;
        Platform::TextFormat(buf, "%6d", *(const int *)(e + NAME + 8));
        UI_RenderText(kCol[4], y, buf, (LPSIZE)0, '\0', 0);

        y += 16;
    }
    return GuildMemberCount;
}

// GuildOverview_Render @ 0x0051DB00 — GuildOverview_Render
// Renders guild war overview panel: win/draw title, total score, kills, deaths.
// Uses GetTextExtentPointA for centering. unaff_EDI is the text width from
// the last GetTextExtentPointA call — approximated via lstrlenA * pixel_per_char.
// GuildMemberCount=0 → "draw" strings; !=0 → "win/loss" strings.
int __cdecl GuildOverview_Render(void)
{
    Gfx::Color3f(0.5f, 0.5f, 0.5f);
    const char *title, *subtitle;
    tagSIZE sz = {};
    HDC hdc = (HDC)(uintptr_t)gFont.GetTextDC();
    if (GuildMemberCount == 0) {
        title    = GuildWinTitleNoGuild;
        subtitle = GuildWinSubtitleNoGuild;
    } else {
        title    = GuildWinTitle;
        subtitle = GuildWinSubtitle;
    }
    int len = Platform::TextLength(title);
    gFont.MeasureText(hdc, title, len, &sz);
    int cx = (int)((unsigned int)(sz.cx * 0x280) / gWindow.GetWidth() >> 1);
    UI_RenderText(0x140 - cx, 0x46, title, (LPSIZE)0, '\0', 0);

    len = Platform::TextLength(subtitle);
    gFont.MeasureText(hdc, subtitle, len, &sz);
    cx = (int)((unsigned int)(sz.cx * 0x280) / gWindow.GetWidth() >> 1);
    UI_RenderText(0x140 - cx, 0x56, subtitle, (LPSIZE)0, '\0', 0);

    Gfx::Color3f(1.0f, 1.0f, 1.0f);
    m_dwTextColor = 0xffffffff;
    gFont.SelectFont(hdc, (HGDIOBJ)gFont.GetFont(FONT_BOLD));

    // Score total
    char buf[256];
    m_dwTextColor = 0xffd2ffd2;
    Platform::TextFormat(buf, &Fmt_ScoreTotal);
    len = Platform::TextLength(buf);
    gFont.MeasureText(hdc, buf, len, &sz);
    cx = (int)((unsigned int)(sz.cx * 0x280) / gWindow.GetWidth() >> 1);
    UI_RenderText(0x140 - cx, 0x6a, buf, (LPSIZE)0, '\0', 0);

    int iY = 0x82;
    if (GuildMemberCount != 0) {
        m_dwTextColor = 0xffd2d2ff;
        Platform::TextFormat(buf, &Fmt_OpponentDeaths, PlayerZen);
        len = Platform::TextLength(buf);
        gFont.MeasureText(hdc, buf, len, &sz);
        cx = (int)((unsigned int)(sz.cx * 0x280) / gWindow.GetWidth() >> 1);
        UI_RenderText(0x140 - cx, 0x82, buf, (LPSIZE)0, '\0', 0);
        iY = 0x9a;
    }
    m_dwTextColor = 0xffffd2d2;
    Platform::TextFormat(buf, &Fmt_OwnKills, Record0Score);
    len = Platform::TextLength(buf);
    gFont.MeasureText(hdc, buf, len, &sz);
    cx = (int)((unsigned int)(sz.cx * 0x280) / gWindow.GetWidth() >> 1);
    return (int)UI_RenderText(0x140 - cx, iY, buf, (LPSIZE)0, '\0', 0);
}

// IDA: FUN_0051D6F0 (0x0051D6F0)
// CreateOkMessageBox — show an OK dialog by setting the UI state.
// Wraps text at 7 chars / 0x26 lines into MessageBoxCustomBuf, sets a fixed panel descriptor,
// then transitions InGameUiState or InGameUiStateNext to state 0x8b.
void __cdecl CreateOkMessageBox(char *msg)
{
    MessageBoxLineCount = SeparateTextIntoLines(msg, (char *)&MessageBoxCustomBuf, 7, 0x26);
    // clear panel descriptor (10 dwords)
    int *p = (int*)&DialogButtonRects[0];
    for (int i = 0; i < 10; i++) p[i] = 0;
    // set fixed descriptor: {1, 0x47, 0x8c, 0x46, 0x15}
    DialogButtonRects[0] = 1;
    DialogButtonRects[1] = 0x47;
    DialogButtonRects[2] = 0x8c;
    DialogButtonRects[3] = 0x46;
    DialogButtonRects[4] = 0x15;
    if (InGameUiState != 0)
        InGameUiStateNext = 0x8b;
    else
        InGameUiState = 0x8b;
}

// GuildMemberList_Update @ 0x0051D9E0 — GuildMemberList_Update
// Sets UI state 0x8c, stores count/param2, copies menu descriptor and member list data.
void __cdecl GuildMemberList_Update(int count, int p2, void *data)
{
    if (InGameUiState == 0) InGameUiState = 0x8c;
    else                   InGameUiStateNext = 0x8c;
    GuildUiAuxParam = (DWORD)p2;
    GuildMemberCount = count;
    // Copy menu descriptor: 5 DWORDs
    DWORD desc[5] = { 1, 0x47, 0x104, 0x46, 0x15 };
    for (int i = 0; i < 5; i++) DialogButtonRects[i] = desc[i];
    // Copy member list data: count registros de GUILD_MEMBER_STRIDE bytes
    // (11 registros, ver globals.h).
    // IDA no acota `count` porque alli el hueco es exactamente ese; aca el
    // clamp evita que un `count` grande escriba sobre los globals vecinos.
    int nMembers = count;
    if (nMembers < 0) nMembers = 0;
    if (nMembers > GUILD_MEMBER_TABLE_BYTES / GUILD_MEMBER_STRIDE)
        nMembers = GUILD_MEMBER_TABLE_BYTES / GUILD_MEMBER_STRIDE;
    unsigned int dwords = (unsigned int)(nMembers * GUILD_MEMBER_STRIDE) >> 2;
    unsigned int *src = (unsigned int*)data;
    unsigned int *dst = (unsigned int*)&GuildMemberTable[0];
    for (unsigned int i = 0; i < dwords; i++) *dst++ = *src++;
}

// GuildMemberList_Add @ 0x0051DA80 — GuildMemberList_Add (single member record)
// Sets UI state 0x9a, stores param_1 as member count, copies 5-entry menu desc + 6 DWORDs data.
void __cdecl GuildMemberList_Add(int p1, void *data)
{
    if (InGameUiState == 0) InGameUiState = 0x9a;
    else                   InGameUiStateNext = 0x9a;
    GuildMemberCount = (int)(DWORD)p1;
    DWORD desc[5] = { 1, 0x47, 0x82, 0x46, 0x15 };
    for (int i = 0; i < 5; i++) DialogButtonRects[i] = desc[i];
    // Un registro (6 DWORDs en el binario; GUILD_MEMBER_STRIDE desde 0.97.40).
    memcpy(&GuildMemberTable[0], data, GUILD_MEMBER_STRIDE);
}

// ItemList_Select @ 0x0051D840 — ItemList_Select(slot)
// Selects character slot `slot` for the in-game item/skill list display.
// Sets g_iCurrentDialogScriptStorage, populates MessageBoxLineCount and skill/item display arrays,
// then transitions UI state to 0x8e.
// Port fiel de sub_51D840: arma el cuadro de dialogo desde g_DialogScript[a1]
// (DIALOG_SCRIPT en globals.h) igual que CSQuest::ShowDialogText, mas el
// memset de los rects de boton y ErrorMessage = 142.
int __cdecl ItemList_Select(int param_1) {
    char szText[72];

    if (param_1 < 0 || param_1 >= DIALOG_SCRIPT_COUNT) return 1;   // guard de port
    const DIALOG_SCRIPT *dlg = &g_DialogScript[param_1];

    g_iCurrentDialogScript = param_1;
    g_iNumLineMessageBoxCustom =
        SeparateTextIntoLines(dlg->m_lpszText, &MessageBoxCustomBuf[0], 7, 38);

    memset(DialogButtonRects, 0, 0x28);              // rects de los botones
    memset(g_lpszDialogAnswer, 0, sizeof(g_lpszDialogAnswer));

    int i = 0;
    g_iNumAnswer = 0;
    int nAnswer = dlg->m_iNumAnswer;
    if (nAnswer > 10) nAnswer = 10;             // la tabla tiene 10 slots
    if (nAnswer > 0) {
        char *dst = &g_lpszDialogAnswer[0][0][0];
        do {
            Platform::TextFormat(szText, "%d) %s", i + 1, dlg->m_lpszAnswer[i]);
            int nLine = SeparateTextIntoLines(szText, dst, 1, 38);
            if (nLine < 0) g_lpszDialogAnswer[i][nLine][0] = 0;
            ++i;
            ++g_iNumAnswer;
            dst += 38;
        } while (i < nAnswer);
    }
    if (dlg->m_iNumAnswer == 0) {
        Platform::TextFormat(szText, "%d) %s", i + 1, GlobalText[609]);
        g_iNumAnswer = 1;
        strcpy(&g_lpszDialogAnswer[0][0][0], szText);
    }

    // IDA: ErrorMessage = 142, o NextErrorMessage si ya hay un cartel arriba.
    if (InGameUiState != 0) InGameUiStateNext = 142;
    else                   InGameUiState = 142;
    return 1;
}
// CServerSelWin_UpdateWhileActive — implemented in src/Scene/Scene_ServerSelect_Input.cpp
