// CharacterVisual_Models.cpp
// Efectos ambientales por modelo de monstruo y NPC: el switch por tipo de
// MoveCharacterVisual (IDA 0x004520C0, L764-1342).
//
// Los patrones que se repiten van como tablas (textura que se desplaza, huesos
// que se copian a los puntos del arma, sonido al azar) y el resto queda como
// una función por modelo.  El orden en que corre cada cosa es el del binario:
// para un mismo modelo la tabla se aplica antes que su función, igual que en
// el case original, y ninguna tabla consume rand().
//
// Los nombres salen de Content/monster (MonsterNN.bmd = modelo 0x10E + NN - 1)
// y de Content/npc/models.toml.
#include <cstdint>
#include "stdafx.h"
#include "game/core/MapManager.h"
#include "structs.h"
#include "globals.h"
#include "functions.h"
#include "game/entity/CharacterVisual_Models.h"

extern void __cdecl AddTerrainLight(float x, float y, float* light, int range, float* buffer);
extern int __cdecl FloatingLabel_Add(intptr_t a1, DWORD *a2, int a3, DWORD *a4, int a5);

namespace {

// ── Tablas ───────────────────────────────────────────────────────────────────

// Textura de la malla de blend que se desplaza con WorldTime:
//     coord = (WorldTime % period) * step
struct TexScroll { short type; bool v; int period; float step; };
constexpr TexScroll kTexScroll[] = {
    { 0x11D, true,   2000, -0.00050000002f },  // Ice Monster
    { 0x128, false, 10000, -0.000099999997f }, // Devil
    { 0x129, true,   1000, -0.001f },          // Balrog
    { 0x142, false, 10000, -0.00039999999f },  // Mega Crust
    { 0x145, true,  10000,  0.000099999997f }, // Phoenix of Darkness
};

// Dos huesos que se copian a los puntos del arma de la entidad (+76 y +64);
// los usan el trail y los efectos del ataque.
struct WeaponBones { short type; short toPos76; short toPos64; };
constexpr WeaponBones kWeaponBones[] = {
    { 0x135, 28, 27 },  // Golden Titan
    { 0x137,  8,  9 },  // Iron Wheel
    { 0x138, 24, 25 },  // Tantalos
    { 0x139, 11, 12 },  // Bloody Wolf
    { 0x13A,  8,  9 },  // Beam Knight
    { 0x13B,  8,  9 },  // Mutant
};

// Sonido al azar: suena cuando (rand() & mask) == 0.
struct AmbientSound { short type; int mask; int sound; };
constexpr AmbientSound kAmbientSound[] = {
    { 0x157, 0xFF, 120 },  // ElfWizard (343)
    { 0x15C, 0x3F, 121 },  // MixNpc (348)
};

template <class T, size_t N>
const T* Find(const T (&table)[N], short type)
{
    for (const T& row : table)
        if (row.type == type) return &row;
    return nullptr;
}

float* Bone(intptr_t entity_ptr, int byteOffset)
{
    return (float *)((intptr_t)AsCharacter(entity_ptr)->Object.BoneMatrices + byteOffset);
}

// ── Un modelo por función ────────────────────────────────────────────────────

// 0x10E Bull Fighter — IDA raw L766-796.
void BullFighter(intptr_t entity_ptr, void* model)
{
    const unsigned char curAction = AsCharacter(entity_ptr)->Object.CurrentAction;
    const float animFrame = AsCharacter(entity_ptr)->Object.AnimationFrame;
    const bool sparkWindow =
        (!curAction && animFrame >= 15.0f && animFrame <= 20.0f) ||
        (curAction == 1 && animFrame >= 20.0f && animFrame <= 25.0f) ||
        (curAction == 2 && ((animFrame >= 2.0f && animFrame <= 3.0f) ||
                            (animFrame >= 5.0f && animFrame <= 6.0f)));
    if (sparkWindow && !(rand() & 1)) {
        float origin[3], local[3] = { 0.0f, -4.0f, 0.0f };
        BMD_TransformPosition(model, Bone(entity_ptr, 1152), local, origin, '\x01');
        Particle_Spawn(BITMAP_SMOKE, origin, (float *)(entity_ptr + 28), (float *)(entity_ptr + 232), 0, 1.0f, 0);
    }
}

// 0x110 Budge Dragon — IDA raw L797-805: fuego de la boca en la acción 3.
// Después sigue con el humo compartido (el case cae en LABEL_253).
void BudgeDragonBreath(intptr_t entity_ptr, void* model, float* Light)
{
    if (AsCharacter(entity_ptr)->Object.CurrentAction == 3 && AsCharacter(entity_ptr)->Object.AnimationFrame <= 4.0f) {
        float p[3], local[3] = {0.0f, (float)(rand() % 32 + 32), 0.0f};
        BMD_TransformPosition(model, Bone(entity_ptr, 336), local, p, '\x01');
        Particle_Spawn(BITMAP_FIRE, p, (float *)(entity_ptr + 28), Light, 1, 1.0f, 0);
    }
}

// 0x110 Budge Dragon / 0x111 Dark Knight / 0x114 Larva / 0x122 Chain Scorpion —
// IDA raw L806-847 (LABEL_253): humo alrededor del cuerpo; el escorpión suma
// una luz en el hueso 7.
void SmokeAroundBody(intptr_t entity_ptr, void* model, short entType, float* Light,
                     float* WorldPosition, float Luminosity)
{
    if (entType == 0x122) {
        float p[3];
        BMD_TransformPosition(model, Bone(entity_ptr, 336), WorldPosition, p, '\x01');
        float spriteLight[3] = {Luminosity, Luminosity * 0.40000001f, Luminosity * 0.2f};
        CreateSprite(BITMAP_LIGHT, p, 1.0f, spriteLight, entity_ptr, 0.0f, 0);
    }
    if (!AsCharacter(entity_ptr)->Dead && !(rand() & 3)) {
        float p[3] = {(float)(rand() % 64 - 32) + AsCharacter(entity_ptr)->Object.Position[0],
                      (float)(rand() % 64 - 32) + AsCharacter(entity_ptr)->Object.Position[1],
                      (float)(rand() % 32 - 16) + AsCharacter(entity_ptr)->Object.Position[2]};
        Particle_Spawn(gMapManager.GetCurrentMap() == MAP_DEVIAS ? 1220 : 1221, p, (float *)(entity_ptr + 28),
                      gMapManager.GetCurrentMap() == MAP_DEVIAS ? (float *)(entity_ptr + 232) : Light, 0, 1.0f, 0);
    }
}

// 0x119 Gorgon — IDA raw L851-873.  Con Level == 2 escupe 10 partículas de
// fuego desde huesos al azar de todo el cuerpo (`v2 + 34` = NumBones).
void Gorgon(intptr_t entity_ptr, void* model, float* Light, float* WorldPosition, float Luminosity)
{
    AsCharacter(entity_ptr)->Object.BlendMeshLight = (float)(rand() % 10) * 0.1f;
    if (AsCharacter(entity_ptr)->Level == 2)
    {
        short numBones = *(short *)((char *)model + 0x22);
        if (numBones > 0)
        {
            float origin[3];
            for (int i = 0; i < 10; ++i)
            {
                BMD_TransformPosition(model, Bone(entity_ptr, 48 * (rand() % numBones)),
                             WorldPosition, origin, '\x01');
                Particle_Spawn(BITMAP_FIRE, origin, (float *)(entity_ptr + 28), Light, 0, 1.0f, 0);
            }
        }
        float L2[3] = { Luminosity, Luminosity * 0.2f, 0.0f };
        AddTerrainLight(AsCharacter(entity_ptr)->Object.Position[0], AsCharacter(entity_ptr)->Object.Position[1], (float*)L2, 2, (float*)PrimaryTerrainLight[0]);
    }
}

// 0x11A Yeti / 0x11B Elite Yeti — IDA raw L874-891: vaho en el hueso 22.
void YetiBreath(intptr_t entity_ptr, void* model)
{
    if (!(rand() & 3)) {
        float local[3] = { 0.0f, 0.0f, 0.0f }, origin[3];
        BMD_TransformPosition(model, Bone(entity_ptr, 1056), local, origin, '\x01');
        Particle_Spawn(BITMAP_SMOKE, origin, (float *)(entity_ptr + 28), (float *)(entity_ptr + 232), 0, 1.0f, 0);
    }
}

// 0x12B Death Knight — IDA raw L901-915.
void DeathKnight(intptr_t entity_ptr, void* model, float* Light, float* WorldPosition)
{
    AsCharacter(entity_ptr)->Object.BlendMesh = 3;
    AsCharacter(entity_ptr)->Object.BlendMeshTexCoordV = (float)((long long)WorldTime % 1000) * -0.001f;
    if (!(rand() & 1)) {
        float p[3];
        BMD_TransformPosition(model, Bone(entity_ptr, 96), WorldPosition, p, '\x01');
        Particle_Spawn(BITMAP_FIRE, p, (float *)(entity_ptr + 28), Light, 0, 1.0f, 0);
    }
}

// 0x12E Bali — IDA raw L916-971.
void Bali(intptr_t entity_ptr, void* model)
{
    float local[3] = { 0.0f, 0.0f, 0.0f }, origin[3];
    float light[3] = { 0.60000002f, 1.0f, 0.80000001f };
    const unsigned char action = AsCharacter(entity_ptr)->Object.CurrentAction;
    int bone = action == 3 ? 33 : action == 4 ? 20 : action == 8 ? 41 : action == 9 ? 49 : -1;
    if (bone >= 0) {
        BMD_TransformPosition(model, Bone(entity_ptr, 48 * bone), local, origin, '\x01');
        Particle_Spawn(BITMAP_ENERGY, origin, (float *)(entity_ptr + 28), light, 0, 1.0f, 0);
        float whitePink[3] = { 1.0f, 0.60000002f, 1.0f };
        Particle_Spawn(BITMAP_FIRE, origin, (float *)(entity_ptr + 28), whitePink, 0, 1.0f, 0);
    }
    if (action == 6 && AsCharacter(entity_ptr)->Object.AnimationFrame < 12.0f) {
        float green[3] = { 0.1f, 0.80000001f, 0.60000002f };
        for (int i = 0; i < 20; ++i) {
            const int randomBone = rand() % *(short *)(static_cast<BYTE*>(model) + 34);
            BMD_TransformPosition(model, Bone(entity_ptr, 48 * randomBone), local, origin, '\x01');
            Particle_Spawn(BITMAP_FIRE, origin, (float *)(entity_ptr + 28), green, 0, 1.0f, 0);
        }
    }
}

// 0x12F Bahamut — IDA raw L972-990.
void Bahamut(intptr_t entity_ptr, float* Light)
{
    if (!AsCharacter(entity_ptr)->Dead && AsCharacter(entity_ptr)->Level == 1 && !(rand() & 3)) {
        float p[3] = {(float)(rand() % 64 - 32) + AsCharacter(entity_ptr)->Object.Position[0],
                      (float)(rand() % 64 - 32) + AsCharacter(entity_ptr)->Object.Position[1],
                      (float)(rand() % 32 - 16) + AsCharacter(entity_ptr)->Object.Position[2]};
        Particle_Spawn(BITMAP_SMOKE + 1, p, (float *)(entity_ptr + 28), Light, 0, 1.0f, 0);
    }
}

// 0x133 Hydra — IDA raw L991-1007: el brillo sube en las acciones 3-4 y baja en el resto.
void Hydra(intptr_t entity_ptr)
{
    if (AsCharacter(entity_ptr)->Object.CurrentAction >= 3 && AsCharacter(entity_ptr)->Object.CurrentAction <= 4)
        AsCharacter(entity_ptr)->Object.BlendMeshLight = (AsCharacter(entity_ptr)->Object.BlendMeshLight + 0.1f > 1.0f) ? 1.0f : AsCharacter(entity_ptr)->Object.BlendMeshLight + 0.1f;
    else
        AsCharacter(entity_ptr)->Object.BlendMeshLight = (AsCharacter(entity_ptr)->Object.BlendMeshLight - 0.1f < 0.0f) ? 0.0f : AsCharacter(entity_ptr)->Object.BlendMeshLight - 0.1f;
}

// 0x138 Tantalos — IDA raw L1016-1034.  SubType 1 es la variante oscura.
void Tantalos(intptr_t entity_ptr, void* model, float* Light, float* WorldPosition)
{
    if (AsCharacter(entity_ptr)->Object.SubType == 1) {
        float p[3];
        BMD_TransformPosition(model, Bone(entity_ptr, 288), WorldPosition, p, '\x01');
        Particle_Spawn(BITMAP_FIRE, p, (float *)(entity_ptr + 28), Light, 0, 1.0f, 0);
        BMD_TransformPosition(model, Bone(entity_ptr, 624), WorldPosition, p, '\x01');
        Particle_Spawn(BITMAP_FIRE, p, (float *)(entity_ptr + 28), Light, 0, 1.0f, 0);
        float darkness[3] = {-1.3f,-1.3f,-1.3f};
        AddTerrainLight(AsCharacter(entity_ptr)->Object.Position[0], AsCharacter(entity_ptr)->Object.Position[1], (float*)darkness, 3, (float*)PrimaryTerrainLight[0]);
    } else { Combat_SpawnIdleAmbientParticle(AsObject(entity_ptr)); Combat_SpawnDeathDustParticles(AsObject(entity_ptr)); }
}

// 0x13A Beam Knight — IDA raw L1039-1113.  Las tablas se leyeron de IDA en
// 0x5597D8 (118 bytes: las últimas lecturas alcanzan 0x55984B).  Hex-Rays
// separa los aliases v169/v170/v171, pero están en el mismo bloque contiguo:
// bones[0], bones[1], bones[30].  El MonsterIndex 63 es la variante con el
// esqueleto de llamas.
void BeamKnight(intptr_t entity_ptr, void* model, float* WorldPosition, float Luminosity)
{
    static const unsigned char boneOrder[35] = {
        5, 6, 33, 53, 35, 49, 50, 45, 46, 41, 42, 37, 38, 11, 31, 13,
        27, 28, 23, 24, 19, 20, 15, 16, 54, 55, 62, 69, 70, 77, 2, 79,
        81, 84, 86
    };
    static const unsigned char linksA[32] = {
        0, 2, 2, 3, 2, 4, 4, 5, 5, 6, 4, 7, 7, 8, 4, 9,
        9, 10, 4, 11, 11, 12, 6, 5, 8, 7, 10, 9, 12, 11, 0, 0
    };
    static const unsigned char linksB[32] = {
        0, 13, 13, 14, 13, 15, 15, 16, 16, 17, 17, 16, 15, 18, 18, 19,
        15, 20, 20, 21, 15, 22, 22, 23, 15, 20, 20, 21, 18, 19, 21, 22
    };
    static const unsigned char linksC[8] = {29, 28, 28, 27, 34, 33, 33, 30};
    static const unsigned char linksD[8] = {26, 25, 25, 24, 32, 31, 31, 30};

    float zero[3] = {0.0f, 0.0f, 0.0f};
    if (AsCharacter(entity_ptr)->MonsterIndex == 63) {
        float targetPosition[3] = {0.0f, 0.0f, 0.0f};
        float bones[35][3] = {};

        for (int bone = 0; bone < 35; ++bone) {
            BMD_TransformPosition(model, Bone(entity_ptr, 48 * boneOrder[bone]),
                          zero, bones[bone], '\x01');
        }

        for (int link = 0; link < 30; link += 2) {
            float particleLight[3];
            MoveHumming(bones[linksA[link + 1]], targetPosition, bones[linksA[link]], 360.0f);
            particleLight[0] = Luminosity; // IDA sólo escribe este componente (Position[0] = v63).
            Particle_Spawn(BITMAP_FLAME, bones[linksA[link]], targetPosition, particleLight, 2,
                          link < 22 ? 1.0f : 0.5f, 0);
            MoveHumming(bones[linksB[link + 1]], targetPosition, bones[linksB[link]], 360.0f);
            particleLight[0] = Luminosity;
            Particle_Spawn(BITMAP_FLAME, bones[linksB[link]], targetPosition, particleLight, 2,
                          link < 22 ? 1.0f : 0.5f, 0);
        }

        for (int link = 0; link < 8; link += 2) {
            float particleLight[3];
            MoveHumming(bones[linksC[link + 1]], targetPosition, bones[linksC[link]], 360.0f);
            particleLight[0] = Luminosity;
            Particle_Spawn(BITMAP_FLAME, bones[linksC[link]], targetPosition, particleLight, 2, 0.60000002f, 0);
            MoveHumming(bones[linksD[link + 1]], targetPosition, bones[linksD[link]], 360.0f);
            particleLight[0] = Luminosity;
            Particle_Spawn(BITMAP_FLAME, bones[linksD[link]], targetPosition, particleLight, 2, 0.60000002f, 0);
        }

        if (!((long long)WorldTime % 2)) {
            float particleLight[3];
            MoveHumming(bones[0], targetPosition, bones[30], 360.0f);
            particleLight[0] = (float)WorldTime;
            Particle_Spawn(BITMAP_FLAME, bones[30], targetPosition, particleLight, 2, 1.3f, 0);
            Particle_Spawn(BITMAP_FLAME, bones[1], targetPosition, particleLight, 3, 0.5f, 0);
        }

        float darkness[3] = {-1.3f, -1.3f, -1.3f};
        AddTerrainLight(AsCharacter(entity_ptr)->Object.Position[0], AsCharacter(entity_ptr)->Object.Position[1], (float*)darkness, 3, (float*)PrimaryTerrainLight[0]);
    } else {
        float targetPosition[3] = {0.0f, 0.0f, 0.0f};
        float effectLight = sinf((float)WorldTime * 0.0020000001f) * 0.30000001f + 0.69999999f;
        float light[3] = {effectLight, effectLight * 0.5f, effectLight * 0.5f};
        float source[3], destination[3];

        BMD_TransformPosition(model, Bone(entity_ptr, 2640), WorldPosition, source, '\x01');
        BMD_TransformPosition(model, Bone(entity_ptr, 2976), WorldPosition, destination, '\x01');
        MoveHumming(source, targetPosition, destination, 360.0f);
        Particle_Spawn(BITMAP_FLAME, destination, targetPosition, light, 1, 0.2f, 0);

        BMD_TransformPosition(model, Bone(entity_ptr, 3360), WorldPosition, source, '\x01');
        BMD_TransformPosition(model, Bone(entity_ptr, 3696), WorldPosition, destination, '\x01');
        MoveHumming(source, targetPosition, destination, 360.0f);
        Particle_Spawn(BITMAP_FLAME, destination, targetPosition, light, 1, 0.2f, 0);
        Combat_SpawnIdleAmbientParticle(AsObject(entity_ptr));
        Combat_SpawnDeathDustParticles(AsObject(entity_ptr));
    }
}

// 0x13E Cursed King — IDA raw L1118-1128; sub_4793F0 == FloatingLabel_Add (verificado en IDA).
void CursedKing(intptr_t entity_ptr)
{
    if (!(rand() % 5)) {
        float p[3] = {(float)(rand() % 21 - 10) * 3.6571429f + AsCharacter(entity_ptr)->Object.Position[0],
                      (float)(rand() % 21 - 10) * 3.6571429f + AsCharacter(entity_ptr)->Object.Position[1], 0.0f};
        FloatingLabel_Add(1205, (DWORD *)p, *(int *)&AsCharacter(entity_ptr)->Object.Angle[0], (DWORD *)(entity_ptr + 232), 0x3F266666);
    }
}

// 0x141 Queen Rainier — IDA raw L1129-1178: cadena de rayos entre huesos.
void QueenRainier(intptr_t entity_ptr, void* model)
{
    float local[3] = { 0.0f, 0.0f, 0.0f }, a[3], b[3];
    const intptr_t boneBase = (intptr_t)AsCharacter(entity_ptr)->Object.BoneMatrices;
    for (int offset = 96; offset < 240; offset += 48) {
        BMD_TransformPosition(model, (float *)(boneBase + offset), local, a, '\x01');
        BMD_TransformPosition(model, (float *)(boneBase + offset + 48), local, b, '\x01');
        Joint_Create(1254, a, b, (float *)(entity_ptr + 28), 7, 0, 14.0f, -1, 0);
    }
    // IDA's j=9 special case reuses bone 2 (+96) and targets
    // 16*(3*j+3) = +480, i.e. bone 10.  Using bone 9 joined the
    // lightning chain back into the wrong segment and made it appear
    // to bounce/reverse at the first branch.
    const int pairs[][2] = {{2,10},{10,11},{2,18},{18,22},{22,23},{23,24},{24,25},{18,31},{31,32},{32,33},{33,34}};
    for (int i = 0; i < 11; ++i) {
        BMD_TransformPosition(model, (float *)(boneBase + 48 * pairs[i][0]), local, a, '\x01');
        BMD_TransformPosition(model, (float *)(boneBase + 48 * pairs[i][1]), local, b, '\x01');
        Joint_Create(1254, a, b, (float *)(entity_ptr + 28), 7, 0, 14.0f, -1, 0);
    }
}

// 0x152 Smith (Hanzo el herrero, OpenNpc case 338) — IDA raw L1185-1221.
void Smith(intptr_t entity_ptr, void* model, float Luminosity)
{
    unsigned char curAction = AsCharacter(entity_ptr)->Object.CurrentAction;  // +0x105
    float         animFrame = AsCharacter(entity_ptr)->Object.AnimationFrame;          // +0x108

    // Golpe de martillo: suena entre los frames 5 y 10 de la anim 0.
    // El buffer 120 (0x78) lo carga OpenNpc justo para este NPC.
    if (curAction == 0 && animFrame >= 5.0f && animFrame <= 10.0f)
        PlayBuffer(120, 0, 0);

    // BlendMesh = 4 → la mesh 4 de Smith01.bmd es `fire_02.jpg`, el
    // fuego de la fragua: se dibuja en modo blend en vez de opaca.
    // BlendMeshLight le da el parpadeo.
    AsCharacter(entity_ptr)->Object.BlendMesh = 4;            // +0x64 BlendMesh
    AsCharacter(entity_ptr)->Object.BlendMeshLight = Luminosity;   // +0x68 BlendMeshLight

    // Luz naranja proyectada sobre el terreno alrededor de la fragua.
    float Light[3] = { Luminosity, Luminosity * 0.40000001f, 0.0f };
    AddTerrainLight(AsCharacter(entity_ptr)->Object.Position[0], AsCharacter(entity_ptr)->Object.Position[1], (float*)Light, 3, (float*)PrimaryTerrainLight[0]);

    // Chispas: 4 por golpe, en la ventana de frames 5..6 (el impacto).
    // El origen es el hueso 17 (BoneTransform + 48*17 = +816) = el yunque.
    Light[0] = 1.0f; Light[1] = 1.0f; Light[2] = 1.0f;
    float WorldPosition[3] = { 0.0f, 0.0f, 0.0f };
    if (curAction == 0 && animFrame >= 5.0f && animFrame <= 6.0f)
    {
        float sparkOrigin[3];
        BMD_TransformPosition(model, Bone(entity_ptr, 816),
                     WorldPosition, sparkOrigin, '\x01');
        for (int i = 0; i < 4; ++i)
        {
            float Angle[3];
            Angle[0] = (float)(rand() % 60 + 150);
            Angle[1] = 0.0f;
            Angle[2] = (float)(rand() % 30);
            Joint_Create(1257, sparkOrigin, sparkOrigin, Angle, 0, 0, 10.0f, -1, 0);
            Particle_Spawn(BITMAP_SPARK, sparkOrigin, Angle, Light, 0, 1.0f, 0);
        }
    }
}

// 0x179 DeviasTrader (377) — IDA raw L1246-1281: chispas y luz de la fragua en la acción 0.
void DeviasTrader(intptr_t entity_ptr, void* model, float Luminosity)
{
    if (!AsCharacter(entity_ptr)->Object.CurrentAction) {
        float white[3] = { 1.0f, 1.0f, 1.0f };
        float local[3] = { 0.0f, 5.0f, 10.0f }, origin[3];
        float terrainLight[3] = { Luminosity * 0.5f, Luminosity * 0.30000001f, 0.0f };
        AddTerrainLight(AsCharacter(entity_ptr)->Object.Position[0], AsCharacter(entity_ptr)->Object.Position[1], (float*)terrainLight, 3, (float*)PrimaryTerrainLight[0]);
        BMD_TransformPosition(model, Bone(entity_ptr, 1776), local, origin, '\x01');
        for (int i = 0; i < 4; ++i) {
            float angle[3] = { (float)(rand() % 60 + 90), 0.0f, (float)(rand() % 30) };
            Joint_Create(1257, origin, origin, angle, 0, 0, 10.0f, -1, 0);
            if (rand() & 1)
                Particle_Spawn(BITMAP_SPARK, origin, angle, white, 0, 1.0f, 0);
        }
    }
}

// 0x186 Player — IDA raw L1282-1342: burbujas en Atlans, chispas en Devil Square
// y fuego en la acción 90.
void Player(intptr_t entity_ptr, void* model, float* Light)
{
    float p[3];
    if (SceneFlag == 5 && gMapManager.GetCurrentMap() == MAP_ATLANS && (long long)WorldTime % 10000 < 1000) {
        float local[3] = {0.0f,20.0f,-10.0f};
        BMD_TransformPosition(model, Bone(entity_ptr, 48 * *(int *)(static_cast<BYTE*>(model) + 84)), local, p, '\x01');
        Particle_Spawn(BITMAP_BUBBLE, p, (float *)(entity_ptr + 28), Light, 0, 1.0f, 0);
    }
    float local[3] = {-15.0f,0.0f,0.0f};
    if (gMapManager.GetCurrentMap() == MAP_DEVIL_SQUARE1) {
        if (!(rand() & 3)) { BMD_TransformPosition(model, Bone(entity_ptr, 1248), local, p, '\x01'); Particle_Spawn(104,p,(float *)(entity_ptr+28),Light,0,1.0f,0); }
        if (!(rand() & 3)) { BMD_TransformPosition(model, Bone(entity_ptr, 1680), local, p, '\x01'); Particle_Spawn(104,p,(float *)(entity_ptr+28),Light,0,1.0f,0); }
    }
    if (AsCharacter(entity_ptr)->Object.CurrentAction == 90) {
        const short n = *(short *)(static_cast<BYTE*>(model) + 34);
        if (n > 0) for (int i = 0; i < 10; ++i) {
            BMD_TransformPosition(model, Bone(entity_ptr, 48 * (rand() % n)), local, p, '\x01');
            Particle_Spawn(BITMAP_FIRE,p,(float *)(entity_ptr+28),Light,0,1.0f,0);
        }
    }
    float tail[3] = {0.0f,-30.0f,0.0f};
    BMD_TransformPosition(model, Bone(entity_ptr, 48 * AsCharacter(entity_ptr)->Weapon[0].LinkBone), tail, p, '\x01');
}

} // namespace

void CharacterVisual_ModelEffects(intptr_t entity_ptr, void* model, float* Light,
                                  float* WorldPosition, float Luminosity)
{
    const short entType = AsCharacter(entity_ptr)->Object.Type;

    if (const TexScroll* s = Find(kTexScroll, entType)) {
        const float coord = (float)((long long)WorldTime % s->period) * s->step;
        if (s->v) AsCharacter(entity_ptr)->Object.BlendMeshTexCoordV = coord;
        else      AsCharacter(entity_ptr)->Object.BlendMeshTexCoordU = coord;
        return;
    }
    if (const AmbientSound* s = Find(kAmbientSound, entType)) {
        if (!(rand() & s->mask)) PlayBuffer(s->sound, 0, 0);
        return;
    }
    if (const WeaponBones* w = Find(kWeaponBones, entType)) {
        float zero[3] = {0.0f, 0.0f, 0.0f};
        BMD_TransformPosition(model, Bone(entity_ptr, 48 * w->toPos76), zero, (float *)(entity_ptr + 76), '\x01');
        BMD_TransformPosition(model, Bone(entity_ptr, 48 * w->toPos64), zero, (float *)(entity_ptr + 64), '\x01');
    }

    switch (entType)
    {
    case 0x10E: BullFighter(entity_ptr, model); break;
    case 0x110:
        BudgeDragonBreath(entity_ptr, model, Light);
        SmokeAroundBody(entity_ptr, model, entType, Light, WorldPosition, Luminosity);
        break;
    case 0x111: case 0x114: case 0x122:
        SmokeAroundBody(entity_ptr, model, entType, Light, WorldPosition, Luminosity);
        break;
    case 0x113: Combat_SpawnDeathDustParticles(AsObject(entity_ptr)); break;  // Giant
    case 0x119: Gorgon(entity_ptr, model, Light, WorldPosition, Luminosity); break;
    case 0x11A: case 0x11B: YetiBreath(entity_ptr, model); break;
    case 0x12B: DeathKnight(entity_ptr, model, Light, WorldPosition); break;
    case 0x12E: Bali(entity_ptr, model); break;
    case 0x12F: Bahamut(entity_ptr, Light); break;
    case 0x133: Hydra(entity_ptr); break;
    case 0x137:  // Iron Wheel
        Combat_SpawnIdleAmbientParticle(AsObject(entity_ptr)); Combat_SpawnDeathDustParticles(AsObject(entity_ptr));
        break;
    case 0x138: Tantalos(entity_ptr, model, Light, WorldPosition); break;
    case 0x139: case 0x13B: Combat_SpawnIdleAmbientParticle(AsObject(entity_ptr)); break;  // Bloody Wolf, Mutant
    case 0x13A: BeamKnight(entity_ptr, model, WorldPosition, Luminosity); break;
    case 0x13E: CursedKing(entity_ptr); break;
    case 0x141: QueenRainier(entity_ptr, model); break;
    case 0x152: Smith(entity_ptr, model, Luminosity); break;
    case 0x179: DeviasTrader(entity_ptr, model, Luminosity); break;
    case 0x186: Player(entity_ptr, model, Light); break;
    default: break;
    }
}
