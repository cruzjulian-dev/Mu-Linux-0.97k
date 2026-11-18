// IDA: OpenSkills @ 0x0050b710 — Model_LoadSkillEffectAssets
// Carga los modelos de los efectos de skills y sus texturas (más las flores):
// Content/skill/models.toml.
//
// Diferencias con el código anterior, sin efecto: salen los DAT_0055a7c4 = 1
// (ya valía 1) y las OpenTexture de los slots 259 y 266 que iban antes de
// cargar su modelo (con el slot vacío no hacían nada; sus texturas las carga
// [textures]).
#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "globals.h"
#include "functions.h"

// IDA: OpenSkills
void __cdecl Model_LoadSkillEffectAssets(void)
{
    AssetList_Load("skill/models.toml", "models");
    SetMaxTextures(700);
    AssetList_Load("skill/models.toml", "textures");
}
