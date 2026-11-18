#include <cstdint>
// Monster_Data.cpp
// Monster_Data_Load @ 0x00511060
//
// Loads monster script / base data files.
// Only executed if DAT_083a410c != 0 (multiplayer / server-mode flag).
//
// Sub-functions:
//   Monster_LoadScriptTable — load monster script table (enable/disable state via DAT_0055a7c4)
//   Monster_ParseSetBase2 — parse and load "Data2/MonsterSetBase2.txt" monster definitions
//
// Globals:
//   DAT_083a410c   — if non-zero: server/party-enabled mode
//   DAT_0055a7c4   — loader phase flag (0 = pre-load, 1 = post-load)
//   s_Data2_MonsterSetBase2_txt (0x00561530) — filename literal

#include "stdafx.h"
#include "audio/SoundTable.h"
#include "assets/model/MonsterModelTable.h"

#include <cctype>
#include <string>

// IDA: Monster_Data_Load
void __cdecl Monster_LoadStartupData(void)
{
    if (DAT_083a410c != '\0') {
        DAT_0055a7c4 = 0;
        Monster_LoadScriptTable();           // load monster script table
        DAT_0055a7c4 = 1;
        if (DAT_083a410c != '\0') {
            Monster_ParseSetBase2("Data2/MonsterSetBase2.txt");  // load MonsterSetBase2.txt
        }
    }
    return;
}


// FUN_0050b510 @ 0x0050b510 — Monster_LoadScriptTable
//
// Monster_LoadScriptTable — loads the monster script/model table.
//
// Calls SetMaxTextures to initialize the table with 0x41a entries.
// Then, only in pre-load phase (DAT_0055a7c4 == 0), registers BMD model
// files from Data2/Monster/ for monster types 0x13d–0x14c and invokes
// OpenMonsterModel for monster IDs 0x32–0x3e to set up their sound+animation
// data.
//
// Globals:
//   DAT_0055a7c4               — loader phase (0 = pre-load)
//   s_Data2_Monster__0055e06c  — "Data2/Monster/" path prefix
//   s_swordbasic_smd_0055e07c  — "swordbasic.smd" filename

void Monster_LoadScriptTable(void)

{
  int iVar1;

  SetMaxTextures(0x41a);
  if (DAT_0055a7c4 == '\0') {
    iVar1 = 0x32;
    do {
      OpenMonsterModel(iVar1);
      iVar1 = iVar1 + 1;
    } while (iVar1 < 0x3f);
  }
  return;
}


// FUN_0047d020 @ 0x0047d020 — Monster_ParseSetBase2
//
// Monster_ParseSetBase2 — parses "Data2/MonsterSetBase2.txt" to
// populate the monster spawn table.
//
// Opens the file via crt_fopen, then reads records with TextParser_GetToken.
// For each record:
//   - field 0 (ftol) = monster type
//   - fields 1,2,3  = skipped
//   - field 4 (ftol) = spawn X
//   - field 5        = skipped
//   - field 6 (ftol) = spawn Y
//   - Stops on record type 2 or when the sentinel string (DAT_00559088)
//     matches TextParserTokenString.
// Creates the monster via CreateMonster and sets field +0x84 = ' '.
//
// Globals:
//   SMDFile_0  — file handle
//   DAT_07d7807c  — next spawn slot index
//   HeroKey  — max spawn slot (skips if equal)
//   DAT_00559088  — sentinel string "END"

void __cdecl Monster_ParseSetBase2(LPCSTR param_1)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  char *pcVar4;
  byte *pbVar5;
  bool bVar6;
  longlong lVar7;
  longlong lVar8;
  longlong lVar9;

  SMDFile_0 = (FILE *)crt_fopen(param_1,DAT_005580ac);
  if (SMDFile_0 != (FILE *)0x0) {
    TextParser_GetToken();
    while( true ) {
      iVar2 = TextParser_GetToken();
      lVar7 = (longlong)ParserTokenNumber;   // IDA sub_47D020: (std::int64_t)TokenNumber
      if (iVar2 == 2) break;
      if (iVar2 == 0) {
        pbVar5 = (byte*)TokenString;
        pbVar3 = (byte*)&DAT_00559088;
        do {
          bVar1 = *pbVar3;
          bVar6 = bVar1 < *pbVar5;
          if (bVar1 != *pbVar5) {
LAB_0047d096:
            iVar2 = (1 - (uint)bVar6) - (uint)(bVar6 != 0);
            goto LAB_0047d09b;
          }
          if (bVar1 == 0) break;
          bVar1 = pbVar3[1];
          bVar6 = bVar1 < pbVar5[1];
          if (bVar1 != pbVar5[1]) goto LAB_0047d096;
          pbVar3 = pbVar3 + 2;
          pbVar5 = pbVar5 + 2;
        } while (bVar1 != 0);
        iVar2 = 0;
LAB_0047d09b:
        if (iVar2 == 0) break;
      }
      TextParser_GetToken();
      TextParser_GetToken();
      TextParser_GetToken();
      lVar8 = (longlong)ParserTokenNumber;   // IDA sub_47D020: (std::int64_t)TokenNumber
      TextParser_GetToken();
      lVar9 = (longlong)ParserTokenNumber;   // IDA sub_47D020: (std::int64_t)TokenNumber
      TextParser_GetToken();
      if (DAT_07d7807c == HeroKey) {
        DAT_07d7807c = DAT_07d7807c + 1;
      }
      iVar2 = DAT_07d7807c;
      DAT_07d7807c = DAT_07d7807c + 1;
      pcVar4 = CreateMonster((uint)lVar7,(int)lVar8,(int)lVar9,iVar2,0);
      if (pcVar4 != (char *)0x0) {
        pcVar4[0x84] = ' ';
      }
    }
    crt_fclose(SMDFile_0);
  }
  return;
}


// IDA: OpenMonsterModel (0x005098C0)
//
// Carga el modelo del monstruo de tipo Type en el slot 270 + Type y lo deja
// listo para animar: velocidades y repetición de las acciones, los cinco
// sonidos del modelo, el hueso de la cabeza y alguna textura extra.  Esos datos
// salen de Content/monster/models.toml; los sonidos en sí, de [monster.N] en
// Content/audio/sound/sounds.toml.
//
// Sólo carga si es la primera vez (DAT_0055a7c4 == 0) o si el modelo todavía no
// tiene acciones ni mallas, y sólo arma el resto si el .bmd trajo mallas.
// El orden es el del binario: velocidades por defecto, factor sobre las
// acciones 0..5, velocidades propias del tipo, sonidos y por último los campos
// del modelo y las texturas extra.
void __cdecl OpenMonsterModel(int Type)
{
  const int modelId = Type + 270;
  BMD* model = AsModel(Models + (intptr_t)modelId * BMD_STRIDE);
  if (DAT_0055a7c4 != '\0' && (model->NumActions >= 1 || model->NumMeshs >= 1))
    return;
  AccessModel(modelId, "Data/Monster/", "Monster", Type + 1);
  if (model->NumMeshs == 0)
    return;
  OpenTexture(modelId, "Monster/", 0x2600, '\x01');

  void* actions = model->Actions;
  auto loop = [actions](int action) -> std::uint8_t& {
    return *(static_cast<std::uint8_t*>(actions) + action * sizeof(ModelAnimationAction));
  };
  const MonsterModelDefaults& defaults = MonsterModel_Defaults();
  for (size_t action = 0; action < defaults.Speeds.size(); action++)
    ModelActionSpeed(actions, (int)action) = defaults.Speeds[action];
  for (int action : defaults.Loops)
    loop(action) = 1;

  const MonsterModel* type = MonsterModel_Get(Type);
  if (type) {
    if (type->HasSpeedScale)
      for (int action = 0; action < 6; action++)
        ModelActionSpeed(actions, action) = ModelActionSpeed(actions, action) * type->SpeedScale;
    for (const auto& speed : type->Speeds)
      ModelActionSpeed(actions, speed.first) = speed.second;
    for (int action : type->Loops)
      loop(action) = 1;
  }

  SoundTable_LoadMonster(Type);
  if (!type)
    return;

  for (const auto& entry : type->Slots) {
    BMD* target = AsModel(Models + (intptr_t)entry.first * BMD_STRIDE);
    const MonsterModelSlot& slot = entry.second;
    if (slot.HasSounds)
      Model_SetAnimationSlots(entry.first, slot.Sounds[0], slot.Sounds[1], slot.Sounds[2],
                              slot.Sounds[3], slot.Sounds[4]);
    if (slot.HasBoneHead) target->BoneHead = slot.BoneHead;
    if (slot.HasStreamMesh) target->StreamMesh = (char)slot.StreamMesh;
  }
  for (const auto& texture : type->Textures) {
    const auto dot = texture.File.rfind('.');
    std::string ext = dot == std::string::npos ? std::string() : texture.File.substr(dot);
    for (char& c : ext) c = (char)std::tolower((unsigned char)c);
    const bool tga = ext == ".tga";
    if (tga)
      OpenTGA(texture.File.c_str(), texture.Slot, texture.Filter, texture.Wrap, 0, '\x01');
    else
      OpenJPG(texture.File.c_str(), texture.Slot, texture.Filter, texture.Wrap, 0, '\x01');
  }
}
