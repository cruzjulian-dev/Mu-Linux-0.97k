// IDA: OpenImages @ 0x0050eb80 — UI_LoadInterfaceTextures
// Texturas del inventario, la tienda, los botones y el escudo de guild:
// [images] de Content/interface/textures.toml.
#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "globals.h"
#include "functions.h"

// IDA: OpenImages
void __cdecl UI_LoadInterfaceTextures(void)
{
    TextureList_Load("images");   // Content/interface/textures.toml [images]
}
