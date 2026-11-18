#pragma once
#include <cstdint>

// Comportamiento por mapa y tipo de un objeto del mundo (la segunda mitad de
// MoveObjects, IDA 0x004FDC00 desde LABEL_38).  `o` es la dirección del OBJECT.
void WorldObject_PerMapTick(std::uintptr_t o);

struct OBJECT;

// ── Objetos interactuables del mundo (sillas, lugares para apoyarse, orbes) ──
//
// Una sola tabla para las dos mitades del binario que los nombran:
//   * CreateObject (0x004FF5A0) los registra en la lista Operates (sub_4FF580)
//     para que el mouse los resalte;
//   * Action (0x0048D640, MOVEMENT_OPERATE) decide la pose al llegar.
enum class OperatePose : unsigned char { Sit, Pose, Healing };

struct OperableObject {
    short Map;
    short Type;
    OperatePose Pose;
    bool Facing;     // el héroe toma el ángulo del objeto (flt_7E118E4)
    bool BigBox;     // BoundingBoxMax = (40, 40, 160) al crearlo
    bool HideMesh;   // HiddenMesh = -2 al crearlo
};

const OperableObject* WorldObject_FindOperable(int map, int type);

// Valores iniciales por mapa y tipo de CreateObject (el switch por mapa de
// 0x004FF5A0); registra en Operates a los objetos interactuables.
void WorldObject_ApplyMapDefaults(OBJECT& object, void* node, int map, int type);
