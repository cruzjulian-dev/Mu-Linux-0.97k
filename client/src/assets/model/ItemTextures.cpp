// IDA: OpenItemTextures @ 0x00508d10 — Model_AssignItemTexturePrefixes
// Carga las texturas de los modelos de items: [textures] de
// Content/item/models.toml.  Los slots de textura nuevos arrancan en 500.
#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "globals.h"
#include "functions.h"

// IDA: OpenItemTextures
void __cdecl Model_AssignItemTexturePrefixes(void)
{
    SetMaxTextures(500);
    AssetList_Load("item/models.toml", "textures");
}
