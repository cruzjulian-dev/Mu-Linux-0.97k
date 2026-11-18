#pragma once
#include <cstdint>

// Efectos ambientales por modelo (el switch por tipo de MoveCharacterVisual,
// IDA 0x004520C0 L764-1342).  Light, WorldPosition y Luminosity son el estado
// común que MoveCharacterVisual arma antes del switch (IDA L756-763).
void CharacterVisual_ModelEffects(intptr_t entity_ptr, void* model, float* Light,
                                  float* WorldPosition, float Luminosity);
