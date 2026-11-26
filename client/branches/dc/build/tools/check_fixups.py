"""Print 1 if the DC asset fixups need re-running, else 0.

The fixups (player_login trim, .dt TGA variants, manifest, object binaries)
are deterministic. They only need to run when something inside dc_assets/Data
changed -- not for source-only rebuilds. A stamp file written after the last
fixup run is compared against the newest file in the tree.
"""
import os
import sys

root = sys.argv[1] if len(sys.argv) > 1 else "dc_assets"
data = os.path.join(root, "Data")
stamp = os.path.join(root, ".fixups_done")

newest = 0
for dirpath, _, filenames in os.walk(data):
    for name in filenames:
        try:
            mtime = os.path.getmtime(os.path.join(dirpath, name))
        except OSError:
            continue
        if mtime > newest:
            newest = mtime

need = not os.path.exists(stamp) or newest > os.path.getmtime(stamp)
print(1 if need else 0)
