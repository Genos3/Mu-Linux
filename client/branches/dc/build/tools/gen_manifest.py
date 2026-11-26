#!/usr/bin/env python3
"""Generate Data/assets.manifest for the DC asset ring cache.

The manifest is a compact binary index of every offline-converted .dt on the
disc. The runtime cache uses it to find an asset's size before reading it and
to decide which assets can fit in a streaming slot.

Usage: python tools/gen_manifest.py [path/to/dc_assets/Data]
"""
import struct
import sys
from pathlib import Path

MAGIC = b"DCAM"
VERSION = 1
PATH_BYTES = 64
ENTRY = struct.Struct("<IHHI")  # file size, width, height, pvr_type


def main() -> None:
    root = Path(sys.argv[1] if len(sys.argv) > 1 else "dc_assets/Data")
    out = root / "assets.manifest"
    entries = []
    for png in sorted(root.rglob("*.dt")):
        rel = png.relative_to(root).with_suffix("").as_posix()
        if len(rel) >= PATH_BYTES:
            print(f"skip (path too long): {rel}")
            continue
        data = png.read_bytes()
        if len(data) < 32 or data[:4] != b"DcTx":
            continue
        w, h = struct.unpack_from("<HH", data, 12)
        pvr_type = struct.unpack_from("<I", data, 16)[0]
        entries.append((rel, len(data), w, h, pvr_type))

    with out.open("wb") as f:
        f.write(MAGIC)
        f.write(struct.pack("<II", VERSION, len(entries)))
        for rel, size, w, h, pvr in entries:
            name = rel.encode("ascii")
            f.write(name + b"\0" * (PATH_BYTES - len(name)))
            f.write(ENTRY.pack(size, w, h, pvr))
    print(f"wrote {out} ({len(entries)} assets, {out.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
