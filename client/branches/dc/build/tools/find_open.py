import struct, collections

BIN = r'C:\Users\usuario\Desktop\mu online\Mu-Linux-0.97.43\client\branches\dc\build\dc_assets\Data\World1\Terrain1.bin'
b = open(BIN, 'rb').read()
count = struct.unpack_from('<I', b, 8)[0]
recs = []
for i in range(count):
    o = 12 + i * 32
    t = struct.unpack_from('<I', b, o)[0]
    x, y, z = struct.unpack_from('<fff', b, o + 4)
    recs.append((t, x, y))

# Count objects within 2400 world units of each candidate grid center.
for gx in (100, 110, 120, 125, 130, 140, 150, 160, 170):
    wx = gx * 100 + 50
    for gy in (100, 120, 125, 140, 160):
        wy = gy * 100 + 50
        near = [r for r in recs if abs(r[1] - wx) < 2400 and abs(r[2] - wy) < 2400]
        trees = sum(1 for r in near if r[0] <= 0x0c)
        walls = sum(1 for r in near if 0x41 <= r[0] <= 0x4e)
        print(f'grid({gx},{gy}) near={len(near):4d} trees={trees:3d} walls={walls:3d}')
