#!/usr/bin/env python3
"""Repetición de efectos (T9a), partículas (T9d) y joints (T9e).

Compila los .cpp reales de la suite (--suite effects: Effect_Create.cpp y
MoveEffect.cpp con tests/EffectReplay.cpp; --suite particles: Particle_Spawn.cpp,
Particle_Move.cpp e ItemDrop_Render2.cpp con tests/ParticleReplay.cpp; --suite joints:
Joint_Create.cpp y MoveJoint.cpp con tests/JointReplay.cpp) y corre el
recorrido de tipos x subtipos x owner.

Con EFFECT_REPLAY_KEEP=ruta se guardan el binario, el .map y el .pdb, para
resolver con cdb dónde crashea un caso.

Con --reference-ref REV compila además esa revisión (en una copia temporal del
árbol, con el harness actual) y exige el mismo hash: es la red de la T9 para
desarmar los switches de efectos sin cambiar lo que hacen.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

CLIENT = Path(__file__).resolve().parents[1]
SUITES = {
    # suite: (harness, fuentes reales que se enlazan con él)
    "effects": ("EffectReplay.cpp", ["src/stdafx.cpp", "src/globals.cpp", "src/render/Effect_Create.cpp",
                                     "src/render/MoveEffect.cpp"]),
    "particles": ("ParticleReplay.cpp", ["src/stdafx.cpp", "src/globals.cpp", "src/render/Particle_Spawn.cpp",
                                         "src/render/Particle_Move.cpp", "src/render/ItemDrop_Render2.cpp"]),
    "joints": ("JointReplay.cpp", ["src/stdafx.cpp", "src/globals.cpp", "src/render/Joint_Create.cpp",
                                   "src/render/MoveJoint.cpp"]),
}


def build_and_run(root, harness, work, args):
    work.mkdir(parents=True)
    files = " ".join(f'"{(root / s).as_posix()}"' for s in SUITES[args.suite][1])
    (work / "CMakeLists.txt").write_text(f"""cmake_minimum_required(VERSION 3.25)
project(EffectReplay LANGUAGES CXX)
add_executable(effect_replay "{harness.as_posix()}" {files})
target_include_directories(effect_replay PRIVATE "{(root / 'src').as_posix()}")
target_compile_features(effect_replay PRIVATE cxx_std_17)
if(MSVC)
  target_compile_definitions(effect_replay PRIVATE WIN32 _WINDOWS _MBCS NDEBUG
      _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
  target_compile_options(effect_replay PRIVATE /EHsc /O2 /fp:precise /utf-8 /W0 /bigobj /Zi)
  target_link_options(effect_replay PRIVATE /DEBUG /MAP)
else()
  target_compile_options(effect_replay PRIVATE -fsigned-char -mfpmath=sse -msse2 -fno-strict-aliasing -w)
endif()
""", encoding="utf-8")
    command = ["cmake", "-S", str(work), "-B", str(work / "build")]
    if args.generator:
        command += ["-G", args.generator]
    if os.name == "nt":
        command += ["-A", args.architecture, "-T", "v143"]
    subprocess.run(command, check=True, stdout=subprocess.DEVNULL)
    subprocess.run(["cmake", "--build", str(work / "build"), "--config", "Release"], check=True)
    binary = work / "build" / "effect_replay"
    if os.name == "nt":
        binary = work / "build" / "Release" / "effect_replay.exe"
    if os.environ.get("EFFECT_REPLAY_KEEP"):
        shutil.copy(binary, os.environ["EFFECT_REPLAY_KEEP"])
        if binary.with_suffix(".map").exists():
            shutil.copy(binary.with_suffix(".map"), os.environ["EFFECT_REPLAY_KEEP"] + ".map")
        if binary.with_suffix(".pdb").exists():
            shutil.copy(binary.with_suffix(".pdb"), str(Path(os.environ["EFFECT_REPLAY_KEEP"]).with_suffix(".pdb")))
    out = subprocess.check_output([str(binary)], text=True, cwd=work)
    m = re.search(r"\w+Replay: .*", out)
    if not m:
        sys.exit("EffectReplay: salida inesperada:\n" + out)
    return m.group(0)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--architecture", choices=("Win32", "x64"), default="x64")
    parser.add_argument("--generator")
    parser.add_argument("--reference-ref")
    parser.add_argument("--suite", choices=sorted(SUITES), default="effects")
    args = parser.parse_args()
    harness = CLIENT / "tests" / SUITES[args.suite][0]
    with tempfile.TemporaryDirectory(prefix="mu-effect-replay-") as temp:
        temp = Path(temp)
        current = build_and_run(CLIENT, harness, temp / "current", args)
        print(current)
        if not args.reference_ref:
            return
        repo = Path(subprocess.check_output(["git", "rev-parse", "--show-toplevel"], cwd=CLIENT, text=True).strip())
        tree = temp / "reference"
        subprocess.run(["git", "worktree", "add", "--detach", str(tree), args.reference_ref], cwd=repo, check=True,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        try:
            reference = build_and_run(tree / "client", harness, temp / "ref", args)
        finally:
            subprocess.run(["git", "worktree", "remove", "--force", str(tree)], cwd=repo,
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        print("referencia", args.reference_ref, reference)
        if reference != current:
            sys.exit("EffectReplay: la revisión actual no hace lo mismo que la referencia")


if __name__ == "__main__":
    main()
