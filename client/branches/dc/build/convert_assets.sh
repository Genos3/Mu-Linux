#!/usr/bin/env bash
# Convert PC assets to DC formats (first-time prerequisite).
#   ./convert_assets.sh [--limit N] [--no-sounds]
set -euo pipefail
cd "$(dirname "$0")"
python3 tools/convert_assets.py "$@"
