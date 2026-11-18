// Repetición de Particle_Spawn + MoveParticles (T9d).
//
// Mismo esquema que EffectReplay: enlaza Particle_Spawn.cpp, Particle_Move.cpp
// y globals.cpp reales con estos stubs, recorre cada tipo x subtipo x owner con
// rand fijo, mueve el pool unos ticks y hashea los slots y cada llamada a un
// servicio.  Los punteros se hashean por la región a la que apuntan.
//
// No certifica que las partículas se vean bien: certifica que dos revisiones
// del código hacen exactamente lo mismo.

#include "stdafx.h"
#include "functions.h"
#include "globals.h"
#include "game/core/MapManager.h"
#include "game/core/RenderFxSnapshots.h"
#include "render/ResourceTables.h"
#include "app/config/UserSettings.h"
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

alignas(16) unsigned char g_owners[2][sizeof(CHARACTER)];
alignas(16) unsigned char g_bones[200 * 48];
alignas(16) unsigned char g_textures[0x40000];
alignas(16) unsigned char g_models[4096 * BMD_STRIDE];

std::uint64_t Region(const void* p)
{
    const auto v = reinterpret_cast<std::uintptr_t>(p);
    auto in = [&](const void* base, std::size_t size, std::uint64_t tag) -> std::uint64_t {
        const auto b = reinterpret_cast<std::uintptr_t>(base);
        return (v >= b && v < b + size) ? (tag << 32) | (v - b) : 0;
    };
    if (!v) return 0;
    if (auto r = in(Particles, sizeof(Particles), 1)) return r;
    if (auto r = in(g_owners, sizeof(g_owners), 2)) return r;
    if (auto r = in(g_bones, sizeof(g_bones), 4)) return r;
    if (auto r = in(g_textures, sizeof(g_textures), 5)) return r;
    if (auto r = in(g_models, sizeof(g_models), 6)) return r;
    return 0xFFull << 32;
}
void MixP(const void* p) { MixU(Region(p)); }
void MixV3(const float* v) { if (!v) { MixU(0); return; } MixP(v); for (int i = 0; i < 3; ++i) MixF(v[i]); }
void Call(const char* name) { ++g_calls; Mix(name, std::strlen(name)); }
float Det(float a, float b) { return a * 0.375f - b * 0.125f + 7.0f; }

} // namespace

// ── Servicios que llaman las partículas ─────────────────────────────────────
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
void __cdecl AngleMatrix(float angles[3], float matrix[3][4])
{
    Call("AngleMatrix"); MixV3(angles);
    for (int i = 0; i < 12; ++i) matrix[i / 4][i % 4] = (angles ? angles[i % 3] : 0.0f) * 0.02f + (float)i * 0.5f;
}
void __cdecl Vector_AddRotated(float* pos, float* dir, float* vel)
{
    Call("Vector_AddRotated"); MixV3(pos); MixV3(dir); MixP(vel);
    if (vel) for (int i = 0; i < 3; ++i) vel[i] = (pos ? pos[i] : 0.0f) * 0.25f - (dir ? dir[i] : 0.0f);
}
void __cdecl ItemDrop_SetupRenderRef(BYTE* particle) { Call("ItemDrop_SetupRenderRef"); MixP(particle); }
float __cdecl RequestTerrainHeight(float x, float y)
{
    Call("RequestTerrainHeight"); MixF(x); MixF(y);
    return Det(x, y);
}
HRESULT __cdecl PlayBuffer(int buffer, const OBJECT* object, BOOL looped)
{
    Call("PlayBuffer"); MixU((unsigned)buffer); MixP(object); MixU((unsigned)looped);
    return 0;
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
void RenderFxSnapshots_New(const void* o) { Call("RenderFxSnapshots_New"); MixP(o); }
// RenderParticles: cada sprite que emite y cada cambio de estado de blend.
void __cdecl RenderSprite_0(int type, float* pos, float w, float h, float* light, float rot, float u, float v, float uw, float vh)
{
    Call("RenderSprite_0"); MixU((unsigned)type); MixV3(pos); MixF(w); MixF(h); MixV3(light);
    MixF(rot); MixF(u); MixF(v); MixF(uw); MixF(vh);
}
void __cdecl GL_SetBlendSrcOver(char mode) { Call("GL_SetBlendSrcOver"); MixU((unsigned char)mode); }
void __cdecl GL_SetBlendAdditive(void) { Call("GL_SetBlendAdditive"); }
void __cdecl GL_SetBlendSrcAlpha(void) { Call("GL_SetBlendSrcAlpha"); }
void __cdecl EnableAlphaBlend(void) { Call("EnableAlphaBlend"); }
void __cdecl EnableAlphaTest(bool enable) { Call("EnableAlphaTest"); MixU(enable); }
CUserSettings gUserSettings;
void* __fastcall PhysicsManager_Construct(void* p) { return p; }
extern "C" void DbgLogPublic(const char*) {}
extern "C" { BYTE OffsetInventoryItems[64 * 68] = {}; }
CMapManager::CMapManager() = default;
CMapManager gMapManager;
namespace ResourceTables {
std::size_t ModelCount() { return 4096; }
char* TextureBase() { return reinterpret_cast<char*>(g_textures); }
}

#if !defined(_MSC_VER)
#include <csetjmp>
#include <csignal>
namespace { sigjmp_buf g_jump; void OnFault(int) { siglongjmp(g_jump, 1); } }
#endif

namespace {

constexpr int kStride = 0x70;

bool Active(int i) { return Particles[i].Live != 0; }

void HashSlot(int i)
{
    MixU((unsigned)i);
    const unsigned char* slot = reinterpret_cast<const unsigned char*>(&Particles[i]);
    Mix(slot, 0x3C);                    // +0x3C: los 32 bits bajos del owner, cambian entre builds
    Mix(slot + 0x40, kStride - 0x40);
    MixP(reinterpret_cast<const void*>(g_ParticleOwner[i]));
}

void ClearPool()
{
    for (int i = 0; i < kParticlePoolSize; ++i)
        if (Active(i)) { std::memset(&Particles[i], 0, kStride); g_ParticleOwner[i] = 0; }
}

void FillOwner(unsigned char* c, unsigned seed)
{
    std::memset(c, 0, sizeof(CHARACTER));
    CHARACTER* ch = AsCharacter(c);
    ch->Class = (BYTE)(seed * 3);
    ch->Key = (WORD)(100 + seed);
    ch->Object.Scale = 1.0f;
    ch->Object.Alpha = 1.0f;
    ch->Object.Live = 1;
    ch->Object.Type = (short)(390 + seed);
    for (int i = 0; i < 3; ++i) { ch->Object.Position[i] = 1000.0f + 100.0f * (float)i + (float)seed; ch->Object.Angle[i] = 30.0f * (float)i; }
    ch->Object.BoneMatrices = reinterpret_cast<vec34_t*>(g_bones);
}

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

struct CaseArgs { int type, sub, ticks; void* owner; };

void RunCase(const CaseArgs& a)
{
    float pos[3] = { 1200.0f, 1300.0f, 40.0f }, angle[3] = { 10.0f, 20.0f, 90.0f }, light[3] = { 1.0f, 0.5f, 0.25f };
    ScrubStack();
    Particle_Spawn(a.type, pos, angle, light, a.sub, 1.25f, (intptr_t)a.owner);
    for (int t = 0; t < a.ticks; ++t) {
        MixU(0x7100u | (unsigned)t);
        ScrubStack();
        MoveParticles();
        MixU(0x7200u | (unsigned)t);
        ScrubStack();
        RenderParticles();
    }
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
    for (std::size_t i = 0; i < sizeof(g_textures) / 4; ++i) reinterpret_cast<float*>(g_textures)[i] = (float)(i % 17) + 1.0f;
    const int minType = 1100, maxType = 1500, maxSub = 16, ticks = 10;
    const bool trace = std::getenv("EFFECT_REPLAY_TRACE") != nullptr;
    FILE* perCase = nullptr;
    if (const char* path = std::getenv("EFFECT_REPLAY_CASES")) perCase = std::fopen(path, "w");
    unsigned cases = 0, crashes = 0;
    for (int type = minType; type < maxType; ++type)
    for (int sub = 0; sub < maxSub; ++sub)
    for (int ownerMode = 0; ownerMode < 3; ++ownerMode) {
        FillOwner(g_owners[0], 0); FillOwner(g_owners[1], 1);
        Hero = reinterpret_cast<char*>(g_owners[0]);
        ClearPool();
        std::srand((unsigned)(type * 977 + sub * 31 + ownerMode));
        void* owner = ownerMode == 0 ? nullptr : g_owners[ownerMode - 1];
        const std::uint64_t total = g_hash;
        g_hash = 14695981039346656037ull;
        MixU(0xC0DE0000u | (unsigned)type); MixU((unsigned)sub); MixU((unsigned)ownerMode);
        if (!RunGuarded({ type, sub, ticks, owner })) {
            ++crashes; MixU(0xDEAD0000u | (unsigned)type);
            if (trace) std::fprintf(stderr, "crash %d %d %d\n", type, sub, ownerMode);
        }
        for (int i = 0; i < kParticlePoolSize; ++i) if (Active(i)) HashSlot(i);
        const std::uint64_t caseHash = g_hash;
        if (perCase) std::fprintf(perCase, "%d %d %d %016llx\n", type, sub, ownerMode, (unsigned long long)caseHash);
        g_hash = total;
        Mix(&caseHash, sizeof(caseHash));
        ++cases;
    }
    ClearPool();
    if (perCase) std::fclose(perCase);
    std::printf("ParticleReplay: %u casos, %u crashes de combinaciones invalidas, %u llamadas a servicios, %016llx\n",
                cases, crashes, g_calls, (unsigned long long)g_hash);
    return 0;
}
