@echo off
rem ============================================================================
rem  build-test.cmd -- compile the "test" branch and deploy it as main_test
rem
rem  Run from anywhere: just double-click or execute this file.
rem    client\branches\test\build\  = scratch source tree (junctions to
rem                             ..\src, and to ../../../lib, tests, cmake plus
rem                             copies of CMakeLists.txt / CMakePresets.json).
rem                             Safe to delete and re-create.
rem    build\                 = build output inside this folder (never touches
rem                             client\build\ used by the main "src" branch)
rem  Final deliverables:
rem    client\bin\Client\main_test.exe   + main_test.map
rem  (the regular client\bin\Client\main.exe from the main branch is NOT touched)
rem ============================================================================
setlocal
cd /d "%~dp0"

rem -- MinGW64 toolchain (ninja, gcc, cmake from MSYS2) ------------------------
set "PATH=C:\games\msys64\mingw64\bin;%PATH%"

rem -- Reuse dependency sources already downloaded by the main build -----------
for %%I in ("%~dp0..\..\..") do set "CLIENT=%%~fI"
set "DEPS=%CLIENT%\build\msys2-mingw-x64\_deps"

echo [1/4] Preparing scratch tree...
rem Junctions to the shared code; only created if missing, so the scratch tree
rem can be deleted and this script rebuilds it.
if not exist "src\"   mklink /J src   "%~dp0..\src"    >nul
if not exist "cmake\" mklink /J cmake "%CLIENT%\cmake" >nul
if not exist "lib\"   mklink /J lib   "%CLIENT%\lib"   >nul
if not exist "tests\" mklink /J tests "%CLIENT%\tests" >nul
rem The port uses ../../lib from src\*.cpp and ../../../lib from
rem src\Render\Texture\*.cpp. The compiler canonicalises the path of the file
rem it opens, so "lib" must also exist at those depths inside branches\.
if not exist "%~dp0..\lib\"     mklink /J "%~dp0..\lib"     "%CLIENT%\lib" >nul
if not exist "%~dp0..\..\lib\"  mklink /J "%~dp0..\..\lib"  "%CLIENT%\lib" >nul
copy /y "%CLIENT%\CMakeLists.txt" . >nul
copy /y "%CLIENT%\CMakePresets.json" . >nul

echo [2/4] Configuring (Release, SDL3 backend)...
cmake -S . -B build -G Ninja ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DMU_PLATFORM_BACKEND=SDL3 ^
  -DFETCHCONTENT_SOURCE_DIR_MU_SDL="%DEPS%\mu_sdl-src" ^
  -DFETCHCONTENT_SOURCE_DIR_MU_MINIAUDIO="%DEPS%\mu_miniaudio-src" ^
  -DFETCHCONTENT_SOURCE_DIR_MU_FREETYPE="%DEPS%\mu_freetype-src" ^
  -DFETCHCONTENT_SOURCE_DIR_MU_HARFBUZZ="%DEPS%\mu_harfbuzz-src" ^
  -DFETCHCONTENT_SOURCE_DIR_MU_SHEENBIDI="%DEPS%\mu_sheenbidi-src" ^
  -DFETCHCONTENT_BASE_DIR="%~dp0build\_deps"
if errorlevel 1 goto :fail

echo [3/4] Building...
cmake --build build --parallel 4
if errorlevel 1 goto :fail

echo [4/4] Deploying to client\bin\Client\main_test.exe ...
copy /y "build\bin\main.map" "%CLIENT%\bin\Client\main_test.map" >nul
copy /y "build\bin\main.exe" "%CLIENT%\bin\Client\main_test.exe" >nul
if errorlevel 1 (
    echo.
    echo WARNING: could not replace main_test.exe.
    echo If the game is running, close it first, then run this script again.
    echo The fresh exe is at: %~dp0build\bin\main.exe
    exit /b 2
)

echo.
echo Done. Run:  %CLIENT%\bin\Client\main_test.exe
exit /b 0

:fail
echo.
echo BUILD FAILED (see messages above).
exit /b 1
