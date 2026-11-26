@echo off
rem Launch the current DC build in the current Flycast.
rem Serial console output goes to serial.log in Flycast's bin folder.
cd /d "%~dp0."
set "FLYCAST=C:\Users\usuario\Desktop\my_folder\dev\guide\sources\flycast\bin\flycast.exe"
if not exist "%FLYCAST%" (
    echo Flycast not found: %FLYCAST%
    exit /b 1
)
start "" "%FLYCAST%" "%~dp0mu_full.cdi"
