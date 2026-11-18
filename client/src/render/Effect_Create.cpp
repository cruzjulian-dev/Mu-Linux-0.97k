// CreateEffect.cpp
// CreateEffect @ 0x00460dc0
//
// CreateEffect -- spawns a visual effect by type ID.

#include "stdafx.h"
#include "game/core/RenderFxSnapshots.h"
#include "game/core/RenderDerivedState.h"
#include "render/Particle_Render.h"


namespace {
// IDA: CreateEffect (0x00460DC0), fragmentos 260..263.
struct EffectDebrisPreset { int SpeedBase; short BoostedType; };
constexpr EffectDebrisPreset EffectDebrisPresets[] = { { 32, 261 }, { 64, 262 } };
constexpr int EffectLifetimeIndex = 0x18; // DWORD, no float.
constexpr int EffectBlendMeshIndex = 0x19; // Índice con signo, no float.
// IDA: CreateEffect (0x00460DC0), flechas 216..224.
// 217/221 conservan BlendMesh; sólo 223/224 escriben gravedad.
struct EffectArrowPreset {
    int BlendMesh;
    bool SetBlendMesh;
    float Scale;
    float ForwardSpeed;
    bool HasGravity;
    int Lifetime;
};
constexpr EffectArrowPreset EffectArrowPresets[] = {
    { 1, true, 0.8f, -70.0f, false, 30 },
    { 0, false, 0.8f, -70.0f, false, 30 },
    { 0, true, 0.8f, -70.0f, false, 30 },
    { 0, true, 0.8f, -70.0f, false, 30 },
    { 0, true, 0.8f, -70.0f, false, 30 },
    { 0, false, 0.8f, -70.0f, false, 30 },
    { -2, true, 0.8f, -70.0f, false, 30 },
    { 0, true, 1.0f, -30.0f, true, 40 },
    { 0, true, 1.8f, -50.0f, true, 30 },
};
} // namespace

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

// Tamachan -- IDA 0.98j CreateEffect (sub_46C290) case 183.
// Desviacion: en el 0.97k el tipo 183 lo usan los peces de Atlans, asi que
// va en el 193 (0xC1), libre en los dos binarios.  Arranca en la accion 1
// (flotando), y nace a 150..300 de su punto de aparicion (+52) en un angulo
// al azar de 120 a 240 grados; ese punto queda tambien como destino (+368).
static void Tamachan_Create(char *o)
{
    *(int *)(o + 96) = rand() % 50 + 50;           // lifetime
    AsObject(o)->AI = 1;                                    // estado: flotando
    *(float *)(o + 216) = -30.0f;                  // 0xC1F00000
    SetAction(AsObject(o), (unsigned char)AsObject(o)->AI);   // SetAction
    *(float *)(o + 192) = 0.0f;                    // velocidad
    *(float *)(o + 196) = 0.0f;
    *(float *)(o + 200) = 0.0f;
    *(float *)(o + 232) = 1.0f;                    // Light
    *(float *)(o + 236) = 1.0f;
    *(float *)(o + 240) = 1.0f;
    for (int i = 0; i < 3; ++i) {
        ((float*)&AsObject(o)->StartPosition[0])[i] = ((float *)(o + 16))[i];
        ((float *)(o + 52))[i]  = ((float *)(o + 16))[i];
    }
    float offset[3], angle[3], matrix[12], rotated[3];
    int r = rand();
    offset[0] = 0.0f;
    offset[1] = (float)(-150.0 - (double)(r % 150));
    offset[2] = 0.0f;
    angle[0] = 0.0f;
    angle[1] = 0.0f;
    angle[2] = (float)((double)(rand() % 120) + 180.0 - 60.0);
    Matrix_BuildFromEuler(angle, matrix);          // AngleMatrix
    Vector_Rotate(offset, matrix, rotated);        // VectorRotate
    for (int i = 0; i < 3; ++i) {
        ((float*)&AsObject(o)->StartPosition[0])[i] = rotated[i] + ((float *)(o + 52))[i];
        ((float *)(o + 16))[i]  = ((float*)&AsObject(o)->StartPosition[0])[i];
    }
}

// ── Handlers de CreateEffect (T9b) ────────────────────────────────────────────
// Cada case limpio del switch principal (sin goto ni labels) vive en su propia
// función.  Reciben por referencia las locales de CreateEffect que usan y
// devuelven true cuando el case termina la función (con su valor en result).

// IDA: CreateEffect (0x00460DC0), case 182.
static bool CreateEffect_Type182(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 2;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  AsObject(pfVar17)->Scale = 10.0;
  AsObject(pfVar17)->Position[1] = AsObject(pfVar17)->Position[1] + 200.0f;
  AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] - 190.0f;
  AsObject(pfVar17)->LightEnable = 0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 185.
static bool CreateEffect_Type185(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 1;
  AsObject(pfVar17)->Alpha = 0.5;
  AsObject(pfVar17)->Scale = 1.5;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_FIRE.
static bool CreateEffect_Fire(float *& param_8, float& fVar27, intptr_t& iVar9, float*& pfVar17, float*& pfVar8, intptr_t& uVar10, float*& result)
{
  *(int*)&pfVar17[EffectBlendMeshIndex] = 1;
  fVar27 = *(float *)&AsObject(pfVar17)->SubType;
  if (fVar27 == 0.0) {
    *(int*)&pfVar17[EffectLifetimeIndex] = 40;
    uVar10 = _rand() % 8;
    param_8 = (float *)(uVar10 + 10);
    AsObject(pfVar17)->Scale = (float)(intptr_t)param_8 * 0.1f;
    uVar10 = _rand() % 32;
  }
  else {
    if (*(int*)&fVar27 == 2) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 40;
      uVar10 = _rand() % 8;
      AsObject(pfVar17)->Scale = (float)(int)(uVar10 + 10) * 0.1f;
      AsObject(pfVar17)->Direction[0] = 0.0;
      AsObject(pfVar17)->Direction[1] = 0.0;
      AsObject(pfVar17)->Direction[2] = -50.0;
      { result = (float*)((float *)0x0); return true; }
    }
    if (*(int*)&fVar27 == 3) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 80;
      AsObject(pfVar17)->Scale = 0.3;
      AsObject(pfVar17)->Direction[0] = 0.0;
      AsObject(pfVar17)->Direction[1] = -12.0;
      AsObject(pfVar17)->Direction[2] = 0.0;
      { result = (float*)(pfVar8); return true; }
    }
    if (*(int*)&fVar27 == 4) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 40;
      iVar9 = _rand();
      param_8 = (float *)(iVar9 % 10 + 0xf);
      AsObject(pfVar17)->Scale = (float)(intptr_t)param_8 * 0.1f;
      uVar10 = _rand();
      param_8 = (float*)(uVar10 % 32);
      AsObject(pfVar17)->Position[0] = (float)(intptr_t)param_8 + AsObject(pfVar17)->Position[0] + 130.0f;
      AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 400.0f;
      AsObject(pfVar17)->Direction[0] = 0.0;
      iVar9 = _rand();
      param_8 = (float *)(iVar9 % 0x14);
      AsObject(pfVar17)->Direction[1] = -((float)(intptr_t)param_8 + 10.0f);
      iVar9 = _rand();
      AsObject(pfVar17)->Direction[2] = -((float)(iVar9 % 10) + 20.0f);
      AsObject(pfVar17)->Angle[0] = 0.0;
      AsObject(pfVar17)->Angle[1] = 20.0;
      AsObject(pfVar17)->Angle[2] = 0.0;
      { result = (float*)((float *)(iVar9 / 10)); return true; }
    }
    if (*(int*)&fVar27 == 5) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 40;
      AsObject(pfVar17)->Gravity = 5.0;
      AsObject(pfVar17)->Direction[0] = 0.0;
      AsObject(pfVar17)->Direction[1] = -30.0;
      AsObject(pfVar17)->Direction[2] = 0.0;
      { result = (float*)(pfVar8); return true; }
    }
    if (*(int*)&fVar27 != 6) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 60;
      uVar10 = _rand() % 4;
      AsObject(pfVar17)->Scale = (float)(int)(uVar10 + 8) * 0.1f;
      AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 120.0f;
      AsObject(pfVar17)->Direction[0] = 0.0;
      AsObject(pfVar17)->Direction[1] = -50.0;
      AsObject(pfVar17)->Direction[2] = 0.0;
      { result = (float*)((float *)0x0); return true; }
    }
    *(int*)&pfVar17[EffectLifetimeIndex] = 40;
    uVar10 = _rand() % 8;
    param_8 = (float *)(uVar10 + 0xf);
    AsObject(pfVar17)->Scale = (float)(intptr_t)param_8 * 0.1f;
    uVar10 = _rand() % 32;
  }
  AsObject(pfVar17)->Position[0] = (float)(int)uVar10 + AsObject(pfVar17)->Position[0] + 130.0f;
  AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 400.0f;
  AsObject(pfVar17)->Direction[0] = 0.0;
  AsObject(pfVar17)->Direction[1] = 0.0;
  AsObject(pfVar17)->Direction[2] = -50.0;
  AsObject(pfVar17)->Angle[0] = 0.0;
  AsObject(pfVar17)->Angle[1] = 20.0;
  AsObject(pfVar17)->Angle[2] = 0.0;
  { result = (float*)((float *)0x0); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_POISON.
static bool CreateEffect_Poison(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectBlendMeshIndex] = 1;
  *(int*)&pfVar17[EffectLifetimeIndex] = 40;
  AsObject(pfVar17)->Scale = 1.0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 194.
static bool CreateEffect_Type194(float *& param_5, float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 50;
  AsObject(pfVar17)->Scale = 0.7;
  if (param_5 != (float *)0x0) {
    *(int*)&pfVar17[EffectBlendMeshIndex] = -4;
    AsObject(pfVar17)->Angle[0] = 0.0;
    AsObject(pfVar17)->Angle[1] = 0.0;
    AsObject(pfVar17)->Angle[2] = 45.0;
    { result = (float*)(pfVar8); return true; }
  }
  *(int*)&pfVar17[EffectBlendMeshIndex] = -3;
  AsObject(pfVar17)->Angle[0] = 0.0;
  AsObject(pfVar17)->Angle[1] = 0.0;
  AsObject(pfVar17)->Angle[2] = 45.0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 196.
static bool CreateEffect_Type196(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 10;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_CIRCLE_LIGHT.
static bool CreateEffect_CircleLight(float *& param_5, float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 40;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  if (param_5 != (float *)0x1) {
    { result = (float*)((float *)0x0); return true; }
  }
  AsObject(pfVar17)->RenderType = 0x80;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 202.
static bool CreateEffect_Type202(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_MAGIC2.
static bool CreateEffect_Magic2(float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  AsObject(pfVar17)->Direction[0] = 0.0;
  AsObject(pfVar17)->Direction[1] = -60.0;
  AsObject(pfVar17)->Direction[2] = 0.0;
  if (AsObject(pfVar17)->SubType != 2) {                      // SubType (DWORD)
    { result = (float*)((float*)(uintptr_t)AsObject(pfVar17)->SubType); return true; }
  }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_STORM.
static bool CreateEffect_Storm(float10& fVar24, float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 59;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  AsObject(pfVar17)->Direction[0] = 0.0;
  AsObject(pfVar17)->Direction[1] = -10.0;
  AsObject(pfVar17)->Direction[2] = 0.0;
  // Se pasa (xf, yf) explícito — effect pos en AsObject(pfVar17)->Position[0]/[5]
  fVar24 = (float10)RequestTerrainHeight(AsObject(pfVar17)->Position[0], AsObject(pfVar17)->Position[1]);
  AsObject(pfVar17)->Position[2] = (float)fVar24;
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_LASER.
static bool CreateEffect_Laser(float*& pfVar17, float*& pfVar8, float*& result)
{
  if ((*(float *)&AsObject(pfVar17)->SubType == 0.0) || (AsObject(pfVar17)->SubType == 3)) {
    *(int*)&pfVar17[EffectLifetimeIndex] = 1;
  }
  else {
    *(int*)&pfVar17[EffectLifetimeIndex] = 30;
    AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 150.0f;
    AsObject(pfVar17)->Direction[0] = 0.0;
    AsObject(pfVar17)->Direction[1] = -80.0;
    AsObject(pfVar17)->Direction[2] = 0.0;
    AsObject(pfVar17)->Light[0] = 1.0;
    AsObject(pfVar17)->Light[1] = 0.0;
    AsObject(pfVar17)->Light[2] = 0.0;
  }
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  AsObject(pfVar17)->BlendMeshLight = AsObject(pfVar17)->Light[0];
  AsObject(pfVar17)->Scale = 1.3;
  AsObject(pfVar17)->RenderType = 0x80;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_SAW.
static bool CreateEffect_Saw(float* const& local_3c, float& local_64, float& local_68, float& local_6c, float*& pfVar12, float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 10;
  AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 130.0f;
  Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
  local_6c = 0.0;
  local_68 = -60.0;
  local_64 = 0.0;
  pfVar12 = (float *)Vector_Rotate(&local_6c,local_3c + 3,AsObject(pfVar17)->Direction);
  { result = (float*)(pfVar12); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 212.
static bool CreateEffect_Type212(float* const& local_3c, float& local_58, float& local_5c, float& local_60, float& local_64, float& local_68, float& local_6c, float*& pfVar12, float*& pfVar17, float*& result)
{
  Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
  local_6c = 0.0;
  local_68 = -40.0;
  local_64 = 150.0;
  pfVar12 = (float *)Vector_Rotate(&local_6c,local_3c + 3,&local_60);
  AsObject(pfVar17)->Position[0] = local_60 + AsObject(pfVar17)->Position[0];
  AsObject(pfVar17)->Position[1] = local_5c + AsObject(pfVar17)->Position[1];
  AsObject(pfVar17)->Position[2] = local_58 + AsObject(pfVar17)->Position[2];
  AsObject(pfVar17)->Direction[1] = -40.0;
  AsObject(pfVar17)->Direction[2] = 10.0;    *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  AsObject(pfVar17)->Scale = 1.2;
  { result = (float*)(pfVar12); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 215.
static bool CreateEffect_Type215(float *& param_8, float* const& local_3c, float& local_58, float& local_5c, float& local_60, float& local_64, float& local_68, float& local_6c, float*& pfVar12, float*& pfVar17, intptr_t& uVar10, float*& result)
{
  uVar10 = _rand() % 16;
  AsObject(pfVar17)->LifeTime = (int)(uVar10 + 0x18);   // lifetime: DWORD, no float
  uVar10 = _rand() % 8;
  param_8 = (float *)(uVar10 + 6);
  AsObject(pfVar17)->Scale = (float)(intptr_t)param_8 * 0.1f;
  uVar10 = _rand();
  param_8 = (float*)(uVar10 % 4);
  AsObject(pfVar17)->Gravity = -(float)(intptr_t)param_8;
  uVar10 = _rand() % 64;
  param_8 = (float *)(uVar10 - 0x20);
  local_6c = (float)(int)(intptr_t)param_8;
  uVar10 = _rand() % 32;
  param_8 = (float *)(uVar10 + 0x32);
  local_68 = -(float)(int)(intptr_t)param_8;
  uVar10 = _rand() % 128;
  param_8 = (float *)(uVar10 + 200);
  local_64 = (float)(int)(intptr_t)param_8;
  Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
  pfVar12 = (float *)Vector_Rotate(&local_6c,local_3c + 3,&local_60);
  AsObject(pfVar17)->Position[0] = local_60 + AsObject(pfVar17)->Position[0];
  AsObject(pfVar17)->Position[1] = local_5c + AsObject(pfVar17)->Position[1];
  AsObject(pfVar17)->Position[2] = local_58 + AsObject(pfVar17)->Position[2];
  { result = (float*)(pfVar12); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 225.
static bool CreateEffect_Type225(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 10000;
  AsObject(pfVar17)->Velocity = 0.3;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 228, 229, 230.
static bool CreateEffect_Type228(float& fVar27, intptr_t& iVar9, float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 100;
  iVar9 = _rand();
  AsObject(pfVar17)->Scale = (float)(iVar9 % 10 + 4) * 0.1f;
  fVar27 = (-15.0f);
  AsObject(pfVar17)->Direction[0] = 0.0;
  AsObject(pfVar17)->Direction[1] = fVar27 / AsObject(pfVar17)->Scale;
  AsObject(pfVar17)->Direction[2] = (-30.0f) / AsObject(pfVar17)->Scale;
  *(float *)&AsObject(pfVar17)->SubType = 1.4013e-45;
  { result = (float*)((float *)(iVar9 / 10)); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_MAGIC_CIRCLE1.
static bool CreateEffect_MagicCircle1(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 30;
  AsObject(pfVar17)->Scale = 0.7;
  *(int*)&pfVar17[EffectBlendMeshIndex] = -2;
  if (AsObject(pfVar17)->SubType != 1) {
    AsObject(pfVar17)->Velocity = 0.1;
    { result = (float*)(pfVar8); return true; }
  }
  *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  AsObject(pfVar17)->Velocity = 0.1;
  *(float *)&AsObject(pfVar17)->HiddenMesh = 0.0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 237.
static bool CreateEffect_Type237(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 50;
  AsObject(pfVar17)->Scale = 0.8;
  AsObject(pfVar17)->Velocity = 1.0;
  AsObject(pfVar17)->Angle[0] = 0.0;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  AsObject(pfVar17)->Direction[1] = -50.0;
  AsObject(pfVar17)->Gravity = -10.0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 244.
static bool CreateEffect_Type244(intptr_t& iVar9, float*& pfVar17, float*& result)
{
  AsObject(pfVar17)->HeadAngle[0] = AsObject(pfVar17)->Angle[0];
  AsObject(pfVar17)->HeadAngle[1] = AsObject(pfVar17)->Angle[1];
  AsObject(pfVar17)->HeadAngle[2] = AsObject(pfVar17)->Angle[2];
  AsObject(pfVar17)->StartPosition[0] = AsObject(pfVar17)->Position[0];
  AsObject(pfVar17)->StartPosition[1] = AsObject(pfVar17)->Position[1];
  AsObject(pfVar17)->StartPosition[2] = AsObject(pfVar17)->Position[2];
  *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  iVar9 = _rand();
  // IDA: `*((_DWORD *)i + 1) = rand() % 100;` -- el SubType es un DWORD.
  // Guardarlo con una conversion a float dejaba los BITS del float en el
  // campo, y MoveEffect lo lee por bits: el angulo de las grietas del
  // suelo (IDA L2095) salia ~1.1e9 en vez de 0..99.
  *(DWORD *)&AsObject(pfVar17)->SubType = (DWORD)(iVar9 % 100);
  AsObject(pfVar17)->Angle[2] = AsObject(pfVar17)->Angle[2] + 330.0f;
  AsObject(pfVar17)->HeadAngle[0] = AsObject(pfVar17)->HeadAngle[0] + 80.0f;
  AsObject(pfVar17)->HeadAngle[2] = AsObject(pfVar17)->HeadAngle[2] + 180.0f;
  AsObject(pfVar17)->Gravity = 50.0;
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_EARTHQUAKE.
static bool CreateEffect_Earthquake(float *& param_7, float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 35;
  AsObject(pfVar17)->Scale = (float)(int)(short)(uintptr_t)param_7 * 0.01f;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 1248.
static bool CreateEffect_Type1248(float*& pfVar17, float*& pfVar8, float*& result)
{
  AsObject(pfVar17)->Angle[0] = 0.0;
  AsObject(pfVar17)->Angle[1] = 0.0;
  AsObject(pfVar17)->Angle[2] = 0.0;
  AsObject(pfVar17)->Position[2] = 100.0;
  *(int*)&pfVar17[EffectLifetimeIndex] = 75;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 1267.
static bool CreateEffect_Type1267(float *& param_2, float& fVar27, float* const& local_3c, float& local_58, float& local_5c, float& local_60, float& local_64, float& local_68, float& local_6c, float*& pfVar12, float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 10;
  if (*(float *)&AsObject(pfVar17)->SubType == 0.0) {
    local_6c = 0.0;
    local_68 = -100.0;
    local_64 = 0.0;
    Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
    pfVar12 = (float *)Vector_Rotate(&local_6c,local_3c + 3,&local_60);
    AsObject(pfVar17)->Position[0] = local_60 + AsObject(pfVar17)->Position[0];
    AsObject(pfVar17)->Position[1] = local_5c + AsObject(pfVar17)->Position[1];
    AsObject(pfVar17)->Position[2] = local_58 + AsObject(pfVar17)->Position[2] + 150.0f;
    { result = (float*)(pfVar12); return true; }
  }
  if (AsObject(pfVar17)->SubType != 1) {
    { result = (float*)((float *)0x0); return true; }
  }
  pfVar12 = (float*)(uintptr_t)(int)param_2[1];
  fVar27 = param_2[2];
  AsObject(pfVar17)->StartPosition[0] = *param_2;
  // 00460DC0 case 1267/subtype 1 copies Position[1] as a float.
  // It is not a pointer-as-float field.
  AsObject(pfVar17)->StartPosition[1] = param_2[1];
  AsObject(pfVar17)->StartPosition[2] = fVar27;
  *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  { result = (float*)(pfVar12); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 1271.
static bool CreateEffect_Type1271(float *& param_8, float& fVar27, intptr_t& iVar9, float*& pfVar12, float*& pfVar17, float*& result)
{
  iVar9 = _rand();
  AsObject(pfVar17)->LifeTime = (int)(iVar9 % 0xf + 0xf);   // lifetime: DWORD, no float
  AsObject(pfVar17)->Gravity = -20.0;
  iVar9 = _rand();
  param_8 = (float *)(iVar9 % 0x168);
  AsObject(pfVar17)->Velocity = (float)(intptr_t)param_8;
  AsObject(pfVar17)->Light[0] = 0.0;
  AsObject(pfVar17)->Light[1] = 0.0;
  AsObject(pfVar17)->Light[2] = 0.0;
  iVar9 = _rand();
  param_8 = (float *)(iVar9 % 200);
  AsObject(pfVar17)->Position[0] = ((float)(intptr_t)param_8 + AsObject(pfVar17)->Position[0]) - 100.0f;
  iVar9 = _rand();
  param_8 = (float *)(iVar9 % 200);
  AsObject(pfVar17)->Position[1] = ((float)(intptr_t)param_8 + AsObject(pfVar17)->Position[1]) - 100.0f;
  AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 500.0f;
  fVar27 = AsObject(pfVar17)->Position[0] + 100.0f;
  AsObject(pfVar17)->Position[0] = fVar27;
  AsObject(pfVar17)->StartPosition[0] = fVar27;
  AsObject(pfVar17)->StartPosition[1] = AsObject(pfVar17)->Position[1];
  AsObject(pfVar17)->StartPosition[2] = AsObject(pfVar17)->Position[2];
  pfVar12 = (float *)PlayBuffer(0x2e,0,0);
  { result = (float*)(pfVar12); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_EARTHQUAKE3, 250.
static bool CreateEffect_Earthquake3(float *& param_7, float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 35;
  AsObject(pfVar17)->Scale = (float)(int)(short)(uintptr_t)param_7 * 0.01f;
  { result = (float*)((float *)(int)(short)(uintptr_t)param_7); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_EARTHQUAKE4, MODEL_EARTHQUAKE7.
static bool CreateEffect_Earthquake4(int& param_1, float *& param_7, float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = param_1 == 0xf8 ? 35 : 40;
  AsObject(pfVar17)->Scale = (float)(int)(short)(uintptr_t)param_7 * 0.01f;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_EARTHQUAKE5, MODEL_EARTHQUAKE8.
static bool CreateEffect_Earthquake5(float *& param_7, float& fVar27, float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 40;
  fVar27 = (float)(int)(short)(uintptr_t)param_7;
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_TAIL.
static bool CreateEffect_Tail(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectBlendMeshIndex] = -2;
  *(int*)&pfVar17[EffectLifetimeIndex] = 6;
  AsObject(pfVar17)->Scale = 1.0;
  AsObject(pfVar17)->Gravity = 80.0;
  AsObject(pfVar17)->Light[0] = 0.5;
  AsObject(pfVar17)->Light[1] = 0.5;
  AsObject(pfVar17)->Light[2] = 0.5;
  AsObject(pfVar17)->Angle[0] = 0.0;
  AsObject(pfVar17)->Angle[1] = 0.0;
  AsObject(pfVar17)->Angle[2] = 45.0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_PIERCING.
static bool CreateEffect_Piercing(float *& param_5, byte& bVar6, float*& pfVar12, float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 100;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  AsObject(pfVar17)->BlendMeshLight = 1.0;
  AsObject(pfVar17)->Gravity = 0.0;
  if (param_5 == (float *)0x0) {
    AsObject(pfVar17)->Scale = 12.0;
    AsObject(pfVar17)->StartPosition[0] = *(float *)((intptr_t)AsObject(pfVar17)->Owner + 0x10);
    AsObject(pfVar17)->StartPosition[1] = *(float *)((intptr_t)AsObject(pfVar17)->Owner + 0x14);
    AsObject(pfVar17)->StartPosition[2] = *(float *)((intptr_t)AsObject(pfVar17)->Owner + 0x18);
  }
  else if (param_5 == (float *)0x1) {
    *(float *)&AsObject(pfVar17)->HiddenMesh = 0.0;
    AsObject(pfVar17)->Scale = 24.0;
  }
  pfVar12 = pfVar17 + 7;
  pfVar8 = pfVar17 + 4;
  bVar6 = (byte)(uintptr_t)param_5;
  Joint_Create(0x4e2,pfVar8,pfVar8,pfVar12,0,(intptr_t)pfVar17,AsObject(pfVar17)->Scale,0x1e,bVar6);
  Joint_Create(0x4e2,pfVar8,pfVar8,pfVar12,1,(intptr_t)pfVar17,AsObject(pfVar17)->Scale,0x1e,bVar6);
  Joint_Create(0x4e2,pfVar8,pfVar8,pfVar12,2,(intptr_t)pfVar17,AsObject(pfVar17)->Scale,0x1e,bVar6);
  Joint_Create(0x4e2,pfVar8,pfVar8,pfVar12,3,(intptr_t)pfVar17,AsObject(pfVar17)->Scale,0x1e,bVar6);
  pfVar12 = (float *)PlayBuffer(0x5c,0,0);
  AsObject(pfVar17)->Scale = 1.0;
  { result = (float*)(pfVar12); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_MAGIC_CAPSULE2.
static bool CreateEffect_MagicCapsule2(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  AsObject(pfVar17)->BlendMeshLight = 1.0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 265.
static bool CreateEffect_Type265(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 60;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_SPEARSKILL.
static bool CreateEffect_Spearskill(float10& fVar24, float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  AsObject(pfVar17)->Scale = 1.5;
  fVar24 = (float10)fsin((float10)AsObject(pfVar17)->Angle[2] * (float10)0.017453292f);
  AsObject(pfVar17)->Direction[0] = (float)(fVar24 * (float10)5.0f);
  fVar24 = (float10)fcos((float10)AsObject(pfVar17)->Angle[2] * (float10)0.017453292f);
  AsObject(pfVar17)->Direction[1] = (float)(fVar24 * (float10)(-5.0f));
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 268.
static bool CreateEffect_Type268(float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 1;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  AsObject(pfVar17)->BlendMeshLight = 1.0;
  AsObject(pfVar17)->Scale = 5.0;
  { result = (float*)(pfVar8); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 238.
static bool CreateEffect_Type238(float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 5;
  *(char *)((char*)CharacterMachine + 1408) = *(char *)((char*)CharacterMachine + 1408) + 1;   // serial de proyectiles
  { result = (float*)(pfVar17); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 239.
static bool CreateEffect_Type239(float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 25;
  *(char *)(pfVar17 + 0x22) = *(char *)((char*)CharacterMachine + 1408);   // serial de proyectiles
  { result = (float*)(pfVar17); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_SKILL_BLAST.
static bool CreateEffect_SkillBlast(float *& param_8, intptr_t& iVar9, float*& pfVar12, float*& pfVar17, intptr_t& uVar10, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 30;
  *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
  uVar10 = _rand() % 8;
  param_8 = (float *)(uVar10 + 10);
  pfVar12 = pfVar17 + 4;
  AsObject(pfVar17)->Scale = (float)(intptr_t)param_8 * 0.1f;
  iVar9 = _rand();
  param_8 = (float *)(iVar9 % 100 + 200);
  *pfVar12 = (float)(int)(intptr_t)param_8 + *pfVar12;
  iVar9 = _rand();
  param_8 = (float *)(iVar9 % 100 + -0x32);
  AsObject(pfVar17)->Position[1] = (float)(intptr_t)param_8 + AsObject(pfVar17)->Position[1];
  iVar9 = _rand();
  param_8 = (float *)(iVar9 % 500 + 300);
  AsObject(pfVar17)->Position[2] = (float)(intptr_t)param_8 + AsObject(pfVar17)->Position[2];
  AsObject(pfVar17)->Direction[0] = 0.0;
  AsObject(pfVar17)->Direction[1] = 0.0;
  iVar9 = _rand();
  param_8 = (float *)(iVar9 % 0x32);
  AsObject(pfVar17)->Direction[2] = (-50.0f) - (float)(intptr_t)param_8;
  AsObject(pfVar17)->Angle[0] = 0.0;
  AsObject(pfVar17)->Angle[1] = 20.0;
  AsObject(pfVar17)->Angle[2] = 0.0;
  pfVar17[0x10] = *pfVar12;
  pfVar17[0x11] = AsObject(pfVar17)->Position[1];
  pfVar17[0x12] = AsObject(pfVar17)->Position[2];
  Joint_Create(0x4ea,pfVar12,pfVar12,AsObject(pfVar17)->Angle,5,(intptr_t)pfVar17,100.0,-1,0);
  *(char *)(pfVar17 + 0x22) = *(char *)((char*)CharacterMachine + 1408);   // serial de proyectiles
  { result = (float*)(pfVar17); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case MODEL_ARROW_DOUBLE.
static bool CreateEffect_ArrowDouble(float*& pfVar12, float*& pfVar17, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 30;
  *(int*)&pfVar17[EffectBlendMeshIndex] = -2;
  AsObject(pfVar17)->Scale = 1.0;
  pfVar12 = pfVar17 + 4;
  AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 130.0f;
  AsObject(pfVar17)->Direction[0] = 0.0;
  AsObject(pfVar17)->Direction[1] = -70.0;
  AsObject(pfVar17)->Direction[2] = 0.0;
  pfVar17[0x10] = *pfVar12;
  pfVar17[0x11] = AsObject(pfVar17)->Position[1];
  pfVar17[0x12] = AsObject(pfVar17)->Position[2];
  Joint_Create(0x4ea,pfVar12,pfVar12,AsObject(pfVar17)->Angle,5,(intptr_t)pfVar17,100.0,-1,0);
  if (*(float *)&AsObject(pfVar17)->SubType != 0.0) {
    CreateEffect(MODEL_PIERCING,pfVar12,AsObject(pfVar17)->Angle,AsObject(pfVar17)->Light,(float *)0x0,pfVar17,(float *)0xffffffff,
                 (float *)0x0,0);
    *(float *)&AsObject(pfVar17)->AttackPoint = 0.0;
    AsObject(pfVar17)->Kind = 1;
  }
  *(char *)(pfVar17 + 0x22) = *(char *)((char*)CharacterMachine + 1408);   // serial de proyectiles
  { result = (float*)(pfVar17); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0), case 243.
static bool CreateEffect_Type243(float& fVar27, intptr_t& iVar9, float* const& local_3c, float& local_58, float& local_5c, float& local_60, float& local_64, float& local_68, float& local_6c, float*& pfVar12, float*& pfVar17, float*& pfVar8, float*& result)
{
  *(int*)&pfVar17[EffectLifetimeIndex] = 30;
  *(int*)&pfVar17[EffectBlendMeshIndex] = -2;
  AsObject(pfVar17)->Scale = 1.0;
  iVar9 = 0;
  AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 130.0f;
  AsObject(pfVar17)->Direction[0] = 0.0;
  AsObject(pfVar17)->Direction[1] = -30.0;
  AsObject(pfVar17)->Direction[2] = 0.0;
  Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
  local_6c = -10.0;
  local_68 = -100.0;
  local_64 = 15.0;
  Vector_Rotate(&local_6c,local_3c + 3,&local_60);
  pfVar8 = AsObject(pfVar17)->StartPosition;
  *pfVar8 = AsObject(pfVar17)->Position[0];
  AsObject(pfVar17)->StartPosition[1] = AsObject(pfVar17)->Position[1];
  AsObject(pfVar17)->StartPosition[2] = AsObject(pfVar17)->Position[2];
  fVar27 = *pfVar8;
  *pfVar12 = 0.0;
  pfVar12[1] = 0.0;
  *pfVar8 = local_60 + fVar27;
  AsObject(pfVar17)->StartPosition[1] = local_5c + AsObject(pfVar17)->StartPosition[1];
  AsObject(pfVar17)->StartPosition[2] = local_58 + AsObject(pfVar17)->StartPosition[2];
  pfVar12[2] = AsObject(pfVar17)->Angle[2];
  do {
    Joint_Create(0x4e1,pfVar8,pfVar8,pfVar12,0xb,(intptr_t)pfVar17,50.0,-1,iVar9 == 1);
    iVar9 = iVar9 + 1;
  } while (iVar9 < 4);
  *(char *)(pfVar17 + 0x22) = *(char *)((char*)CharacterMachine + 1408);   // serial de proyectiles
  { result = (float*)(pfVar17); return true; }
  return false;
}

// IDA: CreateEffect (0x00460DC0)
float * __cdecl
CreateEffect(int param_1,float *param_2,float *param_3,float *param_4,float *param_5,float *param_6,
            float *param_7,float *param_8,byte param_9)

{
  byte bVar1;
  float fVar2;
  float *pfVar3;
  float *pfVar4;
  char cVar5;
  byte bVar6;
  float *pfVar8;
  intptr_t iVar9;
  intptr_t uVar10;
  float *pfVar12;
  float *pfVar14;
  float *_param_9;
  float *pfVar17;
  float10 fVar24;
  float10 fVar25;
  float10 fVar26;
  float fVar27;
  short sVar28;
  // Estos "locales" sueltos son en realidad un bloque CONTIGUO del frame
  // original (ebp-0x6C .. ebp), y el código depende de esa contigüidad — MSVC
  // no la garantiza (mismo patrón que MoveJoint):
  //   Vector_Rotate(&local_6c, local_3c + 3, &local_60)
  //     → entrada  = {local_6c, local_68, local_64}
  //     → salida   = {local_60, local_5c, local_58}
  //   Vector_Rotate(&local_6c, local_3c + 3, &local_48)
  //     → salida   = {local_48, local_44, local_40}
  // Además `Matrix_BuildFromEuler(ang, local_3c + 3)` escribe una matriz 3x4 (12 floats)
  // en local_3c[3..14], o sea un float FUERA del `float local_3c[14]` original.
  // Mapeo por offset de frame: -0x6C=[0] … -0x3C=[12]; a local_3c se le dan 16
  // slots para que la matriz entre completa.
  float __frame[28] = {0};
  float &local_6c = __frame[0];
  float &local_68 = __frame[1];
  float &local_64 = __frame[2];
  float &local_60 = __frame[3];
  float &local_5c = __frame[4];
  float &local_58 = __frame[5];
  float &local_54 = __frame[6];
  float &local_50 = __frame[7];
  float &local_4c = __frame[8];
  float &local_48 = __frame[9];
  float &local_44 = __frame[10];
  float &local_40 = __frame[11];
  float *const local_3c = &__frame[12];
  float *result = nullptr;   // valor de retorno de los handlers

  pfVar12 = param_3;
LAB_00460dd8:
  pfVar8 = (float *)0x0;
  pfVar17 = (float *)Effects;
  // El original corta con `if (0x7b2714f < (int)pfVar17)`, una dirección absoluta
  // del binario. El pool Effects es de kEffectPoolSize slots: se recorre por
  // cantidad de iteraciones.
  {
    bool found_slot = false;
    for (int i = 0; i < kEffectPoolSize; ++i, pfVar17 = pfVar17 + OBJECT_STRIDE / 4) {
      if (*(char *)pfVar17 == '\0') { found_slot = true; break; }
    }
    if (!found_slot) return (float *)0x0;
  }
  RenderFxSnapshots_New(pfVar17);
  RenderDerivedState_Forget(pfVar17);
  Particle_InvalidateRender(pfVar17);
  *(undefined1 *)pfVar17 = 1;
  AsObject(pfVar17)->Type = (short)param_1;
  // 00460DC0 writes SubType and Owner as raw DWORD fields.  The reconstructed
  // prototype still carries them as float pointers, so numeric casts corrupt
  // every non-zero subtype/owner (notably all skill effects) instead of
  // preserving the original bit pattern.
  *(DWORD *)&AsObject(pfVar17)->SubType = (intptr_t)param_5;
  AsObject(pfVar17)->LightEnable = 1;
  AsObject(pfVar17)->HiddenMesh = -1;
  *(int*)&pfVar17[EffectBlendMeshIndex] = -1;
  AsObject(pfVar17)->BlendMeshLight = 1.0;
  AsObject(pfVar17)->BlendMeshTexCoordU = 0.0;
  AsObject(pfVar17)->BlendMeshTexCoordV = 0.0;
  AsObject(pfVar17)->AnimationFrame = 0.0;
  AsObject(pfVar17)->AlphaEnable = 0;
  AsObject(pfVar17)->Alpha = 1.0;
  AsObject(pfVar17)->PriorAnimationFrame = 0.0;
  AsObject(pfVar17)->Scale = 0.9;
  AsObject(pfVar17)->Owner = (OBJECT*)(intptr_t)param_6;
  AsObject(pfVar17)->Velocity = 0.3;
  AsObject(pfVar17)->PKKey = (short)(uintptr_t)param_7;
  AsObject(pfVar17)->Kind = param_9;
  fVar27 = *param_4;
  // CreateEffect's eighth argument is SkillIndex (an integer), not a pointer.
  // IDA writes its low byte directly to effect+0x85.
  AsObject(pfVar17)->Skill = (byte)(uintptr_t)param_8;
  _param_9 = pfVar17 + 0x3a;
  AsObject(pfVar17)->RenderType = 0;
  *(float *)&AsObject(pfVar17)->AttackPoint = 0.0;
  *_param_9 = fVar27;
  fVar27 = param_4[2];
  AsObject(pfVar17)->Light[1] = param_4[1];
  AsObject(pfVar17)->Light[2] = fVar27;
  pfVar14 = pfVar17 + 4;
  AsObject(pfVar17)->Angle[0] = *pfVar12;
  AsObject(pfVar17)->Angle[1] = pfVar12[1];
  AsObject(pfVar17)->Angle[2] = pfVar12[2];
  *pfVar14 = *param_2;
  AsObject(pfVar17)->Position[1] = param_2[1];
  AsObject(pfVar17)->Position[2] = param_2[2];
  AsObject(pfVar17)->Direction[0] = 0.0;
  AsObject(pfVar17)->Direction[1] = 0.0;
  AsObject(pfVar17)->Direction[2] = 0.0;
  if (param_1 == 0xc1) {                     // Tamachan (0.98j case 183)
    Tamachan_Create((char *)pfVar17);
    return pfVar8;
  }
  pfVar4 = (float*)CharacterMachine;
  pfVar3 = (float*)Hero;
  fVar27 = 0.0f;
  if (0xf6 < param_1) {
    if (param_1 < 0x1f1) {
      if (param_1 == 0x1f0) goto LAB_00464ed8;
      switch(param_1) {
      case MODEL_EARTHQUAKE3: case 0xfa:
        if (CreateEffect_Earthquake3(param_7, pfVar17, result)) return result;
        break;
      case MODEL_EARTHQUAKE4: case MODEL_EARTHQUAKE7:
        if (CreateEffect_Earthquake4(param_1, param_7, pfVar17, pfVar8, result)) return result;
        break;
      case MODEL_EARTHQUAKE5: case MODEL_EARTHQUAKE8:
        if (CreateEffect_Earthquake5(param_7, fVar27, pfVar17, result)) return result;
        break;
      case 0xfd:
        *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
        AsObject(pfVar17)->BlendMeshLight = 1.5;
        AsObject(pfVar17)->Scale = 0.5;
        *(int*)&pfVar17[EffectLifetimeIndex] = 15;
        AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] - 15.0f;
        goto LAB_004658be;
      case MODEL_TAIL:
        if (CreateEffect_Tail(pfVar17, pfVar8, result)) return result;
        break;
      case MODEL_PIERCING:
        if (CreateEffect_Piercing(param_5, bVar6, pfVar12, pfVar17, pfVar8, result)) return result;
        break;
      case 0x100:
        goto switchD_00460f25_caseD_100;
      default:
        return (float *)0x0;
      case MODEL_MAGIC_CAPSULE2:
        if (CreateEffect_MagicCapsule2(pfVar17, pfVar8, result)) return result;
        break;
      case 0x104:
      case 0x105:
      case 0x106:
      case 0x107: {
        const auto& debris = EffectDebrisPresets[param_1 < 0x106 ? 0 : 1];
        local_6c = 0.0;
        uVar10 = _rand() % 128;
        param_8 = (float *)(uVar10 + debris.SpeedBase);
        local_64 = 0.0;
        local_68 = (float)(int)(intptr_t)param_8 * 0.1f;
        AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 50.0f;
        uVar10 = _rand() % 16;
        AsObject(pfVar17)->LifeTime = (int)(uVar10 + 0x20);   // lifetime: DWORD, no float
        uVar10 = _rand() % 4;
        param_8 = (float *)(uVar10 + 8);
        AsObject(pfVar17)->Scale = (float)(intptr_t)param_8 * 0.1f;
        iVar9 = _rand();
        param_8 = (float *)(iVar9 % 0x168);
        AsObject(pfVar17)->Angle[2] = (float)(intptr_t)param_8;
        Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
        Vector_Rotate(&local_6c,local_3c + 3,AsObject(pfVar17)->Direction);
        iVar9 = _rand();
        param_8 = (float *)(iVar9 % 5 + 2);
        AsObject(pfVar17)->Gravity = (float)(intptr_t)param_8;
        if ((AsObject(pfVar17)->Type == debris.BoostedType) && (*(float *)&AsObject(pfVar17)->SubType == 0.0)) {
          *(float *)&AsObject(pfVar17)->SubType = 1.4013e-45;
          iVar9 = _rand();
          param_8 = (float *)(iVar9 % 5);
          AsObject(pfVar17)->Gravity = (float)(intptr_t)param_8 + AsObject(pfVar17)->Gravity;
        }
        iVar9 = _rand();
        param_8 = (float *)(iVar9 % 0x168);
        AsObject(pfVar17)->Angle[0] = (float)(intptr_t)param_8;
        iVar9 = _rand();
        param_8 = (float *)(iVar9 % 0x168);
        AsObject(pfVar17)->Angle[1] = (float)(intptr_t)param_8;
        iVar9 = _rand();
        AsObject(pfVar17)->Angle[2] = (float)(iVar9 % 0x168);
        return (float *)(iVar9 / 0x168);
      }
      case 0x109:
        if (CreateEffect_Type265(pfVar17, pfVar8, result)) return result;
        break;
      case MODEL_SPEARSKILL:
        if (CreateEffect_Spearskill(fVar24, pfVar17, pfVar8, result)) return result;
        break;
      case 0x10c:
        if (CreateEffect_Type268(pfVar17, pfVar8, result)) return result;
        break;
      }
LAB_004649e0:
      AsObject(pfVar17)->Scale = fVar27 * 0.01f;
      *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
      if (param_5 != (float *)0x1) {
        return param_5;
      }
      AsObject(pfVar17)->RenderType = 0x80;
      return (float *)0x1;
    }
    if (0x4b0 < param_1) {
      switch(param_1) {
      case 0x4ba:
      case 0x4bb:
      case 0x4bc:
        goto switchD_00465549_caseD_4ba;
      default:
        return (float *)0x0;
      case BITMAP_FIRECRACKER:
        AsObject(pfVar17)->Light[0] = 1.0;
        AsObject(pfVar17)->Light[1] = 1.0;
        AsObject(pfVar17)->Light[2] = 1.0;
        iVar9 = _rand();
        param_8 = (float *)(iVar9 % 9 + -4);
        AsObject(pfVar17)->Direction[0] = (float)(intptr_t)param_8;
        iVar9 = _rand();
        param_8 = (float *)(iVar9 % 9 + -4);
        AsObject(pfVar17)->Direction[1] = (float)(intptr_t)param_8;
        AsObject(pfVar17)->Direction[2] = 26.0;
        *(int*)&pfVar17[EffectLifetimeIndex] = 12;
        pfVar12 = (float *)PlayBuffer(0x44, AsObject(pfVar17),0);
        return pfVar12;
      case 0x4e0:
        if (CreateEffect_Type1248(pfVar17, pfVar8, result)) return result;
        break;
      case BITMAP_MAGIC + 1:
      case BITMAP_MAGIC + 2:
        *(int*)&pfVar17[EffectLifetimeIndex] = 20;
        // IDA `if (*((_DWORD *)i + 1) == 4)` — SubType es un DWORD. Leerlo como float
        // y convertir (`(int)*(float *)&AsObject(pfVar17)->SubType`) daría 0 para cualquier SubType chico.
        if (AsObject(pfVar17)->SubType != 4) {
          return (float*)(uintptr_t)AsObject(pfVar17)->SubType;
        }
        *(int*)&pfVar17[EffectLifetimeIndex] = 60;
        iVar9 = _rand();
        AsObject(pfVar17)->Scale = (float)(iVar9 % 0x32 + 0x32) * 0.04f;
        return (float *)(iVar9 / 0x32);
      case 0x4f3:
        if (CreateEffect_Type1267(param_2, fVar27, local_3c, local_58, local_5c, local_60, local_64, local_68, local_6c, pfVar12, pfVar17, result)) return result;
        break;
      case 0x4f7:
        if (CreateEffect_Type1271(param_8, fVar27, iVar9, pfVar12, pfVar17, result)) return result;
        break;
      case BITMAP_SKULL:
        *(int*)&pfVar17[EffectLifetimeIndex] = 1000;
        pfVar12 = (float*)(uintptr_t)AsObject(pfVar17)->SubType;   // SubType (DWORD)
        if (pfVar12 == (float *)0x1) {
          *(int*)&pfVar17[EffectLifetimeIndex] = 940;
          return pfVar12;
        }
        if (pfVar12 != (float *)0x2) {
          return pfVar12;
        }
        *(int*)&pfVar17[EffectLifetimeIndex] = 8;
        AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 150.0f;
        return pfVar12;
      case BITMAP_ADV_SMOKE + 1:
        *(int*)&pfVar17[EffectLifetimeIndex] = 10;
        AsObject(pfVar17)->Velocity = 1.0;
        pfVar12 = pfVar17 + 7;
        AsObject(pfVar17)->Direction[0] = 0.0;
        AsObject(pfVar17)->Direction[1] = -30.0;
        AsObject(pfVar17)->Direction[2] = 0.0;
        Matrix_BuildFromEuler(pfVar12,local_3c + 3);
        local_6c = 0.0;
        local_68 = -20.0;
        local_64 = 50.0;
        Vector_Rotate(&local_6c,local_3c + 3,&local_60);
        AsObject(pfVar17)->Position[0] = local_60 + AsObject(pfVar17)->Position[0];
        local_6c = -20.0;
        local_68 = -20.0;
        local_64 = 60.0;
        AsObject(pfVar17)->Position[1] = local_5c + AsObject(pfVar17)->Position[1];
        AsObject(pfVar17)->Position[2] = local_58 + AsObject(pfVar17)->Position[2];
        Vector_Rotate(&local_6c,local_3c + 3,&local_60);
        local_48 = local_60 + *param_2;
        local_54 = *pfVar12;
        local_50 = AsObject(pfVar17)->Angle[1];
        local_4c = AsObject(pfVar17)->Angle[2];
        local_44 = local_5c + param_2[1];
        param_8 = (float *)0x0;
        local_40 = local_58 + param_2[2];
        do {
          pfVar8 = param_8;
          iVar9 = _rand();
          _param_9 = (float *)(iVar9 % 0x14);
          local_54 = (float)(int)(intptr_t)_param_9 + *pfVar12 + 5.0f;
          local_50 = (float)(int)(intptr_t)param_8 + local_50;
          Joint_Create(0x4e9,&local_48,&local_48,&local_54,1,0,10.0,-1,0);
          param_8 = (float *)((intptr_t)pfVar8 + 0x12);
        } while ((intptr_t)param_8 < 0x168);
        local_3c[0] = 30.0;
        local_3c[1] = -20.0;
        local_3c[2] = 60.0;
        Vector_Rotate(local_3c,local_3c + 3,&local_60);
        local_48 = local_60 + *param_2;
        local_54 = *pfVar12;
        local_50 = AsObject(pfVar17)->Angle[1];
        local_4c = AsObject(pfVar17)->Angle[2];
        local_44 = local_5c + param_2[1];
        local_40 = local_58 + param_2[2];
        param_8 = (float *)0x0;
        do {
          pfVar8 = param_8;
          iVar9 = _rand();
          _param_9 = (float *)(iVar9 % 0x14);
          local_54 = (float)(int)(intptr_t)_param_9 + *pfVar12 + 5.0f;
          local_50 = (float)(int)(intptr_t)param_8 + local_50;
          pfVar17 = (float *)Joint_Create(0x4e9,&local_48,&local_48,&local_54,1,0,10.0,-1,0);
          param_8 = (float *)((intptr_t)pfVar8 + 0x12);
        } while ((intptr_t)param_8 < 0x168);
        return pfVar17;
      case 0x596:
        *(int*)&pfVar17[EffectLifetimeIndex] = 30;
        AsObject(pfVar17)->Scale = 1.0;
LAB_004658be:
        AsObject(pfVar17)->Light[0] = 1.0;
        AsObject(pfVar17)->Light[1] = 1.0;
        AsObject(pfVar17)->Light[2] = 1.0;
        return (float *)0x3f800000;
      }
    }
    if (param_1 == 0x4b0) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 40;
      *(char *)(pfVar17 + 0x22) = *(char *)((char*)CharacterMachine + 1408);   // serial de proyectiles
      return pfVar17;
    }
    if (0x498 < param_1) {
      switch(param_1) {
      case BITMAP_ENERGY:
        *(int*)&pfVar17[EffectLifetimeIndex] = 20;
        AsObject(pfVar17)->Direction[0] = 0.0;
        AsObject(pfVar17)->Direction[1] = -60.0;
        AsObject(pfVar17)->Direction[2] = 0.0;
        AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 100.0f;
        // IDA 0x460DC0 L2881-2885: `if (SubType == 1) Scale = 1.5; break;`.
        // El port devolvia NULL para cualquier otro subtipo; Energy Ball lo crea
        // con subtipo 0 (MoveCharacter, skill 17).
        if (AsObject(pfVar17)->SubType == 1)
          AsObject(pfVar17)->Scale = 1.5;
        return pfVar8;
      default:
        return (float *)0x0;
      case BITMAP_LIGHTNING + 1:        *(int*)&pfVar17[EffectLifetimeIndex] = 10;
        AsObject(pfVar17)->Scale = 1.5;
        return pfVar8;
      case BITMAP_FIRE:
        goto switchD_00464efe_caseD_4ab;
      case BITMAP_FIRE + 1:
        *(int*)&pfVar17[EffectLifetimeIndex] = 10;
        Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
        local_6c = 0.0;
        local_68 = -60.0;
        local_64 = 0.0;
        pfVar12 = (float *)Vector_Rotate(&local_6c,local_3c + 3,&local_60);
        AsObject(pfVar17)->Position[0] = local_60 + AsObject(pfVar17)->Position[0];
        AsObject(pfVar17)->Position[1] = local_5c + AsObject(pfVar17)->Position[1];
        AsObject(pfVar17)->Position[2] = local_58 + AsObject(pfVar17)->Position[2] + 130.0f;
        return pfVar12;
      }
    }
    if (param_1 == 0x498) {
LAB_00464ed8:
      *(int*)&pfVar17[EffectLifetimeIndex] = 10;
      return pfVar8;
    }
    if (param_1 < 0x47f) {
      if (param_1 == 0x47e) {
        if (*(float *)&AsObject(pfVar17)->SubType == 0.0) {
          AsObject(pfVar17)->Light[0] = 0.3;
          AsObject(pfVar17)->Light[1] = 0.3;
          AsObject(pfVar17)->Light[2] = 0.3;
          iVar9 = _rand();
          fVar24 = (float10)(iVar9 % 5 + 9) * (float10)0.5f;
          fVar25 = (float10)fcos((float10)2.0f);
          fVar26 = (float10)fsin((float10)2.0f);
          AsObject(pfVar17)->Direction[0] = (float)(fVar26 * fVar25 * fVar24);
          fVar26 = (float10)fcos((float10)2.0f);
          AsObject(pfVar17)->Direction[1] = (float)(fVar26 * fVar25 * fVar24);
          fVar25 = (float10)fsin((float10)2.0f);
          AsObject(pfVar17)->Direction[2] = (float)(fVar25 * fVar24);
          AsObject(pfVar17)->Scale = 3.0;
          *(int*)&pfVar17[EffectLifetimeIndex] = 400;
          return (float *)(iVar9 / 5);
        }
        if (AsObject(pfVar17)->SubType != 1) {
          return (float *)0x0;
        }
switchD_00464efe_caseD_4ab:
        *(int*)&pfVar17[EffectLifetimeIndex] = 1000;
        return pfVar8;
      }
      if (param_1 == 0x1f1) {
        AsObject(pfVar17)->Angle[2] = 0.0;
        AsObject(pfVar17)->Angle[1] = 0.0;
        AsObject(pfVar17)->Angle[0] = 0.0;
        *(int*)&pfVar17[EffectLifetimeIndex] = 5;
        return pfVar8;
      }
      if (param_1 != 0x238) {
        return (float *)0x0;
      }
      *(int*)&pfVar17[EffectLifetimeIndex] = 30;
      *(int*)&pfVar17[EffectBlendMeshIndex] = -2;
      AsObject(pfVar17)->Scale = 1.0;
      AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 280.0f;
      AsObject(pfVar17)->Angle[0] = AsObject(pfVar17)->Angle[0] + 20.0f;
      AsObject(pfVar17)->Direction[0] = 0.0;
      AsObject(pfVar17)->Direction[1] = -80.0;
      AsObject(pfVar17)->Direction[2] = -10.0;
      return pfVar8;
    }
    if (param_1 != 0x490) {
      return (float *)0x0;
    }
    fVar27 = pfVar12[2] + 45.0f;
    *(int*)&pfVar17[EffectLifetimeIndex] = 30;
    AsObject(pfVar17)->Light[0] = 0.8;
    AsObject(pfVar17)->Light[1] = 0.8;
    AsObject(pfVar17)->Light[2] = 0.8;
    AsObject(pfVar17)->HeadAngle[0] = 0.0;
    AsObject(pfVar17)->HeadAngle[1] = 0.0;
    AsObject(pfVar17)->HeadAngle[2] = fVar27;
    goto LAB_00463e3d;
  }
  if (param_1 == 0xf6) {
    *(int*)&pfVar17[EffectLifetimeIndex] = 20;
    fVar27 = (float)(int)(short)(uintptr_t)param_7;
    goto LAB_004649e0;
  }
  switch(param_1) {
  case 0xb6:
    if (CreateEffect_Type182(pfVar17, pfVar8, result)) return result;
    break;
  default:
    return pfVar8;   // IDA: default -> return
  case 0xb9:
    if (CreateEffect_Type185(pfVar17, pfVar8, result)) return result;
    break;
  case 0xbe:
    fVar27 = *(float *)&AsObject(pfVar17)->SubType;
    if (fVar27 == 0.0) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 50;
      AsObject(pfVar17)->Scale = 0.8;
      AsObject(pfVar17)->Velocity = 1.0;
      AsObject(pfVar17)->Angle[0] = 0.0;
      *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
      return pfVar8;
    }
    // SubType viaja en los BITS del float (IDA: *((_DWORD *)i + 1)).  Con
    // (int)fVar27 un 1 denormal daba 0 y los subtipos 1/2 del Ice Arrow
    // salian por aca con el slot ya activo y sin inicializar.
    if (*(int *)&fVar27 < 1) {
      return (float *)0x0;
    }
    if (2 < *(int *)&fVar27) {
      return (float *)0x0;
    }
    *(int*)&pfVar17[EffectLifetimeIndex] = 20;
    AsObject(pfVar17)->Scale = 0.8;
    AsObject(pfVar17)->Angle[0] = -20.0;
    *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
    AsObject(pfVar17)->BlendMeshLight = 0.5;
    AsObject(pfVar17)->Gravity = 5.0;
    AsObject(pfVar17)->HeadAngle[0] = 0.0;
    AsObject(pfVar17)->HeadAngle[1] = 0.0;
    iVar9 = _rand();
    param_8 = (float *)(iVar9 % 0x168);
    iVar9 = 3;
    AsObject(pfVar17)->HeadAngle[2] = (float)(intptr_t)param_8;
    do {
      uVar10 = _rand() % 64;
      param_8 = (float *)(uVar10 - 0x20);
      local_48 = (float)(int)(intptr_t)param_8 + AsObject(pfVar17)->Position[0];
      uVar10 = _rand() % 64;
      param_8 = (float *)(uVar10 - 0x20);
      local_44 = (float)(int)(intptr_t)param_8 + AsObject(pfVar17)->Position[1];
      uVar10 = _rand() % 128;
      param_8 = (float *)(uVar10 + 0x20);
      local_40 = (float)(int)(intptr_t)param_8 + AsObject(pfVar17)->Position[2];
      Particle_Spawn(BITMAP_SMOKE,&local_48,AsObject(pfVar17)->Angle,AsObject(pfVar17)->Light,0,1.0,0);
      iVar9 = iVar9 + -1;
    } while (iVar9 != 0);
    fVar27 = AsObject(pfVar17)->Position[0];
    *param_4 = 0.4;
    param_4[1] = 0.3;
    param_4[2] = 0.2;
    AddTerrainLight(fVar27,AsObject(pfVar17)->Position[1], (float*)param_4,2, (float*)&DAT_081cb608[0]);
LAB_00463e3d:
    AsObject(pfVar17)->StartPosition[0] = AsObject(pfVar17)->Position[0];
    AsObject(pfVar17)->StartPosition[1] = AsObject(pfVar17)->Position[1];
    AsObject(pfVar17)->StartPosition[2] = AsObject(pfVar17)->Position[2];
    return (float*)(uintptr_t)(int)AsObject(pfVar17)->Position[1];
  case MODEL_FIRE:
    if (CreateEffect_Fire(param_8, fVar27, iVar9, pfVar17, pfVar8, uVar10, result)) return result;
    break;
  case MODEL_POISON:
    if (CreateEffect_Poison(pfVar17, pfVar8, result)) return result;
    break;
  case 0xc2:
    if (CreateEffect_Type194(param_5, pfVar17, pfVar8, result)) return result;
    break;
  case 0xc4:
    if (CreateEffect_Type196(pfVar17, pfVar8, result)) return result;
    break;
  case 0xc5:
  case 0xc6:
  case 0xd5:
  case 0xd6:
    if (AsObject(pfVar17)->SubType == 5) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 60;
      uVar10 = _rand() % 4;
      AsObject(pfVar17)->Scale = (float)(int)(uVar10 + 8) * 0.1f;
      return (float *)(uVar10 + 8);
    }
  case 199:
  case 0xe7:
  case 0xe8:
    goto switchD_00461001_caseD_c7;
  case 200:
    *(int*)&pfVar17[EffectLifetimeIndex] = 45;
    *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
    if ((float*)(uintptr_t)(intptr_t)AsObject(pfVar17)->Owner != pfVar3) {
      return (float*)(uintptr_t)(intptr_t)AsObject(pfVar17)->Owner;
    }
    pfVar12 = (float*)CharacterMachine;
    cVar5 = *(char *)((char*)CharacterMachine + 1408);
    *(char *)(pfVar17 + 0x22) = cVar5;
    *((char *)pfVar12 + 1408) = cVar5 + '\x01';
    sVar28 = AsObject(pfVar17)->PKKey;
    bVar6 = *(byte *)(pfVar17 + 0x22);
    fVar27 = 300.0;
    goto LAB_00463ca2;
  case MODEL_CIRCLE_LIGHT:
    if (CreateEffect_CircleLight(param_5, pfVar17, pfVar8, result)) return result;
    break;
  case 0xca:
    if (CreateEffect_Type202(pfVar17, pfVar8, result)) return result;
    break;
  case MODEL_MAGIC2:
    if (CreateEffect_Magic2(pfVar17, result)) return result;
    break;
  case MODEL_STORM:
    if (CreateEffect_Storm(fVar24, pfVar17, result)) return result;
    break;
  case MODEL_LASER:
    if (CreateEffect_Laser(pfVar17, pfVar8, result)) return result;
    break;
  case MODEL_SAW:
    if (CreateEffect_Saw(local_3c, local_64, local_68, local_6c, pfVar12, pfVar17, result)) return result;
    break;
  case 0xd2:
    AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 50.0f;
  case 0xd3:
    AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 100.0f;
switchD_00461001_caseD_e2:
    if (AsObject(pfVar17)->SubType == 5) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 60;
      uVar10 = _rand() % 4;
      AsObject(pfVar17)->Scale = (float)(int)(uVar10 + 8) * 0.1f;
      AsObject(pfVar17)->StartPosition[0] = AsObject(pfVar17)->Position[0];
      AsObject(pfVar17)->StartPosition[1] = AsObject(pfVar17)->Position[1];
      AsObject(pfVar17)->StartPosition[2] = AsObject(pfVar17)->Position[2];
      AsObject(pfVar17)->Position[0] = *(float *)((intptr_t)AsObject(pfVar17)->Owner + 0x10);
      AsObject(pfVar17)->Position[1] = *(float *)((intptr_t)AsObject(pfVar17)->Owner + 0x14);
      AsObject(pfVar17)->Position[2] = ((float*)(uintptr_t)(intptr_t)AsObject(pfVar17)->Owner)[6];
      return (float*)(uintptr_t)(intptr_t)AsObject(pfVar17)->Owner;
    }
switchD_00461001_caseD_c7:
    if ((AsObject(pfVar17)->Type == 0xe2) || (AsObject(pfVar17)->Type == 0xe3)) {
      uVar10 = _rand() % 128;
      param_8 = (float *)(uVar10 - 0x40);
      local_6c = (float)(int)(intptr_t)param_8;
      uVar10 = _rand() % 128;
      param_8 = (float *)(uVar10 - 0x40);
      local_68 = (float)(int)(intptr_t)param_8;
      iVar9 = _rand();
      param_8 = (float *)(iVar9 % 0xb4);
      local_64 = (float)(int)(intptr_t)param_8;
      AsObject(pfVar17)->Position[0] = local_6c + AsObject(pfVar17)->Position[0];
      AsObject(pfVar17)->Position[1] = local_68 + AsObject(pfVar17)->Position[1];
      AsObject(pfVar17)->Position[2] = local_64 + AsObject(pfVar17)->Position[2];
    }
    if (param_1 == 199) {
      *(int*)&pfVar17[EffectBlendMeshIndex] = 0;
      AsObject(pfVar17)->BlendMeshLight = 0.3;
      local_6c = 0.0;
      uVar10 = _rand() % 256;
      param_8 = (float *)(uVar10 + 0x40);
      local_68 = (float)(int)(intptr_t)param_8 * 0.1f;
      AsObject(pfVar17)->Position[2] = AsObject(pfVar17)->Position[2] + 50.0f;
    }
    else {
      local_6c = 0.0;
      if (AsObject(pfVar17)->SubType == 1) {
        local_68 = 0.0;
      }
      else {
        uVar10 = _rand() % 256;
        param_8 = (float *)(uVar10 + 0x40);
        local_68 = (float)(int)(intptr_t)param_8 * 0.1f;
      }
    }
    local_64 = 0.0;
    uVar10 = _rand() % 16;
    AsObject(pfVar17)->LifeTime = (int)(uVar10 + 0x20);   // lifetime: DWORD, no float
    uVar10 = _rand() % 4;
    param_8 = (float *)(uVar10 + 8);
    AsObject(pfVar17)->Scale = (float)(intptr_t)param_8 * 0.1f;
    iVar9 = _rand();
    param_8 = (float *)(iVar9 % 0x168);
    AsObject(pfVar17)->Angle[2] = (float)(intptr_t)param_8;
    Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
    Vector_Rotate(&local_6c,local_3c + 3,AsObject(pfVar17)->Direction);
    uVar10 = _rand() % 16;
    AsObject(pfVar17)->Gravity = (float)(int)(uVar10 + 8);
    return (float *)(uVar10 + 8);
  case 0xd4:
    if (CreateEffect_Type212(local_3c, local_58, local_5c, local_60, local_64, local_68, local_6c, pfVar12, pfVar17, result)) return result;
    break;
  case 0xd7:
    if (CreateEffect_Type215(param_8, local_3c, local_58, local_5c, local_60, local_64, local_68, local_6c, pfVar12, pfVar17, uVar10, result)) return result;
    break;
  case 0xd8:
  case 0xd9:
  case 0xda:
  case 0xdb:
  case 0xdc:
  case 0xdd:
  case 0xde:
  case 0xdf:
  case 0xe0: {
    const auto& arrow = EffectArrowPresets[param_1 - 0xd8];
    if (arrow.SetBlendMesh)
      *(int*)&pfVar17[EffectBlendMeshIndex] = arrow.BlendMesh;
    pfVar12 = pfVar17 + 7;
    AsObject(pfVar17)->Velocity = 1.0;
    *(int*)&pfVar17[EffectLifetimeIndex] = 30;
    Matrix_BuildFromEuler(pfVar12,local_3c + 3);
    local_6c = -10.0;
    local_68 = -60.0;
    local_64 = 135.0;
    Vector_Rotate(&local_6c,local_3c + 3,&local_60);
    AsObject(pfVar17)->Position[0] = local_60 + AsObject(pfVar17)->Position[0];
    AsObject(pfVar17)->Position[1] = local_5c + AsObject(pfVar17)->Position[1];
    AsObject(pfVar17)->Position[2] = local_58 + AsObject(pfVar17)->Position[2];
    AsObject(pfVar17)->Scale = arrow.Scale;
    AsObject(pfVar17)->Direction[1] = arrow.ForwardSpeed;
    if (arrow.HasGravity) AsObject(pfVar17)->Gravity = -10.0f;
    if (arrow.Lifetime != 30) *(int*)&pfVar17[EffectLifetimeIndex] = arrow.Lifetime;
    // IDA CreateEffect L685-693 (cases 216-224):
    //     if ( *((_DWORD *)i + 1) == 2 )        // SubType == 2 (flecha de Penetration)
    //     {
    //         if ( Type != 223 ) CreateEffect(255, ...);
    //         *((_DWORD *)i + 61) = 0;
     //        i[132] = 1;
    //     }
    // Este es el bloque que hace que la flecha de Penetration entre por el path A
    // de sub_466440 (que exige `Target + 132 != 0`) y por lo tanto reporte sus
    // blancos al server con el 0x1D.  El original compara el campo como DWORD;
    // el port lo hacia contra el denormal 2.8026e-45, o sea leyendo el slot como
    // float.  Se lee por BITS, igual que ya se hace en la linea 386 de este mismo
    // archivo (`AsObject(pfVar17)->SubType != 4`).
    if (AsObject(pfVar17)->SubType == 2) {
      if (param_1 != 0xdf) {
        CreateEffect(MODEL_PIERCING,AsObject(pfVar17)->Position,pfVar12,AsObject(pfVar17)->Light,(float *)0x0,pfVar17,
                     (float *)0xffffffff,(float *)0x0,0);
      }
      *(float *)&AsObject(pfVar17)->AttackPoint = 0.0;
      AsObject(pfVar17)->Kind = 1;
    }
    pfVar8 = (float*)Hero;
    if ((param_1 == 0xd8) && (AsObject(pfVar17)->SubType == 3)) {
      *(int*)&pfVar17[EffectLifetimeIndex] = 40;
      fVar27 = pfVar8[5];
      AsObject(pfVar17)->StartPosition[0] = pfVar8[4];
      fVar2 = pfVar8[6];
      AsObject(pfVar17)->StartPosition[1] = fVar27;
      AsObject(pfVar17)->StartPosition[2] = fVar2;
      iVar9 = _rand();
      param_8 = (float *)(iVar9 % 0x1e);
      AsObject(pfVar17)->Direction[1] = -((float)(intptr_t)param_8 + 30.0f);
      iVar9 = _rand();
      param_8 = (float *)(iVar9 % 0x14);
      *pfVar12 = -((float)(int)(intptr_t)param_8 + 45.0f);
    }
    *(char *)(pfVar17 + 0x22) = *(char *)((char*)CharacterMachine + 1408);   // serial de proyectiles
    return pfVar17;
  }
  case 0xe1:
    if (CreateEffect_Type225(pfVar17, pfVar8, result)) return result;
    break;
  case 0xe2:
  case 0xe3:
    goto switchD_00461001_caseD_e2;
  case 0xe4:
  case 0xe5:
  case 0xe6:
    if (CreateEffect_Type228(fVar27, iVar9, pfVar17, result)) return result;
    break;
  case MODEL_MAGIC_CIRCLE1:
    if (CreateEffect_MagicCircle1(pfVar17, pfVar8, result)) return result;
    break;
  case 0xed:
    if (CreateEffect_Type237(pfVar17, pfVar8, result)) return result;
    break;
  case 0xee:
    if (CreateEffect_Type238(pfVar17, result)) return result;
    break;
  case 0xef:
    if (CreateEffect_Type239(pfVar17, result)) return result;
    break;
  case MODEL_SKILL_BLAST:
    if (CreateEffect_SkillBlast(param_8, iVar9, pfVar12, pfVar17, uVar10, result)) return result;
    break;
  case 0xf1:
    *(int*)&pfVar17[EffectBlendMeshIndex] = -2;
    AsObject(pfVar17)->Scale = 0.9;
    AsObject(pfVar17)->Velocity = 0.5;
    // SubType es un DWORD: leer los BITS, no convertir el float (daba 0 siempre).
    switch(AsObject(pfVar17)->SubType) {
    case 0:
      AsObject(pfVar17)->Light[0] = 0.8;
      AsObject(pfVar17)->Light[1] = 0.8;
      AsObject(pfVar17)->Light[2] = 0.8;
      *(int*)&pfVar17[EffectLifetimeIndex] = 15;
      break;
    case 1:
      AsObject(pfVar17)->Light[0] = 1.0;
      AsObject(pfVar17)->Light[1] = 0.5;
      AsObject(pfVar17)->Light[2] = 0.2;
      *(int*)&pfVar17[EffectLifetimeIndex] = 35;
      break;
    case 2:
      AsObject(pfVar17)->Light[0] = 0.8;
      AsObject(pfVar17)->Light[1] = 0.8;
      AsObject(pfVar17)->Light[2] = 0.8;
      *(int*)&pfVar17[EffectLifetimeIndex] = 12;
      *(float *)&AsObject(pfVar17)->HiddenMesh = 0.0;
      AsObject(pfVar17)->Scale = 0.5;
      break;
    case 3:
      AsObject(pfVar17)->Light[0] = 0.8;
      AsObject(pfVar17)->Light[1] = 0.8;
      AsObject(pfVar17)->Light[2] = 0.8;
      *(int*)&pfVar17[EffectLifetimeIndex] = 4;
      *(float *)&AsObject(pfVar17)->HiddenMesh = 1.4013e-45;
      AsObject(pfVar17)->Scale = 0.02;
      AsObject(pfVar17)->Velocity = 1.0;
      AsObject(pfVar17)->Angle[0] = 90.0;
      AsObject(pfVar17)->Angle[1] = 0.0;
      AsObject(pfVar17)->Angle[2] = 0.0;
      break;
    case 4:
      AsObject(pfVar17)->Light[0] = 0.8;
      AsObject(pfVar17)->Light[1] = 0.8;
      AsObject(pfVar17)->Light[2] = 0.8;
      *(int*)&pfVar17[EffectLifetimeIndex] = 35;
      *(float *)&AsObject(pfVar17)->HiddenMesh = 1.4013e-45;
      AsObject(pfVar17)->Scale = 0.1;
      AsObject(pfVar17)->Gravity = 0.0;
    }
    pfVar8 = (float*)CharacterMachine;
    // *(float *)&AsObject(pfVar17)->Owner (= slot+252) es el PUNTERO al owner: hay que reinterpretar
    // los bits, no convertir el float (eso destruia el puntero y la comparacion
    // con Hero nunca daba true).
    pfVar12 = (float*)(uintptr_t)(intptr_t)AsObject(pfVar17)->Owner;
    if (pfVar12 != (float*)Hero) {
      return pfVar12;
    }
    if (1 < AsObject(pfVar17)->SubType) {                      // SubType (DWORD)
      return pfVar12;
    }
    *(char *)(pfVar17 + 0x22) = *(char *)((char*)CharacterMachine + 1408);   // serial de proyectiles
    return (float *)Entity_FindNearby_SendPacket((uint)AsObject(pfVar17)->Skill,AsObject(pfVar17)->Position,400.0,
                                                 *(byte *)(pfVar17 + 0x22),AsObject(pfVar17)->PKKey);
  case MODEL_ARROW_DOUBLE:
    if (CreateEffect_ArrowDouble(pfVar12, pfVar17, result)) return result;
    break;
  case 0xf3:
    if (CreateEffect_Type243(fVar27, iVar9, local_3c, local_58, local_5c, local_60, local_64, local_68, local_6c, pfVar12, pfVar17, pfVar8, result)) return result;
    break;
  case 0xf4:
    if (CreateEffect_Type244(iVar9, pfVar17, result)) return result;
    break;
  case MODEL_EARTHQUAKE:
    if (CreateEffect_Earthquake(param_7, pfVar17, pfVar8, result)) return result;
    break;
  }
  // Serial de proyectiles: CharacterMachine+1408 (IDA: i[136] = *(BYTE*)(CharacterMachine
  // + 1408)), el mismo que leen Joint_Create y UseSkillWarrior.  Ghidra lo da como
  // pfVar12[0x160] sobre un float*, o sea 0x160 * 4; el port lo había pasado a bytes
  // sin escalar y leía e incrementaba CharacterMachine+0x160, que es otro campo.
  cVar5 = *(char *)((char*)CharacterMachine + 1408);
  *(char *)(pfVar17 + 0x22) = cVar5;
  *(char *)((char*)CharacterMachine + 1408) = cVar5 + '\x01';
  return pfVar17;
switchD_00460f25_caseD_100:
  pfVar12 = pfVar17 + 7;
  AsObject(pfVar17)->Velocity = 1.0;
  *(int*)&pfVar17[EffectLifetimeIndex] = 30;
  Matrix_BuildFromEuler(pfVar12,local_3c + 3);
  local_6c = -10.0;
  local_68 = -60.0;
  local_64 = 135.0;
  Vector_Rotate(&local_6c,local_3c + 3,&local_60);
  param_4 = _param_9;
  param_9 = 0;
  param_8 = (float *)(uintptr_t)((uint)(uintptr_t)param_8 & 0xffffff00);
  param_7 = (float *)0xffffffff;
  *pfVar14 = local_60 + *pfVar14;
  param_1 = 0xff;
  AsObject(pfVar17)->Position[1] = local_5c + AsObject(pfVar17)->Position[1];
  AsObject(pfVar17)->Position[2] = local_58 + AsObject(pfVar17)->Position[2];
  AsObject(pfVar17)->Scale = 0.8;
  AsObject(pfVar17)->Direction[1] = -70.0;
  param_2 = pfVar14;
  param_6 = pfVar17;
  goto LAB_00460dd8;
switchD_00465549_caseD_4ba:
  // El vector de avance de la estela de los tipos 1210/1211/1212 sale de
  // `VectorRotate(in1, in2, (float *)i + 48)` con IDA poniendo `in1 = (0, -50, 0)`
  // (o `(0, -15, 0)` para el 1212): X y Z en 0, no un valor sobrante de otra rama.
  // El campo +0xC0 es el paso que `RenderEffects` usa para los 30 sprites 1176
  // de la estela.
  *(int*)&pfVar17[EffectLifetimeIndex] = 20;
  if (param_1 == 0x4ba) {
    AsObject(pfVar17)->Light[0] = 0.5;
    AsObject(pfVar17)->Light[1] = 0.7;
    AsObject(pfVar17)->Light[2] = 1.0;
  }
  else {
    if (param_1 != 0x4bb) {
      if (param_1 == 0x4bc) {
        local_6c = 0.0f;   // IDA: in1[0] = 0.0
        local_64 = 0.0f;   // IDA: in1[2] = 0.0
        AsObject(pfVar17)->Light[0] = 1.0;
        AsObject(pfVar17)->Light[1] = 0.4;
        AsObject(pfVar17)->Light[2] = 0.2;
        local_68 = -15.0;
        AsObject(pfVar17)->Scale = 5.0;
      }
      goto LAB_00465641;
    }
    AsObject(pfVar17)->Light[0] = 1.0;
    AsObject(pfVar17)->Light[1] = 0.4;
    AsObject(pfVar17)->Light[2] = 0.2;
  }
  local_6c = 0.0f;   // IDA: in1[0] = 0.0
  local_64 = 0.0f;   // IDA: in1[2] = 0.0
  local_68 = -50.0;
  AsObject(pfVar17)->Scale = 16.0;
LAB_00465641:
  Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
  Vector_Rotate(&local_6c,local_3c + 3,AsObject(pfVar17)->Direction);
  if ((float*)(uintptr_t)(intptr_t)AsObject(pfVar17)->Owner != (float*)Hero) {
    return (float*)Hero;
  }
  fVar27 = AsObject(pfVar17)->Position[1];
  fVar2 = AsObject(pfVar17)->Position[2];
  *param_2 = AsObject(pfVar17)->Position[0];
  param_2[1] = fVar27;
  param_2[2] = fVar2;
  local_6c = 0.0;
  local_68 = -150.0;
  local_64 = 0.0;
  Matrix_BuildFromEuler(AsObject(pfVar17)->Angle,local_3c + 3);
  Vector_Rotate(&local_6c,local_3c + 3,&local_48);
  iVar9 = 4;
  do {
    sVar28 = AsObject(pfVar17)->PKKey;
    bVar6 = *(byte *)(pfVar17 + 0x22);
    bVar1 = AsObject(pfVar17)->Skill;
    *param_2 = local_48 + *param_2;
    param_2[1] = local_44 + param_2[1];
    param_2[2] = local_40 + param_2[2];
    pfVar12 = (float *)Entity_FindNearby_SendPacket((uint)bVar1,param_2,150.0,bVar6,sVar28);
    iVar9 = iVar9 + -1;
  } while (iVar9 != 0);
  return pfVar12;
LAB_00463ca2:
  pfVar12 = (float *)Entity_FindNearby_SendPacket((uint)AsObject(pfVar17)->Skill,AsObject(pfVar17)->Position,fVar27,bVar6,
                                  sVar28);
  return pfVar12;
}

// ── Tamachan: control por red (opcode 0x0B, tipo de evento 2) ────────────────
// IDA 0.98j sub_46C220: apaga todo Tamachan vivo.
void __cdecl Tamachan_Clear(void)
{
  for (int i = 0; i < kEffectPoolSize; ++i) {
    char *o = (char *)&Effects[i];
    if (*o && *(short *)(o + 2) == 0xc1) {
      *o = 0;
      *(int *)(o + 96) = -1;
    }
  }
}

// IDA 0.98j sub_46C190: lo hace aparecer en el puente de Lorencia (tile
// 162,120), mirando al heroe.  El 0.98j pasa el mismo vector como Angle y
// como Light; CreateEffect pisa la luz con (1,1,1) igual.
void __cdecl Tamachan_Spawn(void)
{
  float pos[3];
  float angle[3];
  pos[0] = 16284.0f;
  pos[1] = 12031.0f;
  pos[2] = RequestTerrainHeight(16284.0f, 12031.0f);
  angle[0] = 0.0f;
  angle[1] = 0.0f;
  angle[2] = 0.0f;
  if (Hero)   // guard del port: el binario asume Hero
    angle[2] = CreateAngle(16284.0f, 12031.0f,
                            *(float *)((char *)Hero + 16),
                            *(float *)((char *)Hero + 20));
  CreateEffect(MODEL_TAMA, pos, angle, angle, (float *)0, (float *)0, (float *)-1,
                (float *)0, 0);
}

// IDA 0.98j sub_46C250: fin del evento -- cada Tamachan pasa al estado 3
// (se hunde) y le quedan 10 frames.
void __cdecl Tamachan_Dismiss(void)
{
  for (int i = 0; i < kEffectPoolSize; ++i) {
    char *o = (char *)&Effects[i];
    if (*o && *(short *)(o + 2) == 0xc1) {
      *(int *)(o + 96) = 10;
      AsObject(o)->AI = 3;
    }
  }
}
