#!/usr/bin/env python3
"""Convert Webzen MU 0.97k Data/ assets into Dreamcast-friendly formats.

Runs entirely inside branches/dc/build. Outputs a new Data tree (default:
Data_dc) that can be fed to mkdcdisc via make-cdi.cmd.

Formats (verified against the reconstruction's loaders):
  *.OZJ  = 24-byte Webzen prefix + JPEG      -> strip prefix -> JPEG -> pvrtex
  *.OZT  = 4-byte  Webzen prefix + 32bpp TGA -> strip prefix -> TGA   -> pvrtex
  *.WAV  = PCM                              -> wav2adpcm -t (AICA ADPCM)
  *.BMD  = obfuscated, NOT compressed       -> copied verbatim (game decrypts it)
  other  (.ini/.txt/...)                    -> copied verbatim

Usage:
  python tools/convert_assets.py [--limit N] [--no-textures] [--no-sounds]
"""
import argparse
import pathlib
import shutil
import subprocess
import sys

# MSYS Python hands us mixed separators in __file__ ("tools\\convert_assets.py"),
# so normalize before pathlib computes parents.
BUILD = pathlib.Path(__file__.replace("\\", "/")).resolve().parent.parent
LOCAL_BIN = BUILD / "tools" / "bin"
KOS = pathlib.Path("C:/games/DreamSDK/opt/toolchains/dc/kos/utils")


def pick_tool(local, fallback):
    """Prefer the copy shipped in build/tools/bin; fall back to KOS utils."""
    return local if local.is_file() else fallback


PVRTEX = pick_tool(LOCAL_BIN / "pvrtex.exe", KOS / "pvrtex" / "pvrtex.exe")
WAV2ADPCM = pick_tool(LOCAL_BIN / "wav2adpcm.exe", KOS / "wav2adpcm" / "wav2adpcm.exe")
DEFAULT_SRC = BUILD.parents[2] / "bin" / "Client" / "Data"


def run(cmd, log):
    log.write("+ " + " ".join(str(c) for c in cmd) + "\n")
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    log.write(result.stdout.decode("utf-8", "replace"))
    return result.returncode == 0


def strip_prefixes(src, raw, log):
    """OZJ: skip 24 bytes (small JFIF header; real JPEG SOI is at offset 24).
    OZT: skip 4 bytes. Keep the rest byte-identical."""
    counts = {"ozj": 0, "ozt": 0}
    files = [p for p in sorted(src.rglob("*"))
             if p.is_file() and p.suffix.lower() in (".ozj", ".ozt")]
    # When both .OZJ (detailed JPEG) and .OZT (tiny TGA) exist for the same
    # texture, the .OZT would overwrite the .dt during conversion. Prefer the
    # OZJ so ship sails/decorations keep their real detail.
    ozj_bases = {p.with_suffix("") for p in files if p.suffix.lower() == ".ozj"}
    for path in files:
        ext = path.suffix.lower()
        if ext == ".ozt" and path.with_suffix("") in ozj_bases:
            log.write(f"SKIP short-duplicate {path} (OZJ preferred)\n")
            continue
        skip = 24 if ext == ".ozj" else 4
        out_ext = ".jpg" if ext == ".ozj" else ".tga"
        rel = path.relative_to(src).with_suffix(out_ext)
        dest = raw / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        data = path.read_bytes()
        if len(data) <= skip:
            log.write(f"SKIP short {path}\n")
            continue
        dest.write_bytes(data[skip:])
        counts[ext[1:]] += 1
    return counts


def convert_textures(raw, out, limit, log):
    images = sorted(p for p in raw.rglob("*") if p.suffix.lower() in (".jpg", ".tga"))
    if limit:
        images = images[:limit]
    ok = 0
    for image in images:
        rel = image.relative_to(raw).with_suffix(".dt")
        dest = out / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        # VQ-compressed, dithered, resized to a valid PVR size. AUTO picks
        # RGB565 / ARGB1555 / ARGB4444 depending on the source alpha.
        cmd = [PVRTEX, "-i", image, "-o", dest,
               "-f", "auto", "-d", "-r", "near"]
        if run(cmd, log):
            ok += 1
    return ok, len(images)


def convert_sounds(src, out, limit, log):
    import wave
    sounds = sorted(p for p in src.rglob("*") if p.suffix.lower() == ".wav")
    if limit:
        sounds = sounds[:limit]
    ok = 0
    raw = BUILD / "raw_assets" / "sound16"
    raw.mkdir(parents=True, exist_ok=True)
    for sound in sounds:
        rel = sound.relative_to(src).with_suffix(".adpcm.wav")
        dest = out / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        # wav2adpcm only accepts 16-bit input; normalize 8-bit WAVs first.
        source = sound
        try:
            with wave.open(str(sound), "rb") as wav:
                if wav.getsampwidth() == 1:
                    with wave.open(str(raw / sound.name), "wb") as fixed:
                        fixed.setnchannels(wav.getnchannels())
                        fixed.setsampwidth(2)
                        fixed.setframerate(wav.getframerate())
                        frames = wav.readframes(wav.getnframes())
                        fixed.writeframes(b"".join(
                            bytes((0, (b - 128) & 0xFF)) for b in frames))
                    source = raw / sound.name
        except Exception as exc:
            log.write(f"WAV normalize failed {sound}: {exc}\n")
        if run([WAV2ADPCM, "-t", source, dest], log):
            ok += 1
    return ok, len(sounds)


def copy_other(src, out, log):
    count = 0
    for path in sorted(src.rglob("*")):
        if not path.is_file():
            continue
        if path.suffix.lower() in (".ozj", ".ozt", ".wav", ".bmp"):
            continue
        rel = path.relative_to(src)
        dest = out / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, dest)
        count += 1
    return count


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--src", type=pathlib.Path, default=DEFAULT_SRC)
    ap.add_argument("--out", type=pathlib.Path, default=BUILD / "dc_assets" / "Data")
    ap.add_argument("--limit", type=int, default=0, help="only first N images/sounds (test)")
    ap.add_argument("--no-textures", action="store_true")
    ap.add_argument("--no-sounds", action="store_true")
    ap.add_argument("--sounds-only", action="store_true", help="only redo the ADPCM sounds")
    args = ap.parse_args()

    raw = BUILD / "raw_assets"
    if raw.exists():
        shutil.rmtree(raw)
    raw.mkdir(parents=True, exist_ok=True)
    args.out.mkdir(parents=True, exist_ok=True)
    logfile = BUILD / "tools" / "convert.log"
    with logfile.open("w", encoding="utf-8") as log:
        log.write(f"src={args.src}\nout={args.out}\n")
        if not args.sounds_only:
            stripped = strip_prefixes(args.src, raw, log)
            log.write(f"stripped: {stripped}\n")
            copied = copy_other(args.src, args.out, log)
            log.write(f"copied unchanged: {copied}\n")
        if not args.no_textures and not args.sounds_only:
            ok, total = convert_textures(raw, args.out, args.limit, log)
            log.write(f"pvrtex: {ok}/{total}\n")
        if not args.no_sounds:
            ok, total = convert_sounds(args.src, args.out, args.limit, log)
            log.write(f"wav2adpcm: {ok}/{total}\n")
    print(f"Done. Log: {logfile}")


if __name__ == "__main__":
    sys.exit(main())
