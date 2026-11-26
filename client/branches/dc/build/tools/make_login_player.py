#!/usr/bin/env python3
"""Make a small v10 player BMD for the DC login preview characters.

player.bmd is BMD v12 (encrypted) and ~1.68 MB. On the DC's 16 MB heap its
two parse buffers (encrypted + decrypted) cost ~3.4 MB and OOM. The login
scene only needs a handful of actions, so this tool decrypts the file and
drops the keyframes for every other action, producing a v10 file small
enough to load during login.
"""
import struct
import sys
from pathlib import Path

XOR = bytes([
    0xD1, 0x73, 0x52, 0xF6, 0xD2, 0x9A, 0xCB, 0x27,
    0x3E, 0xAF, 0x59, 0x31, 0x37, 0xB3, 0xE7, 0xA2,
])

# Actions the login preview characters use (idle / celebrate / laugh).
KEEP = {0, 1, 99, 101, 111, 113, 115}


def decrypt(data: bytes) -> bytes:
    assert data[:3] == b"BMD" and data[3] == 0x0C, "expected BMD v12"
    enc_size = struct.unpack_from("<i", data, 4)[0]
    enc = data[8:8 + enc_size]
    out = bytearray(b"BMD\x0C")
    w = 0x5E
    for i, src in enumerate(enc):
        out.append((src ^ XOR[i & 0xF]) - w & 0xFF)
        w = (src + 0x3D) & 0xFF
    return bytes(out)


def main() -> None:
    src = Path(sys.argv[1] if len(sys.argv) > 1 else "Data/Player/player.bmd")
    dst = Path(sys.argv[2] if len(sys.argv) > 2 else "Data/Player/player_login.bmd")
    raw = src.read_bytes()
    dec = decrypt(raw)

    nmesh, nbone, naction = struct.unpack_from("<hhh", dec, 36)
    print(f"player.bmd: meshes={nmesh} bones={nbone} actions={naction}")

    cursor = 42
    out = bytearray(dec[:42])  # signature + name + counts (patched below)

    # Meshes: copy verbatim.
    for _ in range(nmesh):
        v, n, t, f, tex = struct.unpack_from("<hhhhh", dec, cursor)
        size = 10 + v * 16 + n * 20 + t * 8 + f * 64 + 32
        out += dec[cursor:cursor + size]
        cursor += size

    # Actions: keep keyframes only for KEEP; zero-frame the rest.
    frames = []
    for a in range(naction):
        fc, lock = struct.unpack_from("<hB", dec, cursor)
        cursor += 3
        frames.append(fc)
        if a in KEEP:
            out += struct.pack("<hB", fc, lock)
            if lock:
                out += dec[cursor:cursor + fc * 12]
                cursor += fc * 12
        else:
            out += struct.pack("<hB", 0, 0)
            # Still need to advance over the source lock-position blob.
            if lock:
                cursor += fc * 12

    # Bones: copy headers, keep keyframes only for KEEP actions.
    for b in range(nbone):
        dummy = dec[cursor]
        cursor += 1
        out.append(dummy)
        if dummy == 0:
            out += dec[cursor:cursor + 34]  # BoneHeader
            cursor += 34
            for a in range(naction):
                if a in KEEP:
                    size = frames[a] * 12 * 2
                    out += dec[cursor:cursor + size]
                cursor += frames[a] * 12 * 2

    # Emit as v10 so the loader skips decryption entirely.
    out[3] = 0x0A
    dst.write_bytes(out)
    print(f"wrote {dst} ({len(out)} bytes, was {len(raw)})")


if __name__ == "__main__":
    main()
