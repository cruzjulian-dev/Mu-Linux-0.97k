#pragma once

#include <cstddef>
#include "platform/PlatformHeaders.h"
#include "types.h"

// Slot del pool de partículas (IDA: unk_7ABF5F0, stride 0x70).  Los nombres
// salen del uso en Particle_Spawn (0x00475220), MoveParticles (0x00477090) y
// RenderParticles (0x00478C00).
struct PARTICLE {
    BYTE  Live;              // 0x00  slot ocupado
    BYTE  _pad01[3];
    int   Type;              // 0x04  textura / tipo (BITMAP_*)
    int   SubType;           // 0x08
    float Scale;             // 0x0C
    float Position[3];       // 0x10
    float Angle[3];          // 0x1C
    float Light[3];          // 0x28
    float _unk34;            // 0x34  sin uso conocido
    int   LifeTime;          // 0x38
    DWORD TargetLow;         // 0x3C  32 bits bajos del owner; el puntero
                             //       completo vive en g_ParticleOwner[slot]
    float Rotation;          // 0x40
    int   Frame;             // 0x44  contador de frames de algunos tipos
    float Gravity;           // 0x48
    float Velocity[3];       // 0x4C
    float Extra[3];          // 0x58  color secundario o parámetros por tipo
    float StartPosition[3];  // 0x64
};
static_assert(sizeof(PARTICLE) == 0x70, "PARTICLE");
static_assert(offsetof(PARTICLE, Type) == 0x04 && offsetof(PARTICLE, Scale) == 0x0C, "PARTICLE");
static_assert(offsetof(PARTICLE, Position) == 0x10 && offsetof(PARTICLE, Angle) == 0x1C, "PARTICLE");
static_assert(offsetof(PARTICLE, Light) == 0x28 && offsetof(PARTICLE, LifeTime) == 0x38, "PARTICLE");
static_assert(offsetof(PARTICLE, TargetLow) == 0x3C && offsetof(PARTICLE, Rotation) == 0x40, "PARTICLE");
static_assert(offsetof(PARTICLE, Frame) == 0x44 && offsetof(PARTICLE, Gravity) == 0x48, "PARTICLE");
static_assert(offsetof(PARTICLE, Velocity) == 0x4C && offsetof(PARTICLE, Extra) == 0x58, "PARTICLE");
static_assert(offsetof(PARTICLE, StartPosition) == 0x64, "PARTICLE");

constexpr int PARTICLE_STRIDE = sizeof(PARTICLE);
