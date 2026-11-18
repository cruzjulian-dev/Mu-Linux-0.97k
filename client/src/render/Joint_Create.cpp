// Joint_Create.cpp
// Joint_Create @ 0x0046d840
//
// CreateJoint - creates a lightning/beam visual joint by type.

#include "stdafx.h"
#include "game/core/RenderFxSnapshots.h"


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

// Salida común de CreateJoint (IDA 0x46D840: switchD_0046dee7_default): acota
// MaxTails y completa el anillo de segmentos.
static void* CreateJoint_Finish(char *pcVar14)
{
  if (0x32 < AsJoint(pcVar14)->MaxTails) {
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '2';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  }
  {
    // DESVIACION DOCUMENTADA (no esta en IDA 0x46D840).
    //
    // El binario inicializa UNICAMENTE la fila 0 del anillo de segmentos
    // (`v11+22..v11+33` = 0x58..0x87, los 4 vertices) y deja el resto como
    // estaba.  En el original el pool de joints se recicla sin parar y las filas
    // altas conservan coordenadas plausibles del joint anterior.  Aca el pool es
    // un global en BSS: la primera vez que se usa un slot esas filas valen 0 y el
    // renderer (0x00473710) dibujaria un quad hasta el origen del mapa.
    //
    // Se replica la fila 0 en el resto del anillo (quads sobrantes degenerados
    // sobre la posicion del joint), arrancando DESPUES de las filas que la
    // creacion ya construyo (0..segCount) para no pisar los segmentos armados por
    // los bucles de creacion (1254 sub 14 / 1253 sub 4 de las alas del MG, y
    // cualquier subtipo que llame a sub_46FE90 dentro de CreateJoint).
    const int __rowBytes = 0x30;
    const int __maxRows  = MAX_JOINT_TAILS;
    int __n = AsJoint(pcVar14)->MaxTails;
    if (__n > __maxRows) __n = __maxRows;
    const int __built = AsJoint(pcVar14)->NumTails;
    for (int __r = __built + 1; __r < __n; ++__r)
        memcpy(AsJoint(pcVar14)->Tails[__r], AsJoint(pcVar14)->Tails[0], __rowBytes);
  }
  return (void*)pcVar14;
}

// LAB_0046e043 de IDA: guarda la posición de salida del joint.
static void CreateJoint_SaveStart(char *pcVar14, float *pfVar1)
{
  AsJoint(pcVar14)->StartPosition[0] = *pfVar1;
  *(undefined4 *)&AsJoint(pcVar14)->StartPosition[1] = *(undefined4 *)&AsJoint(pcVar14)->Position[1];
  *(undefined4 *)&AsJoint(pcVar14)->StartPosition[2] = *(undefined4 *)&AsJoint(pcVar14)->Position[2];
}

// LAB_0046e37e de IDA: el joint arranca en la posición destino.
static void CreateJoint_ResetPosition(char *pcVar14)
{
  *(undefined4 *)&AsJoint(pcVar14)->Position[0] = *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[0];
  *(undefined4 *)&AsJoint(pcVar14)->Position[1] = *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[1];
  *(undefined4 *)&AsJoint(pcVar14)->Position[2] = *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[2];
}

// ── Handlers de CreateJoint (T9g) ─────────────────────────────────────────────
// Cada case del despacho por tipo vive en su propia función.  Reciben por
// referencia las locales que usan y devuelven true cuando el case termina
// CreateJoint (con el slot en result); false sigue después del switch.

// IDA: CreateJoint (0x0046D840): case BITMAP_JOINT_SPARK:
static bool CreateJoint_JointSpark(intptr_t& iVar6, byte*& local_98, byte*& pbVar10, char*& pcVar14, float*& pfVar3, intptr_t& uVar9, void*& result)
{
  iVar6 = AsJoint(pcVar14)->SubType;
  if (iVar6 == 0) {
    pcVar14[0xc] = '\0';
    pcVar14[0xd] = '\0';
    pcVar14[0xe] = '\0';
    pcVar14[0xf] = '@';
    iVar6 = _rand();
    pbVar10 = (byte *)(iVar6 % 0x14 + 6);
    local_98 = pbVar10;
    AsJoint(pcVar14)->Velocity = (float)(intptr_t)pbVar10;
    uVar9 = _rand();
    uVar9 = uVar9 & 0x80000007;
    if ((int)uVar9 < 0) {
      uVar9 = (uVar9 - 1 | 0xfffffff8) + 1;
    }
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x02';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    *(uint *)&AsJoint(pcVar14)->LifeTime = uVar9 + 8;
  }
  else {
    if (iVar6 != 1) {
      if (iVar6 == 2) {
        pcVar14[0xc] = '\0';
        pcVar14[0xd] = '\0';
        pcVar14[0xe] = -0x80;
        pcVar14[0xf] = '@';
        pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = -0x10;
        pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'A';
        uVar9 = _rand();
        uVar9 = uVar9 & 0x80000003;
        if ((int)uVar9 < 0) {
          uVar9 = (uVar9 - 1 | 0xfffffffc) + 1;
        }
        pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x02';
        pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
        *(uint *)&AsJoint(pcVar14)->LifeTime = uVar9 + 4;
        *pfVar3 = 0.3;
        pcVar14[0x38] = -0x66;
        pcVar14[0x39] = -0x67;
        pcVar14[0x3a] = -0x67;
        pcVar14[0x3b] = '>';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
      }
      { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
    }
    pcVar14[0xc] = '\0';
    pcVar14[0xd] = '\0';
    pcVar14[0xe] = '\0';
    pcVar14[0xf] = '@';
    iVar6 = _rand();
    local_98 = (byte *)(iVar6 % 0x14 + 0x10);
    AsJoint(pcVar14)->Velocity = (float)(intptr_t)local_98;
    uVar9 = _rand();
    uVar9 = uVar9 & 0x80000003;
    if ((int)uVar9 < 0) {
      uVar9 = (uVar9 - 1 | 0xfffffffc) + 1;
    }
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x02';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    *(uint *)&AsJoint(pcVar14)->LifeTime = uVar9 + 4;
  }
  return false;
}

// IDA: CreateJoint (0x0046D840): case BITMAP_JOINT_ENERGY:
static bool CreateJoint_JointEnergy(float& param_7, intptr_t& iVar6, char*& pcVar14, float*& pfVar1, float*& pfVar3, undefined4& uVar23, void*& result)
{
  iVar6 = AsJoint(pcVar14)->SubType;
  AsJoint(pcVar14)->Scale = param_7;
  switch(iVar6) {
  case 2:
  case 3:
  case 4:
  case 5:
  case 8:
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = -1;
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = -0x37;
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = -0x66;
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = ';';
    if (iVar6 == 5) {
      pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
      pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    }
    else {
      pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x14';
      pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    }
    if (iVar6 == 3) {
      *pfVar1 = *(float *)((intptr_t)AsJoint(pcVar14)->Target + 0x4c);
      *(undefined4 *)&AsJoint(pcVar14)->Position[1] = *(undefined4 *)((intptr_t)AsJoint(pcVar14)->Target + 0x50);
      uVar23 = *(undefined4 *)((intptr_t)AsJoint(pcVar14)->Target + 0x54);
    }
    else {
      *pfVar1 = *(float *)((intptr_t)AsJoint(pcVar14)->Target + 0x40);
      *(undefined4 *)&AsJoint(pcVar14)->Position[1] = *(undefined4 *)((intptr_t)AsJoint(pcVar14)->Target + 0x44);
      uVar23 = *(undefined4 *)((intptr_t)AsJoint(pcVar14)->Target + 0x48);
    }
    *(undefined4 *)&AsJoint(pcVar14)->Position[2] = uVar23;
    break;
  case 9:
    *pfVar3 = 0.2;
    pcVar14[0x38] = -0x33;
    pcVar14[0x39] = -0x34;
    pcVar14[0x3a] = 'L';
    pcVar14[0x3b] = '>';
    pcVar14[0x3c] = -0x33;
    pcVar14[0x3d] = -0x34;
    pcVar14[0x3e] = 'L';
    pcVar14[0x3f] = '>';
  case 0:
  case 1:
  case 6:
  case 7:
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = 'x';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\b';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = '@';
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = '@';
  }
  switch(*(undefined4 *)&AsJoint(pcVar14)->SubType) {
  case 0:
  case 6:
    *pfVar3 = 0.4;
    pcVar14[0x38] = -0x66;
    pcVar14[0x39] = -0x67;
    pcVar14[0x3a] = -0x67;
    pcVar14[0x3b] = '>';
    pcVar14[0x3c] = -0x33;
    pcVar14[0x3d] = -0x34;
    pcVar14[0x3e] = 'L';
    pcVar14[0x3f] = '>';
    break;
  case 1:
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = '\0';
    pcVar14[0x3f] = '?';
    *pfVar3 = 0.1;
    pcVar14[0x38] = -0x33;
    pcVar14[0x39] = -0x34;
    pcVar14[0x3a] = -0x34;
    pcVar14[0x3b] = '=';
    break;
  case 2:
  case 3:
  case 5:
    *pfVar3 = 0.5;
    pcVar14[0x38] = -0x33;
    pcVar14[0x39] = -0x34;
    pcVar14[0x3a] = -0x34;
    pcVar14[0x3b] = '=';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = -0x80;
    pcVar14[0x3f] = '?';
    break;
  case 4:
    *pfVar3 = 0.3;
    pcVar14[0x38] = -0x66;
    pcVar14[0x39] = -0x67;
    pcVar14[0x3a] = '\x19';
    pcVar14[0x3b] = '>';
    pcVar14[0x3c] = -0x33;
    pcVar14[0x3d] = -0x34;
    pcVar14[0x3e] = -0x34;
    pcVar14[0x3f] = '=';
    break;
  case 8:
    *pfVar3 = 1.0;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = '\0';
    pcVar14[0x3b] = '\0';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = '\0';
    pcVar14[0x3f] = '?';
  }
  { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  return false;
}

// IDA: CreateJoint (0x0046D840): case BITMAP_JOINT_HEALING:
static bool CreateJoint_JointHealing(float& param_7, intptr_t& iVar6, byte*& local_98, char*& pcVar14, float*& pfVar3, intptr_t& uVar9, void*& result)
{
  pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\f';
  pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
  AsJoint(pcVar14)->Scale = param_7;
  pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = '\0';
  iVar6 = AsJoint(pcVar14)->SubType;
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x02';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  if (iVar6 == 6) {
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x04';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    *pfVar3 = 1.0;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = -0x80;
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = '\0';
    pcVar14[0x3f] = '?';
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  }
  if (iVar6 == 7) {
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x04';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    *pfVar3 = 1.0;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = -0x80;
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = '\0';
    pcVar14[0x3f] = '\0';
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  }
  if (iVar6 == 4) {
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x1e';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    uVar9 = _rand();
    uVar9 = uVar9 & 0x8000003f;
    if ((int)uVar9 < 0) {
      uVar9 = (uVar9 - 1 | 0xffffffc0) + 1;
    }
    pcVar14[0x48 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x49 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x4a + JOINT_FIELD_SHIFT] = ' ';
    pcVar14[0x4b + JOINT_FIELD_SHIFT] = -0x3f;
    pcVar14[0x4c + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x4d + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x4e + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x4f + JOINT_FIELD_SHIFT] = '\0';
    AsJoint(pcVar14)->TargetPosition[0] = (float)(int)(uVar9 - 0x20);
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  }
  if (iVar6 != 5) { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  iVar6 = _rand();
  local_98 = (byte *)(iVar6 % 10 + -5);
  AsJoint(pcVar14)->Scale = (float)(intptr_t)local_98 + AsJoint(pcVar14)->Scale;
  iVar6 = _rand();
  local_98 = (byte *)(iVar6 % 0x14 + 6);
  AsJoint(pcVar14)->Velocity = (float)(intptr_t)local_98;
  uVar9 = _rand();
  uVar9 = uVar9 & 0x80000007;
  if ((int)uVar9 < 0) {
    uVar9 = (uVar9 - 1 | 0xfffffff8) + 1;
  }
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\b';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  *(uint *)&AsJoint(pcVar14)->LifeTime = uVar9 + 8;
  return false;
}

// IDA: CreateJoint (0x0046D840): case 0x4ec:
static bool CreateJoint_JointLaser(float& param_7, char*& pcVar14, void*& result)
{
  pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = -0x74;
  pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
  pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '1';
  pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
  AsJoint(pcVar14)->Scale = param_7;
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x06';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  return false;
}

// IDA: CreateJoint (0x0046D840): case 0x48e:, case BITMAP_JOINT_LASER + 1:
static bool CreateJoint_MotionBlur(char*& pcVar14, float*& pfVar3, void*& result)
{
  pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = ' ';
  pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
  pcVar14[0xc] = '\0';
  pcVar14[0xd] = '\0';
  pcVar14[0xe] = 'p';
  pcVar14[0xf] = 'B';
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '2';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';

  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x02';
  pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
  if (AsJoint(pcVar14)->SubType != 0) {
    if (AsJoint(pcVar14)->Type == BITMAP_JOINT_LASER + 1) {
      *pfVar3 = 0.35;
      pcVar14[0x38] = -0x33;
      pcVar14[0x39] = -0x34;
      pcVar14[0x3a] = -0x34;
      pcVar14[0x3b] = '=';
      pcVar14[0x3c] = '\0';
      pcVar14[0x3d] = '\0';
      pcVar14[0x3e] = -0x80;
      pcVar14[0x3f] = '?';
    }
    else {
      *pfVar3 = 0.0;
      pcVar14[0x38] = -0x66;
      pcVar14[0x39] = -0x67;
      pcVar14[0x3a] = -0x67;
      pcVar14[0x3b] = '>';
      pcVar14[0x3c] = '\0';
      pcVar14[0x3d] = '\0';
      pcVar14[0x3e] = -0x80;
      pcVar14[0x3f] = '?';
    }
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  }
  return false;
}

// IDA: CreateJoint (0x0046D840): case BITMAP_MOTION_BLUR_R2:
static bool CreateJoint_MotionBlurR2(float& param_7, float& fVar24, float* const& local_6c, float& local_70, float& local_74, float& local_78, byte*& local_88, byte*& local_8c, float& local_90, char*& pcVar14, float*& pfVar1, float*& pfVar2, void*& result)
{
  pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = '\0';
  fVar24 = AsJoint(pcVar14)->Angle[2] + 30.0f;
  AsJoint(pcVar14)->Scale = param_7;
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[2] = *(undefined4 *)&AsJoint(pcVar14)->Position[2];
  AsJoint(pcVar14)->Angle[2] = fVar24;
  AsJoint(pcVar14)->Direction[2] = fVar24;
  AsJoint(pcVar14)->TargetPosition[0] = *pfVar1;
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[1] = *(undefined4 *)&AsJoint(pcVar14)->Position[1];
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x12';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x50 + JOINT_FIELD_SHIFT] = -1;
  pcVar14[0x51 + JOINT_FIELD_SHIFT] = -1;
  pcVar14[0x52 + JOINT_FIELD_SHIFT] = -1;
  pcVar14[0x53 + JOINT_FIELD_SHIFT] = -1;
  pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x14';
  pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
  local_78 = 0.0;
  local_74 = -180.0;
  local_70 = 0.0;
  AsJoint(pcVar14)->Direction[0] = *pfVar2;
  *(undefined4 *)&AsJoint(pcVar14)->Direction[1] = *(undefined4 *)&AsJoint(pcVar14)->Angle[1];
  Matrix_BuildFromEuler(pfVar2,local_6c);
  Vector_Rotate(&local_78,local_6c,(float *)&local_90);
  *pfVar1 = (float)local_90 + *pfVar1;
  AsJoint(pcVar14)->Position[1] = *(float*)&local_8c + AsJoint(pcVar14)->Position[1];
  AsJoint(pcVar14)->Position[2] = *(float*)&local_88 + AsJoint(pcVar14)->Position[2];
  return false;
}

// IDA: CreateJoint (0x0046D840): case 0x4e1:, case BITMAP_FLARE_BLUE:
static bool CreateJoint_Flare(float *& param_3, float& param_7, float10& fVar20, float10& fVar21, float& fVar24, intptr_t& iVar6, float* const& local_3c, float* const& local_6c, float& local_70, float& local_74, float& local_78, byte*& local_88, byte*& local_8c, float& local_90, byte*& local_98, char*& pcVar14, float*& pfVar1, float*& pfVar2, float*& pfVar22, float*& pfVar3, undefined4& uVar23, undefined4& uVar25, intptr_t& uVar9, void*& result)
{
  iVar6 = AsJoint(pcVar14)->SubType;
  pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = 'd';
  pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x14';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0xc] = '\0';
  pcVar14[0xd] = '\0';
  pcVar14[0xe] = ' ';
  pcVar14[0xf] = 'A';
  if (iVar6 == 0) {
    AsJoint(pcVar14)->Scale = param_7;
    iVar6 = _rand();
    local_98 = (byte *)(iVar6 % 0x96);
    AsJoint(pcVar14)->Direction[2] = (float)(intptr_t)local_98 * 0.01f;
    iVar6 = _rand();
    local_98 = (byte *)(iVar6 % 500 + -0xfa);
    AsJoint(pcVar14)->Direction[1] = (float)(intptr_t)local_98;
    if (10.0f < AsJoint(pcVar14)->Scale) {
      pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '2';
      pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
      iVar6 = _rand();
      fVar24 = (float)(iVar6 % 0xfa + 200);
LAB_0046f54e:
      AsJoint(pcVar14)->Direction[2] = fVar24 * 0.01f;
    }
LAB_0046f55a:
    iVar6 = (intptr_t)AsJoint(pcVar14)->Target;
    *pfVar3 = *(float *)(iVar6 + 0xe8);
    *(undefined4 *)&AsJoint(pcVar14)->Light[1] = *(undefined4 *)(iVar6 + 0xec);
    *(undefined4 *)&AsJoint(pcVar14)->Light[2] = *(undefined4 *)(iVar6 + 0xf0);
  }
  else {
    if (iVar6 == 0xe) {
      AsJoint(pcVar14)->Scale = param_7;
      iVar6 = _rand();
      local_98 = (byte *)(iVar6 % 0x96);
      AsJoint(pcVar14)->Direction[2] = (float)(intptr_t)local_98 * 0.01f;
      iVar6 = _rand();
      local_98 = (byte *)(iVar6 % 500 + -0xfa);
      AsJoint(pcVar14)->Direction[1] = (float)(intptr_t)local_98;
      iVar6 = _rand();
      local_98 = (byte *)(iVar6 % 0x32);
      AsJoint(pcVar14)->Position[2] = (float)(intptr_t)local_98 + AsJoint(pcVar14)->Position[2];
      if (10.0f < AsJoint(pcVar14)->Scale) {
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '2';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        iVar6 = _rand();
        fVar24 = (float)(iVar6 % 0xfa + 200);
        goto LAB_0046f54e;
      }
      goto LAB_0046f55a;
    }
    if (iVar6 == 2) {
      iVar6 = _rand();
      local_98 = (byte *)(iVar6 % 0x14 + 0x23);
      AsJoint(pcVar14)->Scale = param_7;
      AsJoint(pcVar14)->Direction[2] = (float)(intptr_t)local_98;
      iVar6 = _rand();
      *pfVar3 = 1.0;
      pcVar14[0x38] = '\0';
      pcVar14[0x39] = '\0';
      pcVar14[0x3a] = -0x80;
      pcVar14[0x3b] = '?';
      pcVar14[0x3c] = '\0';
      pcVar14[0x3d] = '\0';
      pcVar14[0x3e] = -0x80;
      pcVar14[0x3f] = '?';
      AsJoint(pcVar14)->LifeTime = iVar6 % 0x32 + 0x19;
    }
    else if (iVar6 == 3) {
      pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = 'H';
      pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
      *pfVar2 = 0.0;
      AsJoint(pcVar14)->Scale = param_7;
      pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x05';
      pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
      pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x2c] = '\0';
      pcVar14[0x2d] = '\0';
      pcVar14[0x2e] = '4';
      pcVar14[0x2f] = 'B';
      pcVar14[0x30] = '\0';
      pcVar14[0x31] = '\0';
      pcVar14[0x32] = -0x4c;
      pcVar14[0x33] = -0x3e;
      Matrix_BuildFromEuler(pfVar2,local_3c + 3);
      local_78 = 0.0;
      local_74 = 100.0;
      local_70 = 0.0;
      Vector_Rotate(&local_78,local_3c + 3,(float *)&local_90);
      uVar25 = 0;
      *pfVar1 = (float)local_90 + *pfVar1;
      AsJoint(pcVar14)->Position[1] = *(float*)&local_8c + AsJoint(pcVar14)->Position[1];
      AsJoint(pcVar14)->Position[2] = *(float*)&local_88 + AsJoint(pcVar14)->Position[2];
      iVar6 = _rand();
      local_98 = (byte *)(iVar6 % 0x168);
      fVar24 = (float)(int)(intptr_t)local_98;
      uVar23 = 0;
      pfVar22 = pfVar3;
      uVar9 = _rand();
      uVar9 = uVar9 & 0x80000007;
      if ((int)uVar9 < 0) {
        uVar9 = (uVar9 - 1 | 0xfffffff8) + 1;
      }
      local_98 = (byte *)(uVar9 + 8);
      CreateSprite(BITMAP_SHINY + 1,pfVar1,(float)(intptr_t)local_98 * 0.3f,pfVar22,uVar23,fVar24,uVar25)
      ;
      fVar24 = *pfVar2 * (-1.0f);
      *pfVar3 = 1.0;
      pcVar14[0x38] = '\0';
      pcVar14[0x39] = '\0';
      pcVar14[0x3a] = -0x80;
      pcVar14[0x3b] = '?';
      pcVar14[0x3c] = '\0';
      pcVar14[0x3d] = '\0';
      pcVar14[0x3e] = -0x80;
      pcVar14[0x3f] = '?';
      *pfVar2 = fVar24;
      AsJoint(pcVar14)->Angle[1] = AsJoint(pcVar14)->Angle[1] * (-1.0f);
      AsJoint(pcVar14)->Angle[2] = AsJoint(pcVar14)->Angle[2] * (-1.0f);
    }
    else if (((iVar6 == 4) || (iVar6 == 6)) || (iVar6 == 0xc)) {
      AsJoint(pcVar14)->Scale = param_7;
      if (iVar6 == 0xc) {
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = 'F';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x54 + JOINT_FIELD_SHIFT] = '2';
        pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
        *pfVar3 = 0.1;
        pcVar14[0x38] = -0x33;
        pcVar14[0x39] = -0x34;
        pcVar14[0x3a] = -0x34;
        pcVar14[0x3b] = '=';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
        iVar6 = _rand();
        AsJoint(pcVar14)->Direction[0] = (float)(iVar6 % 0x168);
        fVar21 = (float10)(-0.017453292f);
        fVar20 = (float10)fsin((float10)AsJoint(pcVar14)->Angle[2] * fVar21);
        AsJoint(pcVar14)->Direction[1] = (float)(fVar20 * (float10)(-4.0f));
        fVar21 = (float10)fcos((float10)AsJoint(pcVar14)->Angle[2] * fVar21);
        fVar21 = fVar21 * (float10)(-4.0f);
      }
      else {
        if (iVar6 == 6) {
          pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x14';
          pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x1e';
          pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
          *pfVar3 = 1.0;
          pcVar14[0x38] = '\0';
          pcVar14[0x39] = '\0';
          pcVar14[0x3a] = -0x80;
          pcVar14[0x3b] = '?';
          pcVar14[0x3c] = '\0';
          pcVar14[0x3d] = '\0';
          pcVar14[0x3e] = -0x80;
          pcVar14[0x3f] = '?';
        }
        else {
          iVar6 = (intptr_t)AsJoint(pcVar14)->Target;
          pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = 'n';
          pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x54 + JOINT_FIELD_SHIFT] = -0x38;
          pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
          *pfVar3 = *(float *)(iVar6 + 0xe8);
          *(undefined4 *)&AsJoint(pcVar14)->Light[1] = *(undefined4 *)(iVar6 + 0xec);
          *(undefined4 *)&AsJoint(pcVar14)->Light[2] = *(undefined4 *)(iVar6 + 0xf0);
        }
        iVar6 = _rand();
        AsJoint(pcVar14)->Direction[0] = (float)(iVar6 % 0x168);
        fVar21 = (float10)(-0.017453292f);
        fVar20 = (float10)fsin((float10)AsJoint(pcVar14)->Angle[2] * fVar21);
        AsJoint(pcVar14)->Direction[1] = (float)(fVar20 * (float10)(-2.0f));
        fVar21 = (float10)fcos((float10)AsJoint(pcVar14)->Angle[2] * fVar21);
        fVar21 = fVar21 * (float10)(-2.0f);
      }
      AsJoint(pcVar14)->Direction[2] = (float)fVar21;
    }
    else if (iVar6 == 5) {
      pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x02';
      pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x03';
      pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
      iVar6 = _rand();
      *pfVar3 = 1.0;
      pcVar14[0x38] = '\0';
      pcVar14[0x39] = '\0';
      pcVar14[0x3a] = -0x80;
      pcVar14[0x3b] = '?';
      pcVar14[0x3c] = '\0';
      pcVar14[0x3d] = '\0';
      pcVar14[0x3e] = -0x80;
      pcVar14[0x3f] = '?';
      AsJoint(pcVar14)->Direction[2] = -(float)(iVar6 % 3 + 0x28);
    }
    else if (iVar6 == 7) {
      pcVar14[0x3c] = '\0';
      pcVar14[0x3d] = '\0';
      pcVar14[0x3e] = -0x80;
      pcVar14[0x3f] = '?';
      *pfVar3 = 0.2;
      pcVar14[0x38] = -0x33;
      pcVar14[0x39] = -0x34;
      pcVar14[0x3a] = 'L';
      pcVar14[0x3b] = '>';
      iVar6 = _rand();
      pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x0f';
      pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
      AsJoint(pcVar14)->MultiUse = iVar6 % 10;
      AsJoint(pcVar14)->LifeTime = iVar6 % 10 + 0x1e;
      iVar6 = _rand();
      pcVar14[0xc] = '\0';
      pcVar14[0xd] = '\0';
      pcVar14[0xe] = -0x10;
      pcVar14[0xf] = 'A';
      AsJoint(pcVar14)->Direction[0] = (float)(iVar6 % 3000);
    }
    else if (iVar6 == 8) {
      local_78 = 0.0;
      *pfVar2 = 0.0;
      pcVar14[0x2c] = '\0';
      pcVar14[0x2d] = '\0';
      pcVar14[0x2e] = '\0';
      pcVar14[0x2f] = '\0';
      pcVar14[0x30] = '\0';
      pcVar14[0x31] = '\0';
      pcVar14[0x32] = '\0';
      pcVar14[0x33] = '\0';
      pcVar14[0x9c4 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c5 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c6 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c7 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c8 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c9 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9ca + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9cb + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9cc + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9cd + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9ce + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9cf + JOINT_FIELD_SHIFT] = '\0';
      AsJoint(pcVar14)->StartPosition[0] = *pfVar1;
      *(undefined4 *)&AsJoint(pcVar14)->StartPosition[1] = *(undefined4 *)&AsJoint(pcVar14)->Position[1];
      *(undefined4 *)&AsJoint(pcVar14)->StartPosition[2] = *(undefined4 *)&AsJoint(pcVar14)->Position[2];
      local_74 = -50.0;
      local_70 = 0.0;
      Matrix_BuildFromEuler(AsJoint(pcVar14)->TargetPosition,local_6c);
      Vector_Rotate(&local_78,local_6c,(float *)&local_90);
      *pfVar1 = (float)local_90 + AsJoint(pcVar14)->StartPosition[0];
      AsJoint(pcVar14)->Position[1] = *(float*)&local_8c + AsJoint(pcVar14)->StartPosition[1];
      AsJoint(pcVar14)->Position[2] = *(float*)&local_88 + AsJoint(pcVar14)->StartPosition[2];
    }
    else if (iVar6 == 9) {
      *pfVar2 = 0.0;
      pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x2c] = '\0';
      pcVar14[0x2d] = '\0';
      pcVar14[0x2e] = '\0';
      pcVar14[0x2f] = '\0';
      pcVar14[0x30] = '\0';
      pcVar14[0x31] = '\0';
      pcVar14[0x32] = '\0';
      pcVar14[0x33] = '\0';
      AsJoint(pcVar14)->Scale = param_7;
      *pfVar3 = 0.3;
      pcVar14[0x38] = -0x66;
      pcVar14[0x39] = -0x67;
      pcVar14[0x3a] = -0x67;
      pcVar14[0x3b] = '>';
      pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
      pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x3c] = '\0';
      pcVar14[0x3d] = '\0';
      pcVar14[0x3e] = -0x80;
      pcVar14[0x3f] = '?';
      AsJoint(pcVar14)->StartPosition[0] = (*param_3 - *pfVar1) * 0.1f;
      AsJoint(pcVar14)->StartPosition[1] = (param_3[1] - AsJoint(pcVar14)->Position[1]) * 0.1f;
      AsJoint(pcVar14)->StartPosition[2] = (param_3[2] - AsJoint(pcVar14)->Position[2]) * 0.1f;
      Matrix_BuildFromEuler(pfVar2,local_6c);
      iVar6 = 0;
      if (0 < AsJoint(pcVar14)->MaxTails) {
        do {
          *pfVar1 = *pfVar1 + AsJoint(pcVar14)->StartPosition[0];
          AsJoint(pcVar14)->Position[1] = AsJoint(pcVar14)->Position[1] + AsJoint(pcVar14)->StartPosition[1];
          AsJoint(pcVar14)->Position[2] = AsJoint(pcVar14)->StartPosition[2] + AsJoint(pcVar14)->Position[2];
          Joint_SegmentTick((intptr_t)pcVar14,local_6c);
          iVar6 = iVar6 + 1;
        } while (iVar6 < AsJoint(pcVar14)->MaxTails);
      }
    }
    else {
      if (iVar6 == 10) {
        AsJoint(pcVar14)->Scale = param_7;
        iVar6 = _rand();
        local_98 = (byte *)(iVar6 % 0x96);
        AsJoint(pcVar14)->Direction[2] = (float)(intptr_t)local_98 * 0.01f;
        iVar6 = _rand();
        local_98 = (byte *)(iVar6 % 500 + -0xfa);
        AsJoint(pcVar14)->Direction[1] = (float)(intptr_t)local_98;
        if (10.0f < AsJoint(pcVar14)->Scale) {
          pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '2';
          pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
          pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
          iVar6 = _rand();
          fVar24 = (float)(iVar6 % 0xfa + 200);
          goto LAB_0046f54e;
        }
        goto LAB_0046f55a;
      }
      if (iVar6 == 0xb) {
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
        *pfVar3 = 0.2;
        pcVar14[0x38] = -0x33;
        pcVar14[0x39] = -0x34;
        pcVar14[0x3a] = 'L';
        pcVar14[0x3b] = '>';
        pcVar14[0x9d4 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9d5 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9d6 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9d7 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x1e';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x0f';
        pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
        iVar6 = _rand();
        pcVar14[0xc] = '\0';
        pcVar14[0xd] = '\0';
        pcVar14[0xe] = -0x10;
        pcVar14[0xf] = 'A';
        AsJoint(pcVar14)->Direction[0] = (float)(iVar6 % 3000);
      }
      else if (iVar6 == 0xd) {
        iVar6 = _rand();
        local_98 = (byte *)(iVar6 % 0x14 + 0x23);
        AsJoint(pcVar14)->Scale = param_7;
        AsJoint(pcVar14)->Direction[2] = (float)(intptr_t)local_98;
        iVar6 = _rand();
        *pfVar3 = 0.5;
        pcVar14[0x38] = '\0';
        pcVar14[0x39] = '\0';
        pcVar14[0x3a] = '\0';
        pcVar14[0x3b] = '?';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = '\0';
        pcVar14[0x3f] = '?';
        AsJoint(pcVar14)->LifeTime = iVar6 % 0x32 + 0x19;
      }
    }
  }
  AsJoint(pcVar14)->TargetPosition[0] = *param_3;
  AsJoint(pcVar14)->TargetPosition[1] = param_3[1];
  AsJoint(pcVar14)->TargetPosition[2] = param_3[2];
  { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  return false;
}

// IDA: CreateJoint (0x0046D840): case 0x4e8:
static bool CreateJoint_Type1256(float& fVar24, char*& pcVar14, void*& result)
{
  fVar24 = AsJoint(pcVar14)->TargetPosition[2] + 130.0f;
  pcVar14[0xc] = '\0';
  pcVar14[0xd] = '\0';
  pcVar14[0xe] = -0x74;
  pcVar14[0xf] = 'B';
  pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = 'H';
  pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\b';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x14';
  pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
  AsJoint(pcVar14)->TargetPosition[2] = fVar24;
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[0] = *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[0];
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[1] = *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[1];
  { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  return false;
}

// IDA: CreateJoint (0x0046D840): case 0x498:
static bool CreateJoint_Type1176(intptr_t& iVar6, byte*& local_94, char*& pcVar14, float*& pfVar1, float*& pfVar3, void*& result)
{
  pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = 'd';
  pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x14';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0xc] = '\0';
  pcVar14[0xd] = '\0';
  pcVar14[0xe] = ' ';
  pcVar14[0xf] = 'A';
  if (AsJoint(pcVar14)->SubType == 0) {
    iVar6 = _rand();
    local_94 = (byte *)(iVar6 % 0x14 + 0x23);
    AsJoint(pcVar14)->Direction[2] = (float)(intptr_t)local_94;
    iVar6 = _rand();
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x19';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    AsJoint(pcVar14)->Scale = (float)(iVar6 % 0x14) + 20.0f;
  }
  AsJoint(pcVar14)->TargetPosition[0] = *pfVar1;
  *pfVar3 = 1.0;
  pcVar14[0x38] = '\0';
  pcVar14[0x39] = '\0';
  pcVar14[0x3a] = -0x80;
  pcVar14[0x3b] = '?';
  pcVar14[0x3c] = '\0';
  pcVar14[0x3d] = '\0';
  pcVar14[0x3e] = -0x80;
  pcVar14[0x3f] = '?';
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[1] = *(undefined4 *)&AsJoint(pcVar14)->Position[1];
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[2] = *(undefined4 *)&AsJoint(pcVar14)->Position[2];
  { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  return false;
}

// IDA: CreateJoint (0x0046D840): case 0x10a:
static bool CreateJoint_Type266(float *& param_3, float& param_7, intptr_t& iVar6, byte*& local_94, char*& pcVar14, float*& pfVar2, float*& pfVar3, void*& result)
{
  iVar6 = (intptr_t)AsJoint(pcVar14)->Target;
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[0] = *(undefined4 *)(iVar6 + 0x10);
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[1] = *(undefined4 *)(iVar6 + 0x14);
  *(undefined4 *)&AsJoint(pcVar14)->TargetPosition[2] = *(undefined4 *)(iVar6 + 0x18);
  switch(*(undefined4 *)&AsJoint(pcVar14)->SubType) {
  case 0:
    *pfVar3 = 1.0;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = -0x80;
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = -0x80;
    pcVar14[0x3f] = '?';
    break;
  case 1:
    *pfVar3 = 0.2;
    pcVar14[0x38] = -0x33;
    pcVar14[0x39] = -0x34;
    pcVar14[0x3a] = 'L';
    pcVar14[0x3b] = '>';
    pcVar14[0x3c] = -0x33;
    pcVar14[0x3d] = -0x34;
    pcVar14[0x3e] = 'L';
    pcVar14[0x3f] = '>';
    break;
  case 2:
    AsJoint(pcVar14)->TargetPosition[0] = *param_3;
    AsJoint(pcVar14)->TargetPosition[1] = param_3[1];
    AsJoint(pcVar14)->TargetPosition[2] = param_3[2];
    *pfVar3 = 1.0;
    pcVar14[0x38] = -0x66;
    pcVar14[0x39] = -0x67;
    pcVar14[0x3a] = -0x67;
    pcVar14[0x3b] = '>';
    pcVar14[0x3c] = -0x66;
    pcVar14[0x3d] = -0x67;
    pcVar14[0x3e] = -0x67;
    pcVar14[0x3f] = '>';
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x14';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x05';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    AsJoint(pcVar14)->Scale = param_7;
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  case 3:
    if (iVar6 == 0) {
      *pfVar3 = 0.5;
      pcVar14[0x38] = '\0';
      pcVar14[0x39] = '\0';
      pcVar14[0x3a] = '\0';
      pcVar14[0x3b] = '\0';
      pcVar14[0x3c] = '\0';
      pcVar14[0x3d] = '\0';
      pcVar14[0x3e] = '\0';
      pcVar14[0x3f] = '\0';
    }
    else {
      *pfVar3 = *(float *)(iVar6 + 0xe8);
      *(undefined4 *)&AsJoint(pcVar14)->Light[1] = *(undefined4 *)(iVar6 + 0xec);
      *(undefined4 *)&AsJoint(pcVar14)->Light[2] = *(undefined4 *)(iVar6 + 0xf0);
    }
    *pfVar2 = -90.0;
    pcVar14[0x2c] = '\0';
    pcVar14[0x2d] = '\0';
    pcVar14[0x2e] = '\0';
    pcVar14[0x2f] = '\0';
    pcVar14[0x30] = '\0';
    pcVar14[0x31] = '\0';
    pcVar14[0x32] = '\0';
    pcVar14[0x33] = '\0';
    pcVar14[0x9c4 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c5 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c6 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c7 + JOINT_FIELD_SHIFT] = '\0';
    iVar6 = _rand();
    pcVar14[0x9cc + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9cd + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ce + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9cf + JOINT_FIELD_SHIFT] = '\0';
    local_94 = (byte *)(iVar6 % 500);
    AsJoint(pcVar14)->Direction[1] = (float)(intptr_t)local_94;
    iVar6 = _rand();
    AsJoint(pcVar14)->Velocity = (float)(iVar6 % 5) + 5.0f;
    break;
  case 4:
    *pfVar3 = 0.4;
    pcVar14[0x38] = -0x33;
    pcVar14[0x39] = -0x34;
    pcVar14[0x3a] = 'L';
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = -0x33;
    pcVar14[0x3d] = -0x34;
    pcVar14[0x3e] = 'L';
    pcVar14[0x3f] = '>';
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x10';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\'';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x1e';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    AsJoint(pcVar14)->Scale = param_7;
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  default:
    AsJoint(pcVar14)->Scale = param_7;
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  }
  pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '?';
  pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = 'B';
  pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\x0f';
  pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x1e';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  AsJoint(pcVar14)->Scale = param_7;
  { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  return false;
}

// IDA: CreateJoint (0x0046D840): case 0x4e2:
static bool CreateJoint_Type1250(float *& param_3, float& param_7, short& param_8, float& fVar24, intptr_t& iVar6, float* const& local_6c, char*& pcVar14, float*& pfVar1, float*& pfVar2, float*& pfVar3, void*& result)
{
  pcVar14[0x9d0 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9d1 + JOINT_FIELD_SHIFT] = '\0';
  AsJoint(pcVar14)->LifeTime = (int)param_8;
  pcVar14[0x54 + JOINT_FIELD_SHIFT] = '2';
  pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
  if (*(char *)&AsJoint(pcVar14)->Skill == '\0') {
    pcVar14[0xc] = '\0';
    pcVar14[0xd] = '\0';
    pcVar14[0xe] = -0x60;
    pcVar14[0xf] = 'A';
    *pfVar3 = 0.5;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = '\0';
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = '\0';
    pcVar14[0x3f] = '?';
  }
  else if (*(char *)&AsJoint(pcVar14)->Skill == '\x01') {
    pcVar14[0xc] = '\0';
    pcVar14[0xd] = '\0';
    pcVar14[0xe] = ' ';
    pcVar14[0xf] = 'B';
    *pfVar3 = 1.0;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = '\0';
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = '\0';
    pcVar14[0x3f] = '\0';
  }
  switch(*(undefined4 *)&AsJoint(pcVar14)->SubType) {
  case 0:
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = '\0';
    break;
  case 1:
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = -0x4c;
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
    break;
  case 2:
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = '4';
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'C';
    break;
  case 3:
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = 'p';
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'C';
    break;
  case 4:
    *pfVar2 = 0.0;
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x2c] = '\0';
    pcVar14[0x2d] = '\0';
    pcVar14[0x2e] = '\0';
    pcVar14[0x2f] = '\0';
    pcVar14[0x30] = '\0';
    pcVar14[0x31] = '\0';
    pcVar14[0x32] = '\0';
    pcVar14[0x33] = '\0';
    AsJoint(pcVar14)->Scale = param_7;
    *pfVar3 = 1.0;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = -0x80;
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = -0x80;
    pcVar14[0x3f] = '?';
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    AsJoint(pcVar14)->StartPosition[0] = (*param_3 - *pfVar1) * 0.1f;
    AsJoint(pcVar14)->StartPosition[1] = (param_3[1] - AsJoint(pcVar14)->Position[1]) * 0.1f;
    AsJoint(pcVar14)->StartPosition[2] = (param_3[2] - AsJoint(pcVar14)->Position[2]) * 0.1f;
    Matrix_BuildFromEuler(pfVar2,local_6c);
    iVar6 = 0;
    if (0 < AsJoint(pcVar14)->MaxTails) {
      do {
        *pfVar1 = *pfVar1 + AsJoint(pcVar14)->StartPosition[0];
        AsJoint(pcVar14)->Position[1] = AsJoint(pcVar14)->Position[1] + AsJoint(pcVar14)->StartPosition[1];
        AsJoint(pcVar14)->Position[2] = AsJoint(pcVar14)->StartPosition[2] + AsJoint(pcVar14)->Position[2];
        Joint_SegmentTick((intptr_t)pcVar14,local_6c);
        iVar6 = iVar6 + 1;
      } while (iVar6 < AsJoint(pcVar14)->MaxTails);
    }
  default:
    AsJoint(pcVar14)->TargetPosition[0] = *param_3;
    AsJoint(pcVar14)->TargetPosition[1] = param_3[1];
    AsJoint(pcVar14)->TargetPosition[2] = param_3[2];
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  }
  fVar24 = param_7 * 1.5f;
  pcVar14[0x9c8 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c9 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ca + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9cb + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9cc + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9cd + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9ce + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9cf + JOINT_FIELD_SHIFT] = '\0';
  AsJoint(pcVar14)->Direction[0] = fVar24;
  AsJoint(pcVar14)->TargetPosition[0] = *param_3;
  AsJoint(pcVar14)->TargetPosition[1] = param_3[1];
  AsJoint(pcVar14)->TargetPosition[2] = param_3[2];
  { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  return false;
}

// IDA: CreateJoint (0x0046D840): case 0x4e5:
static bool CreateJoint_JointSpirit(float *& param_3, float& param_7, intptr_t& iVar6, float* const& local_6c, byte*& local_88, float& local_88f, byte*& local_8c, float& local_8cf, float& local_90, char*& pcVar14, float*& pfVar1, float*& pfVar2, float*& pfVar3, void*& result)
{
  switch(*(undefined4 *)&AsJoint(pcVar14)->SubType) {
  case 0:
    // IDA (0x46D840): el case descifra CharacterMachine, copia el serial de
    // proyectiles a Weapon (v11[2515] = CharacterMachine[1408]) y lo vuelve
    // a cifrar.  Este port no cifra CharacterMachine: queda la copia.
    *(char *)&AsJoint(pcVar14)->Weapon = ((byte*)CharacterMachine)[0x580];
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = -0x74;
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '1';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
      // Este slot es la **Scale** del joint (+0x0C): Ghidra lo tipo como `float**`
      // y le asignaba `param_4` (el PUNTERO al vec3 de angulos).  Confirmado con
      // MU 5.2 CreateJoint, case 0 de BITMAP_JOINT_SPIRIT:  Velocity = 70;
      // LifeTime = 49; Scale; MaxTails = 6.
      AsJoint(pcVar14)->Scale = param_7;
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x06';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  case 1:
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = -0x74;
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '1';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    AsJoint(pcVar14)->Scale = param_7;
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x06';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    PlayBuffer(0x2a,0,0);
    break;
  case 2:
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = 'H';
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
    AsJoint(pcVar14)->Scale = param_7;
    *pfVar3 = 0.5;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = '\0';
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = '\0';
    pcVar14[0x3f] = '?';
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x14';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x03';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    AsJoint(pcVar14)->StartPosition[0] = *pfVar1;
    *(undefined4 *)&AsJoint(pcVar14)->StartPosition[1] = *(undefined4 *)&AsJoint(pcVar14)->Position[1];
    *(undefined4 *)&AsJoint(pcVar14)->StartPosition[2] = *(undefined4 *)&AsJoint(pcVar14)->Position[2];
    break;
  case 3:
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = '\f';
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'C';
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '1';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    AsJoint(pcVar14)->Scale = param_7;
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    *pfVar3 = 1.0;
    pcVar14[0x38] = '\0';
    pcVar14[0x39] = '\0';
    pcVar14[0x3a] = '\0';
    pcVar14[0x3b] = '?';
    pcVar14[0x3c] = -0x33;
    pcVar14[0x3d] = -0x34;
    pcVar14[0x3e] = -0x34;
    pcVar14[0x3f] = '=';
    PlayBuffer(0x49,0,0);
    break;
  case 4:
    *pfVar2 = 0.0;
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x2c] = '\0';
    pcVar14[0x2d] = '\0';
    pcVar14[0x2e] = '\0';
    pcVar14[0x2f] = '\0';
    pcVar14[0x30] = '\0';
    pcVar14[0x31] = '\0';
    pcVar14[0x32] = '\0';
    pcVar14[0x33] = '\0';
    AsJoint(pcVar14)->Scale = param_7;
    *pfVar3 = 0.3;
    pcVar14[0x38] = -0x66;
    pcVar14[0x39] = -0x67;
    pcVar14[0x3a] = -0x67;
    pcVar14[0x3b] = '>';
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x3c] = '\0';
    pcVar14[0x3d] = '\0';
    pcVar14[0x3e] = -0x80;
    pcVar14[0x3f] = '?';
    AsJoint(pcVar14)->StartPosition[0] = (*param_3 - *pfVar1) * 0.1f;
    local_90 = AsJoint(pcVar14)->StartPosition[0];
    AsJoint(pcVar14)->StartPosition[1] = (param_3[1] - AsJoint(pcVar14)->Position[1]) * 0.1f;
    local_8cf = AsJoint(pcVar14)->StartPosition[1];
    local_88f = (param_3[2] - AsJoint(pcVar14)->Position[2]) * 0.1f;
    AsJoint(pcVar14)->StartPosition[2] = local_88f;
    Matrix_BuildFromEuler(pfVar2,local_6c);
    iVar6 = 0;
    if (AsJoint(pcVar14)->MaxTails != 1 && -1 < AsJoint(pcVar14)->MaxTails + -1) {
      do {
        if ((intptr_t)AsJoint(pcVar14)->Target == 0) {
          local_90 = AsJoint(pcVar14)->StartPosition[0];
          local_8cf = AsJoint(pcVar14)->StartPosition[1];
        }
        else {
          local_90 = AsJoint(pcVar14)->StartPosition[0];
          local_8cf = AsJoint(pcVar14)->StartPosition[1];
        }
        *pfVar1 = local_90 + *pfVar1;
        AsJoint(pcVar14)->Position[1] = local_8cf + AsJoint(pcVar14)->Position[1];
        AsJoint(pcVar14)->Position[2] = local_88f + AsJoint(pcVar14)->Position[2];
        Joint_SegmentTick((intptr_t)pcVar14,local_6c);
        iVar6 = iVar6 + 1;
        *pfVar1 = (*pfVar1 - local_90) + AsJoint(pcVar14)->StartPosition[0];
        AsJoint(pcVar14)->Position[1] =
             (AsJoint(pcVar14)->Position[1] - local_8cf) + AsJoint(pcVar14)->StartPosition[1];
      } while (iVar6 < AsJoint(pcVar14)->MaxTails + -1);
    }
    CreateJoint_ResetPosition(pcVar14); { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  case 5:
    // IDA (0x46D840): el case descifra CharacterMachine, copia el serial de
    // proyectiles a Weapon (v11[2515] = CharacterMachine[1408]) y lo vuelve
    // a cifrar.  Este port no cifra CharacterMachine: queda la copia.
    *(char *)&AsJoint(pcVar14)->Weapon = ((byte*)CharacterMachine)[0x580];
    pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = -0x10;
    pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'A';
    pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '1';
    pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
      // Este slot es la **Scale** del joint (+0x0C): Ghidra lo tipo como `float**`
      // y le asignaba `param_4` (el PUNTERO al vec3 de angulos).  Confirmado con
      // MU 5.2 CreateJoint, case 0 de BITMAP_JOINT_SPIRIT:  Velocity = 70;
      // LifeTime = 49; Scale; MaxTails = 6.
      AsJoint(pcVar14)->Scale = param_7;
    pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\f';
    pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
    pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
    { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  }
  { result = (void*)(CreateJoint_Finish(pcVar14)); return true; }
  return false;
}

// IDA: CreateJoint
void * __cdecl
Joint_Create(int param_1,float *param_2,float *param_3,float *param_4,undefined4 param_5,intptr_t param_6
            ,float param_7,short param_8,byte param_9)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  intptr_t iVar6;
  intptr_t uVar9;
  byte *pbVar10;
  char *pcVar14;
  float10 fVar20;
  float10 fVar21;
  float *pfVar22;
  undefined4 uVar23;
  float fVar24;
  undefined4 uVar25;
  // El frame original se reconstruye como UN bloque: el código depende de que
  // {local_90,local_8c,local_88} y {local_78,local_74,local_70} sean tríos
  // contiguos (vec3 de entrada y salida de Vector_Rotate):
  //     local_90 = Scale * -0.5f;  local_8c = 0;  local_88 = 0;
  //     Vector_Rotate(&local_90, local_6c, &local_78);   // in y out son vec3
  //     *(float*)(slot+0x58) = local_78 + Position.x;   // out[0]
  //     *(float*)(slot+0x5c) = local_74 + Position.y;   // out[1]
  //     *(float*)(slot+0x60) = local_70 + Position.z;   // out[2]
  // Varios de estos slots son dual-use (float o byte*), así que se respalda todo
  // con un bloque de bytes y los nombres quedan como referencias tipadas al
  // offset correcto: ebp-0x98 → __fr[0x00] … ebp-0x3C → __fr[0x5C], total 0x98.
  alignas(8) unsigned char __fr[0x98] = {0};
  byte *&local_98 = *(byte **)(__fr + 0x00);
  byte *&local_94 = *(byte **)(__fr + 0x04);
  float &local_90 = *(float  *)(__fr + 0x08);
  byte *&local_8c = *(byte **)(__fr + 0x0c);   // dual-use: float coord OR ptr
  byte *&local_88 = *(byte **)(__fr + 0x10);   // dual-use: float coord OR ptr
  byte *&local_84 = *(byte **)(__fr + 0x14);
  byte *&local_80 = *(byte **)(__fr + 0x18);
  byte *&local_7c = *(byte **)(__fr + 0x1c);
  float &local_78 = *(float  *)(__fr + 0x20);
  float &local_74 = *(float  *)(__fr + 0x24);
  float &local_70 = *(float  *)(__fr + 0x28);
  float *const local_6c = (float *)(__fr + 0x2c);   // 12 floats
  float *const local_3c = (float *)(__fr + 0x5c);   // 15 floats
  // Vistas FLOAT de los slots que Ghidra tipó como `byte*`: cuando forman el
  // vec3 de Vector_Rotate guardan FLOATS, así que hay que reinterpretar los bits
  // (no convertir el valor entero).
  float &local_94f = *(float *)(__fr + 0x04);
  float &local_8cf = *(float *)(__fr + 0x0c);
  float &local_88f = *(float *)(__fr + 0x10);
  // Ghidra ghost stack-slot variables (used as byte* for 4-byte comparisons)
  undefined4 stack0xffffff60 = 0;
  undefined4 stack0xffffff64 = 0;
  undefined4 stack0xffffff5c = 0;

  void* result = nullptr;   // valor de retorno de los handlers
  pcVar14 = reinterpret_cast<char*>(Joints);
  const uintptr_t __jp_end = (uintptr_t)Joints + sizeof(Joints);
  while (*pcVar14 != '\0') {
    pcVar14 = pcVar14 + JOINT_STRIDE;
    if ((uintptr_t)pcVar14 >= __jp_end) {
      return nullptr;
    }
  }
  RenderFxSnapshots_New(pcVar14);
  *pcVar14 = '\x01';
  AsJoint(pcVar14)->Type = param_1;
  *(undefined4 *)&AsJoint(pcVar14)->SubType = param_5;
  *(char *)&AsJoint(pcVar14)->Collision = '\0';
  AsJoint(pcVar14)->PKKey = param_8;
  *(char *)&AsJoint(pcVar14)->Skill = param_9;
  pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = '\0';
  // DESVIACION DOCUMENTADA (no esta en IDA 0x46D840).
  // +0x9C8 y +0x9CC son el avance por tick de la TargetPosition que aplica
  // LABEL_439 de MoveJoint (`TargetPos.x += o+2504`, `TargetPos.y += o+2508`),
  // o sea la velocidad del joint.  El binario limpia +0x9C0 aca pero NO estos
  // dos, asi que un subtipo que no los escriba en su propio case (p.ej. el
  // 1249/sub7, los disparos de Alquamos) heredaria los del joint anterior que
  // ocupo el slot.  Limpiarlos deja a cada joint con la velocidad que define su
  // propio case.
  *(int *)&AsJoint(pcVar14)->Direction[1] = 0;
  *(int *)&AsJoint(pcVar14)->Direction[2] = 0;
  // Target es un puntero: en x64 son 8 bytes, y poner en cero sólo los 4 de
  // 0x40..0x43 (lo que hace el binario) dejaba la mitad alta del joint que
  // usó el slot antes.  Con un joint sin target, MoveJoint lo tomaba como no
  // nulo y lo deferenciaba.
  AsJoint(pcVar14)->Target = nullptr;
  pfVar1 = AsJoint(pcVar14)->Position;
  pfVar2 = AsJoint(pcVar14)->Angle;
  *pfVar1 = *param_2;
  AsJoint(pcVar14)->Position[1] = param_2[1];
  AsJoint(pcVar14)->Position[2] = param_2[2];
  *pfVar2 = *param_4;
  AsJoint(pcVar14)->Angle[1] = param_4[1];
  AsJoint(pcVar14)->Angle[2] = param_4[2];
  pfVar3 = AsJoint(pcVar14)->Light;
  *pfVar3 = 1.0;
  pcVar14[0x38] = '\0';
  pcVar14[0x39] = '\0';
  pcVar14[0x3a] = -0x80;
  pcVar14[0x3b] = '?';
  pcVar14[0x3c] = '\0';
  pcVar14[0x3d] = '\0';
  pcVar14[0x3e] = -0x80;
  pcVar14[0x3f] = '?';
  if (param_6 == 0) {
    AsJoint(pcVar14)->TargetPosition[0] = *param_3;
    AsJoint(pcVar14)->TargetPosition[1] = param_3[1];
    AsJoint(pcVar14)->TargetPosition[2] = param_3[2];
  }
  else {
    if ((param_1 == 0x10a) && (AsJoint(pcVar14)->SubType == 2)) {
      AsJoint(pcVar14)->TargetPosition[0] = *param_3;
      AsJoint(pcVar14)->TargetPosition[1] = param_3[1];
      AsJoint(pcVar14)->TargetPosition[2] = param_3[2];
    }
    AsJoint(pcVar14)->Target = (OBJECT*)param_6;
  }
  pcVar14[0x50 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x51 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x52 + JOINT_FIELD_SHIFT] = '\0';
  pcVar14[0x53 + JOINT_FIELD_SHIFT] = '\0';
  Matrix_BuildFromEuler(pfVar2,local_6c);
  local_90 = AsJoint(pcVar14)->Scale * (-0.5f);
  local_8c = (byte*)0x0;
  local_88 = (byte*)0x0;
  Vector_Rotate(&local_90,local_6c,&local_78);
  AsJoint(pcVar14)->Tails[0][0][0] = local_78 + *pfVar1;
  local_8c = (byte*)0x0;
  local_88 = (byte*)0x0;
  AsJoint(pcVar14)->Tails[0][0][1] = local_74 + AsJoint(pcVar14)->Position[1];
  AsJoint(pcVar14)->Tails[0][0][2] = local_70 + AsJoint(pcVar14)->Position[2];
  local_90 = AsJoint(pcVar14)->Scale * 0.5f;
  Vector_Rotate(&local_90,local_6c,&local_78);
  AsJoint(pcVar14)->Tails[0][1][0] = local_78 + *pfVar1;
  local_90 = 0.0f;
  local_8c = (byte*)0x0;
  AsJoint(pcVar14)->Tails[0][1][1] = local_74 + AsJoint(pcVar14)->Position[1];
  AsJoint(pcVar14)->Tails[0][1][2] = local_70 + AsJoint(pcVar14)->Position[2];
  local_88f = AsJoint(pcVar14)->Scale * (-0.5f);
  Vector_Rotate(&local_90,local_6c,&local_78);
  AsJoint(pcVar14)->Tails[0][2][0] = local_78 + *pfVar1;
  local_90 = 0.0f;
  local_8c = (byte*)0x0;
  AsJoint(pcVar14)->Tails[0][2][1] = local_74 + AsJoint(pcVar14)->Position[1];
  AsJoint(pcVar14)->Tails[0][2][2] = local_70 + AsJoint(pcVar14)->Position[2];
  local_88f = AsJoint(pcVar14)->Scale * 0.5f;
  Vector_Rotate(&local_90,local_6c,&local_78);
  AsJoint(pcVar14)->Tails[0][3][0] = local_78 + *pfVar1;
  AsJoint(pcVar14)->Tails[0][3][1] = local_74 + AsJoint(pcVar14)->Position[1];
  AsJoint(pcVar14)->Tails[0][3][2] = local_70 + AsJoint(pcVar14)->Position[2];
  pbVar10 = (byte*)CharacterMachine;
  // 0046D840, Type 1253 / SubType 0 (Evil Spirit): this is the joint
  // initialisation that feeds the per-frame 00470030 branch.  Without these
  // exact counters the slot is allocated but expires as an unconfigured beam.
  if (param_1 == 1253 && AsJoint(pcVar14)->SubType == 0) {
    *(char *)&AsJoint(pcVar14)->Weapon = pbVar10 ? pbVar10[1408] : 0; // CharacterMachine+1408
    AsJoint(pcVar14)->Velocity = 70.0f;
    AsJoint(pcVar14)->LifeTime = 49;
    AsJoint(pcVar14)->Scale = param_7;
    AsJoint(pcVar14)->MaxTails = 6;
  }
  // 0x4E1 y 0x48E comparten el cuerpo de BITMAP_FLARE_BLUE y BITMAP_JOINT_LASER + 1
  // (IDA salta a esos cases desde esta cadena): los atiende el switch de abajo.
  // Cadena de tipos de IDA, en forma de switch (cada case termina CreateJoint).
  if (param_1 < 0x4e9 && param_1 != 0x4e1 && param_1 != 0x48e) {
    switch (param_1) {
    case 0x4e8:
      if (CreateJoint_Type1256(fVar24, pcVar14, result)) return result;
      break;
    case 0x498:
      if (CreateJoint_Type1176(iVar6, local_94, pcVar14, pfVar1, pfVar3, result)) return result;
      break;
    case 0x10a:
      if (CreateJoint_Type266(param_3, param_7, iVar6, local_94, pcVar14, pfVar2, pfVar3, result)) return result;
      break;
    case 0x4e2:
      if (CreateJoint_Type1250(param_3, param_7, param_8, fVar24, iVar6, local_6c, pcVar14, pfVar1, pfVar2, pfVar3, result)) return result;
      break;
    case 0x4e5:
      if (CreateJoint_JointSpirit(param_3, param_7, iVar6, local_6c, local_88, local_88f, local_8c, local_8cf, local_90, pcVar14, pfVar1, pfVar2, pfVar3, result)) return result;
      break;
    case 0x4e7:
      pcVar14[0x54 + JOINT_FIELD_SHIFT] = '2';
      pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
      AsJoint(pcVar14)->Scale = param_7;
      iVar6 = _rand();
      pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x14';
      pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
      AsJoint(pcVar14)->Velocity = (float)(iVar6 % 0x14) + 80.0f;
      AsJoint(pcVar14)->Light[2] = (float)AsJoint(pcVar14)->MaxTails;
      CreateJoint_SaveStart(pcVar14, pfVar1); return CreateJoint_Finish(pcVar14);
    case 0x4e6:
      pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = 'H';
      pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
      AsJoint(pcVar14)->Scale = param_7;
      pcVar14[0x54 + JOINT_FIELD_SHIFT] = '2';
      pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
      pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
      switch(*(undefined4 *)&AsJoint(pcVar14)->SubType) {
      case 1:
        iVar6 = _rand();
        local_94 = (byte *)(iVar6 % 10);
        AsJoint(pcVar14)->Velocity = (float)(intptr_t)local_94 + 20.0f;
        uVar9 = _rand();
        uVar9 = uVar9 & 0x80000007;
        if ((int)uVar9 < 0) {
          uVar9 = (uVar9 - 1 | 0xfffffff8) + 1;
        }
        *(uint *)&AsJoint(pcVar14)->LifeTime = uVar9 + 8;
        break;
      case 2:
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x02';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        *pfVar3 = 1.0;
        pcVar14[0x38] = -0x33;
        pcVar14[0x39] = -0x34;
        pcVar14[0x3a] = -0x34;
        pcVar14[0x3b] = '=';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = '\0';
        pcVar14[0x3f] = '\0';
        local_3c[0] = 0.0;
        local_3c[1] = -150.0;
        local_3c[2] = 0.0;
        // DAT_05828d58 + 0x11e68 = dirección absoluta 0x0583AB40 (artefacto Ghidra).
        // TransformPosition con base bogus; omitimos hasta resolver bone matrix real.
        // BMD_TransformPosition((void *)(DAT_05828d58 + 0x11e68),
        //              (float *)(*(int *)((intptr_t)AsJoint(pcVar14)->Target + 0x114) + 0x630),local_3c,
        //              AsJoint(pcVar14)->TargetPosition,'\x01');
        break;
      case 3:
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
        AsJoint(pcVar14)->LifeTime = (int)param_8;
        *(uint *)&AsJoint(pcVar14)->MaxTails = (uint)param_9;
        *pfVar3 = 0.5;
        pcVar14[0x38] = '\0';
        pcVar14[0x39] = '\0';
        pcVar14[0x3a] = '\0';
        pcVar14[0x3b] = '?';
        AsJoint(pcVar14)->Velocity = (float)param_9;
        pcVar14[0x9d0 + JOINT_FIELD_SHIFT] = -1;
        pcVar14[0x9d1 + JOINT_FIELD_SHIFT] = -1;
        CreateJoint_SaveStart(pcVar14, pfVar1); return CreateJoint_Finish(pcVar14);
      case 4:
      case 5:
        pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = 'p';
        pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = 'B';
        pcVar14[0x9c4 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c5 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c6 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c7 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c8 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ca + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9cb + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9cc + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9cd + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ce + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9cf + JOINT_FIELD_SHIFT] = '\0';
        *pfVar3 = 0.5;
        pcVar14[0x38] = '\0';
        pcVar14[0x39] = '\0';
        pcVar14[0x3a] = '\0';
        pcVar14[0x3b] = '?';
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '2';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
        pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
        AsJoint(pcVar14)->TargetPosition[0] = *param_3;
        AsJoint(pcVar14)->TargetPosition[1] = param_3[1];
        AsJoint(pcVar14)->TargetPosition[2] = param_3[2];
        AsJoint(pcVar14)->StartPosition[0] = *pfVar1;
        *(undefined4 *)&AsJoint(pcVar14)->StartPosition[1] = *(undefined4 *)&AsJoint(pcVar14)->Position[1];
        *(undefined4 *)&AsJoint(pcVar14)->StartPosition[2] = *(undefined4 *)&AsJoint(pcVar14)->Position[2];
        break;
      case 6:
        iVar6 = _rand();
        AsJoint(pcVar14)->LifeTime = iVar6 % 0x14 + 6;
        iVar6 = _rand();
        local_94 = (byte *)(iVar6 % 10);
        AsJoint(pcVar14)->Velocity = (float)(intptr_t)local_94 + 15.0f;
        iVar6 = _rand();
        fVar24 = (float)(iVar6 % 10) * 0.06666667f + 0.1f;
        *pfVar3 = fVar24;
        AsJoint(pcVar14)->Light[1] = fVar24;
        AsJoint(pcVar14)->Light[2] = fVar24;
        CreateJoint_SaveStart(pcVar14, pfVar1); return CreateJoint_Finish(pcVar14);
      case 7:
        pcVar14[0x9c0 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c1 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9c2 + JOINT_FIELD_SHIFT] = -0x60;
        pcVar14[0x9c3 + JOINT_FIELD_SHIFT] = '@';
        AsJoint(pcVar14)->StartPosition[0] = *pfVar1;
        *pfVar3 = 0.5;
        pcVar14[0x38] = '\0';
        pcVar14[0x39] = '\0';
        pcVar14[0x3a] = '\0';
        pcVar14[0x3b] = '?';
        pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x03';
        pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x01';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
        *(undefined4 *)&AsJoint(pcVar14)->StartPosition[1] = *(undefined4 *)&AsJoint(pcVar14)->Position[1];
        *(undefined4 *)&AsJoint(pcVar14)->StartPosition[2] = *(undefined4 *)&AsJoint(pcVar14)->Position[2];
        MoveHumming(pfVar1,pfVar2,AsJoint(pcVar14)->TargetPosition,360.0);
        break;
      case 8:
        pcVar14[0xc] = '\0';
        pcVar14[0xd] = '\0';
        pcVar14[0xe] = -0x60;
        pcVar14[0xf] = '@';
        iVar6 = _rand();
        pbVar10 = (byte *)(iVar6 % 10 + 6);
        local_94 = pbVar10;
        // Misma cola que BITMAP_JOINT_SPARK SubType 0 (antes un goto a ese case).
        AsJoint(pcVar14)->Velocity = (float)(intptr_t)pbVar10;
        uVar9 = _rand();
        uVar9 = uVar9 & 0x80000007;
        if ((int)uVar9 < 0) {
          uVar9 = (uVar9 - 1 | 0xfffffff8) + 1;
        }
        pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\x02';
        pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
        *(uint *)&AsJoint(pcVar14)->LifeTime = uVar9 + 8;
        *pfVar3 = 1.0;
        pcVar14[0x38] = '\0';
        pcVar14[0x39] = '\0';
        pcVar14[0x3a] = -0x80;
        pcVar14[0x3b] = '?';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
        return CreateJoint_Finish(pcVar14);
      case 9:
        iVar6 = _rand();
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x1e';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        local_94 = (byte *)(iVar6 % 0x14);
        AsJoint(pcVar14)->Velocity = (float)(intptr_t)local_94 + 80.0f;
        iVar6 = _rand();
        fVar24 = (float)(iVar6 % 10) * 0.06666667f + 0.1f;
        *pfVar3 = fVar24;
        AsJoint(pcVar14)->Light[1] = fVar24;
        AsJoint(pcVar14)->Light[2] = fVar24;
        CreateJoint_SaveStart(pcVar14, pfVar1);
        break;
      case 10:
        *pfVar2 = 0.0;
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x2c] = '\0';

        pcVar14[0x2d] = '\0';
        pcVar14[0x2e] = '\0';
        pcVar14[0x2f] = '\0';
        pcVar14[0x30] = '\0';
        pcVar14[0x31] = '\0';
        pcVar14[0x32] = '\0';
        pcVar14[0x33] = '\0';
        AsJoint(pcVar14)->Scale = param_7;
        *pfVar3 = 0.3;
        pcVar14[0x38] = -0x66;
        pcVar14[0x39] = -0x67;
        pcVar14[0x3a] = -0x67;
        pcVar14[0x3b] = '>';
        pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
        pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
        AsJoint(pcVar14)->StartPosition[0] = (*param_3 - *pfVar1) * 0.1f;
        AsJoint(pcVar14)->StartPosition[1] = (param_3[1] - AsJoint(pcVar14)->Position[1]) * 0.1f;
        local_90 = AsJoint(pcVar14)->StartPosition[0];
        local_8cf = AsJoint(pcVar14)->StartPosition[1];
        local_88f = (param_3[2] - AsJoint(pcVar14)->Position[2]) * 0.1f;
        AsJoint(pcVar14)->StartPosition[2] = local_88f;
        Matrix_BuildFromEuler(pfVar2,local_6c);
        iVar6 = 0;
        if (AsJoint(pcVar14)->MaxTails != 1 && -1 < AsJoint(pcVar14)->MaxTails + -1) {
          do {
            if ((intptr_t)AsJoint(pcVar14)->Target == 0) {
              uVar9 = _rand();
              local_94 = (byte *)(uVar9 & 0x80000007);
              if ((intptr_t)local_94 < 0) {
                local_94 = (byte *)(((intptr_t)local_94 - 1U | 0xfffffff8) + 1);
              }
              local_90 = ((float)(int)(intptr_t)local_94 + AsJoint(pcVar14)->StartPosition[0]) - 4.0f;
              uVar9 = _rand();
              local_94 = (byte *)(uVar9 & 0x80000007);
              if ((intptr_t)local_94 < 0) {
                local_94 = (byte *)(intptr_t)(int)(((uint)(uintptr_t)(local_94 + -1) | 0xfffffff8) + 1);
              }
              local_8cf = (local_94f + AsJoint(pcVar14)->StartPosition[1]) - 4.0f;
            }
            else {
              uVar9 = _rand();
              local_94 = (byte *)(uVar9 & 0x8000000f);
              if ((intptr_t)local_94 < 0) {
                local_94 = (byte *)(((intptr_t)local_94 - 1U | 0xfffffff0) + 1);
              }
              local_90 = ((float)(int)(intptr_t)local_94 + AsJoint(pcVar14)->StartPosition[0]) - 8.0f;
              uVar9 = _rand();
              local_94 = (byte *)(uVar9 & 0x8000000f);
              if ((intptr_t)local_94 < 0) {
                local_94 = (byte *)(intptr_t)(int)(((uint)(uintptr_t)(local_94 + -1) | 0xfffffff0) + 1);
              }
              local_8cf = (local_94f + AsJoint(pcVar14)->StartPosition[1]) - 8.0f;
            }
            *pfVar1 = local_90 + *pfVar1;
            AsJoint(pcVar14)->Position[1] = local_8cf + AsJoint(pcVar14)->Position[1];
            AsJoint(pcVar14)->Position[2] = local_88f + AsJoint(pcVar14)->Position[2];
            Joint_SegmentTick((intptr_t)pcVar14,local_6c);
            iVar6 = iVar6 + 1;
            *pfVar1 = (*pfVar1 - local_90) + AsJoint(pcVar14)->StartPosition[0];
            AsJoint(pcVar14)->Position[1] =
                 (AsJoint(pcVar14)->Position[1] - local_8cf) + AsJoint(pcVar14)->StartPosition[1];
          } while (iVar6 < AsJoint(pcVar14)->MaxTails + -1);
        }
        CreateJoint_ResetPosition(pcVar14); return CreateJoint_Finish(pcVar14);
      case 0xb:
        *pfVar3 = 1.0;
        pcVar14[0x38] = '\0';
        pcVar14[0x39] = '\0';
        pcVar14[0x3a] = '\0';
        pcVar14[0x3b] = '?';
        pcVar14[0x3c] = -0x33;
        pcVar14[0x3d] = -0x34;
        pcVar14[0x3e] = -0x34;
        pcVar14[0x3f] = '=';
      case 0:
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x02';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        break;
      case 0xc:
        *pfVar3 = 1.0;
        pcVar14[0x38] = -0x33;
        pcVar14[0x39] = -0x34;
        pcVar14[0x3a] = -0x34;
        pcVar14[0x3b] = '=';
        pcVar14[0x3c] = -0x33;
        pcVar14[0x3d] = -0x34;
        pcVar14[0x3e] = -0x34;
        pcVar14[0x3f] = '=';
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\n';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        break;
      case 0xd:
        *pfVar3 = 1.0;
        pcVar14[0x38] = -0x33;
        pcVar14[0x39] = -0x34;
        pcVar14[0x3a] = -0x34;
        pcVar14[0x3b] = '=';
        pcVar14[0x3c] = -0x33;
        pcVar14[0x3d] = -0x34;
        pcVar14[0x3e] = -0x34;
        pcVar14[0x3f] = '=';
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\x02';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[8] = '\v';
        pcVar14[9] = '\0';
        pcVar14[10] = '\0';
        pcVar14[0xb] = '\0';
        break;
      case 0xe:
        *pfVar2 = 0.0;
        pcVar14[0x9b8 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9b9 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9ba + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x9bb + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x2c] = '\0';
        pcVar14[0x2d] = '\0';
        pcVar14[0x2e] = '\0';
        pcVar14[0x2f] = '\0';
        pcVar14[0x30] = '\0';
        pcVar14[0x31] = '\0';
        pcVar14[0x32] = '\0';
        pcVar14[0x33] = '\0';
        AsJoint(pcVar14)->Scale = param_7;
        *pfVar3 = 0.3;
        pcVar14[0x38] = -0x66;
        pcVar14[0x39] = -0x67;
        pcVar14[0x3a] = -0x67;
        pcVar14[0x3b] = '>';
        pcVar14[0x54 + JOINT_FIELD_SHIFT] = '\n';
        pcVar14[0x55 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x56 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x57 + JOINT_FIELD_SHIFT] = '\0';
        pcVar14[0x3c] = '\0';
        pcVar14[0x3d] = '\0';
        pcVar14[0x3e] = -0x80;
        pcVar14[0x3f] = '?';
        AsJoint(pcVar14)->StartPosition[0] = (*param_3 - *pfVar1) * 0.1f;
        local_90 = AsJoint(pcVar14)->StartPosition[0];
        AsJoint(pcVar14)->StartPosition[1] = (param_3[1] - AsJoint(pcVar14)->Position[1]) * 0.1f;
        local_8cf = AsJoint(pcVar14)->StartPosition[1];
        local_88f = (param_3[2] - AsJoint(pcVar14)->Position[2]) * 0.1f;
        AsJoint(pcVar14)->StartPosition[2] = local_88f;
        Matrix_BuildFromEuler(pfVar2,local_6c);
        iVar6 = 0;
        if (AsJoint(pcVar14)->MaxTails != 1 && -1 < AsJoint(pcVar14)->MaxTails + -1) {
          do {
            if ((intptr_t)AsJoint(pcVar14)->Target == 0) {
              local_90 = AsJoint(pcVar14)->StartPosition[0];
              local_8cf = AsJoint(pcVar14)->StartPosition[1];
            }
            else {
              local_90 = AsJoint(pcVar14)->StartPosition[0];
              local_8cf = AsJoint(pcVar14)->StartPosition[1];
            }
            *pfVar1 = local_90 + *pfVar1;
            AsJoint(pcVar14)->Position[1] = local_8cf + AsJoint(pcVar14)->Position[1];
            AsJoint(pcVar14)->Position[2] = local_88f + AsJoint(pcVar14)->Position[2];
            Joint_SegmentTick((intptr_t)pcVar14,local_6c);
            iVar6 = iVar6 + 1;
            *pfVar1 = (*pfVar1 - local_90) + AsJoint(pcVar14)->StartPosition[0];
            AsJoint(pcVar14)->Position[1] =
                 (AsJoint(pcVar14)->Position[1] - local_8cf) + AsJoint(pcVar14)->StartPosition[1];
          } while (iVar6 < AsJoint(pcVar14)->MaxTails + -1);
        }
        CreateJoint_ResetPosition(pcVar14);
      }
      return CreateJoint_Finish(pcVar14);
    default:
      return CreateJoint_Finish(pcVar14);
    }
  }
  switch(param_1) {
  case BITMAP_JOINT_SPARK:
    if (CreateJoint_JointSpark(iVar6, local_98, pbVar10, pcVar14, pfVar3, uVar9, result)) return result;
    break;
  case BITMAP_JOINT_ENERGY:
    if (CreateJoint_JointEnergy(param_7, iVar6, pcVar14, pfVar1, pfVar3, uVar23, result)) return result;
    break;
  case BITMAP_JOINT_HEALING:
    if (CreateJoint_JointHealing(param_7, iVar6, local_98, pcVar14, pfVar3, uVar9, result)) return result;
    break;
  case 0x4ec:
    if (CreateJoint_JointLaser(param_7, pcVar14, result)) return result;
    break;
  case 0x48e:
  case BITMAP_JOINT_LASER + 1:
    if (CreateJoint_MotionBlur(pcVar14, pfVar3, result)) return result;
    break;
  case BITMAP_MOTION_BLUR_R2:
    if (CreateJoint_MotionBlurR2(param_7, fVar24, local_6c, local_70, local_74, local_78, local_88, local_8c, local_90, pcVar14, pfVar1, pfVar2, result)) return result;
    break;
  default:
    return CreateJoint_Finish(pcVar14);
  case 0x4e1:
  case BITMAP_FLARE_BLUE:
    if (CreateJoint_Flare(param_3, param_7, fVar20, fVar21, fVar24, iVar6, local_3c, local_6c, local_70, local_74, local_78, local_88, local_8c, local_90, local_98, pcVar14, pfVar1, pfVar2, pfVar22, pfVar3, uVar23, uVar25, uVar9, result)) return result;
    break;
  }
  *pfVar3 = 1.0;
  pcVar14[0x38] = '\0';
  pcVar14[0x39] = '\0';
  pcVar14[0x3a] = -0x80;
  pcVar14[0x3b] = '?';
  pcVar14[0x3c] = '\0';
  pcVar14[0x3d] = '\0';
  pcVar14[0x3e] = -0x80;
  pcVar14[0x3f] = '?';
  return CreateJoint_Finish(pcVar14);
}

// Puente de compatibilidad IDA: conserva el nombre ABI CreateJoint y delega en Joint_Create.
void* __cdecl CreateJoint(int type, float* p1, float* p2, float* p3, unsigned int subType,
                            intptr_t owner, float scale, short link, unsigned char flag)
{
  return Joint_Create(type, p1, p2, p3, subType, owner, scale, link, flag);
}
