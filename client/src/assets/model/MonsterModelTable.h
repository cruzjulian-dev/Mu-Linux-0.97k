#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

// Cómo se arma el modelo de cada tipo de monstruo (Content/monster/models.toml).

// Campos de un modelo que se escriben después de cargarlo.
struct MonsterModelSlot {
    bool  HasSounds = false;
    short Sounds[5] = {};       // BMD::Sounds, contados desde el slot 170
    bool  HasBoneHead = false;
    int   BoneHead = 0;
    bool  HasStreamMesh = false;
    int   StreamMesh = 0;
};

struct MonsterModelTexture {
    int         Slot = 0;
    std::string File;           // .jpg -> OpenJPG, .tga -> OpenTGA
    int         Filter = 0;     // GL_NEAREST / GL_LINEAR
    int         Wrap = 0;       // GL_CLAMP / GL_REPEAT
};

struct MonsterModel {
    bool  HasSpeedScale = false;
    float SpeedScale = 1.0f;    // factor sobre las acciones 0..5
    std::vector<std::pair<int, float>> Speeds;  // (acción, velocidad), en orden
    std::vector<int> Loops;
    std::map<int, MonsterModelSlot> Slots;      // slot del modelo -> campos
    std::vector<MonsterModelTexture> Textures;
};

// [default]: velocidad de las acciones 0, 1, 2... y las que se repiten.
struct MonsterModelDefaults {
    std::vector<float> Speeds;
    std::vector<int>   Loops;
};

const MonsterModelDefaults& MonsterModel_Defaults();

// Definición del tipo de monstruo, o nullptr si no tiene sección.
const MonsterModel* MonsterModel_Get(int type);
