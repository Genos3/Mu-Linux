#!/usr/bin/env python3
"""Convert encrypted EncTerrain*.obj map-object files into a DC-friendly binary.

Input:  Data/WorldN/EncTerrainN.obj  (source format, pre-conversion only)
Output: Data/WorldN/TerrainN.bin     (plain, 32-byte aligned entries)

The DC client can then load objects with a single fread and no decryption.
"""
import struct
import sys
from pathlib import Path

KEY = bytes([
    0xD1, 0x73, 0x52, 0xF6, 0xD2, 0x9A, 0xCB, 0x27,
    0x3E, 0xAF, 0x59, 0x31, 0x37, 0xB3, 0xE7, 0xA2,
])
MAGIC = b'DOBJ'
VERSION = 1
ENTRY_SIZE = 30
OUT_ENTRY_SIZE = 32  # u32 type + 7 floats, naturally aligned


def decrypt(data: bytes) -> bytes:
    out = bytearray(len(data))
    w = 0x5E
    for i, c in enumerate(data):
        out[i] = ((c ^ KEY[i & 0x0F]) - w) & 0xFF
        w = (c + 0x3D) & 0xFF
    return bytes(out)


def convert(src: Path, dst: Path) -> str:
    dec = decrypt(src.read_bytes())
    # byte 0 = magic 0, byte 1 = map number (1..12), not a version field.
    if len(dec) < 4 or dec[0] != 0:
        return f"skip (bad header {dec[:2].hex()})"
    count = struct.unpack_from('<h', dec, 2)[0]
    max_count = (len(dec) - 4) // ENTRY_SIZE
    count = min(count, max_count)
    if count < 0:
        count = 0

    with dst.open('wb') as f:
        f.write(MAGIC)
        f.write(struct.pack('<II', VERSION, count))
        for i in range(count):
            p = 4 + i * ENTRY_SIZE
            typ = struct.unpack_from('<h', dec, p)[0]
            pos = struct.unpack_from('<fff', dec, p + 2)
            tgt = struct.unpack_from('<fff', dec, p + 14)
            scale = struct.unpack_from('<f', dec, p + 26)[0]
            # 2-byte type + 2-byte pad + 7 floats = 32 bytes. The pad keeps
            # the 4-byte float alignment while preserving the original layout.
            f.write(struct.pack('<Hhfffffff', typ & 0xFFFF, 0, *pos, *tgt, scale))
    return f"{count} objects"


def main() -> int:
    root = Path(sys.argv[1] if len(sys.argv) > 1 else '.')
    files = sorted(root.glob('World*/EncTerrain*.obj'))
    if not files:
        print(f'no EncTerrain*.obj under {root}')
        return 1
    for src in files:
        dst = src.with_name(src.name.replace('EncTerrain', 'Terrain').replace('.obj', '.bin'))
        print(f'{src.name} -> {dst.name}: {convert(src, dst)}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
