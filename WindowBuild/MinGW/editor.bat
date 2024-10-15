@echo off
set "batFile=%~dp0\"
set "batFileFolder=%batFile:~0,-1%"
for %%i in ("%batFileFolder%") do set "batFileFolder=%%~dpi"
set "batFileFolder=%batFileFolder:~0,-1%"
for %%i in ("%batFileFolder%") do set "batFileParent=%%~dpi"
set "batFileParent=%batFileParent:~0,-1%"
for %%i in ("%batFileParent%") do set "sourceFolder=%%~dpi"
set "sourceFolder=%sourceFolder:~0,-1%"

cd "%sourceFolder%\build"
Editor.exe
cd "%sourceFolder%"
