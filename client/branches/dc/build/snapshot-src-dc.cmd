@echo off
rem ============================================================================
rem  snapshot-src-dc.cmd -- commit a local backup of the "dc" branch source
rem
rem  client\branches\dc\src has its own small git repository
rem  (client\branches\dc\src\.git) that is completely independent from this
rem  project's repo: nothing here is pushed and checking out other branches
rem  never deletes the folder.
rem
rem  Run this before risky edits / after a working change to keep a restore point.
rem  Restore or inspect:  git -C "..\src" log --oneline
rem ============================================================================
setlocal
cd /d "%~dp0..\src" || goto :fail

git add -A
git commit -m "backup %DATE% %TIME%" >nul 2>&1
if errorlevel 1 (
    echo Nothing to commit -- "dc\src" is unchanged since the last snapshot.
) else (
    echo Snapshot saved.
)
echo.
git log --oneline -3
exit /b 0

:fail
echo Could not find "..\src" next to this script.
exit /b 1
