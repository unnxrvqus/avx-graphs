@echo off

cmake --build build

if errorlevel 1 (
    echo.
    echo ============================
    echo BUILD FAILED
    echo ============================
    pause
    exit /b 1
)

cd build

AvxGraphs.exe

pause