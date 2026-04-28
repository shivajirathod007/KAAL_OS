@echo off
setlocal EnableDelayedExpansion
title KAAL OS Launcher
cls

:: ─── Fancy Banner ───────────────────────────────────────────────
echo.
echo   ██╗  ██╗ █████╗  █████╗ ██╗           ██████╗ ███████╗
echo   ██║ ██╔╝██╔══██╗██╔══██╗██║          ██╔═══██╗██╔════╝
echo   █████╔╝ ███████║███████║██║          ██║   ██║███████╗
echo   ██╔═██╗ ██╔══██║██╔══██║██║          ██║   ██║╚════██║
echo   ██║  ██╗██║  ██║██║  ██║███████╗     ╚██████╔╝███████║
echo   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝      ╚═════╝ ╚══════╝
echo.
echo         KAAL OS Simulator  v2.0  Enterprise
echo           [-] Built by Shivazz [-]
echo.
echo   ════════════════════════════════════════════════
echo.

:: ─── Resolve script directory (handles spaces in paths) ─────────
set "SCRIPT_DIR=%~dp0"
:: Go up one level from scripts\ to project root
cd /d "%SCRIPT_DIR%.."
set "PROJECT_DIR=%CD%"

echo   [~] Project Root : %PROJECT_DIR%
echo.

:: ─── Check for g++ (MinGW) ──────────────────────────────────────
echo   [~] Checking for g++ compiler...
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo   [!] ERROR: g++ not found on this system!
    echo.
    echo   KAAL OS requires MinGW-w64 (g++) to compile.
    echo   Install it using ONE of the following methods:
    echo.
    echo   Option 1 - MSYS2 (Recommended):
    echo     1. Download from https://www.msys2.org/
    echo     2. Run:  pacman -S mingw-w64-x86_64-gcc
    echo     3. Add   C:\msys64\mingw64\bin   to your PATH
    echo.
    echo   Option 2 - MinGW via Chocolatey:
    echo     Run in Administrator CMD:  choco install mingw
    echo.
    echo   Option 3 - WinLibs Standalone:
    echo     Download from https://winlibs.com/
    echo     Extract and add the bin\ folder to your PATH
    echo.
    pause
    exit /b 1
)

:: Show compiler version found
for /f "tokens=*" %%v in ('g++ --version 2^>^&1 ^| findstr /i "g++"') do (
    echo   [+] Found: %%v
)
echo.

:: ─── Create build directory ─────────────────────────────────────
if not exist "%PROJECT_DIR%\build" (
    mkdir "%PROJECT_DIR%\build"
    echo   [+] Created build\ directory
)

:: ─── Compile ────────────────────────────────────────────────────
echo   [~] Compiling KAAL OS (Phase 3 SOLID build)...
echo.

set "SOURCES=src\core\Memory.cpp src\core\CPU.cpp src\core\OS.cpp src\ui\main.cpp"
set "OUTPUT=build\kaal_os.exe"
set "INCLUDES=-Iinclude"
set "FLAGS=-std=c++17 -Wall -O2"

g++ %FLAGS% %INCLUDES% %SOURCES% -o %OUTPUT% 2>&1

if %errorlevel% neq 0 (
    echo.
    echo   [!] Compilation FAILED. See errors above.
    echo.
    echo   Common fixes:
    echo     - Make sure you downloaded the full KAAL_OS project (not just one file)
    echo     - Ensure g++ supports C++17  (g++ --version should show 7.0+)
    echo     - Check that all source files exist in src\core\ and src\ui\
    echo.
    pause
    exit /b 1
)

echo.
echo   [+] Compilation successful!  ->  %OUTPUT%
echo.

:: ─── Copy input.txt if missing inside build\ ───────────────────
if not exist "%PROJECT_DIR%\build\input.txt" (
    if exist "%PROJECT_DIR%\input.txt" (
        copy /Y "%PROJECT_DIR%\input.txt" "%PROJECT_DIR%\build\input.txt" >nul
        echo   [~] Copied input.txt to build\
    )
)

:: ─── Launch in new CMD window ────────────────────────────────────
echo   [~] Launching KAAL OS in a new terminal window...
echo.
echo   ════════════════════════════════════════════════
echo.

start "KAAL OS Simulator" cmd /k "cd /d "%PROJECT_DIR%\build" && kaal_os.exe & echo. & echo [Session ended. Close this window or press any key.] & pause >nul"

:: Keep launcher window open briefly then close
timeout /t 2 /nobreak >nul
exit /b 0
