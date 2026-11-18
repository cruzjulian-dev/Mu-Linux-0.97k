#!/usr/bin/env python3
"""Verifica las reservas, lector/escritor y selección de registros de skills.

Usa skill.bmd real y los cuerpos de producción. La conversión de nombres se
sustituye por un validador de direcciones; no se prueba aquí la conversión UTF-8.
SkillAttribute.Raw es la única tabla primaria (en el binario, el bloque al que
apunta 0x07D29D20); la sombra DAT_07cf1ff8 sigue reservada aparte.
"""
import argparse
import json
import os
from pathlib import Path
import re
import tempfile

from GatePointer import PREAMBLE as IO_PREAMBLE
from SceneObjectPointer import ROOT, build_and_run, function, read_source


def optional_definition(match):
    # Desde la T8o los DAT_ sin uso no están en globals: el código actual no los
    # necesita y las revisiones viejas (--reference-ref) siguen encontrándolos.
    return match.group() if match else ''

EXTRA = r'''
using UINT = unsigned int;
DWORD DAT_00b43000 = 0x00b43000;
const char s__s___File_corrupted__00559bd4[] = "archivo corrupto";
BYTE* reservations[1];
int allocationIndex;
void* AllocateSkills(std::size_t n) {
    Require(n == 0xa00 && allocationIndex < 1);
    return reservations[allocationIndex++] + 16;
}
'''

TEXT = r'''
namespace TextEngine {
void NormalizeTextFileInPlace(char* p, int size) {
    const auto address = reinterpret_cast<std::uintptr_t>(p);
    const auto raw = reinterpret_cast<std::uintptr_t>(SkillAttribute.Raw);
    const auto shadow = reinterpret_cast<std::uintptr_t>(reinterpret_cast<void*>(DAT_07cf1ff8));
    Require(size == 32 && ((address >= raw && address + size <= raw + 2560) ||
                          (address >= shadow && address + size <= shadow + 2560)));
}
}
'''

MAIN = r'''
std::vector<BYTE> Read(const char* path) {
    std::ifstream f(path, std::ios::binary); Require(f.good());
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}
void Run(std::uintptr_t address, const std::vector<BYTE>& file) {
    allocationIndex = 0;
    for(int i = 0; i < 1; ++i) {
        reservations[i] = static_cast<BYTE*>(Reserve(address + i * 0x10000));
        Require(reservations[i] != nullptr);
        std::memset(reservations[i], 0xa5, 2592);
    }
    InitializeSkills();
    Require(allocationIndex == 1);
    Require(reinterpret_cast<void*>(DAT_07cf1ff8) == reservations[0] + 16);
    std::memset(SkillAttribute.Raw, 0, sizeof(SkillAttribute.Raw));
    Skill_LoadBMD(SAMPLE_PATH);
    const auto* primary = reinterpret_cast<const BYTE*>(SkillAttribute.Raw);
    const auto* shadow = reinterpret_cast<const BYTE*>(DAT_07cf1ff8);
    for(int i = 0; i < 2560; ++i) {
        const BYTE plain = BYTE(file[i] ^ BYTE(DAT_00559bb4[i % 40 % 3]));
        Require(BYTE(SkillAttribute.Raw[i]) == plain);
        Require(shadow[i] == BYTE(i % 40 == 38 ? plain * 2 : plain));
    }
    Require(Warrior_GetSkillRecord97k(-1) == nullptr);
    Require(Warrior_GetSkillRecord97k(64) == nullptr);
    for(int i = 0; i < 64; ++i)
        Require(Warrior_GetSkillRecord97k(i) == shadow + i * 40);
    const auto savedShadow = DAT_07cf1ff8;
    DAT_07cf1ff8 = 0;
    for(int i = 0; i < 64; ++i)
        Require(Warrior_GetSkillRecord97k(i) == primary + i * 40);
    DAT_07cf1ff8 = savedShadow;
    Skill_SaveBMD("skills-roundtrip.bmd");
    Require(Read("skills-roundtrip.bmd") == file);
    for(int table = 0; table < 1; ++table)
        for(int i = 0; i < 16; ++i)
            Require(reservations[table][i] == 0xa5 && reservations[table][2576 + i] == 0xa5);
}
int main() {
    const auto file = Read(SAMPLE_PATH); Require(file.size() == 2564);
    Run(0x50000000u, file);
    if(sizeof(void*) > 4) {
        Run(std::uint64_t(0x150000000ull), file);
        std::puts("tablas de skills sobre 4 GiB: reservas, lector/escritor y fallback correctos");
    }
    std::puts("skill.bmd: 64 registros de 40 bytes, sombra y checksum sin cambios");
}
'''


def harness(revision=None):
    header = read_source("src/globals.h", revision)
    globals_source = read_source("src/globals.cpp", revision)
    entry = read_source("src/platform/ClientEntry.cpp", revision)
    definitions, allocations = "", ""
    for name in ("DAT_07cf1ff8",):
        definitions += optional_definition(re.search(rf"extern [^;\n]*\b{name}\s*;", header))
        definitions += re.search(rf"^[^/\n]*\b{name}\s*=\s*[^;]+;", globals_source, re.M).group()
        allocations += re.search(rf"^[ \t]*{name}\s*=\s*[^;]+;", entry, re.M).group().replace(
            "malloc(0xa00)", "AllocateSkills(0xa00)")
    raw = re.search(r"struct _SkillAttrEntry\s*\{.*?\};", header, re.S).group()
    raw += "\n_SkillAttrEntry SkillAttribute{};\n"
    scene = read_source("src/scenes/Scene_CharSelect_Nav.cpp", revision)
    data = read_source("src/assets/data/Skill_Data.cpp", revision)
    warrior = read_source("src/game/combat/Skills_WarriorLegacy.cpp", revision)
    sample = __import__("data_paths").asset(ROOT, "Local/skill.bmd")
    if not sample.is_file():
        raise FileNotFoundError(sample)
    return (IO_PREAMBLE.split("BYTE* reservation;")[0] + EXTRA + definitions + raw + TEXT
            + "void InitializeSkills() {" + allocations + "}\n"
            + function(scene, "BuxConvert_0") + function(data, "Skill_LoadBMD")
            + function(data, "Skill_SaveBMD") + function(warrior, "Warrior_GetSkillRecord97k")
            + MAIN.replace("SAMPLE_PATH", json.dumps(str(sample))))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--architecture", choices=("Win32", "x64"), default="x64")
    parser.add_argument("--generator")
    parser.add_argument("--reference-ref")
    args = parser.parse_args()
    if args.reference_ref and (os.name != "nt" or args.architecture != "Win32"):
        parser.error("La referencia int sólo se compara en Windows x86")
    with tempfile.TemporaryDirectory(prefix="mu-skill-pointer-") as temp:
        directory = Path(temp)
        current = build_and_run(directory / "current", harness(), args)
        print(current, end="")
        if args.reference_ref:
            reference = build_and_run(directory / "reference", harness(args.reference_ref), args)
            if current != reference:
                raise RuntimeError("Diferencia con la referencia x86")
            print(f"Equivalencia de bytes con {args.reference_ref}: OK")


if __name__ == "__main__":
    main()
