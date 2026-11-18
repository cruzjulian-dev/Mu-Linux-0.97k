#include <cstdint>
// Entity_Render_3D.cpp
// Entity_SpawnEffects (IDA 0x004FC070): efectos que emiten los objetos del mapa
// por mapa y tipo (llamas de antorchas, chispas de portales, humo, nubes).
// No dibuja geometría: publica sprites y lanza partículas y joints.  Lo llama
// WorldEffects_Tick con los huesos SIM del objeto ya calculados en g_BoneScratch.

#include "stdafx.h"
#include "render/WorldEffects.h"
#include <stdio.h>
#include "render/Entity_Render_3D.h"

// IDA: Entity_SpawnEffects (0x004FC070)
// DESVIACION: sólo se llama desde WorldEffects_Tick, con huesos SIM frescos.
// Emite partículas/joints y publica sprites; el draw reproduce esa muestra.
//
// El decompile era un switch por mapa con saltos entre cases (LAB_004fc55a,
// el fall-through de Lost Tower a Arena).  Acá: una función por mapa, una por
// objeto, y una tabla para las llamas fijas de Noria.  Los huesos se nombran por
// índice dentro de g_BoneScratch (stride 0x30): DAT_06970a9c es el 0,
// DAT_06970acc el 1, etc.
namespace {

float* Bone(int index) { return (float *)(g_BoneScratch + index * 0x30); }

float* ObjectAngle(intptr_t o) { return (float *)(o + 0x1c); }
float* ObjectLight(intptr_t o) { return (float *)(o + 0xe8); }
float* ObjectPosition(intptr_t o) { return (float *)(o + 0x10); }

// Ángulo de los pares de chispas que giran: (int64)(WorldTime * 0.1) % 360,
// con el truncado a 32 bits del binario.
float SpinAngle()
{
    ulonglong uVar11 = (longlong)(WorldTime * 0.1f);
    return (float)(int)((longlong)((ulonglong)(uint)((int)uVar11 >> 0x1f) << 0x20
                                   | uVar11 & 0xffffffff) % 0x168);
}

// Hueso -> sprite.  `offset` es la entrada de TransformPosition (local al hueso).
void BoneSprite(void* model, intptr_t o, int bone, float* offset, unsigned short type,
                float scale, float* light, float angle = 0)
{
    float position[3];
    BMD_TransformPosition(model, Bone(bone), offset, position, '\0');
    WorldEffects_AddSprite(type, position, scale, light, o, angle, 0);
}

void BoneSpritePair(void* model, intptr_t o, int bone, float* offset, unsigned short type,
                    float scale, float* light, float angle)
{
    float position[3];
    BMD_TransformPosition(model, Bone(bone), offset, position, '\0');
    WorldEffects_AddSprite(type, position, scale, light, o, angle, 0);
    WorldEffects_AddSprite(type, position, scale, light, o, -angle, 0);
}

// ── Lorencia ─────────────────────────────────────────────────────────────────

// 0x38 — portal: dos chorros de fuego.  Ojo: acá la posición local (0,0,0) es la
// SALIDA de TransformPosition y el resultado va al segundo argumento.
void LorenciaPortal(void* model, intptr_t o, float size)
{
    float scale = size * 5.0f;
    float light[3] = { size * 0.6f, size * 0.3f, size * 0.1f };
    float local[3] = {};
    float position[3];
    BMD_TransformPosition(model, Bone(48), local, position, '\0');
    WorldEffects_AddSprite(0x47e, position, scale, light, o, 0, 0);
    BMD_TransformPosition(model, Bone(57), local, position, '\0');
    WorldEffects_AddSprite(0x47e, position, scale, light, o, 0, 0);
}

// 0x69 — chimenea: textura que se mueve y, la mitad de los cuadros, dos bocanadas
// de humo (abajo y arriba).
void LorenciaChimney(void* model, intptr_t o)
{
    *(undefined4 *)(o + 0x68) = 0x3f800000;
    longlong lVar10 = (longlong)WorldTime;   // IDA: (-(std::int64_t)WorldTime % 1000) * 0.001
    *(float *)(o + 0x70) = (float)(-(int)lVar10 % 1000) * 0.001f;
    if (rand() & 1) return;
    float position[3];
    float local[3];
    local[0] = (float)((rand() & 0x1f) - 0x10);
    local[1] = -20.0f;
    local[2] = (float)((rand() & 0x1f) - 0x10);
    BMD_TransformPosition(model, Bone(1), local, position, '\0');
    Particle_Spawn(BITMAP_SMOKE, position, ObjectAngle(o), ObjectLight(o), 0, 1.0f, 0);
    local[0] = (float)((rand() & 0x1f) - 0x10);
    local[1] = -80.0f;
    local[2] = (float)((rand() & 0x1f) - 0x10);
    BMD_TransformPosition(model, Bone(4), local, position, '\0');
    Particle_Spawn(BITMAP_SMOKE, position, ObjectAngle(o), ObjectLight(o), 0, 1.0f, 0);
}

void Lorencia(void* model, intptr_t o, short type, float size)
{
    if (type == 0x38) LorenciaPortal(model, o, size);
    else if (type == 0x69) LorenciaChimney(model, o);
}

// ── Devias ───────────────────────────────────────────────────────────────────

// 0x64 — par de chispas que giran a 150 de altura.
void DeviasSparks(void* model, intptr_t o, float size)
{
    float light[3] = { size, size, size };
    float local[3] = { 0.0f, 0.0f, 150.0f };
    const float angle = SpinAngle();
    BoneSpritePair(model, o, 0, local, 0x4a7, 2.5f, light, angle);
}

// 0x67 — al terminar el ciclo de la animación, 1/32 de pasar a la acción 1.
void DeviasAnimatedProp(void* model, intptr_t o)
{
    const int lastFrame = *(short *)((intptr_t)AsObject(o)->CurrentAction * sizeof(ModelAnimationAction) + 8
                                       + (intptr_t)AsModel(model)->Actions) - 1;
    if ((int)*(short *)(static_cast<unsigned char*>(model) + 0xa8 + BMD_FIELD_SHIFT) != lastFrame) return;
    SetAction(AsObject(o), (rand() & 0x1f) == 0 ? 1 : 0);
}

void Devias(void* model, intptr_t o, short type, float size)
{
    if (type == 0x64) DeviasSparks(model, o, size);
    else if (type == 0x67) DeviasAnimatedProp(model, o);
}

// ── Noria ────────────────────────────────────────────────────────────────────

// Llamas fijas en huesos, con luz (0.4, 0.7, 1.0) * size.
struct NoriaFlame { short Type; float Scale; unsigned char Count; unsigned char Bones[4]; };
constexpr NoriaFlame kNoriaFlames[] = {
    { 1,    0.5f, 3, { 2, 4, 6 } },
    { 9,    1.5f, 1, { 1 } },
    { 0x11, 1.0f, 4, { 4, 7, 10, 13 } },
    { 0x23, 1.5f, 1, { 3 } },
};

// 0x27 — portal: dos chispas que giran, cuatro llamas con brillos al azar, y
// 1/8 de una ráfaga de chispas.
void NoriaPortal(void* model, intptr_t o, float size)
{
    float zero[3] = {};
    float light[3] = { size * 0.4f, size * 0.8f, size };
    const float angle = SpinAngle();
    float position[3];
    BMD_TransformPosition(model, Bone(57), zero, position, '\0');
    WorldEffects_AddSprite(0x4a7, position, 1.0f, light, o, angle, 0);
    WorldEffects_AddSprite(0x4a7, position, 1.0f, light, o, -angle, 0);
    light[0] = light[1] = light[2] = 1.0f;
    for (int i = 0; i < 4; ++i) {
        BMD_TransformPosition(model, Bone(61 + i), zero, position, '\0');
        WorldEffects_AddSprite(0x47e, position, 1.0f, light, o, 0, 0);
        if ((rand() & 0x1f) == 0) {
            Particle_Spawn(BITMAP_SHINY, position, ObjectAngle(o), light, 0, 1.0f, 0);
            Particle_Spawn(BITMAP_SHINY, position, ObjectAngle(o), light, 1, 1.0f, 0);
        }
    }
    BMD_TransformPosition(model, Bone(58), zero, position, '\0');
    if ((rand() & 7) != 0) return;
    for (int i = 0; i < 8; ++i) {
        float sparkAngle[3] = { (float)(rand() % 0x3c + 0x3c), 140.0f, 0.0f };
        sparkAngle[2] = (float)(rand() % 0x1e);
        Joint_Create(0x4e9, position, position, sparkAngle, 0, 0, 10.0f, -1, 0);
    }
}

void Noria(void* model, intptr_t o, short type, float size)
{
    for (const NoriaFlame& flame : kNoriaFlames) {
        if (flame.Type != type) continue;
        float zero[3] = {};
        float light[3] = { size * 0.4f, size * 0.7f, size };
        for (int i = 0; i < flame.Count; ++i)
            BoneSprite(model, o, flame.Bones[i], zero, 0x47e, flame.Scale, light);
        return;
    }
    if (type == 0x27) NoriaPortal(model, o, size);
}

// ── Lost Tower y Arena ───────────────────────────────────────────────────────

// Lost Tower 0x13 / 0x14 — lámparas con pares de chispas que giran (la 0x13 roja,
// con otro sprite).
void LostTowerLamp(void* model, intptr_t o, short type, float size)
{
    unsigned short sprite;
    float light[3];
    if (type == 0x13) { sprite = 0x4f0; light[0] = size;        light[1] = size * 0.2f; light[2] = 0.0f; }
    else              { sprite = 0x4a7; light[0] = size * 0.4f; light[1] = size * 0.8f; light[2] = size; }
    float zero[3] = {};
    const float angle = SpinAngle();
    BoneSpritePair(model, o, 15, zero, sprite, 0.3f, light, angle);
    BoneSpritePair(model, o, 19, zero, sprite, 0.3f, light, angle);
    BoneSpritePair(model, o, 21, zero, sprite, 1.5f, light, angle);
}

// Lost Tower 0x28 — par de chispas que giran sobre el objeto, a 260 de altura.
void LostTowerCrystal(intptr_t o, float size)
{
    float light[3] = { size, size, size };
    const float angle = SpinAngle();
    float position[3] = { *(float *)(o + 0x10), *(float *)(o + 0x14), *(float *)(o + 0x18) + 260.0f };
    WorldEffects_AddSprite(0x4a7, position, 2.5f, light, o, angle, 0);
    WorldEffects_AddSprite(0x4a7, position, 2.5f, light, o, -angle, 0);
}

// Tipo 9 en Lost Tower y en la Arena — antorcha.  La entrada de TransformPosition
// es la posición (0,0,0) y la salida va al offset, igual que en el binario.
void Torch(void* model, intptr_t o, float size)
{
    float light[3] = { size * 0.6f, size * 0.3f, size * 0.1f };
    float zero[3] = {};
    float position[3];
    BMD_TransformPosition(model, Bone(1), zero, position, '\0');
    WorldEffects_AddSprite(0x47e, position, size * 5.0f, light, o, 0, 0);
}

void LostTower(void* model, intptr_t o, short type, float size)
{
    if (type > 0x12 && type < 0x15) LostTowerLamp(model, o, type, size);
    else if (type == 0x28) LostTowerCrystal(o, size);
    if (type == 9) Torch(model, o, size);   // el binario cae al case de la Arena
}

// ── Tarkan ───────────────────────────────────────────────────────────────────

// Luz que late (sin(t) * 0.3 + 0.7) con un tinte que baja el rojo.
float10 TarkanPulse(float10 t) { return fsin(t * (float10)0.002f) * (float10)0.3f + (float10)0.7f; }

void Tarkan(void* model, intptr_t o, short type)
{
    float zero[3] = {};
    switch (type) {
    case 0: {   // lámpara
        const float10 v = TarkanPulse((float10)WorldTime);
        float light[3] = { (float)(v * (float10)0.58823526f), (float)v, (float)v };
        BoneSprite(model, o, 6, zero, 0x498, (float)v * 5.0f, light);
        break;
    }
    case 0x3c:  // humo que sale una vez al crearse
        if (*(int *)(o + 0x58) != -2)
            for (int i = 0; i < 0x14; ++i)
                Particle_Spawn(BITMAP_SMOKE, ObjectPosition(o), ObjectAngle(o), ObjectLight(o), 6, *(float *)(o + 0xc), 0);
        *(undefined4 *)(o + 0x58) = 0xfffffffe;
        break;
    case 0x3f: {   // brasero (azul)
        const float10 v = TarkanPulse((float10)*(float *)(o + 0x24) * (float10)5.0f + (float10)WorldTime);
        float light[3] = { (float)(v * (float10)0.58823526f), (float)v, (float)v };
        BoneSprite(model, o, 2, zero, 0x597, (float)v * 1.5f, light);
        break;
    }
    case 0x40: {   // brasero (rojo)
        const float v = (float)TarkanPulse((float10)*(float *)(o + 0x24) * (float10)5.0f + (float10)WorldTime);
        float light[3] = { v, v * 0.32f, v * 0.32f };
        BoneSprite(model, o, 2, zero, 0x597, v * 1.5f, light);
        break;
    }
    case 0x46:  // vapor, 1/5 de los cuadros
        *(undefined4 *)(o + 0x58) = 0xfffffffe;
        if (rand() % 5 == 0)
            Particle_Spawn(BITMAP_SMOKE, ObjectPosition(o), ObjectAngle(o), ObjectLight(o), 7, *(float *)(o + 0xc), 0);
        break;
    case 0x4c: {   // géiser: el último 10% de cada ciclo de 5 s
        *(undefined4 *)(o + 0x58) = 0xfffffffe;
        ulonglong uVar11 = (longlong)WorldTime;   // IDA: (std::int64_t)WorldTime % 5000 > 4500
        uVar11 = (ulonglong)(uint)((int)uVar11 >> 0x1f) << 0x20 | uVar11 & 0xffffffff;
        if (0x1194 < (int)((longlong)uVar11 % 5000))
            Particle_Spawn(BITMAP_SMOKE, ObjectPosition(o), ObjectAngle(o), ObjectLight(o), 4, *(float *)(o + 0xc), 0);
        break;
    }
    case 0x53: {   // fumarola: ventana de 0.5 s desfasada por el ángulo, con chispas
        *(undefined4 *)(o + 0x58) = 0xfffffffe;
        const int phase = (int)(longlong)(*(float *)(o + 0x24)) * 10;   // IDA: 10 * (std::int64_t)*(float*)(o+36)
        ulonglong uVar11 = (longlong)WorldTime;   // IDA: (std::int64_t)WorldTime % 10000
        const int t = (int)((longlong)((ulonglong)(uint)((int)uVar11 >> 0x1f) << 0x20 | uVar11 & 0xffffffff) % 10000);
        if (!((int)(phase + 0xdac) < t && t < phase + 4000)) break;
        Particle_Spawn(BITMAP_SMOKE, ObjectPosition(o), ObjectAngle(o), ObjectLight(o), 8, *(float *)(o + 0xc), 0);
        if (rand() % 3 != 0) break;
        float position[3];
        position[0] = (float)((rand() & 0x7f) - 0x40) + *ObjectPosition(o);
        const int dy = (rand() & 0x7f) - 0x40;
        position[2] = *(float *)(o + 0x18);
        position[1] = (float)dy + *(float *)(o + 0x14);
        Particle_Spawn(BITMAP_SMOKE, position, ObjectAngle(o), ObjectLight(o), 4, *(float *)(o + 0xc) * 0.5f, 0);
        CreateEffect((rand() & 1) + 0xc5, ObjectPosition(o), ObjectAngle(o), ObjectLight(o), 0, 0, (float *)0xffffffff, 0, 0);
        break;
    }
    }
}

// ── Devil Square ─────────────────────────────────────────────────────────────

// 0x2 — estatua: chispas en las dos manos (huesos 23 y 31).
void DevilSquareStatue(void* model, intptr_t o)
{
    float light[3] = { 1.0f, 1.0f, 1.0f };
    float local[3] = { -15.0f, 0.0f, 0.0f };
    float position[3];
    if ((rand() & 3) == 0) {
        BMD_TransformPosition(model, Bone(23), local, position, '\0');
        Particle_Spawn(0x68, position, ObjectAngle(o), light, 0, 1.0f, 0);
    }
    if ((rand() & 3) == 0) {
        BMD_TransformPosition(model, Bone(31), local, position, '\0');
        Particle_Spawn(0x68, position, ObjectAngle(o), light, 0, 1.0f, 0);
    }
    BMD_TransformPosition(model, Bone(23), local, position, '\0');
    Particle_Spawn(0x68, position, ObjectAngle(o), light, 0, 1.0f, 0);
}

// ── Icarus ───────────────────────────────────────────────────────────────────

// 0..5 — nubes: al crearse sueltan 20 (tipos 0-2) o 10 (tipos 3-5) partículas.
// IDA pasa `Angle` (reusado como Light) = (0.1, 0.1, 0.1).
void IcarusClouds(intptr_t o, int count)
{
    if (*(int *)(o + 0x58) != -2) {
        float light[3] = { 0.1f, 0.1f, 0.1f };
        for (int i = 0; i < count; ++i)
            Particle_Spawn(BITMAP_CLOUD, ObjectPosition(o), ObjectAngle(o), light, (int)*(short *)(o + 2), *(float *)(o + 0xc), o);
    }
    *(undefined4 *)(o + 0x58) = 0xfffffffe;
}

void Icarus(void* model, intptr_t o, short type)
{
    if (type >= 0 && type <= 2) IcarusClouds(o, 0x14);
    else if (type >= 3 && type <= 5) IcarusClouds(o, 10);
    else if (type == 0xa) {
        float zero[3] = {};
        float light[3] = { 1.0f, 1.0f, 1.0f };
        float position[3];
        BMD_TransformPosition(model, Bone(3), zero, position, '\0');
        Particle_Spawn(BITMAP_LIGHT, position, ObjectAngle(o), light, 0, 1.0f, 0);
    }
}

// ── Blood Castle ─────────────────────────────────────────────────────────────

// 0xb — antorchas en siete huesos, con un brillo que ondula por el ángulo.
// IDA 0x004FC070: Angle[0] contiene bytes {1,2,4,6}; los tres bytes siguientes
// son {9,10,11}. No son un float sin inicializar.
void BloodCastleTorches(void* model, intptr_t o)
{
    static const unsigned char waveBones[7] = {1, 2, 4, 6, 9, 10, 11};
    float10 v = fsin(((float10)*(float *)(o + 0x24) * (float10)20.0f + (float10)WorldTime) * (float10)0.001f);
    v = (v + (float10)1.0f) * (float10)0.5f;
    // El tercer canal no se escribe: queda en 0, el valor inicial del frame.
    float light[3] = { (float)v, (float)(v * (float10)0.5f), 0.0f };
    float local[3] = { 0.0f, 0.0f, 2.0f };
    for (int i = 0; i < 7; ++i)
        BoneSprite(model, o, waveBones[i], local, 0x47e, 0.5f, light);
}

// 0xd — luz que late en el hueso 3.
void BloodCastleLight(void* model, intptr_t o)
{
    float10 v = fsin((float10)WorldTime * (float10)0.001f);
    v = v * (float10)0.3f + (float10)0.7f;
    float light[3] = { (float)v, (float)v, (float)v };
    float zero[3] = {};
    BoneSprite(model, o, 3, zero, 0x4e1, (float)v + 0.5f, light);
}

// 0x25 — humo y relámpagos al azar; el contador en +0x80 elige cuál.
void BloodCastleSmoke(intptr_t o)
{
    float light[3] = { 1.0f, 1.0f, 1.0f };
    if ((rand() & 1) == 0) {
        const ulonglong uVar11 = (longlong)(*(float *)(o + 0x80) + 2.0f);   // IDA: (int)(std::int64_t)(*(float*)(o+128)+2.0) % 4
        const uint remainder = (uint)uVar11 & 0x80000003;
        bool hit = (uVar11 & 0x80000003) == 0;
        if ((int)remainder < 0) hit = ((remainder - 1 | 0xfffffffc) == 0xffffffff);
        *(float *)(o + 0x80) = *(float *)(o + 0x80) + 1.0f;
        if (hit) {
            Particle_Spawn(BITMAP_ADV_SMOKE + 1, ObjectPosition(o), ObjectAngle(o), light, 0, 1.0f, 0);
            Particle_Spawn(BITMAP_ADV_SMOKE, ObjectPosition(o), ObjectAngle(o), light, 0, 1.0f, 0);
        }
    }
    if ((rand() & 1) != 0) return;
    const ulonglong uVar11 = (longlong)(*(float *)(o + 0x80));   // IDA: (std::int64_t)*(float*)(o+128) % 4
    uint remainder = (uint)uVar11 & 0x80000003;
    bool hit = (uVar11 & 0x80000003) == 0;
    if ((int)remainder < 0) hit = ((remainder - 1U | 0xfffffffc) + 1) == 0;
    *(float *)(o + 0x80) = *(float *)(o + 0x80) + 1.0f;
    if (!hit) return;
    Particle_Spawn(BITMAP_CLOUD, ObjectPosition(o), ObjectAngle(o), light, 6, 1.0f, 0);
    Particle_Spawn(BITMAP_ADV_SMOKE, ObjectPosition(o), ObjectAngle(o), light, 1, 1.0f, 0);
    light[0] = 1.0f; light[1] = 0.8f; light[2] = 0.8f;
    Particle_Spawn(BITMAP_FLARE, ObjectPosition(o), ObjectAngle(o), light, 4, 0.19f, 0);
}

void BloodCastle(void* model, intptr_t o, short type)
{
    if (type == 0xb) BloodCastleTorches(model, o);
    else if (type == 0xd) BloodCastleLight(model, o);
    else if (type == 0x25) BloodCastleSmoke(o);
}

} // namespace

void __cdecl Entity_SpawnEffects(intptr_t param_1)
{
    void* const model = (void *)(Models + *(short *)(param_1 + 2) * BMD_STRIDE);
    // Tamaño base al azar [0.70, 0.99]; lo usan casi todos los efectos.
    const float size = (float)(_rand() % 0x1e + 0x46) * 0.01f;
    const short type = *(short *)(param_1 + 2);

    // Login: el barco del fondo (tipo 0x3c) con sus dos antorchas.  translate = 0
    // (fiel al IDA): la matriz del hueso ya está en espacio de mundo.
    if (SceneFlag == 2 && type == 0x3c) {
        float light[3] = { size, size * 0.5f, size * 0.2f };
        float zero[3] = {};
        BoneSprite(model, param_1, 8, zero, 0x47e, 1.0f, light);
        BoneSprite(model, param_1, 9, zero, 0x47e, 1.0f, light);
    }

    switch (gMapManager.GetCurrentMap()) {
    case MAP_LORENCIA:    Lorencia(model, param_1, type, size); break;
    case MAP_DEVIAS:      Devias(model, param_1, type, size); break;
    case MAP_NORIA:       Noria(model, param_1, type, size); break;
    case MAP_LOST_TOWER:  LostTower(model, param_1, type, size); break;
    case MAP_ARENA:       if (type == 9) Torch(model, param_1, size); break;
    case MAP_TARKAN:      Tarkan(model, param_1, type); break;
    case MAP_DEVIL_SQUARE1: if (type == 2) DevilSquareStatue(model, param_1); break;
    case MAP_ICARUS:      Icarus(model, param_1, type); break;
    case MAP_BLOOD_CASTLE1: case MAP_BLOOD_CASTLE2: case MAP_BLOOD_CASTLE3:
    case MAP_BLOOD_CASTLE4: case MAP_BLOOD_CASTLE5: case MAP_BLOOD_CASTLE6:
        BloodCastle(model, param_1, type);
        break;
    }
}
