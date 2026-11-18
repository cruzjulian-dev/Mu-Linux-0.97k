// IDA: OpenPlayerTextures @ 0x00507610 — Model_LoadPlayerEquipmentTextures
// Loads player skin textures (slots 0x12d-0x133) and binds texture paths to
// all class/tier equipment model slots using OpenTexture (Model_LoadTextures).
// Also loads robe textures at the end.
// Monster models are handled by Monster_Data.cpp (Monster_LoadStartupData, IDA: Monster_Data_Load).
#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "globals.h"
#include "functions.h"

// IDA: OpenPlayerTextures
void __cdecl Model_LoadPlayerEquipmentTextures(void)
{
    // Initialise texture slot 0x12d (Barbarian skin)
    SetMaxTextures(0x12d);
    OpenJPG("Player\\skin_barbarian_01.jpg", 0x12d, 0x2600, 0x2900, 0, '\x01');
    TextureCurrent++;
    OpenJPG("Player\\level_man022.jpg",      0x12e, 0x2600, 0x2900, 0, '\x01');
    TextureCurrent++;
    OpenJPG("Player\\skin_wizard_01.jpg",    0x12f, 0x2600, 0x2900, 0, '\x01');
    TextureCurrent++;
    OpenJPG("Player\\level_man01.jpg",       0x130, 0x2600, 0x2900, 0, '\x01');
    TextureCurrent++;
    OpenJPG("Player\\skin_archer_01.jpg",    0x131, 0x2600, 0x2900, 0, '\x01');
    TextureCurrent++;
    OpenJPG("Player\\level_man033.jpg",      0x132, 0x2600, 0x2900, 0, '\x01');
    TextureCurrent++;
    OpenJPG("Player\\skin_special_01.jpg",   0x133, 0x2600, 0x2900, 0, '\x01');
    TextureCurrent++;

    // Texturas de los modelos del personaje y de la capa del Magic Gladiator:
    // [textures] de Content/player/models.toml.
    SetMaxTextures(0x136);
    AssetList_Load("player/models.toml", "textures");
}
