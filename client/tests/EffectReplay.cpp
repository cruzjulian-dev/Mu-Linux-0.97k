// Repetición de CreateEffect + MoveEffect (T9a).
//
// Enlaza Effect_Create.cpp, MoveEffect.cpp y globals.cpp reales con estos
// stubs.  Cada servicio que llaman los efectos (joints, partículas, sprites,
// sonido, terreno, BMD, matemática) registra sus argumentos en el hash y
// responde de forma determinista según esos argumentos, así que pasar mal un
// argumento al desarmar los switches cambia el resultado.
//
// El driver recorre cada tipo x subtipo x owner con rand fijo: crea el efecto,
// lo mueve unos ticks y hashea los slots que quedaron vivos.  Los punteros se
// hashean por la región a la que apuntan (pool, owner, scratch), no por su
// dirección, para que dos builds distintos den el mismo hash.
//
// No certifica que los efectos se vean bien: certifica que dos revisiones del
// código hacen exactamente lo mismo.

#include "stdafx.h"
#include "functions.h"
#include "globals.h"
#include "game/core/MapManager.h"
#include "game/core/RenderDerivedState.h"
#include "game/core/RenderFxSnapshots.h"
#include "render/Particle_Render.h"
#include "render/ResourceTables.h"
#include "assets/model/ModelTypes.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

std::uint64_t g_hash = 14695981039346656037ull;
unsigned g_calls = 0;

void Mix(const void* p, std::size_t n)
{
    const unsigned char* b = static_cast<const unsigned char*>(p);
    while (n--) { g_hash ^= *b++; g_hash *= 1099511628211ull; }
}
void MixU(std::uint64_t v) { Mix(&v, sizeof(v)); }
void MixF(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); MixU(u); }

// Objetos de prueba: dos personajes (héroe y otro) y un scratch para lo que
// devuelven los servicios.
alignas(16) unsigned char g_owners[2][sizeof(CHARACTER)];
alignas(16) unsigned char g_scratch[0x4000];
alignas(16) unsigned char g_bones[200 * 48];
alignas(16) unsigned char g_textures[0x40000];
// Tabla de modelos en ceros: en el juego la aloca el cargador; sin ella los
// efectos con modelo leen Models + tipo*BMD_STRIDE sobre una base nula.
alignas(16) unsigned char g_models[4096 * BMD_STRIDE];
// CharacterMachine: los efectos leen el serial de proyectiles (+1408) y la cola
// de CreateEffect lo incrementa.  Con el puntero nulo esos casos crasheaban.
alignas(16) unsigned char g_charMachine[0x800];

// Puntero -> identificador estable entre builds.
std::uint64_t Region(const void* p)
{
    const auto v = reinterpret_cast<std::uintptr_t>(p);
    auto in = [&](const void* base, std::size_t size, std::uint64_t tag) -> std::uint64_t {
        const auto b = reinterpret_cast<std::uintptr_t>(base);
        return (v >= b && v < b + size) ? (tag << 32) | (v - b) : 0;
    };
    if (!v) return 0;
    if (auto r = in(Effects, sizeof(Effects), 1)) return r;
    if (auto r = in(g_owners, sizeof(g_owners), 2)) return r;
    if (auto r = in(g_scratch, sizeof(g_scratch), 3)) return r;
    if (auto r = in(g_bones, sizeof(g_bones), 4)) return r;
    if (auto r = in(g_textures, sizeof(g_textures), 5)) return r;
    if (auto r = in(g_models, sizeof(g_models), 6)) return r;
    if (auto r = in(g_charMachine, sizeof(g_charMachine), 7)) return r;
    return 0xFFull << 32;   // otra memoria (globales): la dirección cambia entre builds
}
void MixP(const void* p) { MixU(Region(p)); }
void MixV3(const float* v) { if (!v) { MixU(0); return; } MixP(v); for (int i = 0; i < 3; ++i) MixF(v[i]); }

void Call(const char* name)
{
    ++g_calls;
    Mix(name, std::strlen(name));
}

// Respuesta determinista a partir de los argumentos.
float Det(float a, float b) { return a * 0.375f - b * 0.125f + 7.0f; }

} // namespace

// ── Servicios que llaman los efectos ────────────────────────────────────────
bool __cdecl BMD__PlayAnimation(void* model, float* rot, float* scale, void* extra, float speed)
{
    Call("PlayAnimation"); MixP(model); MixP(rot); MixP(scale); MixP(extra); MixF(speed);
    if (rot) *rot += speed;
    return rot ? (*rot < 8.0f) : true;
}
float* __cdecl Entity_FindNearby_SendPacket(uint a, float* pos, float range, int b, short c)
{
    Call("FindNearby"); MixU(a); MixV3(pos); MixF(range); MixU((unsigned)b); MixU((unsigned short)c);
    return nullptr;
}
float* __cdecl Vector_Rotate(float* pt, float* mat12, float* out)
{
    Call("Vector_Rotate"); MixV3(pt); MixP(mat12);
    float r[3];
    for (int i = 0; i < 3; ++i) r[i] = (pt ? pt[i] : 0.0f) + (mat12 ? mat12[i * 4] * 0.5f + mat12[i * 4 + 3] : 0.0f);
    if (out) for (int i = 0; i < 3; ++i) out[i] = r[i];
    return out;
}
void __cdecl Matrix_BuildFromEuler(float* angles, float* out)
{
    Call("Matrix_BuildFromEuler"); MixV3(angles);
    if (out) for (int i = 0; i < 12; ++i) out[i] = (angles ? angles[i % 3] : 0.0f) * 0.01f + (float)i;
}
float __cdecl CreateAngle(float x1, float y1, float x2, float y2)
{
    Call("CreateAngle"); MixF(x1); MixF(y1); MixF(x2); MixF(y2);
    return Det(x2 - x1, y2 - y1);
}
float __cdecl MoveHumming(float* pos, float* angle, float* target, float turn)
{
    Call("MoveHumming"); MixV3(pos); MixV3(angle); MixV3(target); MixF(turn);
    if (angle) angle[2] += turn * 0.5f;
    return Det(pos ? pos[0] : 0.0f, target ? target[0] : 0.0f);
}
float __cdecl RequestTerrainHeight(float x, float y)
{
    Call("RequestTerrainHeight"); MixF(x); MixF(y);
    return Det(x, y);
}
int __cdecl CreateSprite(unsigned short type, float* pos, float scale, float* light, intptr_t owner, float rot, int sub)
{
    Call("CreateSprite"); MixU(type); MixV3(pos); MixF(scale); MixV3(light); MixP((void*)owner); MixF(rot); MixU((unsigned)sub);
    return 1;
}
int __cdecl Particle_Spawn(int type, float* pos, float* angle, float* light, int sub, float scale, intptr_t owner)
{
    Call("Particle_Spawn"); MixU((unsigned)type); MixV3(pos); MixV3(angle); MixV3(light); MixU((unsigned)sub); MixF(scale); MixP((void*)owner);
    return 1;
}
int __cdecl TERRAIN_INDEX(int x, int y) { Call("TERRAIN_INDEX"); MixU((unsigned)x); MixU((unsigned)y); return (y & 255) * 256 + (x & 255); }
int __cdecl Terrain_GetTileIndex(unsigned int x, unsigned int y) { Call("Terrain_GetTileIndex"); MixU(x); MixU(y); return (int)((y & 255) * 256 + (x & 255)); }
HRESULT __cdecl PlayBuffer(int buffer, const OBJECT* object, BOOL looped)
{
    Call("PlayBuffer"); MixU((unsigned)buffer); MixP(object); MixU((unsigned)looped);
    return 0;
}
void* AntiTamper_HashNode(void) { return g_scratch + 0x3F00; }
void* __cdecl Joint_Create(int type, float* pos, float* target, float* angle, unsigned int sub, intptr_t owner, float scale, short key, unsigned char flag)
{
    Call("Joint_Create"); MixU((unsigned)type); MixV3(pos); MixV3(target); MixV3(angle); MixU(sub); MixP((void*)owner); MixF(scale);
    MixU((unsigned short)key); MixU(flag);
    return g_scratch;
}
void* __fastcall PhysicsManager_Construct(void* p) { return p; }
void* __cdecl STRUCT_ENCRYPT(void*, void* data) { return data; }
void* __cdecl SetAction(OBJECT* o, int action)
{
    Call("SetAction"); MixP(o); MixU((unsigned)action);
    if (o) o->CurrentAction = (BYTE)action;
    return o;
}
void __cdecl AddTerrainLight(float x, float y, float* light, int range, float* buffer)
{
    Call("AddTerrainLight"); MixF(x); MixF(y); MixV3(light); MixU((unsigned)range); MixP(buffer);
}
void __cdecl BMD_TransformPosition(void* model, float* bone, float* in, float* out, char flag)
{
    Call("BMD_TransformPosition"); MixP(model); MixP(bone); MixV3(in); MixU((unsigned char)flag);
    if (out) for (int i = 0; i < 3; ++i) out[i] = (in ? in[i] : 0.0f) + (float)(i + 1) * 3.0f;
}
void __cdecl CErrorReport_Write(void*, const char*, ...) {}
void __cdecl Effect_CollisionCheck(intptr_t o) { Call("Effect_CollisionCheck"); MixP((void*)o); }
void __cdecl Effect_SpawnLightningBurst(intptr_t o) { Call("Effect_SpawnLightningBurst"); MixP((void*)o); }
void __cdecl Effect_SpawnProximityHit(intptr_t o) { Call("Effect_SpawnProximityHit"); MixP((void*)o); }
void __cdecl Effect_SpawnSmokeBurst(float* pos, char kind) { Call("Effect_SpawnSmokeBurst"); MixV3(pos); MixU((unsigned char)kind); }
void __cdecl Effect_SpawnSmokeExplosion(undefined4* pos, char kind) { Call("Effect_SpawnSmokeExplosion"); MixP(pos); MixU((unsigned char)kind); }
void __cdecl HashTable_Insert(void*, void*, void*) {}
void __cdecl Joint_BoneOffsetApply(BYTE* o, int bone) { Call("Joint_BoneOffsetApply"); MixP(o); MixU((unsigned)bone); }
void Particle_InvalidateRender(const void* o) { Call("Particle_InvalidateRender"); MixP(o); }
void RenderDerivedState_Forget(const void* o) { Call("RenderDerivedState_Forget"); MixP(o); }
void RenderFxSnapshots_New(const void* o) { Call("RenderFxSnapshots_New"); MixP(o); }
void __cdecl Ring_ComputeOrbit(int i, float* out, float r)
{
    Call("Ring_ComputeOrbit"); MixU((unsigned)i); MixF(r);
    if (out) { out[0] = r * (float)(i % 7); out[1] = r * (float)(i % 5); out[2] = 0.0f; }
}
void __cdecl operator_delete(void*) {}
extern "C" void DbgLogPublic(const char*) {}
// globals.cpp toma su dirección; vive en HUD_Pass3.cpp, que no se enlaza.
extern "C" { BYTE OffsetInventoryItems[64 * 68] = {}; }
CMapManager::CMapManager() = default;
CMapManager gMapManager;
namespace ResourceTables {
std::size_t ModelCount() { return 4096; }
char* TextureBase() { return reinterpret_cast<char*>(g_textures); }
}

// ── Driver ──────────────────────────────────────────────────────────────────
namespace {

constexpr std::size_t kPointerFields[] = { 0xFC, 0x114, 0x184, 0x1B4, 0x1B8 };

FILE* g_dump = nullptr;   // EFFECT_REPLAY_DUMP=archivo: los bytes de cada slot, para diagnosticar

void HashSlot(int index)
{
    if (g_dump) { std::fwrite(&index, 4, 1, g_dump); std::fwrite((char *)&Effects[index], OBJECT_STRIDE, 1, g_dump); }
    const unsigned char* slot = reinterpret_cast<const unsigned char*>(&Effects[index]);
    MixU((unsigned)index);
    std::size_t pos = 0;
    for (std::size_t x86 : kPointerFields) {
        const std::size_t at = EntityOffset(x86);
        Mix(slot + pos, at - pos);
        void* p; std::memcpy(&p, slot + at, sizeof(p)); MixP(p);
        pos = at + sizeof(void*);
    }
    Mix(slot + pos, OBJECT_STRIDE - pos);
}

bool Active(int index) { return *reinterpret_cast<const char*>(&Effects[index]) != 0; }

void ClearPool()
{
    for (int i = 0; i < kEffectPoolSize; ++i)
        if (Active(i)) std::memset((char *)&Effects[i], 0, OBJECT_STRIDE);
}

void FillOwner(unsigned char* c, unsigned seed)
{
    // En ceros: los campos puntero (Owner, Prior/Next, los Cloth de cada
    // PART_t) tienen que quedar nulos, no con basura.
    std::memset(c, 0, sizeof(CHARACTER));
    CHARACTER* ch = AsCharacter(c);
    ch->Class = (BYTE)(seed * 3);
    ch->Key = (WORD)(100 + seed);
    for (int i = 0; i < 2; ++i) { ch->Weapon[i].Type = (short)(400 + i + seed); ch->Weapon[i].LinkBone = (BYTE)(10 + i); }
    ch->Object.Scale = 1.0f;
    ch->Object.Alpha = 1.0f;
    ch->Object.Live = 1;
    ch->Object.Type = (short)(390 + seed);
    for (int i = 0; i < 3; ++i) { ch->Object.Position[i] = 1000.0f + 100.0f * (float)i + (float)seed; ch->Object.Angle[i] = 30.0f * (float)i; }
    ch->Object.BoneMatrices = reinterpret_cast<vec34_t*>(g_bones);
    ch->Object.Owner = nullptr;
}

} // namespace

// ── Guarda por caso ─────────────────────────────────────────────────────────
// Cada tipo espera su propio owner (un personaje, otro efecto, un objeto del
// mapa) y el recorrido los prueba todos con los mismos dos personajes, así que
// algunas combinaciones leen campos que en el juego nunca están nulos.  Ese
// crash no es un bug del port: se registra en el hash y el recorrido sigue.
// Dos revisiones equivalentes crashean en los mismos casos.
#if !defined(_MSC_VER)
#include <csetjmp>
#include <csignal>
namespace { sigjmp_buf g_jump; void OnFault(int) { siglongjmp(g_jump, 1); } }
#endif

namespace {

struct CaseArgs { int type, sub, ownerMode, ticks; void* owner; };

// El decompile lee algunas locales sin inicializarlas.  Lo que encuentran en el
// stack depende de la ruta del binario y del entorno (sobre todo en Win32), asi
// que antes de cada llamada se pone en ceros la zona que van a ocupar: el
// resultado no cambia segun desde donde se corra.
#if defined(_MSC_VER)
__declspec(noinline)
#else
__attribute__((noinline))
#endif
void ScrubStack()
{
    volatile unsigned char buffer[64 * 1024];
    for (std::size_t i = 0; i < sizeof(buffer); ++i) buffer[i] = 0;
}

void RunCase(const CaseArgs& a)
{
    float pos[3] = { 1200.0f, 1300.0f, 40.0f }, angle[3] = { 0.0f, 0.0f, 90.0f }, light[3] = { 1.0f, 0.5f, 0.25f };
    ScrubStack();
    // El valor de retorno no se hashea: CreateEffect es void en el binario y
    // ningún caller lo usa; el port devuelve lo que quede en un registro.
    CreateEffect(a.type, pos, angle, light, (float*)(intptr_t)a.sub, (float*)a.owner, (float*)(intptr_t)-1, nullptr, 0);
    for (int t = 0; t < a.ticks; ++t)
        for (int i = 0; i < kEffectPoolSize; ++i)
            if (Active(i)) { MixU(0x7100u | (unsigned)t); ScrubStack(); MoveEffect(reinterpret_cast<float*>(&Effects[i]), i); }
}

bool RunGuarded(const CaseArgs& a)
{
#if defined(_MSC_VER)
    __try { RunCase(a); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
#else
    struct sigaction act = {}, oldSegv, oldBus;
    act.sa_handler = OnFault;
    sigemptyset(&act.sa_mask);
    sigaction(SIGSEGV, &act, &oldSegv);
    sigaction(SIGBUS, &act, &oldBus);
    bool ok = true;
    if (sigsetjmp(g_jump, 1) == 0) RunCase(a); else ok = false;
    sigaction(SIGSEGV, &oldSegv, nullptr);
    sigaction(SIGBUS, &oldBus, nullptr);
    return ok;
#endif
}

} // namespace

int main()
{
    Models = reinterpret_cast<std::uintptr_t>(g_models);
    for (std::size_t i = 0; i < sizeof(g_bones) / 4; ++i) reinterpret_cast<float*>(g_bones)[i] = (float)(i % 13) * 0.5f;
    const int maxType = 1400, maxSub = 16, ticks = 6;
    const bool trace = std::getenv("EFFECT_REPLAY_TRACE") != nullptr;
    unsigned cases = 0, crashes = 0;
    if (const char* dump = std::getenv("EFFECT_REPLAY_DUMP")) g_dump = std::fopen(dump, "wb");
    // EFFECT_REPLAY_CASES=archivo: el hash de cada caso por separado, para ubicar
    // en qué combinación difieren dos revisiones.
    FILE* perCase = nullptr;
    if (const char* path = std::getenv("EFFECT_REPLAY_CASES")) perCase = std::fopen(path, "w");
    for (int type = 0; type < maxType; ++type)
    for (int sub = 0; sub < maxSub; ++sub)
    for (int ownerMode = 0; ownerMode < 3; ++ownerMode) {
        FillOwner(g_owners[0], 0); FillOwner(g_owners[1], 1);
        Hero = reinterpret_cast<char*>(g_owners[0]);
        std::memset(g_charMachine, 0, sizeof(g_charMachine));
        g_charMachine[1408] = (unsigned char)(0x40 + type);   // serial de proyectiles
        CharacterMachine = g_charMachine;
        ClearPool();
        std::srand((unsigned)(type * 977 + sub * 31 + ownerMode));
        void* owner = ownerMode == 0 ? nullptr : g_owners[ownerMode - 1];
        // Cada caso se hashea aparte y se pliega al total: una diferencia queda
        // localizada en su caso en vez de arrastrarse a todos los siguientes.
        const std::uint64_t total = g_hash;
        g_hash = 14695981039346656037ull;
        MixU(0xC0DE0000u | (unsigned)type); MixU((unsigned)sub); MixU((unsigned)ownerMode);
        if (!RunGuarded({ type, sub, ownerMode, ticks, owner })) {
            ++crashes; MixU(0xDEAD0000u | (unsigned)type);
            if (trace) std::fprintf(stderr, "crash %d %d %d\n", type, sub, ownerMode);
        }
        if (g_dump) { int tag[4] = { -1, type, sub, ownerMode }; std::fwrite(tag, 4, 4, g_dump); }
        for (int i = 0; i < kEffectPoolSize; ++i) if (Active(i)) HashSlot(i);
        Mix(g_charMachine, sizeof(g_charMachine));
        const std::uint64_t caseHash = g_hash;
        if (perCase) std::fprintf(perCase, "%d %d %d %016llx\n", type, sub, ownerMode, (unsigned long long)caseHash);
        g_hash = total;
        Mix(&caseHash, sizeof(caseHash));
        ++cases;
    }
    ClearPool();
    if (perCase) std::fclose(perCase);
    std::printf("EffectReplay: %u casos, %u crashes de combinaciones invalidas, %u llamadas a servicios, %016llx\n",
                cases, crashes, g_calls, (unsigned long long)g_hash);
    return 0;
}
