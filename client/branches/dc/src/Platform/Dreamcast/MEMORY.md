# Dreamcast memory layout (measured 2026-11-23)

The DC has **16 MB main RAM** (0x8C000000–0x8D000000) and a separate
**8 MB PVR VRAM**. KOS grows the `malloc` heap upward from the end of the
program's BSS until `_arch_mem_top - THD_KERNEL_STACK_SIZE`; when that fails
you get `Out of memory. Requested sbrk_base ...` followed by `std::bad_alloc`.

## ELF sections (`sh-elf-size -A mu.elf`)

| Section | Size | Where | Notes |
|---|---:|---|---|
| `.text` | 1,567,704 B (~1.5 MB) | main RAM | code |
| `.rodata` | 601,256 B (~587 KB) | main RAM | tables, strings |
| `.data` | 1,105,400 B (~1.05 MB) | main RAM | initialized globals |
| `.bss` | 8,690,824 B (~8.3 MB) | main RAM | zeroed globals |
| `.eh_frame` + `.gcc_except_table` | 166,164 B | main RAM | unwind tables |
| **Program total** | **~12.1 MB** | | |
| KOS, thread stacks, framebuffer etc. | ~1–2 MB | | |
| **Available `malloc` heap** | **~4 MB** | | observed arena max ~3.7 MB |

Note: the 640x480 RGB565 display framebuffer (~600 KB) and the 1 MB PVR
vertex buffer live in **VRAM**, not main RAM, so removing them would not help
heap pressure. The framebuffer is required for display.

## What fills `.bss`

Top static arrays (from `sh-elf-nm -S --size-sort -C mu.elf`):

| Symbol | Size | Purpose |
|---|---:|---|
| `JointPoolBase` | 1,261,792 B (1.20 MB) | 500 skeletal joints (evaluated bone matrices), ~2.5 KB each |
| `g_BoneVertexBuf` | 786,432 B (768 KB) | skinned positions: 32 meshes × 2048 verts × 12 B |
| `g_BoneLightBuf` | 786,432 B (768 KB) | skinned lighting values, same layout |
| `g_BoneChromeNormalBuf` | 786,432 B (768 KB) | skinned "chrome" normals, same layout |
| `SocketClient` | 804,124 B (785 KB) | net context: 8 KB send + 8 KB recv + 96 packet slots × 8 KB |
| `parts` (anon, `.data`) | 794,492 B (776 KB) | part definitions |
| `GroundItemPool` | 515,488 B (503 KB) | 1000 dropped items |
| `EffectPoolArray` | 445,400 B (435 KB) | 1002 transient visual effects |
| `simulationSprites` | 445,400 B (435 KB) | **removed** — render-interpolation snapshot (see below) |
| `SpritePoolBase` | 335,488 B (328 KB) | 3000 particles |
| `GlobalText` | 300,000 B (293 KB) | localized UI text |
| `g_BoneNormalBuf` | 262,144 B (256 KB) | skinned normals: 32 × 2048 × 4 B |
| `DialogScriptTable` | 204,800 B (200 KB) | NPC dialogue scripts |
| `items` (anon, `.data`) | 180,000 B (176 KB) | item definitions |
| terrain layers + A* statics | ~416 KB | 3×64 KB terrain, 128+64+32 KB pathfinding |

Those ~15 arrays account for nearly all of `.bss`.

`kMeshVertStride` is already DC-reduced from 15000 to 2048 (largest shipped
BMD has 1116 vertices). `SocketClientSlots` is 96 (PC: 300), `kJointPoolSize`
500 (PC: 2000), `kSpritePoolSize` 1002 (PC: 3000).

## Measured runtime heap (`mallinfo()` once/second, login scene)

| Moment | Used | Free | Arena | Free chunks |
|---|---:|---:|---:|---:|
| just after boot | 2,243,712 B | 37,272 B | 2,280,984 B | 1 |
| after login textures load | 3,227,872 B | ~490,000 B | 3,710,488 B | 17–18 |
| steady state (115 s) | 3,230,600 B | ~480,000 B | ~3,710,480 B | 17 |

The heap is **flat** after loading — there is no per-frame leak. The problem
was fragmentation: ~480 KB total free split across 17 chunks, so a single
128 KB request could fail.

## OOM root cause and fix (commit `e3900e8`)

`DCPVR_LoadDT` used to stage each whole `.dt` in a `std::vector` (128 KB for a
player skin, 256 KB for a sky). The login scene loads a dozen 131 KB skins plus
two 256 KB skies; after that the heap was fragmented enough that the next
128 KB staging allocation threw `std::bad_alloc`.

Fix: stream the `.dt` payload straight into PVR VRAM in 16 KB store-queue
copies (`pvr_txr_load`) using a small stack buffer. No large main-RAM staging
allocation. Verified byte-identical frame and zero OOM over 115 s.

## Simulation sprite snapshot (removed)

`simulationSprites` (435 KB) was a port-side addition, not original client
logic: the original drew once per 24 fps simulation tick, while this port
decouples render (60 fps) from simulation (25 fps). `Sprite_CaptureStep()`
copied the effect pool into the snapshot at the end of a SIM step, cleared the
live pool, and `Sprite_ReplayStep()` restored it at render start so effects
would not disappear on render-only frames.

Now the renderer no longer consumes sprites (`Render_DrawSpritePool` no longer
clears the active flag); stale slots are cleared once at the start of each SIM
step. The 435 KB array and both helpers are gone in all source trees (DC,
`client/src`, `client/branches/test/src`). Trade-off: effects are redrawn at
their SIM-time positions on render-only frames rather than being interpolated
to owner motion — no loss for the login scene.

## Asset ring cache footprint

`Platform/Dreamcast/DCAssetCache.*` prototypes a 2 × 40 KB ring (80 KB) fed by
a KOS worker thread, prefetching login ship textures from `Data/assets.manifest`
(generated offline by `tools/gen_manifest.py`). With the sprite array removed,
OOM is gone. An earlier 4 × 64 KB ring + a 143 KB runtime manifest table
pushed the heap over the edge — keep the cache footprint small until the big
static pools are reduced.

## Candidate further reductions (need visual testing)

1. Bone output buffers: 32 mesh slots → 8 saves ~1.9 MB (four parallel arrays).
2. `GroundItemPool` 1000 → 300 saves ~350 KB.
3. `SocketClientSlots` 96 → 32 saves ~500 KB.
4. `EffectPoolArray` and `SpritePoolBase` sizes vs actual peak usage.

These sizes are tied to IDA-verified offsets/gameplay behavior; validate each
with in-game tests before committing.
