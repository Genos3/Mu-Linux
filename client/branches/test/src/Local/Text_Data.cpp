// Text_Data.cpp — localized string pool loader.
// Port of:
//   OpenTextData   @ 0x00510FE0
//   sub_479830     @ 0x00479830  (binary Text.bmd loader)
//
// The binary Text.bmd is 300000 bytes = 1000 entries × 300 bytes, XOR'd with
// the 3-byte key {0xFC, 0xCF, 0xAB} (BuxConvert_0 / BuxConvert_0).
//
// Call tree:
//   WinMain → OpenTextData() → LoadTextData_Bin("Data\\Local\\Text.bmd")
//                           → fopen → fread(300000) → BuxConvert_0 → memcpy → fclose
//
// If the file is missing, original sends WM_QUIT.  Here we log + continue with
// an empty pool (UI strings just render blank rather than killing the client).

#include "stdafx.h"
#include "Platform/Text.h"
#include "Platform/IFileSystem.h"
#include "Local/ClientText.h"
#include "Text/TextCore.h"
#include "globals.h"
#include "functions.h"

#include <cstdio>
#include <cstring>

extern "C" { void DbgLogPublic(const char *msg); }

// ─────────────────────────────────────────────────────────────────────────────
// LoadTextData_Bin — port of sub_479830 @ 0x00479830
// ─────────────────────────────────────────────────────────────────────────────
int __cdecl LoadTextData_Bin(const char *FileName)
{
    FILE *fp = Platform::GetFileSystem().Open(FileName, "rb");
    if (!fp) {
        char msg[256];
        Platform::TextFormat(msg, "OpenTextData: %s file not found", FileName);
        DbgLogPublic(msg);
        return 0;
    }

    // Read the full 300000-byte blob into a scratch buffer, XOR-decrypt, then
    // copy into GlobalText[].  Mirrors the IDA layout: the .bmd is literally
    // the in-memory GlobalText array XOR'd with BuxConvert_0.
    // OJO: NO usar sizeof(GlobalText) — el array esta sobredimensionado a
    // GLOBALTEXT_ROWS para absorber indices heredados; el archivo solo trae
    // GLOBALTEXT_ROWS_FILE filas.
    const size_t poolBytes = (size_t)GLOBALTEXT_ROWS_FILE * 300;   // 300000
    BYTE *buf = (BYTE *)malloc(poolBytes);
    if (!buf) {
        DbgLogPublic("OpenTextData: malloc failed");
        fclose(fp);
        return 0;
    }

    // DESVIACION (Fase 4): el 0.97k exige el tamaño exacto. Acá un archivo
    // más corto deja vacías las filas que faltan y uno más largo se ignora
    // a partir de la fila 1000.
    memset(buf, 0, poolBytes);
    size_t got = fread(buf, 1, poolBytes, fp);
    if (got != poolBytes) {
        char msg[256];
        Platform::TextFormat(msg, "OpenTextData: %s trae %u bytes de %u; se completan filas vacias",
                             FileName, (unsigned)got, (unsigned)poolBytes);
        DbgLogPublic(msg);
    }

    // XOR-decrypt in place with the 3-byte rolling key (BuxConvert_0).
    BuxConvert_0(buf, (int)got);

    // Copy decrypted pool into GlobalText.  Each 300-byte row is already a
    // null-terminated string in the file; we don't truncate or reformat.
    memcpy(GlobalText, buf, poolBytes);
    // DESVIACION (Fase 4, texto Unicode): en memoria el texto del cliente es
    // UTF-8. El archivo original está en Windows-1252, pero una traducción puede
    // venir en UTF-8: lo que ya es UTF-8 se deja y el resto se
    // toma como Windows-1252 (NormalizeTextFileInPlace).
    for (int row = 0; row < GLOBALTEXT_ROWS_FILE; ++row)
        TextEngine::NormalizeTextFileInPlace(GlobalText[row], 300);

    free(buf);
    fclose(fp);

    return 1;
}

// ─────────────────────────────────────────────────────────────────────────────
// OpenTextData — port of 0x00510FE0
// ─────────────────────────────────────────────────────────────────────────────
// Original:
//   if (0x0055A7C4)            // == g_tex_ext_mode, set to 1 in our build
//       sub_479830("Data\\Local\\Text.bmd");
//       OpenMacro("Data\\Macro.txt");     // key-macros file
//   else
//       sub_4797B0("Data2\\Local\\Text(%s).txt", dword_55C9E50);   // text form
//       sub_479950(...);
//
// g_tex_ext_mode == 1 in our build (see globals.cpp), so we take the
// binary-file branch.
void __cdecl OpenTextData(void)
{
    if (g_tex_ext_mode) {
        // DESVIACION (DLL Language.cpp): Text_<idioma>.bmd si hay selección.
        char path[MAX_PATH];
        LoadTextData_Bin(LocalizedDataPath("Data\\Local\\Text", ".bmd", path, sizeof(path)));
        OpenMacro("Data\\Macro.txt");   // macros de chat (Alt+1..0)
    } else {
        // Plain-text branch — not taken in our build, but implement for parity.
        // sub_4797B0 tokenises "index value" pairs out of Text(xxx).txt.
        DbgLogPublic("OpenTextData: plain-text branch requested but not active");
    }

    // ── ESC-menu string buffer sync (IDA 0051AF50 L175-212 / L233-262) ───────
    // The original binary resolves the ESC-menu button labels inline from
    // GlobalText[381..388].  Our Ghidra port materialised them as six
    // distinct 300-byte buffers (DlgText6E_0..07d463d4) with Spanish
    // placeholders.  After loading Text.bmd we copy the real localized
    // strings into those buffers so the menu picks them up unchanged.
    if (GlobalText[381][0]) Platform::TextCopy(DlgText6E_0, GlobalText[381], 300);  // title ("Salir del juego")
    if (GlobalText[382][0]) Platform::TextCopy(DlgText6E_1, GlobalText[382], 300);  // "Elegir servidor"
    if (GlobalText[383][0]) Platform::TextCopy(DlgText6E_2, GlobalText[383], 300);  // "Cambiar personaje"
    if (GlobalText[384][0]) Platform::TextCopy(DlgText6E_3, GlobalText[384], 300);  // "Cancelar"
    if (GlobalText[385][0]) Platform::TextCopy(DlgText6E_4, GlobalText[385], 300);  // "Opciones"
    if (GlobalText[388][0]) Platform::TextCopy(DlgText96_3, GlobalText[388], 300);  // "Cerrar"

    // Options-panel On/Off labels — original formats them at render time via
    //   sprintf(buf, "%s  On"/"%s  Off", GlobalText[386/387]).
    // Our buffers Txt_AutoAttackOn / Txt_AutoAttackOff etc are 32 bytes each,
    // so pre-compose once to keep the render code path unchanged.
    if (GlobalText[386][0]) {
        Platform::TextFormat(Txt_AutoAttackOn,  "%s  On",  GlobalText[386]);  // Auto-attack
        Platform::TextFormat(Txt_AutoAttackOff, "%s  Off", GlobalText[386]);
    }
    if (GlobalText[387][0]) {
        Platform::TextFormat(Txt_WhisperOn,  "%s  On",  GlobalText[387]);  // Whisper sound
        Platform::TextFormat(Txt_WhisperOff, "%s  Off", GlobalText[387]);
    }

    // ── Dialog/error message strings (IDA 0x07d4662c .. 0x07d68cf4) ──────────
    // En el binario original estas direcciones son slots de 300 bytes dentro
    // del pool GlobalText[] (cargado desde Text.bmd). Ghidra las partió en
    // `char X = 0;` independientes → cartel de error de login salía VACÍO
    // (UI_StatsPanel_Render lee `&lpString_07dXXXX` como LPCSTR y siempre
    // encontraba 0x00 → strlen=0 → no renderizaba texto).
    //
    // Base=0x07d45ba0 = GlobalText[381]; stride=300 (0x12c).
    // Índice GT = 381 + (addr - 0x07d45ba0) / 300.
    if (GlobalText[390][0]) Platform::TextCopy(DlgText7A, GlobalText[390], 300);
    if (GlobalText[391][0]) Platform::TextCopy(DlgText7B, GlobalText[391], 300);
    if (GlobalText[392][0]) Platform::TextCopy(DlgText73_0, GlobalText[392], 300);
    if (GlobalText[393][0]) Platform::TextCopy(DlgText73_1, GlobalText[393], 300);
    if (GlobalText[394][0]) Platform::TextCopy(DlgText36_0, GlobalText[394], 300);
    if (GlobalText[395][0]) Platform::TextCopy(DlgText36_1, GlobalText[395], 300);
    if (GlobalText[396][0]) Platform::TextCopy(DlgText37, GlobalText[396], 300);
    if (GlobalText[398][0]) Platform::TextCopy(DlgText72_1, GlobalText[398], 300);
    if (GlobalText[399][0]) Platform::TextCopy(DlgText84_0, GlobalText[399], 300);
    if (GlobalText[400][0]) Platform::TextCopy(DlgText84_1, GlobalText[400], 300);
    if (GlobalText[401][0]) Platform::TextCopy(DlgText8A, GlobalText[401], 300);
    if (GlobalText[402][0]) Platform::TextCopy(DlgText71, GlobalText[402], 300);
    if (GlobalText[403][0]) Platform::TextCopy(DlgText64, GlobalText[403], 300);
    if (GlobalText[404][0]) Platform::TextCopy(DlgText65, GlobalText[404], 300);
    if (GlobalText[405][0]) Platform::TextCopy(DlgText1A_0, GlobalText[405], 300);
    if (GlobalText[406][0]) Platform::TextCopy(DlgText1A_1, GlobalText[406], 300);
    if (GlobalText[407][0]) Platform::TextCopy(DlgText15, GlobalText[407], 300);
    if (GlobalText[408][0]) Platform::TextCopy(DlgText1B, GlobalText[408], 300);
    if (GlobalText[409][0]) Platform::TextCopy(DlgText1C, GlobalText[409], 300);
    if (GlobalText[410][0]) Platform::TextCopy(DlgText1D, GlobalText[410], 300);
    if (GlobalText[411][0]) Platform::TextCopy(DlgText1E, GlobalText[411], 300);
    if (GlobalText[412][0]) Platform::TextCopy(DlgText1F, GlobalText[412], 300);
    if (GlobalText[413][0]) Platform::TextCopy(DlgText20, GlobalText[413], 300);
    if (GlobalText[414][0]) Platform::TextCopy(DlgText16, GlobalText[414], 300);
    if (GlobalText[415][0]) Platform::TextCopy(DlgText17, GlobalText[415], 300);
    if (GlobalText[416][0]) Platform::TextCopy(DlgText18, GlobalText[416], 300);
    if (GlobalText[417][0]) Platform::TextCopy(DlgText19, GlobalText[417], 300);
    if (GlobalText[418][0]) Platform::TextCopy(PartyRequestFormatA, GlobalText[418], 300);
    if (GlobalText[420][0]) Platform::TextCopy(DlgText74_0, GlobalText[420], 300);
    if (GlobalText[421][0]) Platform::TextCopy(DlgText74_1, GlobalText[421], 300);
    if (GlobalText[422][0]) Platform::TextCopy(DlgText74_2, GlobalText[422], 300);
    if (GlobalText[423][0]) Platform::TextCopy(DlgText75, GlobalText[423], 300);
    if (GlobalText[424][0]) Platform::TextCopy(DlgText76, GlobalText[424], 300);
    if (GlobalText[425][0]) Platform::TextCopy(PartyRequestFormatB, GlobalText[425], 300);
    if (GlobalText[426][0]) Platform::TextCopy(DlgText7F, GlobalText[426], 300);
    if (GlobalText[427][0]) Platform::TextCopy(DlgText7E_0, GlobalText[427], 300);
    if (GlobalText[428][0]) Platform::TextCopy(DlgText7E_1, GlobalText[428], 300);
    if (GlobalText[433][0]) Platform::TextCopy(DlgText25, GlobalText[433], 300);
    if (GlobalText[434][0]) Platform::TextCopy(DlgText85, GlobalText[434], 300);
    if (GlobalText[435][0]) Platform::TextCopy(DlgText26, GlobalText[435], 300);
    if (GlobalText[438][0]) Platform::TextCopy(DlgText3A_0, GlobalText[438], 300);
    if (GlobalText[439][0]) Platform::TextCopy(DlgText3A_3, GlobalText[439], 300);
    if (GlobalText[440][0]) Platform::TextCopy(DlgText86, GlobalText[440], 300);
    if (GlobalText[441][0]) Platform::TextCopy(DlgText87, GlobalText[441], 300);
    if (GlobalText[442][0]) Platform::TextCopy(DlgText88, GlobalText[442], 300);
    if (GlobalText[443][0]) Platform::TextCopy(DlgText98_0, GlobalText[443], 300);
    if (GlobalText[444][0]) Platform::TextCopy(DlgText98_1, GlobalText[444], 300);
    if (GlobalText[445][0]) Platform::TextCopy(DlgText89, GlobalText[445], 300);
    if (GlobalText[597][0]) Platform::TextCopy(DlgText22, GlobalText[597], 300);
    if (GlobalText[598][0]) Platform::TextCopy(DlgText23, GlobalText[598], 300);
    if (GlobalText[599][0]) Platform::TextCopy(DlgText24, GlobalText[599], 300);
    if (GlobalText[734][0]) Platform::TextCopy(DlgText8F, GlobalText[734], 300);
    if (GlobalText[857][0]) Platform::TextCopy(GuildWinTitle, GlobalText[857], 300);
    if (GlobalText[858][0]) Platform::TextCopy(GuildWinSubtitle, GlobalText[858], 300);
    if (GlobalText[859][0]) Platform::TextCopy(GuildWinTitleNoGuild, GlobalText[859], 300);
    if (GlobalText[860][0]) Platform::TextCopy(GuildWinSubtitleNoGuild, GlobalText[860], 300);
}
