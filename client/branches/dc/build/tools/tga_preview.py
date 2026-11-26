#!/usr/bin/env python3
"""Create desktop-previewable PNG copies of the stripped TGA assets.

Input:  build/raw_assets/**/*.tga   (Webzen OZT/OZT sources, prefix stripped)
Output: build/tga_previews/**/*.png (same relative path/name, PNG format)

PC reference folders are preserved, e.g. raw_assets/Object1/ship05.tga becomes
tga_previews/Object1/ship05.png.
"""
import struct
import sys
import zlib
from pathlib import Path


def read_tga(path):
    d = path.read_bytes()
    if len(d) < 18 or d[2] != 2 or d[16] != 32:
        return None                      # only uncompressed 32bpp TGA
    id_len = d[0]
    w, h = struct.unpack_from('<HH', d, 12)
    desc = d[17]
    px = d[18 + id_len:]
    if len(px) < w * h * 4:
        return None
    bottom_up = (desc & 0x20) == 0
    rows = []
    for y in range(h):
        sy = (h - 1 - y) if bottom_up else y
        row = bytearray()
        for x in range(w):
            i = (sy * w + x) * 4
            b, g, r, a = px[i], px[i + 1], px[i + 2], px[i + 3]
            row += bytes((r, g, b, a))
        rows.append(bytes(row))
    return w, h, rows


def write_png(path, w, h, rows):
    raw = b''.join(b'\0' + r for r in rows)

    def chunk(tag, data):
        return (struct.pack('>I', len(data)) + tag + data +
                struct.pack('>I', zlib.crc32(tag + data) & 0xffffffff))

    path.write_bytes(b'\x89PNG\r\n\x1a\n' +
                     chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0)) +
                     chunk(b'IDAT', zlib.compress(raw)) +
                     chunk(b'IEND', b''))


def main():
    build = Path(__file__.replace("\\", "/")).resolve().parent.parent
    src = build / "raw_assets"
    dst = build / "tga_previews"
    if not src.exists():
        print(f"missing {src}")
        return 1
    made = skipped = 0
    for tga in sorted(src.rglob("*.tga")):
        parsed = read_tga(tga)
        if parsed is None:
            skipped += 1
            continue
        w, h, rows = parsed
        out = dst / tga.relative_to(src).with_suffix(".png")
        out.parent.mkdir(parents=True, exist_ok=True)
        write_png(out, w, h, rows)
        made += 1
    print(f"wrote {made} PNG previews to {dst} (skipped {skipped} unsupported TGA)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
