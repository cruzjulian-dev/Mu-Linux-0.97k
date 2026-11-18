#!/usr/bin/env python3
"""Entity_SpawnEffects entera: cada llamada a sprites, partículas, joints y efectos, por mapa y tipo.

Compila la función real de Entity_Render_3D.cpp con servicios simulados que
registran cada llamada (con sus argumentos y punteros relativos al objeto o a
g_BoneScratch), el OBJECT después del tick y la secuencia de rand(), sobre 18
mapas x 112 tipos x 120 estados.  Reemplaza a SceneAnimationGuardPointer,
SceneTypeSelectors y SceneNumericRemainders, que probaban fragmentos sueltos de
la versión anterior de la función (S13b).
"""
import argparse
import os
import re
import tempfile
from pathlib import Path
from SceneObjectPointer import build_and_run, read_source

SERVICES = r'''
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
typedef unsigned char BYTE; typedef unsigned char byte; typedef unsigned int DWORD; typedef unsigned int uint;
typedef unsigned int undefined4; typedef long long longlong; typedef unsigned long long ulonglong; typedef long double float10;
#define fsin(x) sinl(x)
#define _rand() TestRand()
#define rand() TestRand()
static BYTE models[300 * 188]; std::uintptr_t Models = (std::uintptr_t)models;
char g_BoneScratch[200 * 0x30];
#define DAT_06970a9c   (*(DWORD*)(g_BoneScratch + 0x000))
#define DAT_06970acc   (*(DWORD*)(g_BoneScratch + 0x030))
#define DAT_06970afc   (*(DWORD*)(g_BoneScratch + 0x060))
#define DAT_06970b2c   (*(DWORD*)(g_BoneScratch + 0x090))
#define DAT_06970b5c   (*(DWORD*)(g_BoneScratch + 0x0c0))
#define DAT_06970bbc   (*(DWORD*)(g_BoneScratch + 0x120))
#define DAT_06970bec   (*(DWORD*)(g_BoneScratch + 0x150))
#define DAT_06970c1c   (*(DWORD*)(g_BoneScratch + 0x180))
#define DAT_06970c4c   (*(DWORD*)(g_BoneScratch + 0x1b0))
#define DAT_06970c7c   (*(DWORD*)(g_BoneScratch + 0x1e0))
#define DAT_06970d0c   (*(DWORD*)(g_BoneScratch + 0x270))
#define DAT_06970d6c   (*(DWORD*)(g_BoneScratch + 0x2d0))
#define DAT_06970e2c   (*(DWORD*)(g_BoneScratch + 0x390))
#define DAT_06970e8c   (*(DWORD*)(g_BoneScratch + 0x3f0))
#define DAT_06970eec   (*(DWORD*)(g_BoneScratch + 0x450))
#define DAT_0697106c   (*(DWORD*)(g_BoneScratch + 0x5d0))
#define DAT_0697139c   (*(DWORD*)(g_BoneScratch + 0x900))
#define DAT_0697154c   (*(DWORD*)(g_BoneScratch + 0xab0))
#define DAT_0697157c   (*(DWORD*)(g_BoneScratch + 0xae0))
#define DAT_0697160c   (*(DWORD*)(g_BoneScratch + 0xb70))
int SceneFlag; float WorldTime;
struct MapMgr { int map; int GetCurrentMap() { return map; } } gMapManager;
struct ModelAnimationAction { BYTE b[16]; };
struct ObjectView { unsigned char CurrentAction; }; static ObjectView objectView;
ObjectView* AsObject(std::intptr_t) { return &objectView; }
struct ModelView { ModelAnimationAction* Actions; }; static ModelView modelView; static ModelAnimationAction actions[64];
ModelView* AsModel(void*) { return &modelView; }
static unsigned rstate; int TestRand() { rstate = rstate * 1103515245u + 12345u; return (rstate >> 16) & 0x7fff; }
static std::uint64_t hash = 14695981039346656037ull; static unsigned long long calls;
static BYTE obj[0x1BC];
static void H(const void* p, std::size_t n) { const BYTE* b = (const BYTE*)p; while (n--) { hash ^= *b++; hash *= 1099511628211ull; } }
static void Hi(long long v) { H(&v, sizeof(v)); }
static void Hf(float f) { H(&f, 4); }
static void Hv(const float* v) { H(v, 12); }
static void Hp(const void* p) {
    const BYTE* b = (const BYTE*)p;
    if (b >= obj && b < obj + sizeof(obj)) Hi(0x100000 + (b - obj));
    else if (b >= (const BYTE*)g_BoneScratch && b < (const BYTE*)g_BoneScratch + sizeof(g_BoneScratch)) Hi(0x200000 + (b - (const BYTE*)g_BoneScratch));
    else if (!p) Hi(0); else if ((std::uintptr_t)p == 0xffffffffu || (std::intptr_t)p == -1) Hi(-1); else Hi(1);
}
void BMD_TransformPosition(void*, float* bone, float* in, float* out, char f) {
    ++calls; Hi(1); Hp(bone); Hv(in); Hi(f);
    long long o = (const BYTE*)bone - (const BYTE*)g_BoneScratch;
    out[0] = (float)o + in[0]; out[1] = (float)(o * 3) + in[1]; out[2] = (float)(o * 7) + in[2];
}
int WorldEffects_AddSprite(unsigned short t, float* p, float s, float* l, std::intptr_t o, float a, int x) {
    ++calls; Hi(2); Hi(t); Hv(p); Hf(s); Hv(l); Hp((void*)o); Hf(a); Hi(x); return 0; }
int Particle_Spawn(int t, float* a, float* b, float* c, int f, float al, std::intptr_t md) {
    ++calls; Hi(3); Hi(t); Hv(a); Hp(b); Hv(c); Hi(f); Hf(al); Hp((void*)md); return 0; }
void* Joint_Create(int t, float* a, float* b, float* c, unsigned s, std::intptr_t tg, float sc, short k, unsigned char m) {
    ++calls; Hi(4); Hi(t); Hv(a); Hv(b); Hv(c); Hi(s); Hi(tg); Hf(sc); Hi(k); Hi(m); return nullptr; }
int CreateEffect(int t, float* a, float* b, float* c, void* d, void* e, float* f, void* g, int h) {
    ++calls; Hi(5); Hi(t); Hp(a); Hp(b); Hp(c); Hp(d); Hp(e); Hp(f); Hp(g); Hi(h); return 0; }
int SetAction(ObjectView*, int a) { ++calls; Hi(6); Hi(a); return a; }
'''

MAIN = r'''
static void Setup(int map, int type, unsigned seed) {
    unsigned s = seed * 2654435761u + type * 40503u + map * 977u;
    for (unsigned i = 0; i < sizeof(obj); ++i) obj[i] = (BYTE)(i * 11 + seed);
    *(short*)(obj + 2) = (short)type;
    *(float*)(obj + 0xc) = 1.25f; *(float*)(obj + 0x10) = 1000.f; *(float*)(obj + 0x14) = 2000.f; *(float*)(obj + 0x18) = 30.f;
    *(float*)(obj + 0x24) = (float)((s >> 3) % 360);
    *(int*)(obj + 0x58) = (s >> 5) % 3 == 0 ? -2 : 0;
    *(float*)(obj + 0x80) = (float)((int)((s >> 7) % 40) - 10) + ((s >> 11) % 2 ? 0.5f : 0.f);
    for (int i = 0; i < 200 * 12; ++i) ((float*)g_BoneScratch)[i] = (float)((i * 7 + seed) % 50);
    std::memset(models, 0, sizeof(models));
    *(short*)(models + (type & 0xff) * 188 + 0xa8) = (short)((s >> 9) % 6 - (seed % 17 == 0 ? 32768 : 0));
    for (int i = 0; i < 64; ++i) *(short*)(actions[i].b + 8) = (short)((i * 3 + seed) % 7);
    modelView.Actions = actions; objectView.CurrentAction = (unsigned char)((s >> 13) % 8);
    WorldTime = (float)((s >> 4) % 900000) + ((s >> 2) % 2 ? 0.37f : 0.f);
    SceneFlag = (s >> 17) % 3 == 0 ? 2 : 5; gMapManager.map = map;
}
int main() {
    unsigned cases = 0;
    for (int map = 0; map <= 17; ++map) for (int type = 0; type < 0x70; ++type) for (unsigned seed = 0; seed < 120; ++seed) {
        Setup(map, type, seed); rstate = seed; Run((std::intptr_t)obj);
        H(obj, sizeof(obj)); Hi(rstate); ++cases;
    }
    std::printf("SpawnEffectsReplay: %u casos, %llu llamadas, %016llx\n", cases, calls, (unsigned long long)hash);
}
'''


def harness(revision=None):
    source = read_source("src/render/Entity_Render_3D.cpp", revision)
    start = source.index("// IDA: Entity_SpawnEffects (0x004FC070)")
    function = source[start:]
    legacy = "float * __cdecl Entity_SpawnEffects" in function
    function, n = re.subn(r"(?:float \*|void) __cdecl Entity_SpawnEffects\(intptr_t param_1\)",
                          "void Run(intptr_t param_1)", function)
    assert n == 1
    # La versión anterior devolvía float*; en Run los `return x;` pasan a sentencias.
    if legacy:
        function = re.sub(r"\breturn ([^;]+);", r"{ (void)(\1); return; }", function)
    return SERVICES + function + MAIN


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--architecture", choices=("Win32", "x64"), default="x64")
    parser.add_argument("--generator")
    parser.add_argument("--reference-ref")
    args = parser.parse_args()
    if args.reference_ref and (os.name != "nt" or args.architecture != "Win32"):
        parser.error("Referencia sólo en Windows x86")
    with tempfile.TemporaryDirectory(prefix="mu-spawn-effects-") as temp:
        directory = Path(temp)
        current = build_and_run(directory / "current", harness(), args)
        print(current, end="")
        if args.reference_ref:
            reference = build_and_run(directory / "reference", harness(args.reference_ref), args)
            if current != reference:
                raise RuntimeError("Diferencia con referencia x86")
            print(f"Equivalencia con {args.reference_ref}: OK")


if __name__ == "__main__":
    main()
