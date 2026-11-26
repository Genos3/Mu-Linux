import re, struct, os

SRC = r'C:\Users\usuario\Desktop\mu online\Mu-Linux-0.97.43\client\branches\dc\src\Terrain\Terrain_LegacyLoad.cpp'
DIR = r'C:\Users\usuario\Desktop\mu online\Mu-Linux-0.97.43\client\branches\dc\build\dc_assets\Data\Object1'
BIN = r'C:\Users\usuario\Desktop\mu online\Mu-Linux-0.97.43\client\branches\dc\build\dc_assets\Data\World1\Terrain1.bin'

text = open(SRC).read()
mapping = {int(a, 16): b for a, b in re.findall(r'\{\s*0x([0-9a-fA-F]+)\s*,\s*"([^"]+)"', text)}
b = open(BIN, 'rb').read()
count = struct.unpack_from('<I', b, 8)[0]
types = {struct.unpack_from('<I', b, 12 + i * 32)[0] for i in range(count)}

total = 0
found = 0
missing = []
for t in sorted(types):
    name = mapping.get(t)
    if not name:
        missing.append((t, 'no mapping'))
        continue
    path = os.path.join(DIR, name + '.bmd')
    if os.path.exists(path):
        total += os.path.getsize(path)
        found += 1
    else:
        missing.append((t, name))

print('objects', count, 'distinct types', len(types))
print('mapped models', found, 'total model file bytes', total, '=', round(total / 1048576, 2), 'MB')
print('missing', len(missing), missing[:20])
print('object structs bytes', count * 444, '=', round(count * 444 / 1048576, 2), 'MB')
