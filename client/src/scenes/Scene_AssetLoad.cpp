// Scene_AssetLoad.cpp

#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "assets/text/ClientText.h"
#include "render/text/TextCore.h"
#include "globals.h"
#include "functions.h"

// IDA: OpenBasicData @ 0x00510320 — Scene_LoadGameAssets
// Carga los cursores, el HUD y las texturas de efectos ([basic] de
// Content/interface/textures.toml) y después los modelos, los datos y los sonidos.
void __cdecl Scene_LoadGameAssets(void) {
    CHAR local_64[100];
    TextureList_Load("basic");   // Content/interface/textures.toml [basic]
    // Model loaders
    Model_LoadPlayerAndItemMeshes(); Model_LoadPlayerEquipmentTextures(); Model_LoadItemMeshes();
    Model_AssignItemTexturePrefixes(); Model_LoadSkillEffectAssets(); UI_LoadInterfaceTextures(); Sound_LoadGameSamples();
    // Data/BMD loaders (locale-sensitive)
    if (DAT_0055a7c4 == '\0') {
        crt_sprintf(local_64, "Data2/Local/item_%s.txt", (char*)DAT_0055a7a4);
        Item_LoadTextData(local_64);
        Item_SaveBMD("Data/Local/Item.bmd");
        crt_sprintf(local_64, "Data2/Local/skill_%s.txt", (char*)DAT_0055a7a4);
        Skill_LoadTextData(local_64);
        Skill_SaveBMD("Data/Local/Skill.bmd");
        Gate_LoadTextData("Data2/Gate.txt");
        Gate_SaveBMD("Data/Gate.bmd");
        crt_sprintf(local_64, "Data2/Local/Filter_%s.txt", (char*)DAT_0055a7a4);
        Filter_LoadTextFile(local_64);
        Filter_SaveBMD("Data/Local/Filter.bmd");
        crt_sprintf(local_64, "Data2/Local/FilterName_%s.txt", (char*)DAT_0055a7a4);
        FilterName_LoadData(local_64);
        FilterName_SaveBMD("Data/Local/FilterName.bmd");
        // OJO: copy-paste sin corregir — esto carga Dialog_%s.txt con el loader
        // de FilterName y lo guarda encima de FilterName.bmd, o sea pisa el
        // archivo anterior y nunca produce Dialog.bmd.  Ademas IDA usa
        // "Data2\\Local\\Dialog(%s).txt" (parentesis, no guion bajo).  No se
        // corrige porque no existen Dialog_LoadData/Dialog_SaveBMD y toda esta
        // rama es codigo muerto: DAT_0055a7c4 ya vale 1 (igual que en el
        // binario), asi que el convertidor Data2 -> .bmd no corre nunca.
        crt_sprintf(local_64, "Data2/Local/Dialog_%s.txt", (char*)DAT_0055a7a4);
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
    crt_sprintf(local_64, "Data/Local/NPCName.txt");   // cliente: archivo sin sufijo de locale
    NPCName_LoadTextData(local_64);
    // DESVIACION (Fase 4, texto Unicode): las palabras de los filtros de chat y
    // de nombres (Filter.bmd / FilterName.bmd, 1000 x 20 bytes) a UTF-8;
    // lo que ya es UTF-8 se deja y el resto se
    // toma como Windows-1252 (NormalizeTextFileInPlace).
    for (int i = 0; i < 1000; ++i) {
        TextEngine::NormalizeTextFileInPlace(DAT_07d73104 + i * 0x14, 0x14);
        TextEngine::NormalizeTextFileInPlace(DAT_07d27610 + i * 0x14, 0x14);
    }
    // Sound preloads (tail of OpenBasicData @ 0x00510F65-0x00510FC5).
    // IDs y filenames verificados contra IDA Hex-Rays 00510320_OpenBasicData.c
    // y los binary patches del companion project (Mu-linux-97K/Source/Client/
    // Main/Sound.cpp:162-166). Ghidra había inventado los nombres
    // iPickUp/iGet/iDrop/iEquip — no existen como archivos .wav en Data/Sound.
    LoadWaveFile(4,    "Data\\Sound\\iTitle.wav",        1, '\0');
    LoadWaveFile(0x1b, "Data\\Sound\\iButtonMove.wav",   2, '\0');  // 27
    LoadWaveFile(0x19, "Data\\Sound\\iButtonClick.wav",  1, '\0');  // 25
    LoadWaveFile(0x1a, "Data\\Sound\\iButtonError.wav",  1, '\0');  // 26
    LoadWaveFile(0x1c, "Data\\Sound\\iCreateWindow.wav", 1, '\0');  // 28
    LoadWaveFile(0x25, "Data\\Sound\\iRepair.wav",       1, '\0');  // 37
    LoadWaveFile(0x26, "Data\\Sound\\iWhisper.wav",      1, '\0');  // 38
}
// Monster_LoadStartupData (IDA: Monster_Data_Load) — implemented in src/assets/data/Monster_Data.cpp
// GL_CaptureScreenshot — implemented in src/render/GL_State.cpp
// GL_GetModelViewMatrix — implemented in src/render/GL_State.cpp
