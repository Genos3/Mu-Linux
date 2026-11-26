@echo off
rem Build mu.elf + mu_full.cdi in this folder.
rem   make-cdi.cmd              CDI with the original Data/ tree
rem   make-cdi.cmd converted    CDI with Data_dc\ (after convert-assets.cmd)
rem   make-cdi.cmd run          build then launch the CDI in Flycast
setlocal
cd /d "%~dp0."

call "%~dp0build-dc.cmd"
if errorlevel 1 exit /b 1

set "DATA=%~dp0..\..\..\bin\Client\Data"
if /i "%1"=="converted" set "DATA=%~dp0dc_assets\Data"

if /i "%1"=="converted" (
  rem Login preview needs a trimmed player.bmd to fit the 16 MB heap.
  python tools\make_login_player.py "%~dp0dc_assets\Data\Player\player.bmd" "%~dp0dc_assets\Data\Player\player_login.bmd"
  rem Alpha-preserving .dt variants for the ship rigging (.tga/.OZT sources).
  python tools\make_tga_variants.py
  rem Asset index for the DC ring-buffer streaming cache.
  python tools\gen_manifest.py "%~dp0dc_assets\Data"
  rem Pre-decrypted, aligned binary map objects (no runtime decryption on DC).
  python tools\make_obj_bin.py "%~dp0dc_assets\Data"
)

echo Packing CDI from %DATA% ...
tools\mkdcdisc.exe -e mu.elf -d "%DATA%" -o mu_full.cdi --allow-overwrite
if errorlevel 1 exit /b 1
echo CDI: %~dp0mu_full.cdi

if /i "%1"=="run" start "" "C:\Users\usuario\Desktop\my_folder\dev\guide\sources\flycast\bin\flycast.exe" "%~dp0mu_full.cdi"
