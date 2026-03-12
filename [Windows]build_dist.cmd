@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: ============================================================
::  build_dist.cmd  -  Package IMU_Viewer.exe with PyInstaller
::
::  Output: dist\IMU_Viewer\IMU_Viewer.exe
::
::  Requirements:
::    - Python 3.14 x64 installed at C:\python\
::    - PyInstaller installed: C:\python\python.exe -m pip install pyinstaller
::    - CMake project built (x64-Debug or x64-Release)
:: ============================================================

set SCRIPT_DIR=%~dp0
set PYTHON=C:\python\python.exe
set SPEC=%SCRIPT_DIR%IMU_Viewer.spec

:: ── Check Python ──
if not exist "%PYTHON%" (
    echo [ERROR] Python not found: %PYTHON%
    echo Please install Python 3.14 x64 to C:\python\
    pause
    exit /b 1
)

:: ── Check PyInstaller ──
"%PYTHON%" -m PyInstaller --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] PyInstaller not installed, installing now...
    "%PYTHON%" -m pip install pyinstaller
    if errorlevel 1 (
        echo [ERROR] PyInstaller installation failed
        pause
        exit /b 1
    )
)

:: ── Check spec file ──
if not exist "%SPEC%" (
    echo [ERROR] Spec file not found: %SPEC%
    pause
    exit /b 1
)

:: ── Check pybind .pyd ──
set PYD_FOUND=0
for %%d in (
    "%SCRIPT_DIR%out\build\x64-Release\python"
    "%SCRIPT_DIR%out\build\x64-release\python"
    "%SCRIPT_DIR%out\build\x64-Debug\python"
    "%SCRIPT_DIR%out\build\x64-debug\python"
) do (
    if exist "%%~d\imulink_sdk_pybind*.pyd" set PYD_FOUND=1
)
if "%PYD_FOUND%"=="0" (
    echo [ERROR] imulink_sdk_pybind*.pyd not found, please build the CMake project first
    pause
    exit /b 1
)

:: ── Run PyInstaller ──
echo.
echo [INFO] Starting packaging...
echo [INFO] Spec file: %SPEC%
echo.

cd /d "%SCRIPT_DIR%"
"%PYTHON%" -m PyInstaller "%SPEC%" --noconfirm

if errorlevel 1 (
    echo.
    echo [ERROR] Packaging failed, see errors above
    pause
    exit /b 1
)

echo.
echo ============================================================
echo  Build complete: %SCRIPT_DIR%dist\IMU_Viewer\
echo  Run: double-click dist\IMU_Viewer\IMU_Viewer.exe
echo ============================================================
pause
