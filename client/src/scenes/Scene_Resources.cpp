// Scene_Resources.cpp
// Texture and model resource loading/unloading for login/account/char-select scenes.
//
// OpenLogoSceneData @ 0x0050fcf0 — Scene_LoadAccountResources
// OpenCharacterSceneData @ 0x0050ff50 — Scene_LoadCharSelectResources
// ReleaseLogoSceneData @ 0x0050ff10 — Scene_UnloadAccountResources
// IDA: ReleaseCharacterSceneData (0x005102C0)

#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "render/MeshDraw.h"
#include <vector>
#include <algorithm>

namespace {
bool selectBackdropWide = false;
std::vector<int> selectBackdropTextures;

const char* SelectBackdropPath()
{
  return gWindow.IsWide() ? "Data\\Logo\\Wide\\" : "Data\\Logo\\";
}
}

void Scene_RefreshCharSelectBackdrop()
{
  if (selectBackdropWide == gWindow.IsWide()) return;
  BMD* model = AsModel(Models) + 0xa4;
  // Mantener las texturas de ambas variantes durante esta escena. El modelo
  // comparte texturas con otras mallas y puede alternar repetidas veces.
  const int count = model->NumMeshs;
  const short* textures = model->IndexTexture;
  for (int i = 0; i < count; ++i) {
    const int id = textures[i];
    if (std::find(selectBackdropTextures.begin(), selectBackdropTextures.end(), id)
        == selectBackdropTextures.end()) {
      ++g_BitmapsRaw[id * TEXTURE_STRIDE + 0x30];
      selectBackdropTextures.push_back(id);
    }
  }
  // Las mallas residentes guardan punteros a la topología que se libera.
  MeshDraw::Release();
  BMD__Release(model);
  AccessModel(0xa4,SelectBackdropPath(),(const char*)&DAT_005606ac,5);
  OpenTexture(0xa4,"Logo\\",0x2601,'\x01');
  selectBackdropWide = gWindow.IsWide();
}


// OpenLogoSceneData — Scene_LoadAccountResources
// Loads textures and 3D models for the Account Creation / New Character screens.
// Textures: Logo_0Account_new.tga, On_Botton.jpg, etc. (slots 0xc–0x14).
// Models: ship.smd (0x3c), logo objects (0xa0–0xa3).
// Preloads model data and activates rendering for slots 0x2601.
// IDA: FUN_0050FCF0
void Scene_LoadAccountResources(void)
{
  int iVar1;
  int iVar2;

  TextureList_Load("account");   // Content/interface/textures.toml [account]
  AccessModel(0x3c,"Data\\Object1\\",(const char*)&DAT_0055e834,1);
  iVar2 = 0xa0;
  do {
    AccessModel(iVar2,"Data\\Logo\\",(const char*)&DAT_005606ac,iVar2 + -0x9f);
    iVar1 = iVar2 + -0x9f;
    iVar2 = iVar2 + 1;
  } while (iVar1 < 4);
  SetMaxTextures(0x56a);
  OpenTexture(0x3c,"Object1\\",0x2601,'\x01');
  iVar2 = 0;
  do {
    OpenTexture(iVar2 + 0xa0,"Logo\\",0x2601,'\x01');
    iVar2 = iVar2 + 1;
  } while (iVar2 < 4);
  return;
}


// OpenCharacterSceneData — Scene_LoadCharSelectResources
// Loads textures and 3D models for the Character Selection screen.
// Textures: Logo_Interface01–04.tga, New_Character201.tga, etc. (slots 0xc–0x1d).
// Models: main.smd (0xa4), warrior/fairy/dark/etc. (0xaa–0xad).
// Also sets scale for character preview models (0xbc–0x2f0) from DAT_05828d58.
// IDA: FUN_0050FF50
void Scene_LoadCharSelectResources(void)
{
  int iVar1;
  intptr_t iVar2;

  TextureList_Load("char_select");   // Content/interface/textures.toml [char_select]
  // El diorama clásico termina en los bordes del encuadre 4:3. La variante
  // amplia agrega suelo y bosque lateral para el viewport real del mundo.
  AccessModel(0xa4,SelectBackdropPath(),(const char*)&DAT_005606ac,5);
  selectBackdropWide = gWindow.IsWide();
  iVar2 = 0xaa;
  do {
    AccessModel(iVar2,"Data\\Logo\\",(const char*)&DAT_005607c0,iVar2 + -0xa9);
    iVar1 = iVar2 + -0xa9;
    iVar2 = iVar2 + 1;
  } while (iVar1 < 4);
  SetMaxTextures(0x56a);
  OpenTexture(0xa4,"Logo\\",0x2601,'\x01');
  iVar2 = 0;
  do {
    OpenTexture(iVar2 + 0xaa,"Logo\\",0x2600,'\x01');
    iVar2 = iVar2 + 1;
  } while (iVar2 < 4);
  iVar2 = 0;
  do {
    iVar1 = iVar2 + 0xbc;
    ModelActionSpeed(AsModel(Models + BMD_STRIDE * (170 + iVar2 / 0xbc))->Actions, 0) = 0.3f;
    ModelActionSpeed(AsModel(Models + BMD_STRIDE * (170 + iVar2 / 0xbc))->Actions, 1) = 0.3f;
    iVar2 = iVar1;
  } while (iVar1 < 0x2f0);
  return;
}


// ReleaseLogoSceneData — Scene_UnloadAccountResources
// Frees texture slots 0xc–0x14 and 0x56a–0x5a9 used by the account scene,
// then calls DeleteObjects (clear model cache) and ClearCharacters(-1) (reset entities).
// IDA: FUN_0050FF10
void Scene_UnloadAccountResources(void)
{
  int iVar1;

  iVar1 = 0;
  do {
    UnloadImage(iVar1 + 0xc);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 9);
  iVar1 = 0x56a;
  do {
    UnloadImage(iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x5aa);
  DeleteObjects();
  ClearCharacters(-1);
  return;
}


// IDA: ReleaseCharacterSceneData (0x005102C0)
// Frees texture slots 0xc–0x1d and 0x56a–0x5a9 used by the char-select scene,
// clears model data for slots 0x7cd8–0x7fc8 (stride 0xbc),
// then calls DeleteObjects and ClearCharacters(-1).
// IDA: ReleaseCharacterSceneData (0x005102C0)
void ReleaseCharacterSceneData(void)
{
  int iVar1;

  for (int texture : selectBackdropTextures) UnloadImage(texture);
  selectBackdropTextures.clear();

  iVar1 = 0;
  do {
    UnloadImage(iVar1 + 0xc);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x12);
  iVar1 = 0x56a;
  do {
    UnloadImage(iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x5aa);
  DeleteObjects();
  for (int slot = 0xaa; slot < 0xae; ++slot)
    BMD__Release(AsModel(Models) + slot);
  ClearCharacters(-1);
  return;
}
