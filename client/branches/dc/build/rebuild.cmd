@echo off
rem Force a full rebuild (needed after editing a shared header, since the
rem KOS .md dep files don't track headers).
setlocal
cd /d "%~dp0."
set "PATH=C:\games\DreamSDK\mingw64\bin;C:\games\DreamSDK\opt\toolchains\dc\sh-elf\bin;%PATH%"
set "KOS_BASE=C:\games\DreamSDK\opt\toolchains\dc\kos"
set "KOS_PORTS=C:\games\DreamSDK\opt\toolchains\dc\kos-ports"
set "KOS_DEPDIR=.build"
mingw32-make -f Makefile.kos clean
mingw32-make -f Makefile.kos -j4
