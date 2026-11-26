import collections

KEY = bytes([0xD1,0x73,0x52,0xF6,0xD2,0x9A,0xCB,0x27,
             0x3E,0xAF,0x59,0x31,0x37,0xB3,0xE7,0xA2])
P = r'C:\Users\usuario\Desktop\mu online\Mu-Linux-0.97.43\client\branches\dc\build\dc_assets\Data\World1\EncTerrain1.map'
enc = open(P, 'rb').read()
dec = bytearray(len(enc))
w = 0x5E
for i, c in enumerate(enc):
    dec[i] = ((c ^ KEY[i & 15]) - w) & 0xFF
    w = (c + 0x3D) & 0xFF

l1 = dec[2:2 + 0x10000]
for gx, gy in [(125,125), (160,160), (170,160), (100,100)]:
    vals = collections.Counter()
    for y in range(gy-8, gy+9):
        for x in range(gx-8, gx+9):
            vals[l1[y*256+x]] += 1
    print(f'grid({gx},{gy})', dict(vals.most_common(8)))
