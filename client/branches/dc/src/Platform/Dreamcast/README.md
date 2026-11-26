# Platform/Dreamcast - KallistiOS backend skeleton

Native Dreamcast backend for the Mu 0.97k client. **No SDL, no OpenGL** -
window creation, input, audio, networking and (eventually) rendering are KOS
or KOS/PVR. The shared game code never includes KOS headers beyond
`Platform/Posix/WinTypes.h` (an LP32 type shim); it only sees the `Platform::I*`
interfaces, exactly like the SDL3 and Win32 backends.

Build skeleton: see `../build/Makefile.kos` and `../build/build-dc.cmd`
(run from `branches\dc\build`). It excludes the PC backends
(`Platform/Win32`, `Platform/SDL3`, `Render/Backend/GL`, `Render/Backend/WebGPU`),
`GameGuard/` and `Text/TextShaper.cpp` (no HarfBuzz on DC). `branches\dc\build`
is a scratch tree like `branches\test\build`: junctions to `..\src`, `lib`
(the client's shared `lib/`, used via `..\..\lib` and `..\..\..\lib` includes)
and `cmake`, plus the dependency dir (`.build/`). Build artifacts stay inside
`branches\dc\build`; the worktree itself is kept clean (`*.o` are git-ignored).

## Build status (verified 2026-11-19)

- **The whole in-scope tree compiles for SH-4** (`347/369` translation units;
  the 22 expected failures are the PC backends + HarfBuzz, excluded by the
  makefile). All GL-3.3/WebGPU/SDL3/Win32 code paths have working stubs or
  DC implementations.
- **`mu.elf` links and fits Dreamcast RAM.**
  - `.text` 2.64 MB, `.data` 1.09 MB, **`.bss` 8.7 MB** — total 12.4 MB,
    verified with `sh-elf-size`.
  - Started at **40 MB BSS** (would not even load); now comfortably fits a
    16 MB Dreamcast with room for heap growth (music decode, model streaming).
- Applied DC-only cuts (all other builds unchanged): mesh stride `kMeshVertStride`
  15000 → 2048 (max vertex across all shipped BMDs: 1116),
  `kEffectPoolSize` 1000 → 200, `kJointPoolSize` 2000 → 500,
  `kParticlePoolSize` 6000 → 3000, `kSpritePoolSize` 3000 → 1002,
  `SocketClient` 300 → 96 slots, `GLOBALTEXT_ROWS` 4000 → 1000,
  `MiniMap` texture buffer moved off BSS to a heap temporary,
  and the nine big terrain/light/wind tables (~4.5 MB) moved to lazy heap
  allocations (`DCEnsureTerrainTables()`, called from `Terrain_Clear()`).
  `mu.elf` is a build artifact and is intentionally not tracked;
  `make -f Makefile.kos clean` removes it and the `.build/` dep dir.
- Also fixed: three untracked duplicate sources in `Render/` (`Crypto.cpp`,
  `Cloth_Simulation.cpp`, `ObjectPose.cpp`) were byte-identical copies of
  tracked files (`Render/BMD_LegacyEffects.cpp`, `Render/Entity_DrawByType.cpp`);
  they broke the build with duplicate symbols and were deleted with user
  approval (backups in `opencode/dc_dup_backup/`).
- Next highest-impact item after the renderer is the asset pipeline
  (GD-ROM/ISO9660 image, offline `.OZJ/.OZT` → `.pvr` conversion, streaming
  hooks) rather than further micro-optimising the decompiled pools.
- Boot milestones on hardware/Flycast (2026-11-19): KOS crt0 → `main()` →
  `DCWindow`/`DM_640x480`/`pvr_init()` succeeds, `InitOpenGL` proceeds past
  the `BufferedDraw` stub, then `HWID_Init`/`HWID_Send`. Everything prints on
  the serial console (`DCLog`/`printf`; the game's `DbgLog` writes only to a
  host-side `debug.log` and is invisible on DC).
- The `.cdi` used for bring-up is built with `mkdcdisc -e mu.elf -d <assets>`
  and opened in Flycast by pointing it at the `.cdi`. The assets are placed
  under `Data/...` inside the disc image, so the iso9660 driver mounts them.

## What exists today

| File | Role | Status |
|---|---|---|
| `DCPlatform.h` | umbrella: `<kos.h>`, LP32 shim, `DCLog()`, `DC_NYI` | works |
| `DCInit.cpp` | `KOS_INIT_FLAGS(INIT_DEFAULT \| INIT_NET)` + `DCLog()` | works |
| `DCWindow.cpp` | video mode `DM_640x480/PM_RGB565`, `pvr_init()`, `pump` = maple | works, minimal |
| `DCPresent.cpp` | `IGLContext` over `pvr_wait_ready()` | stub |
| `DCTimer.cpp` | QueryPerformanceCounter-compatible µs timer (`timer_*_gettime64`) | works |
| `DCInput.cpp` | maple mouse/keyboard/controller → `InputEvent`s + `KeyState()` | works, basic |
| `DCSocket.cpp` | BSD sockets port of the SDL3 backend (`select()`, `O_NONBLOCK`) | works (untested on HW) |
| `DCFileSystem.cpp` | ISO9660 `fopen` + case-insensitive retry, minimal INI reader | partial |
| `DCIdentity.cpp` | HWID from BBA MAC, persisted on VMU (`/vmu/a1/mu_hwid.bin`) | works-ish |
| `DCDiagnostics.h` | console + best-effort `debug.log` | stub |
| `DCFont.h/.cpp` | box-glyph stub font + trivial `FontStack` (UTF-8 pass-through) | placeholder |
| `DCNativeBridge.cpp` | `BindNativeDispatch` → game event sinks (replaces `SDLNativeBridge`) | works |
| `../../Render/Backend/DC/DCRenderStubs.cpp` | links the binary (no drawing yet) | placeholder |

Shared-source guards added for `__DREAMCAST__`:
`Platform/PlatformHeaders.h`, `Platform/CompilerCompat.h` (attribute shims +
`-Wattributes` silencing), `Platform/Identity.h`, `Platform/Diagnostics.h`,
`types.h` (`float10` → double), `functions.h` (same), `Render/TransformWorkers.h`
(no SSE), `Text/TextShaper.{h,cpp}`, `Text/TextAtlas.cpp`,
`Platform/FontFrontend.cpp` (`Platform::PortableFont` = `DCFont` on DC),
`Platform/ClientEntry.cpp`, `Sound/SoundManager.cpp` (miniaudio sections
stubbed; AICA backend TODO), `Platform/Text.h` and the 32 sites using
`std::snprintf`/`std::vsnprintf` (rewritten to `::`, which is where newlib
puts them). `branches\dc\lib` is a **copy** of the client's `lib/` (~7 MB:
utf8proc, libunibreak, miniaudio, stb) so the `../../lib` relative includes
resolve from this branch.

## How the frame flows on DC

```
ClientEntry::main (non-Windows branch, unchanged)
  └─ for (;;)  Platform::GetWindow().ServiceEvents()      → DCWindow → DCInputPump()
               ├─ maple poll (mouse/keyboard/controller)
               ├─ InputEvents → Input_OnEvent (game code)  → chat text, clicks, cursor
               └─ IInput::KeyState(VK_*) for held keys
       Scene_Dispatch(hdc)                                 → game logic (unchanged)
       Platform::GetGLContext().Present()                  → DCPresent::Present() (PVR drain)
```

## Known gaps (in order of pain)

1. **Renderer**: `Render/Backend/DC/` currently contains only `DCRenderStubs.cpp`
   (empty `GfxDevice::GL::*` symbols so it links). The real one implements
   `GfxDevice::PVR::*`, `BufferedDraw::PVR::*`, `MeshDraw::PVR::*`,
   `Skinning::PVR::*` (same X-macros, `Render/Backend/Api.h`), twiddled 16-bit
   or 4/8-bpp palettized textures, TA opaque/translucent passes, fog via
   `pvr_set_fog_table`, alpha-test via `ISP_ALPHATEST`, blob shadows instead of
   shadow maps. `MU_RENDER_WEBGPU` is never defined on DC; once PVR lands, the
   stub is deleted.
2. **Audio**: `IAudio` backend for AICA (`dc/sound/sfxmgr.h` for SFX, `stream.h`
   + `libmp3`/`libtremor` for the music) — the miniaudio backend does not run on
   DC. PCM `.wav` SFX load into SPU RAM (2 MB) with an LRU; music streams.
3. **Text**: `DCFont` is a box-glyph stub. Wire `kos-ports/freetype` (or a tiny
   fixed bitmap font for ASCII+CP1252) — `DCFont.cpp` keeps the API to drop it in.
4. **INI**: `WriteIniString` is NYI; writable `Config.ini` on VMU/SD pending a
   mount (`fs_vmu` or `libkosfat`).
5. **Keyboard mapping**: digits/punctuation beyond ASCII, arrows as text-mode
   navigation, dead keys — all TODO in `DCInput.cpp`.
6. **`<filesystem>`/`<thread>`**: `DataPrefetch.cpp` and friends use the C++
   stdlib (`std::filesystem`, `std::thread`); they need KOS builds of the
   standard library or an isolation pass. Not attempted yet.

## Map object streaming (design note)

The binary already keeps static map objects in a spatial cell grid:
`Scene/SceneObjectCell.h` (IDA 0x004FF5A0): linked-list head/tail per cell +
`Visible` byte, 16 bytes/cell on 32-bit - the 0.97k renderer iterates object
cells by view frustum, which is exactly the hook DC needs for RAM budgeting:

- Keep the **cell grid + `Visible` culling** untouched.
- Add a **residency tier** on top: a cell is "resident" when the player is
  within `R_near` of it, "loaded" within `R_load`, and nothing at all beyond
  `R_evict`. Loading = `.obj`/`.att` decode into a small LRU; eviction drops
  the BMD handle and its textures from VRAM.
- Budget target: ~6-8 MB of models/textures for the *current* map, swap-in
  only near/visible cells, GD-ROM seek time ~40x is the lower bound for
  swap-in, so keep `R_load` comfortably inside the fog distance and stream on
  the scene graph side (`Scene_ObjectTick` / `Scene_ObjectUpdate`), not in
  `Terrain.cpp`.
- Textures for `.OZJ/.OZT` should be converted offline to palettized `.pvr`
  (or VQ with `vqenc`) and prebound to `.att` blocks; Oz decompression at
  runtime on SH-4 is the wrong place for it.

This is the path for "only objects near the player are loaded" - it doesn't
require changing the cell data layout, just the load/evict policy around it.

## Asset packing: use mkdcdisc v0.0.4+

Older mkdcdisc (v0.51, 2025) keyed cached file data by **file size**: two files
with the same byte count could serve the same content from the CDI. That made
`Webzenlogo.dt` and `Explotion01.dt` (both 131104 bytes) collide, and
`Everyone.dt` (65568 bytes) collide with ~70 other files.

This was fixed upstream in mkdcdisc **v0.0.4** (2026). `tools/mkdcdisc.exe`
in this build tree is the fixed version, built from
https://gitlab.com/simulant/mkdcdisc with meson (`MSYSTEM=MINGW64`). No
asset padding workaround is needed. If a texture ever shows another asset's
image, check the mkdcdisc version first.

## Rendering notes

`DCPVR::Submit()` applies the model-view-projection matrix and viewport to
each vertex (the GL backend does this in its vertex shader). Without it the
login background was drawn in raw world coordinates and its textures tiled
across the framebuffer. Y is flipped from GL's bottom-up convention to PVR's
top-down framebuffer. Texture V is **not** flipped — offline `.dt` files
already store rows top-down, so flipping V would invert them.

## Licensing

KOS is BSD-like with mandatory attribution (see `kos/doc/FAQ.md`: "THE KOS
LICENSE DOES NOT ALLOW USE WITHOUT ATTRIBUTION") - ship a credits line.
`DCIdentity.cpp` derives an HWID from the BBA MAC; that MAC never leaves the
console (it is hashed into the 35-char string before use).
