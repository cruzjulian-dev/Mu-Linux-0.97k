#!/usr/bin/env python3
"""Armado de modelos de monstruo (Content/monster/models.toml) con el OpenMonsterModel real.

Compila OpenMonsterModel junto con MonsterModelTable.cpp, con AccessModel /
OpenTexture / OpenJPG / OpenTGA / SoundTable_LoadMonster registrando cada
llamada, y corre los tipos -1..66.  Por cada tipo vuelca las llamadas y cómo
quedan las acciones (velocidad y repetición) y los campos de los modelos
270..340 (sonidos, BoneHead, StreamMesh).  Comprueba que no haya líneas
rechazadas y que las texturas extra existan.

Con --reference-ref REV compila además OpenMonsterModel tal como estaba en REV
y exige el mismo resultado.  Las llamadas posteriores a los sonidos (campos del
modelo y texturas extra) se comparan sin orden: son independientes y el switch
viejo no las hacía siempre en el mismo orden.  La tabla se generó así desde el
switch de la S2 del refactor.
"""
import argparse
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

from MonsterSpawnPointer import build

ROOT = Path(__file__).resolve().parents[1]

STUBS = r'''
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>
typedef unsigned int undefined4; typedef unsigned char undefined1;
uintptr_t Models;
char DAT_0055a7c4;
static std::vector<std::string> trace;
static void T(const std::string& s) { trace.push_back(s); }
static std::uint8_t modelMem[400 * sizeof(BMD)];
static std::uint8_t actMem[400][16 * sizeof(ModelAnimationAction)];
void AccessModel(int id, const char* dir, const char* file, int idx) {
    T("A|" + std::to_string(id) + "|" + dir + "|" + file + "|" + std::to_string(idx));
    BMD* m = AsModel(Models + (intptr_t)id * BMD_STRIDE);
    m->NumMeshs = 3; m->NumActions = 12; m->Actions = actMem[id];
}
void OpenTexture(int id, const char* p, int f, char l) {
    T("T|" + std::to_string(id) + "|" + p + "|" + std::to_string(f) + "|" + std::to_string((int)l)); }
static int Image(const char* kind, const char* p, int id, int a, int b, int c, char d) {
    T(std::string(kind) + "|" + p + "|" + std::to_string(id) + "|" + std::to_string(a) + "|" +
      std::to_string(b) + "|" + std::to_string(c) + "|" + std::to_string((int)d));
    return 0;
}
int OpenJPG(const char* p, int id, int a, int b, int c, char d) { return Image("J", p, id, a, b, c, d); }
int OpenTGA(const char* p, int id, int a, int b, int c, char d) { return Image("G", p, id, a, b, c, d); }
void SoundTable_LoadMonster(int t) { T("S|" + std::to_string(t)); }
void Model_SetAnimationSlots(int m, int a, int b, int c, int d, int e) {
    auto* model = AsModel(Models) + m;
    model->Sounds[0] = (short)a; model->Sounds[1] = (short)b; model->Sounds[2] = (short)c;
    model->Sounds[3] = (short)d; model->Sounds[4] = (short)e;
}
static void DbgLogPublic(const char* msg) { std::printf("WARN|%s\n", msg); }
'''

MAIN = r'''
static void Reset() {
    std::memset(modelMem, 0, sizeof(modelMem));
    for (int m = 0; m < 400; ++m) {
        for (size_t i = 0; i < sizeof(actMem[m]); ++i) actMem[m][i] = (std::uint8_t)(i * 13 + m);
        for (int a = 0; a < 16; ++a) {
            float v = 0.1f * (float)((a + m) % 9 + 1);
            std::memcpy(actMem[m] + a * sizeof(ModelAnimationAction) + 4, &v, 4);
        }
        AsModel(Models + (intptr_t)m * BMD_STRIDE)->Actions = actMem[m];
    }
    trace.clear();
}
int main() {
    Models = (uintptr_t)modelMem;
    for (int type = -1; type <= 66; ++type) {
        Reset();
        Run(type);
        auto t = trace; size_t k = 0;
        while (k < t.size() && t[k].rfind("S|", 0) != 0) ++k;
        if (k < t.size()) std::sort(t.begin() + k + 1, t.end());
        for (auto& s : t) std::printf("%d|%s\n", type, s.c_str());
        for (int a = 0; a < 12; ++a) {
            const std::uint8_t* r = actMem[type + 270] + a * sizeof(ModelAnimationAction);
            unsigned bits; std::memcpy(&bits, r + 4, 4);
            std::printf("%d|act|%d|%08x|%u\n", type, a, bits, (unsigned)r[0]);
        }
        for (int m = 270; m <= 340; ++m) {
            const BMD* b = AsModel(Models + (intptr_t)m * BMD_STRIDE);
            if (b->Sounds[0] || b->Sounds[4] || b->BoneHead || b->StreamMesh)
                std::printf("%d|model|%d|%d,%d,%d,%d,%d|%d|%d\n", type, m, b->Sounds[0], b->Sounds[1],
                            b->Sounds[2], b->Sounds[3], b->Sounds[4], b->BoneHead, (int)b->StreamMesh);
        }
    }
}
'''


def block(text, start):
    i = text.index(start)
    depth, k = 0, text.index("{", i)
    for j in range(k, len(text)):
        depth += {"{": 1, "}": -1}.get(text[j], 0)
        if depth == 0:
            return text[i:j + 1]


def header(path):
    return (ROOT / "src" / path).read_text(encoding="utf-8").replace("#pragma once", "")


def model_types():
    return "#include <cstddef>\n#include <cstdint>\n#include <type_traits>\n" + header("assets/model/ModelTypes.h")


def current_harness():
    table = (ROOT / "src/assets/model/MonsterModelTable.cpp").read_text(encoding="utf-8")
    table = table.replace('#include "stdafx.h"', "")
    table = table.replace('#include "assets/model/MonsterModelTable.h"', header("assets/model/MonsterModelTable.h"))
    table = table.replace('#include "assets/data/TomlLines.h"', header("assets/data/TomlLines.h"))
    table = table.replace('extern "C" void DbgLogPublic(const char* msg);', "")
    function = block((ROOT / "src/assets/data/Monster_Data.cpp").read_text(encoding="utf-8"),
                     "void __cdecl OpenMonsterModel(int Type)")
    function = function.replace("void __cdecl OpenMonsterModel(int Type)", "static void Run(int Type)")
    return model_types() + STUBS + table + function + MAIN


def reference_harness(revision):
    text = subprocess.check_output(["git", "show", f"{revision}:client/src/assets/data/Monster_Data.cpp"],
                                   cwd=ROOT).decode("utf-8")
    function = block(text, "void __cdecl OpenMonsterModel(int param_1)")
    function = function.replace("void __cdecl OpenMonsterModel(int param_1)", "static void Run(int param_1)")
    return model_types() + STUBS + function + MAIN


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--architecture", choices=("Win32", "x64"), default="x64")
    parser.add_argument("--generator")
    parser.add_argument("--reference-ref")
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="mu-monster-model-") as temp:
        data = Path(temp) / "data"
        (data / "Content/monster").mkdir(parents=True)
        shutil.copyfile(ROOT / "bin/Client/Content/monster/models.toml", data / "Content/monster/models.toml")
        rows = build(Path(temp) / "current", current_harness(), args, cwd=data)
        warnings = [r for r in rows if r.startswith("WARN|")]
        if warnings:
            raise RuntimeError("el lector rechazó líneas:\n" + "\n".join(warnings))
        images = sorted({r.split("|")[2] for r in rows if r.split("|")[1] in ("J", "G")})
        # Los assets están en Content con su orden nuevo (alias.toml): basta con
        # que el archivo exista con la extensión que lee el loader (.jpg -> .OZJ).
        files = {p.name.lower() for p in (ROOT / "bin/Client/Content").rglob("*") if p.is_file()}
        missing = [i for i in images
                   if re.sub(r"\.jpg$", ".ozj", re.sub(r"\.tga$", ".ozt", i.lower())).rsplit("/", 1)[-1]
                   not in files]
        if missing:
            raise RuntimeError(f"texturas que no están: {missing}")
        loads = sum(1 for r in rows if r.split("|")[1] == "A")
        print(f"MonsterModel: {loads} tipos, {len(rows)} filas, {len(images)} texturas extra presentes")
        if args.reference_ref:
            reference = build(Path(temp) / "reference", reference_harness(args.reference_ref), args, cwd=data)
            if reference != rows:
                for a, b in zip(reference, rows):
                    if a != b:
                        raise RuntimeError(f"difiere:\n  código {a}\n  tabla  {b}")
                raise RuntimeError(f"cantidad distinta: código {len(reference)} / tabla {len(rows)}")
            print(f"Equivalencia con {args.reference_ref}: OK ({len(rows)} filas)")


if __name__ == "__main__":
    main()
