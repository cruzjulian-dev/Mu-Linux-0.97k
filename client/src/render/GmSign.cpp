// Signo sobre la cabeza de los GameMasters.
//
// Port del 5.2 (CSParts.cpp, ZzzObject.cpp): CreatePartsFactory arma un
// CSParts(MODEL_WEBZEN_MARK, hueso 20, offset (70, -5, 0), ángulo (0, 0, 45))
// y CSParts::IRender lo pone sobre el hueso, avanza la animación a 0.5 y lo
// dibuja en dos pasadas: textura con luz (1, 1, 1) y brillo + cromo con
// (1, 0.5, 0).  El 0.97k no tiene nada de esto.
//
// Dos modelos según el nivel de GameMaster.txt: el del 5.2 para los
// administradores (nivel 2) y uno propio para los GM (nivel 1).  Los dos se
// llaman musign.bmd, así que cada uno va en su carpeta (Content/effect/model).

#include "stdafx.h"
#include "render/GmSign.h"
#include "assets/data/ContentCatalog.h"
#include "render/EquipmentVisuals.h"
#include "platform/ITimer.h"
#include "assets/model/ModelTypes.h"
#include "game/entity/EntityTypes.h"
#include "globals.h"
#include "functions.h"
#include <cstring>

namespace GmSign {
namespace {

constexpr int kSlots = 400;   // CharactersClient
constexpr int kHeadBone = 20;

struct State {
    WORD  key = 0xFFFF;   // Key del personaje al que pertenece el nivel
    BYTE  level = 0;
    BYTE  priorAction = 0;
    float frame = 0.0f;
    float priorFrame = 0.0f;
    DWORD lastTick = 0;
};

State s_State[kSlots];
BYTE  s_HeroLevel = 0;

int SlotOf(const void* character)
{
    if (!CharactersClient || !character) return -1;
    const uintptr_t base = CharactersClient, at = (uintptr_t)character;
    if (at < base || (at - base) % CHARACTER_STRIDE) return -1;
    const uintptr_t slot = (at - base) / CHARACTER_STRIDE;
    return slot < (uintptr_t)kSlots ? (int)slot : -1;
}

// El modelo se carga fuera del dibujo (al llegar el nivel por red): el render
// restaura la tabla de modelos al terminar cada cuadro (RenderDerivedState), y
// una carga hecha ahí se perdería.
int s_Model[3] = { -2, -2, -2 };   // -2 = todavía no se intentó

int Model(int level)
{
    if (level < 1 || level > 2) return -1;
    if (s_Model[level] == -2)
        s_Model[level] = level == 2 ? gContentCatalog.LoadClientModel("Skill/AdminSign/", "musign")
                                    : gContentCatalog.LoadClientModel("Skill/GmSign/", "musign");
    return s_Model[level];
}

} // namespace

void SetLevel(const void* character, int level)
{
    const int slot = SlotOf(character);
    if (slot < 0) return;
    State& state = s_State[slot];
    const WORD key = static_cast<const CHARACTER*>(character)->Key;
    if (state.key != key || state.level != (BYTE)level) state = State{};
    state.key = key;
    state.level = (BYTE)(level < 0 ? 0 : level > 2 ? 2 : level);
    Model(state.level);
}

void SetHeroLevel(int level)
{
    s_HeroLevel = (BYTE)(level < 0 ? 0 : level > 2 ? 2 : level);
    Model(s_HeroLevel);
}

void Render(void* character)
{
    // Sólo dentro del mundo: el nivel llega con el viewport, y en la selección
    // de personaje Hero apunta a un slot de la lista con el nivel de la sesión
    // anterior.
    if (SceneFlag != 5) return;
    const int slot = SlotOf(character);
    if (slot < 0) return;
    State& state = s_State[slot];
    CHARACTER& c = *AsCharacter(character);
    if (character == (void*)(uintptr_t)Hero) {
        if (state.key != c.Key || state.level != s_HeroLevel) state = State{};
        state.key = c.Key;
        state.level = s_HeroLevel;
    }
    if (!state.level || state.key != c.Key) return;
    OBJECT& owner = c.Object;
    if (owner.Type != 390 || owner.Alpha < 0.01f || !owner.BoneMatrices || !Models) return;
    void* ownerModel = (void*)(Models + owner.Type * BMD_STRIDE);
    if (kHeadBone >= AsModel(ownerModel)->NumBones) return;
    const int model = s_Model[state.level];
    if (model < 0 || AsModel((void*)(Models + model * BMD_STRIDE))->NumMeshs <= 0) return;

    // CSParts::IRender: posición sobre el hueso, con la pose que se acaba de
    // dibujar.  El contexto del BMD del jugador es compartido: se fija el del
    // dueño y se restaura al salir.
    float offset[3] = { 70.0f, -5.0f, 0.0f };
    alignas(16) unsigned char buffer[sizeof(OBJECT)] = {};
    OBJECT& o = *reinterpret_cast<OBJECT*>(buffer);
    {
        EquipmentModelContext savedOwner(ownerModel);
        AsModel(ownerModel)->BodyScale = owner.Scale;
        memcpy(AsModel(ownerModel)->BodyOrigin, owner.Position, sizeof(owner.Position));
        BMD_TransformPosition(ownerModel, (float*)owner.BoneMatrices + 12 * kHeadBone, offset, o.Position, 1);
    }

    // CSParts::CSParts.
    o.Type = (short)model;
    o.Live = 1;
    o.LightEnable = 1;
    o.HiddenMesh = -1;
    o.BlendMesh = -1;
    o.BlendMeshLight = 1.0f;
    o.Scale = 1.0f;
    o.Alpha = 1.0f;
    o.AlphaTarget = 1.0f;
    o.Light[0] = o.Light[1] = o.Light[2] = 1.0f;
    o.Angle[2] = 45.0f;
    o.Velocity = 0.5f;

    void* signModel = (void*)(Models + model * BMD_STRIDE);
    EquipmentModelContext savedSign(signModel);
    BMD* b = AsModel(signModel);
    b->CurrentAction = 0;
    // El 5.2 avanza 0.5 cuadros por frame a 25 fps; acá se dibuja más seguido,
    // así que el avance va por tiempo (40 ms = un frame del original).
    const DWORD now = Platform::GetTimer().Ticks();
    DWORD elapsed = state.lastTick ? now - state.lastTick : 40;
    if (elapsed > 200) elapsed = 200;
    state.lastTick = now;
    if (elapsed)
        BMD__PlayAnimation(b, &state.frame, &state.priorFrame, &state.priorAction, o.Velocity * (float)elapsed / 40.0f);
    o.AnimationFrame = state.frame;
    o.PriorAnimationFrame = state.priorFrame;
    o.PriorAction = state.priorAction;

    // RenderObject(o, true) con el caso MODEL_WEBZEN_MARK de ZzzObject.cpp.
    if (!Calc_RenderObject(&o, 1, 0)) return;
    b->BodyLight[0] = 1.0f; b->BodyLight[1] = 1.0f; b->BodyLight[2] = 1.0f;
    BMD__RenderBody(b, 0x02, o.Alpha, o.BlendMesh, o.BlendMeshLight,
                    o.BlendMeshTexCoordU, o.BlendMeshTexCoordV, o.HiddenMesh, -1);
    b->BodyLight[0] = 1.0f; b->BodyLight[1] = 0.5f; b->BodyLight[2] = 0.0f;
    BMD__RenderBody(b, 0x40 | 0x04, o.Alpha, o.BlendMesh, o.BlendMeshLight,
                    o.BlendMeshTexCoordU, o.BlendMeshTexCoordV, o.HiddenMesh, -1);
}

} // namespace GmSign
