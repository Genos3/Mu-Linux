#!/usr/bin/env python3
"""Create separate alpha-preserving .dt variants for Webzen .OZT assets.

A BMD can reference both "ship05.jpg" and "ship05.tga"; both basenames map to
the same ship05.dt, and the converter prefers the detailed JPG. The .tga
meshes are the rigging/ropes and need their alpha channel, so give them their
own "<name>_t.dt" file and let DC_ResolveDtPath route .tga names to it.
"""
import subprocess
from pathlib import Path

BUILD = Path(__file__.replace("\\", "/")).resolve().parent.parent
SRC = Path("C:/Users/usuario/Desktop/mu online/Mu-Linux-0.97.43/client/bin/Client/Data/Object1")
RAW = BUILD / "raw_assets" / "Object1"
OUT = BUILD / "dc_assets" / "Data" / "Object1"
PVRTEX = BUILD / "tools" / "bin" / "pvrtex.exe"
NAMES = ("ship05", "ship06", "ship07")


def main() -> None:
    RAW.mkdir(parents=True, exist_ok=True)
    for name in NAMES:
        src = SRC / f"{name}.OZT"
        if not src.exists():
            print(f"{name}: no OZT, skip")
            continue
        data = src.read_bytes()[4:]          # strip 4-byte Webzen prefix
        tga = RAW / f"{name}_t.tga"
        tga.write_bytes(data)
        dst = OUT / f"{name}_t.dt"
        r = subprocess.run([str(PVRTEX), "-i", str(tga), "-o", str(dst),
                            "-f", "auto", "-d", "-r", "near"],
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        size = dst.stat().st_size if dst.exists() else 0
        print(f"{name}: rc={r.returncode} {dst.name} {size} bytes")


if __name__ == "__main__":
    main()
