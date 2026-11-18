// WorldObject_Behavior.cpp
// Comportamiento por mapa y tipo de los objetos del mundo: la segunda mitad de
// MoveObjects (IDA 0x004FDC00, desde LABEL_38).
//
// El decompile era un switch por mapa con saltos entre mapas (LABEL_51, _59,
// _92, _111, _116, _123, _132).  Acá lo que se repite va como tabla
// (`kObjects`) y lo demás como una función por objeto.  Para un mismo objeto
// la fila de la tabla corre antes que su función; las escrituras de la fila son
// independientes de las de la función, así que el resultado es el del binario.
#include <cstdint>
#include "stdafx.h"
#include <array>
#include "globals.h"
#include "functions.h"
#include <cstring>
#include "game/entity/EntityTypes.h"
#include "scenes/WorldObject_Behavior.h"

extern void __cdecl Effect_PhysicsTick(std::uintptr_t Object); // Efectos del mapa 4

namespace {

// ── Llamas y humo fijos (WorldObject_EmitFireSmoke) ──────────────────────────
struct SceneFlamePreset {
    int Variant;
    int Count;
    float Offset[2][3];
    bool RandomAlpha;
    bool HideMesh;
};
constexpr SceneFlamePreset SceneFlames[] = {
    { 0, 1, {{0, 0, 200}}, false, false },
    { 0, 1, {{0, -30, 60}}, false, false },
    { 0, 1, {{0, 0, 60}}, true, false },
    { 0, 2, {{-150, -150, 140}, {150, -150, 140}}, false, false },
    { 0, 2, {{90, -200, 30}, {90, 200, 30}}, false, false },
    { 0, 1, {{0, 0, 0}}, false, true },
    { 1, 1, {{0, 0, 0}}, false, true },
    { 2, 1, {{0, 0, 0}}, false, true },
    { 0, 1, {{0, -30, 240}}, false, false },
    { 0, 1, {{0, 0, 190}}, false, false },
    { 0, 1, {{0, 0, 50}}, false, false },
};
constexpr auto MakeLorenciaFlameIndices()
{
    std::array<unsigned char, 133> indices{};
    indices[50] = 0; indices[51] = 1; indices[52] = 2;
    indices[55] = 3; indices[80] = 4;
    indices[130] = 5; indices[131] = 6; indices[132] = 7;
    return indices;
}
constexpr auto LorenciaFlameIndices = MakeLorenciaFlameIndices();

void EmitSceneFlames(char* object, const SceneFlamePreset& preset)
{
    for (int i = 0; i < preset.Count; ++i) {
        const auto& offset = preset.Offset[i];
        WorldObject_EmitFireSmoke(preset.Variant, AsObject(object), offset[0], offset[1], offset[2]);
    }
    if (preset.RandomAlpha)
        *(float*)(object + 104) = (double)(rand() % 6 + 4) * 0.1;
    if (preset.HideMesh)
        *(int*)(object + 88) = -2;
}

// ── Tabla de comportamientos simples ─────────────────────────────────────────
//
// Campos del OBJECT que tocan:  +88 HiddenMesh · +100 BlendMesh ·
// +104 BlendMeshLight · +108 BlendMeshTexCoordU · +112 BlendMeshTexCoordV.
// `ModelFlag` es el byte +136 del modelo (BMD).

// Textura que se desplaza:  campo = (double)(t % period) * step, con
// t = WorldTime o -WorldTime (`NegateFirst`, el `-(int64)WorldTime % N` del
// binario: da el mismo valor salvo por el signo del cero).
struct Scroll { unsigned short Field; bool NegateFirst; int Period; double Step; };

struct SimpleObject {
    short Map;
    short Type;
    signed char ModelFlag;    // -1 = no se toca
    signed char BlendMesh;    // -1 = no se toca
    bool HideMesh;
    unsigned char LightMod;   // != 0: BlendMeshLight = (rand() % LightMod + LightAdd) * 0.1
    unsigned char LightAdd;
    Scroll Scrolls[2];        // Field 0 = sin desplazamiento
};

constexpr unsigned short U = 108, V = 112;

constexpr SimpleObject kObjects[] = {
    // Lorencia
    { MAP_LORENCIA, 0x75, -1, -1, false, 4, 4, {} },
    { MAP_LORENCIA, 0x7A, -1, -1, false, 4, 4, {} },
    { MAP_LORENCIA, 0x76, -1, -1, false, 0, 0, {{ V, true, 1000, 0.001 }} },
    { MAP_LORENCIA, 0x77, -1, -1, false, 0, 0, {{ V, true, 1000, 0.001 }} },
    // Dungeon
    { MAP_DUNGEON, 0x16, 1, -1, false, 0, 0, {{ V, false, 1000, -0.001 }} },
    { MAP_DUNGEON, 0x17, 1, -1, false, 0, 0, {{ V, false, 1000, -0.001 }} },
    { MAP_DUNGEON, 0x18, 1, -1, false, 0, 0, {{ V, false, 1000, -0.001 }} },
    { MAP_DUNGEON, 0x27, -1, -1, true, 0, 0, {} },
    { MAP_DUNGEON, 0x28, -1, -1, true, 0, 0, {} },
    { MAP_DUNGEON, 0x33, -1, -1, true, 0, 0, {} },
    // Devias
    { MAP_DEVIAS, 78, -1, -1, false, 4, 4, {} },
    // Noria
    { MAP_NORIA, 0x12, -1, -1, false, 0, 0, {{ V, false, 1000, 0.001 }} },
    { MAP_NORIA, 0x27, -1, 1, false, 0, 0, {} },
    { MAP_NORIA, 0x29, -1, 0, false, 0, 0, {{ V, false, 2000, 0.00050000002 }} },
    { MAP_NORIA, 0x2A, 0, -1, false, 0, 0, {{ U, false, 500, -0.0020000001 }} },
    { MAP_NORIA, 0x2B, 0, -1, false, 0, 0, {{ U, false, 500, 0.0020000001 }} },
    // Lost Tower
    { MAP_LOST_TOWER, 3, -1, -1, false, 0, 0, {{ U, true, 1000, 0.001 }} },
    { MAP_LOST_TOWER, 4, -1, -1, false, 0, 0, {{ U, true, 1000, 0.001 }} },
    { MAP_LOST_TOWER, 0x12, -1, 1, false, 0, 0, {} },
    { MAP_LOST_TOWER, 0x17, -1, 1, false, 0, 0, {} },
    { MAP_LOST_TOWER, 0x13, -1, 4, false, 0, 0, {{ U, true, 1000, 0.001 }} },
    { MAP_LOST_TOWER, 0x14, -1, 4, false, 0, 0, {{ U, true, 1000, 0.001 }} },
    { MAP_LOST_TOWER, 0x19, -1, -1, true, 0, 0, {} },
    // Exile
    { MAP_EXILE, 2, -1, 0, false, 0, 0, {} },
    { MAP_EXILE, 3, -1, 0, false, 4, 6, {} },
    // Arena
    { MAP_ARENA, 21, -1, 3, false, 0, 0, {{ V, true, 1000, 0.001 }} },
    { MAP_ARENA, 38, -1, -1, true, 0, 0, {} },
    // Atlans
    { MAP_ATLANS, 0x17, -1, 0, false, 0, 0, {} },
    { MAP_ATLANS, 0x20, -1, 1, false, 0, 0, {} },
    { MAP_ATLANS, 0x22, -1, 1, false, 0, 0, {} },
    { MAP_ATLANS, 0x26, -1, 0, false, 0, 0, {} },
    { MAP_ATLANS, 0x28, -1, 0, false, 0, 0, {} },
    // Tarkan
    { MAP_TARKAN, 2, -1, 0, false, 0, 0, {{ U, true, 1000, 0.001 }} },
    { MAP_TARKAN, 4, -1, 0, false, 0, 0, {{ V, true, 10000, 0.000099999997 }} },
    { MAP_TARKAN, 7, -1, 0, false, 0, 0, {} },
    { MAP_TARKAN, 0xB, -1, -1, false, 0, 0, {{ V, true, 10000, 0.00019999999 }} },
    { MAP_TARKAN, 0xD, -1, -1, false, 0, 0, {{ V, true, 10000, 0.00019999999 }} },
    { MAP_TARKAN, 0x49, -1, -1, false, 0, 0, {{ V, true, 10000, 0.00019999999 }} },
    { MAP_TARKAN, 0x4B, -1, -1, false, 0, 0, {{ V, true, 10000, 0.00019999999 }} },
    { MAP_TARKAN, 0x4F, -1, -1, false, 0, 0, {{ V, true, 10000, 0.00019999999 }} },
    { MAP_TARKAN, 0xC, -1, -1, false, 0, 0, {{ U, true, 50000, 0.000049999999 }, { V, true, 50000, 0.000049999999 }} },
    { MAP_TARKAN, 0x3D, -1, 1, false, 0, 0, {{ V, true, 1000, 0.001 }} },
    { MAP_TARKAN, 0x3F, -1, -1, true, 0, 0, {} },
    { MAP_TARKAN, 0x40, -1, -1, true, 0, 0, {} },
    { MAP_TARKAN, 0x41, -1, 1, false, 0, 0, {{ V, true, 1000, 0.001 }} },
    { MAP_TARKAN, 0x42, -1, 1, false, 0, 0, {{ V, true, 1000, 0.001 }} },
    { MAP_TARKAN, 0x48, -1, 0, false, 0, 0, {{ V, true, 10000, 0.00019999999 }} },
    { MAP_TARKAN, 0x52, -1, 0, false, 0, 0, {} },
};

const SimpleObject* FindSimple(int map, short type)
{
    for (const SimpleObject& row : kObjects)
        if (row.Map == map && row.Type == type) return &row;
    return nullptr;
}

void ApplySimple(std::uintptr_t o, const SimpleObject& row)
{
    if (row.ModelFlag >= 0)
        *(BYTE *)(Models + BMD_STRIDE * *(short *)(o + 2) + 136 + BMD_FIELD_SHIFT) = (BYTE)row.ModelFlag;
    if (row.BlendMesh >= 0)
        *(DWORD *)(o + 100) = (DWORD)row.BlendMesh;
    if (row.HideMesh)
        *(DWORD *)(o + 88) = -2;
    if (row.LightMod)
        *(float *)(o + 104) = (double)(rand() % row.LightMod + row.LightAdd) * 0.1;
    for (const Scroll& s : row.Scrolls) {
        if (!s.Field) continue;
        const std::int64_t t = s.NegateFirst ? -(std::int64_t)WorldTime : (std::int64_t)WorldTime;
        *(float *)(o + s.Field) = (double)(t % s.Period) * s.Step;
    }
}

// ── Funciones por objeto ─────────────────────────────────────────────────────

// Luz cálida sobre el terreno alrededor del objeto (LABEL_116 del binario).
void TerrainGlow(std::uintptr_t o, float r, float g, float b, int range)
{
    float light[3] = { r, g, b };
    AddTerrainLight(*(float *)(o + 16), *(float *)(o + 20), light, range, PrimaryTerrainLight[0]);
}

// Lorencia 0x5A — fogata.
void LorenciaCampfire(std::uintptr_t o)
{
    const double v = (double)(rand() % 2 + 6) * 0.1;
    TerrainGlow(o, (float)v, (float)(v * 0.80000001), (float)(v * 0.60000002), 3);
}

// Lorencia 0x96 — antorcha (LABEL_51).
void LorenciaTorch(std::uintptr_t o)
{
    const double v = (double)(rand() % 4 + 3) * 0.1;
    TerrainGlow(o, (float)v, (float)(v * 0.60000002), (float)(v * 0.2), 3);
}

// Dungeon 0x34 — humo de la caldera; 2 de cada 3 cuadros sólo oculta la malla.
void DungeonSmoke(std::uintptr_t o)
{
    if (rand() % 3) {
        *(DWORD *)(o + 88) = -2;
        return;
    }
    CreateEffect(215, (float *)(o + 16), (float *)(o + 28), (float *)(o + 232),
                 0, 0, (float *)-1, 0, 0);
    *(DWORD *)(o + 88) = -2;
}

// Devias 20/65/86/88 — puertas que se abren cuando el héroe se acerca.
void DeviasDoor(std::uintptr_t o)
{
    if (EditFlag) return;
    float* const position = (float *)(o + 16);
    const short type = *(WORD *)(o + 2);
    const double dy = *(float *)(Hero + 20) - *(float *)(o + 56);
    const double dx = *(float *)(Hero + 16) - *(float *)(o + 52);
    const long double distance = sqrt(dy * dy + dx * dx);
    const float ob = distance;
    if (distance >= 200.0) {
        *(float *)(o + 36) = TurnAngle2(*(float *)(o + 36), *(float *)(o + 48), 10.0);
        *position = (*(float *)(o + 52) - *position) * 0.2 + *position;
        *(float *)(o + 20) = (*(float *)(o + 56) - *(float *)(o + 20)) * 0.2 + *(float *)(o + 20);
    } else if (type == 86) {
        // Puerta corrediza: se desplaza según su orientación (90/270/0/180 grados).
        if (*(DWORD *)(o + 36) == 1119092736)
            *(float *)(o + 20) = 200.0 - ob + 200.0 - ob + *(float *)(o + 56);
        if (*(DWORD *)(o + 36) == 1132920832)
            *(float *)(o + 20) = *(float *)(o + 56) - (200.0 - ob + 200.0 - ob);
        if (*(float *)(o + 36) == 0.0)
            *position = 200.0 - ob + 200.0 - ob + *(float *)(o + 52);
        if (*(DWORD *)(o + 36) == 1127481344)
            *position = *(float *)(o + 52) - (200.0 - ob + 200.0 - ob);
        PlayBuffer(18, 0, 0);
    } else {
        // Puerta de bisagra: gira según su orientación de reposo.
        if (*(DWORD *)(o + 48) == 1119092736)
            *(float *)(o + 36) = 30.0 - (200.0 - ob) * 0.5;
        if (*(DWORD *)(o + 48) == 1132920832)
            *(float *)(o + 36) = (200.0 - ob) * 0.5 + 330.0;
        if (*(float *)(o + 48) == 0.0)
            *(float *)(o + 36) = 300.0 - (200.0 - ob) * 0.5;
        if (*(DWORD *)(o + 48) == 1127481344)
            *(float *)(o + 36) = (200.0 - ob) * 0.5 + 240.0;
        PlayBuffer(17, 0, 0);
    }
}

// Lost Tower 0x18 — llama: oculta la malla y cada tanto emite fuego.
void LostTowerFlame(std::uintptr_t o)
{
    *(DWORD *)(o + 88) = -2;
    signed int r = rand() & 0x8000003F;
    bool hit = r == 0;
    if (r < 0)
        hit = (((BYTE)r - 1) | 0xFFFFFFC0) == -1;
    if (hit)
        CreateEffect(BITMAP_FLAME, (float *)(o + 16), (float *)(o + 28), (float *)(o + 232), 0, 0, (float *)-1, 0, 0);
}

// Atlans 0x16 — burbujas: un contador en +128 que cada 5 décimas larga una.
void AtlansBubbles(std::uintptr_t o)
{
    const double counter = *(float *)(o + 128) + 0.1;
    *(DWORD *)(o + 88) = -2;
    *(float *)(o + 128) = counter;
    if (counter > 10.0)
        *(DWORD *)(o + 128) = 0;
    if (*(float *)(o + 128) > 5.0)
        Particle_Spawn(BITMAP_BUBBLE, (float *)(o + 16), (float *)(o + 28), (float *)(o + 232), 0, 1.0, 0);
}

// Tarkan 4 — brillo que late, con su luz sobre el terreno.
void TarkanPulseLamp(std::uintptr_t o)
{
    const long double v = sin(WorldTime * 0.0020000001) * 0.34999999 + 0.64999998;
    *(float *)(o + 104) = v;
    TerrainGlow(o, (float)v, (float)v, (float)v, 3);
}

// Tarkan 7 — antorcha que late con un desfase por su ángulo (LABEL_51).
void TarkanTorch(std::uintptr_t o)
{
    const double t = *(float *)(o + 36) * 100.0 + WorldTime;
    const long double v = sin(t * 0.0020000001) * 0.34999999 + 0.64999998;
    *(float *)(o + 104) = v;
    TerrainGlow(o, (float)v, (float)(v * 0.60000002), (float)(v * 0.2), 3);
}

// Tarkan 0x3D/0x41/0x42 — luz que late, de alcance 2 (LABEL_123).
void TarkanSmallGlow(std::uintptr_t o)
{
    const long double v = sin(WorldTime * 0.0020000001) * 0.34999999 + 0.64999998;
    float light[3] = { (float)v, (float)(v * 0.60000002), (float)(v * 0.2) };
    AddTerrainLight(*(float *)(o + 16), *(float *)(o + 20), light, 2, PrimaryTerrainLight[0]);
}

} // namespace

void WorldObject_PerMapTick(std::uintptr_t o)
{
    const int map = gMapManager.GetCurrentMap();
    const short type = *(WORD *)(o + 2);

    // Blood Castle: las mitades de la puerta (9/10) quedan ocultas hasta que
    // el derrumbe les pone +134 = 4.
    if (map >= MAP_BLOOD_CASTLE1 && map <= MAP_BLOOD_CASTLE6) {
        const int t = *(short *)(o + 2);
        if (t >= 9 && t <= 10 && *(WORD *)(o + 134) != 4)
            *(DWORD *)(o + 88) = -2;
        return;
    }

    if (const SimpleObject* row = FindSimple(map, type))
        ApplySimple(o, *row);

    switch (map)
    {
    case MAP_LORENCIA:
        switch (type) {
        case 0x32: case 0x33: case 0x34: case 0x37: case 0x50:
        case 0x82: case 0x83: case 0x84:
            EmitSceneFlames((char*)o, SceneFlames[LorenciaFlameIndices[type]]);
            break;
        case 0x5A: LorenciaCampfire(o); break;
        case 0x96: LorenciaTorch(o); break;
        }
        break;
    case MAP_DUNGEON:
        switch (type) {
        case 0x29: EmitSceneFlames((char*)o, SceneFlames[8]); break;
        case 0x2A: EmitSceneFlames((char*)o, SceneFlames[9]); break;
        case 0x34: DungeonSmoke(o); break;
        }
        break;
    case MAP_DEVIAS:
        switch (type) {
        case 20: case 65: case 86: case 88: DeviasDoor(o); break;
        case 30: case 66: EmitSceneFlames((char*)o, SceneFlames[10]); break;
        }
        break;
    case MAP_LOST_TOWER:
        switch (type) {
        case 0x18: LostTowerFlame(o); break;
        case 0x26: case 0x27: Effect_PhysicsTick(o); break;
        }
        break;
    case MAP_ATLANS:
        switch (type) {
        case 0x16: AtlansBubbles(o); break;
        case 0x17: *(float *)(o + 104) = sin(WorldTime * 0.0020000001) * 0.30000001 + 0.5; break;
        case 0x20: case 0x22: *(float *)(o + 104) = (sin(WorldTime * 0.0040000002) + 1.0) * 0.5; break;
        case 0x28:
            *(DWORD *)(o + 204) = 1028443341;   // 0.05f: velocidad de la animación
            *(float *)(o + 104) = sin(WorldTime * 0.0040000002) * 0.30000001 + 0.5;
            break;
        }
        break;
    case MAP_TARKAN:
        switch (type) {
        case 4: TarkanPulseLamp(o); break;
        case 7: TarkanTorch(o); break;
        case 0x3D: case 0x41: case 0x42: TarkanSmallGlow(o); break;
        case 0x52:
            *(DWORD *)(o + 232) = 1065353216;   // luz del objeto = 1.0
            *(DWORD *)(o + 236) = 1065353216;
            *(DWORD *)(o + 240) = 1065353216;
            break;
        }
        break;
    default:
        break;
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Creación: valores iniciales por mapa y tipo (CreateObject 0x004FF5A0)
// ═════════════════════════════════════════════════════════════════════════════

namespace {

using P = OperatePose;
constexpr OperableObject kOperable[] = {
    // Lorencia
    { MAP_LORENCIA,   6,   P::Sit,     false, false, false },
    { MAP_LORENCIA, 145,   P::Sit,     true,  false, false },
    { MAP_LORENCIA, 146,   P::Sit,     false, false, false },
    { MAP_LORENCIA, 133,   P::Pose,    true,  true,  true  },
    // Dungeon
    { MAP_DUNGEON,   59,   P::Sit,     false, false, false },
    { MAP_DUNGEON,   60,   P::Pose,    true,  true,  true  },
    // Devias
    { MAP_DEVIAS,    22,   P::Sit,     true,  false, false },
    { MAP_DEVIAS,    25,   P::Sit,     true,  false, false },
    { MAP_DEVIAS,    40,   P::Sit,     true,  false, false },
    { MAP_DEVIAS,    45,   P::Sit,     false, false, false },
    { MAP_DEVIAS,    55,   P::Sit,     true,  false, false },
    { MAP_DEVIAS,    73,   P::Sit,     false, false, false },
    { MAP_DEVIAS,    91,   P::Pose,    true,  true,  true  },
    // Noria: 38 son los orbes sobre los que se flota.
    { MAP_NORIA,      8,   P::Sit,     false, false, false },
    { MAP_NORIA,     38,   P::Healing, true,  false, true  },
    // Atlans
    { MAP_ATLANS,    39,   P::Pose,    true,  false, true  },
    // Tarkan.  En el decompile el 78 cae en una cola que no lo atiende (huele a
    // plegado de Hex-Rays); MU 5.2 tiene `WD_8TARKAN: case 78: Sit`.
    { MAP_TARKAN,    78,   P::Sit,     false, false, false },
};

// Lo que CreateObject fija para objetos que no se registran.  Velocity y
// CollisionRange se guardan como bits del binario.
struct MapObjectInit {
    short Map;
    short Type;
    signed char BlendMesh;       // -1 = no se toca
    bool HideMesh;
    bool SetVelocity;
    std::uint32_t VelocityBits;
    bool SetCollision;
    std::uint32_t CollisionBits;
};
constexpr MapObjectInit kMapObjectInit[] = {
    // Lorencia
    { MAP_LORENCIA, 0x34, 1, false, false, 0, false, 0 },
    { MAP_LORENCIA, 0x3b, -1, false, true, 0, false, 0 },
    { MAP_LORENCIA, 0x50, -1, false, false, 0, true, 0xc2480000 },   // -50.0
    { MAP_LORENCIA, 0x5a, 1, false, true, 0x3e99999a, false, 0 },    // 0.3
    { MAP_LORENCIA, 0x96, 1, false, true, 0x3e99999a, false, 0 },
    { MAP_LORENCIA, 0x60, -1, false, true, 0x3e99999a, false, 0 },
    { MAP_LORENCIA, 0x61, -1, false, true, 0x3e99999a, false, 0 },
    { MAP_LORENCIA, 0x62, 2, false, false, 0, false, 0 },
    { MAP_LORENCIA, 0x77, 2, false, false, 0, false, 0 },
    { MAP_LORENCIA, 0x69, 3, false, false, 0, false, 0 },
    { MAP_LORENCIA, 0x75, 4, false, false, 0, false, 0 },
    { MAP_LORENCIA, 0x7a, 4, false, false, 0, false, 0 },
    { MAP_LORENCIA, 0x76, 8, false, false, 0, false, 0 },
    // Devias
    { MAP_DEVIAS, 0x13, 0, false, false, 0, false, 0 },
    { MAP_DEVIAS, 0x5c, 0, false, false, 0, false, 0 },
    { MAP_DEVIAS, 0x5d, 0, false, false, 0, false, 0 },
    { MAP_DEVIAS, 0x36, 1, false, false, 0, false, 0 },
    { MAP_DEVIAS, 0x38, 1, false, false, 0, false, 0 },
    { MAP_DEVIAS, 0x4e, 3, false, false, 0, false, 0 },
    { MAP_DEVIAS, 100, -1, true, false, 0, false, 0 },
    // Noria
    { MAP_NORIA, 1, 1, false, false, 0, false, 0 },
    { MAP_NORIA, 9, 3, false, false, 0, false, 0 },
    { MAP_NORIA, 0x11, 0, false, false, 0, false, 0 },
    { MAP_NORIA, 0x13, 0, false, false, 0, false, 0 },
    { MAP_NORIA, 0x25, 0, false, false, 0, false, 0 },
    { MAP_NORIA, 0x12, 2, false, false, 0, false, 0 },
};

void StoreBits(float& value, std::uint32_t bits) { memcpy(&value, &bits, sizeof(value)); }

} // namespace

const OperableObject* WorldObject_FindOperable(int map, int type)
{
    for (const OperableObject& row : kOperable)
        if (row.Map == map && row.Type == type) return &row;
    return nullptr;
}

void WorldObject_ApplyMapDefaults(OBJECT& object, void* node, int map, int type)
{
    if (const OperableObject* op = WorldObject_FindOperable(map, type)) {
        Entity_InitRenderState(node);   // sub_4FF580: lista Operates
        if (op->BigBox) {
            StoreBits(object.BoundingBoxMax[0], 0x42200000);   // 40
            StoreBits(object.BoundingBoxMax[1], 0x42200000);
            StoreBits(object.BoundingBoxMax[2], 0x43200000);   // 160
        }
        if (op->HideMesh) object.HiddenMesh = -2;
        return;
    }

    // Puertas de Devias (20/65/86/88).  Guarda el estado de REPOSO que usa la
    // animación de apertura de DeviasDoor:
    //   HeadAngle[0..2]       (+40,+44,+48) = Angle[0..2]
    //   HeadTargetAngle[0..2] (+52,+56,+60) = Position[0..2]
    // y normaliza el giro:  Angle[2] = HeadAngle[2] = (int)Angle[2] % 360.
    // El `% 360` es sobre el VALOR truncado, no sobre los bits del float (IDA:
    // `(std::int64_t)*((float *)v6 + 9) % 360`); el tick compara HeadAngle[2] con
    // 90/270/0/180.
    if (map == MAP_DEVIAS && (type == 0x14 || type == 0x41 || type == 0x56 || type == 0x58)) {
        memcpy(object.HeadAngle, object.Angle, sizeof(object.HeadAngle));
        memcpy(object.HeadTargetAngle, object.Position, sizeof(object.HeadTargetAngle));
        const float normalized = (float)(long long)(((long long)object.Angle[2]) % 360);
        object.Angle[2] = object.HeadAngle[2] = normalized;
        return;
    }

    for (const MapObjectInit& row : kMapObjectInit) {
        if (row.Map != map || row.Type != type) continue;
        if (row.BlendMesh >= 0) object.BlendMesh = row.BlendMesh;
        if (row.HideMesh) object.HiddenMesh = -2;
        if (row.SetVelocity) StoreBits(object.Velocity, row.VelocityBits);
        if (row.SetCollision) StoreBits(object.CollisionRange, row.CollisionBits);
        return;
    }
}
