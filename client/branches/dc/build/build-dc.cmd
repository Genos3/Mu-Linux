@echo off
rem ============================================================================
rem  build-dc.cmd -- compile the "dc" (Dreamcast / KallistiOS) branch -> mu.elf
rem
rem  Run from anywhere: just double-click or execute this file.
rem    client\branches\dc\build\  = scratch build tree (this folder):
rem                               junctions to ..\src (the worktree), to
rem                               ..\..\..\lib, ..\..\..\cmake plus copies of
rem                               Makefile.kos. Safe to delete and re-create
rem                               (junctions and .build/ are remade as needed).
rem    src\                      = the dc worktree (own git, like test\src)
rem    output\                   = build artifacts (mu.elf, .build deps)
rem    mu.elf lands in this folder (not in src\, which stays clean for git).
rem ============================================================================
setlocal
cd /d "%~dp0."

rem -- DreamSDK / KOS toolchain ----------------------------------------------
set "PATH=C:\games\DreamSDK\mingw64\bin;C:\games\DreamSDK\opt\toolchains\dc\sh-elf\bin;C:\games\DreamSDK\opt\toolchains\dc\kos\utils\build_wrappers;%PATH%"
set "KOS_BASE=C:\games\DreamSDK\opt\toolchains\dc\kos"
set "KOS_PORTS=C:\games\DreamSDK\opt\toolchains\dc\kos-ports"
set "KOS_LOADER=dc-tool-ip -t 127.0.0.1"

echo [1/3] Preparing scratch tree...
rem Junctions to the shared code; only created if missing, so the scratch tree
rem can be deleted and this script rebuilds it.
if not exist "src\"   mklink /J src   "%~dp0..\src"         >nul
if not exist "cmake\" mklink /J cmake "%~dp0..\..\..\cmake"  >nul
if not exist "lib\"   mklink /J lib   "%~dp0..\..\..\lib"    >nul
rem The port compiles via the src junction; its files reference ..\..\lib
rem (from src\*.cpp) and ..\..\..\lib (from src\Render\Texture\*.cpp), so lib
rem must also exist at those depths inside branches\.
if not exist "%~dp0..\lib\"    mklink /J "%~dp0..\lib"    "%~dp0..\..\..\lib" >nul
if not exist "%~dp0..\..\lib\" mklink /J "%~dp0..\..\lib" "%~dp0..\..\..\lib" >nul

echo [2/3] Building mu.elf (KOS at %KOS_BASE%)...
set KOS_DEPDIR=.build
mkdir .build 2>nul
"C:\games\DreamSDK\mingw64\bin\mingw32-make.exe" -f Makefile.kos -j4
if errorlevel 1 goto :fail

echo [3/3] Done.
echo   Output: %~dp0mu.elf
echo   Run:     %KOS_LOADER% -x mu.elf
echo   See:     %~dp0..\src\Platform\Dreamcast\README.md
exit /b 0

:fail
echo.
echo BUILD FAILED (see messages above).
exit /b 1
