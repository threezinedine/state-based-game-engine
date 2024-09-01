cd C:\Users\Acer\Games Dev\state-based-game-engine\build
@REM cmake -G "MinGW Makefiles" -S .. -B . -DCMAKE_BUILD_TYPE=Release
@REM cmake -G "Unix Makefiles" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -S .. -B . -DCMAKE_BUILD_TYPE=Release
cmake -G "Unix Makefiles" -S .. -B . -DCMAKE_BUILD_TYPE=Release
cd C:\Users\Acer\Games Dev\state-based-game-engine