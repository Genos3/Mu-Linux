#!/usr/bin/env bash
# Linux build for the Dreamcast branch.
#   ./build_dc.sh              ELF only
#   ./build_dc.sh converted    ELF + fixed-up DC assets + mu_full.cdi
# Environment:
#   KOS_BASE   path to KallistiOS (default: $HOME/dreamsdk/kos)
#   KOS_PORTS  path to kos-ports  (default: $HOME/dreamsdk/kos-ports)
set -euo pipefail
cd "$(dirname "$0")"

: "${KOS_BASE:="$HOME/dreamsdk/kos"}"
: "${KOS_PORTS:="$HOME/dreamsdk/kos-ports"}"
export KOS_BASE KOS_PORTS
export PATH="$KOS_BASE/utils/build_wrappers:$PATH"

if [ ! -d "$KOS_BASE" ]; then
    echo "ERROR: KOS_BASE not found: $KOS_BASE (set KOS_BASE to your KallistiOS tree)" >&2
    exit 1
fi

# Same layout build-dc.cmd creates with mklink /J, using symlinks.
mkdir -p .build
ln -sfn ../src src
ln -sfn ../../../cmake cmake
ln -sfn ../../../lib lib
ln -sfn ../../../lib ../../lib
ln -sfn ../lib ../lib

echo "[1/2] Building mu.elf (KOS at $KOS_BASE) ..."
make -f Makefile.kos -j4

if [ "${1:-}" != "converted" ]; then
    echo "Done. mu.elf is in $(pwd) (run with ./build_dc.sh converted to pack the CDI)"
    exit 0
fi

echo "[2/2] Preparing converted assets ..."
python3 tools/make_login_player.py dc_assets/Data/Player/player.bmd dc_assets/Data/Player/player_login.bmd
python3 tools/make_tga_variants.py
python3 tools/gen_manifest.py dc_assets/Data
python3 tools/make_obj_bin.py dc_assets/Data
DATA="dc_assets/Data"

# mkdcdisc is a Windows binary; use a native build if available, else Wine.
if command -v mkdcdisc >/dev/null 2>&1; then
    PACK=(mkdcdisc)
elif command -v wine >/dev/null 2>&1 && [ -f tools/mkdcdisc.exe ]; then
    PACK=(wine tools/mkdcdisc.exe)
else
    echo "ERROR: no CDI packer found. Install mkdcdisc on PATH, or wine + tools/mkdcdisc.exe" >&2
    exit 1
fi

echo "Packing CDI from $DATA ..."
"${PACK[@]}" -e mu.elf -d "$DATA" -o mu_full.cdi --allow-overwrite
echo "CDI: $(pwd)/mu_full.cdi"
