#!/usr/bin/env python3
"""Caché de binarios de vcpkg para los jobs del cliente.

baseline            imprime el commit de vcpkg fijado en client/vcpkg-configuration.json
                    (el job clona vcpkg en ese commit y no en master: el ABI de cada
                    paquete incluye los scripts de vcpkg, y con master cambiaba en cada
                    actualización y la caché nunca acertaba).
prune STATUS CACHE MARKER
                    deja en CACHE sólo los paquetes cuyo ABI figura en el STATUS de
                    vcpkg_installed (los que usa este build) y escribe changed=true en
                    $GITHUB_OUTPUT si se compiló algo nuevo (archivos más nuevos que
                    MARKER) o se borró algo viejo.  Con changed=false el job no vuelve
                    a subir la caché.
"""
import json
import os
import sys
from pathlib import Path


def baseline():
    config = Path(__file__).resolve().parents[2] / "client" / "vcpkg-configuration.json"
    print(json.loads(config.read_text(encoding="utf-8"))["default-registry"]["baseline"])


def prune(status, cache, marker):
    status, cache, marker = Path(status), Path(cache), Path(marker)
    abis = {line.split(":", 1)[1].strip() for line in status.read_text(encoding="utf-8").splitlines()
            if line.startswith("Abi:")}
    since = marker.stat().st_mtime if marker.exists() else 0.0
    kept = removed = built = 0
    for package in cache.rglob("*.zip"):
        if package.stem not in abis:
            package.unlink()
            removed += 1
            continue
        kept += 1
        if package.stat().st_mtime > since:
            built += 1
    changed = bool(built or removed)
    print(f"vcpkg-cache: {kept} paquetes en uso ({built} nuevos), {removed} viejos borrados")
    out = os.environ.get("GITHUB_OUTPUT")
    if out:
        with open(out, "a", encoding="utf-8") as stream:
            stream.write(f"changed={'true' if changed else 'false'}\n")


if __name__ == "__main__":
    if sys.argv[1:] == ["baseline"]:
        baseline()
    elif len(sys.argv) == 5 and sys.argv[1] == "prune":
        prune(*sys.argv[2:])
    else:
        sys.exit(__doc__)
