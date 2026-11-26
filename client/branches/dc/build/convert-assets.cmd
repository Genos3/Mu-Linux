@echo off
rem Convert Mu Online assets to DC formats, entirely inside this build folder.
rem   convert-assets.cmd                 convert everything (~2000 files, slow)
rem   convert-assets.cmd --limit 5       first 5 images + 5 sounds (smoke test)
rem   convert-assets.cmd --no-sounds     textures only
rem Output: dc_assets\Data\  (then: make-cdi.cmd converted)
cd /d "%~dp0."
python tools\convert_assets.py %*
