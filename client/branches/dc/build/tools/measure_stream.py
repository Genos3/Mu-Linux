import struct, collections

BIN = r'C:\Users\usuario\Desktop\mu online\Mu-Linux-0.97.43\client\branches\dc\build\dc_assets\Data\World1\Terrain1.bin'
b = open(BIN, 'rb').read()
n = struct.unpack_from('<I', b, 8)[0]
recs = []
for i in range(n):
    o = 12 + i * 32
    t = struct.unpack_from('<H', b, o)[0]
    x, y, z = struct.unpack_from('<fff', b, o + 4)
    recs.append((t, x, y))

for gx, gy in [(125, 125), (160, 160)]:
    cx, cy = int((gx * 100 + 50) * 0.000625), int((gy * 100 + 50) * 0.000625)
    visible = [r for r in recs if abs(int(r[1] * 0.000625) - cx) <= 2
                              and abs(int(r[2] * 0.000625) - cy) <= 2]
    types = {r[0] for r in visible}
    print(f'grid({gx},{gy}): visible objects={len(visible)}  OBJECT RAM={len(visible)*444} B '
          f'({len(visible)*444/1048576:.2f} MB)  distinct model types={len(types)}')

print(f'eager: {n} objects -> {n*444} B ({n*444/1048576:.2f} MB) + 109 BMDs 1.82 MB = ~3.0-3.5 MB')
