DREAMCAST BUILD TOOLS
=====================

All asset/format conversion tools live in this folder. Nothing under build/
is tracked by git; this is a scratch build tree.

Scripts (this folder)
---------------------
  convert_assets.py    Webzen Data/ assets -> dc_assets/Data.
                       OZJ (24-byte prefix + JPEG) and OZT (4-byte prefix +
                       32bpp TGA) -> pvrtex .dt. Prefers OZJ when both exist
                       for the same basename (ship sails need the detail).
                       WAV -> AICA ADPCM via wav2adpcm. Other files copied.
                       Usage: python tools/convert_assets.py [--limit N]
                                  [--no-textures] [--no-sounds]

  make_login_player.py Decrypt BMD v12 player.bmd and trim keyframes for all
                       actions except the login preview set. Writes v10
                       player_login.bmd (86 KB) so the loader skips decryption.
                       Called automatically by make-cdi.cmd converted.

  make_tga_variants.py Convert ship05/06/07.OZT (32bpp alpha TGA) to
                       shipNN_t.dt (ARGB4444). The BMD references both
                       shipNN.jpg and shipNN.tga; the JPG owns the plain .dt,
                       so the alpha rigging meshes need their own files.
                       Called by make-cdi.cmd converted.

  gen_manifest.py      Scan dc_assets/Data for .dt files and write
                       Data/assets.manifest (offline asset index for the
                       streaming cache). Called by make-cdi.cmd converted.

  mkdcdisc.exe         Sega Dreamcast disc image generator v0.0.4.
                       Packs dc_assets/Data into mu_full.cdi.

raw_assets/ (one level up from tools/)
-------------------------------------
Stripped raw source files extracted from the PC client's Data/ tree. Folder
names mirror the original PC Data folders (Object1, Logo, Player, ...), and
file names are the original asset names with the Webzen prefix removed.
A BMD texture name can be looked up directly, e.g. Ship01.bmd references
"ship05.tga" -> raw_assets/Object1/ship05.tga. No decoding needed.
The "_t.tga" variants are the alpha TGA sources used for the ship rigging
.dt files.

  tga_preview.py   Create desktop-previewable PNG copies of every stripped
                   TGA in raw_assets, preserving the folder layout:
                   raw_assets/Object1/ship05.tga -> tga_previews/Object1/ship05.png
                   Useful when an image viewer won't open 32bpp TGA directly.
                   Run: python tools\tga_preview.py

A combined, desktop-browsable copy of every source image (JPG + TGA-as-PNG)
is kept at:
    ..\..\..\..\guide\references\asset previews\
Same folder layout and names as raw_assets, with TGA converted to PNG.

Binaries (bin/)
---------------
  bin/pvrtex.exe       KOS texture converter (RGB565 / ARGB4444 -> .dt).
  bin/wav2adpcm.exe    PCM WAV -> 4-bit AICA ADPCM.

  convert_assets.py prefers these local copies and falls back to the
  DreamSDK KOS utils folder if they are missing.

Other files
-----------
  convert.log              Output of the last asset conversion run.
  mkdcdisc-v0.51.exe.bak   Previous mkdcdisc version (kept as a backup).

Typical workflow (from branches\dc\build)
-----------------------------------------
  1. python tools\convert_assets.py
  2. make-cdi.cmd converted
       - rebuilds mu.elf
       - regenerates player_login.bmd
       - regenerates assets.manifest
       - packs mu_full.cdi
  3. Open mu_full.cdi in Flycast (close Flycast when done).

Notes
-----
- Only .dt textures are shipped/consumed on DC. No raw JPG/TGA decoding.
- Do not flip V for offline .dt assets; keep the screen-space Y flip.
