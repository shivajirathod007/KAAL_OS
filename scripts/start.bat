@echo off
title KAAL OS Launcher
cls
cd %~dp0
echo [~] Compiling KAAL OS...
g++ -o kaal_os_sim.exe main.cpp os.cpp
if %errorlevel% neq 0 (
    echo [-] Compilation failed. Please check your C++ syntax.
    pause
    exit /b %errorlevel%
)
echo [+] Compilation successful. Launching KAAL OS...
start cmd /k "kaal_os_sim.exe"
