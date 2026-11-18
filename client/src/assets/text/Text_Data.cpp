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
#include "platform/UserPath.h"
#include "platform/Text.h"
#include "platform/IFileSystem.h"
#include "assets/text/ClientText.h"
#include "render/text/TextCore.h"
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
//   if (unk_55A7C4)            // == DAT_0055a7c4, set to 1 in our build
//       sub_479830("Data\\Local\\Text.bmd");
//       OpenMacro("Data\\Macro.txt");     // key-macros file
//   else
//       sub_4797B0("Data2\\Local\\Text(%s).txt", dword_55C9E50);   // text form
//       sub_479950(...);
//
// DAT_0055a7c4 == 1 in our build (see globals.cpp), so we take the
// binary-file branch.
void __cdecl OpenTextData(void)
{
    if (DAT_0055a7c4) {
        // DESVIACION (DLL Language.cpp): Text_<idioma>.bmd si hay selección.
        char path[MAX_PATH];
        LoadTextData_Bin(LocalizedDataPath("Data\\Local\\Text", ".bmd", path, sizeof(path)));
        OpenMacro(const_cast<char*>(Platform::UserMacroPath()));   // macros de chat (Alt+1..0); Fase 5: User/
    } else {
        // Plain-text branch — not taken in our build, but implement for parity.
        // sub_4797B0 tokenises "index value" pairs out of Text(xxx).txt.
        DbgLogPublic("OpenTextData: plain-text branch requested but not active");
    }

    // Options-panel On/Off labels — original formats them at render time via
    //   sprintf(buf, "%s  On"/"%s  Off", GlobalText[386/387]).
    // Our buffers s__s_On_0056184c / s__s_Off_00561854 etc are 32 bytes each,
    // so pre-compose once to keep the render code path unchanged.
    if (GlobalText[386][0]) {
        Platform::TextFormat(s__s_On_0056184c,  "%s  On",  GlobalText[386]);  // Auto-attack
        Platform::TextFormat(s__s_Off_00561854, "%s  Off", GlobalText[386]);
    }
    if (GlobalText[387][0]) {
        Platform::TextFormat(s__s_On_0056185c,  "%s  On",  GlobalText[387]);  // Whisper sound
        Platform::TextFormat(s__s_Off_00561864, "%s  Off", GlobalText[387]);
    }
}
