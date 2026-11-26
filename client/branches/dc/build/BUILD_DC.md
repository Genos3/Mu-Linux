# Building the Dreamcast port

All commands run from this folder (`client/branches/dc/build`). Double-click the
`.cmd` files or run them from a command prompt.

## First time (or after game assets change)

```
convert-assets.cmd
```

Converts the PC assets into `dc_assets\Data` (~2000 files, slow). This is a
**manual prerequisite** — `make-cdi.cmd converted` does not run it. You only
need to repeat this when the original assets change; it is not needed for
source-only edits.

## Normal build

```
make-cdi.cmd converted
```

This:
1. calls `build-dc.cmd` -> `mu.elf` (KOS/DreamSDK toolchain)
2. runs the small Python fixups (login player trim, `.dt` TGA variants, asset
   manifest, pre-decrypted object binaries) against `dc_assets\Data`
3. packs `mu_full.cdi` with the converted Data tree

For source-only edits this one command is enough — no need to re-run
`convert-assets.cmd`.

## Other commands

| Command | Purpose |
|---|---|
| `build-dc.cmd` | compile only, produces `mu.elf` (no CDI) |
| `rebuild.cmd` | clean full `-j4` rebuild of `mu.elf`; needed after editing shared headers |
| `run-flycast.cmd` | launch `mu_full.cdi` in the current Flycast |
| `snapshot-src-dc.cmd` | commit a local git backup of `..\src` before risky edits |
| `make-cdi.cmd` (no args) | builds a CDI from the *original* `bin\Client\Data` tree — not the one used for testing |

`mu.elf`, `*.cdi`, `*.log` and generated folders are gitignored here.

## Test scene

The DC build is compiled with `MU_TEST_SCENE_MAP` (in `src\globals.cpp`), so it
boots straight into Lorencia at grid (125,125) — no login/server needed.
Serial output goes to Flycast's `bin\serial.log` (look for `SCENE: 5 ->
INGAME/MAP`).
