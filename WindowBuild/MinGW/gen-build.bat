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
cmake -G "MinGW Makefiles" -S .. -B . -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DNTT_BUILD_SHARED=ON
cd "%sourceFolder%"