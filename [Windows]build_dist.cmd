@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: ============================================================
::  build_dist.cmd  —  使用 PyInstaller 打包 IMU_Viewer.exe
::
::  输出目录：dist\IMU_Viewer\IMU_Viewer.exe
::
::  依赖：
::    - Python 3.14 x64 安装在 C:\python\
::    - 已安装 PyInstaller：C:\python\python.exe -m pip install pyinstaller
::    - 已编译 CMake 工程（x64-Debug 或 x64-Release）
:: ============================================================

set SCRIPT_DIR=%~dp0
set PYTHON=C:\python\python.exe
set SPEC=%SCRIPT_DIR%IMU_Viewer.spec

:: ── 检查 Python ──
if not exist "%PYTHON%" (
    echo [ERROR] 未找到 Python：%PYTHON%
    echo 请将 Python 3.14 x64 安装到 C:\python\
    pause
    exit /b 1
)

:: ── 检查 PyInstaller ──
"%PYTHON%" -m PyInstaller --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] 未安装 PyInstaller，正在安装...
    "%PYTHON%" -m pip install pyinstaller
    if errorlevel 1 (
        echo [ERROR] PyInstaller 安装失败
        pause
        exit /b 1
    )
)

:: ── 检查 spec 文件 ──
if not exist "%SPEC%" (
    echo [ERROR] 未找到 %SPEC%
    pause
    exit /b 1
)

:: ── 检查 pybind .pyd ──
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
    echo [ERROR] 未找到 imulink_sdk_pybind*.pyd，请先编译 CMake 工程
    pause
    exit /b 1
)

:: ── 运行 PyInstaller ──
echo.
echo [INFO] 开始打包...
echo [INFO] spec 文件：%SPEC%
echo.

cd /d "%SCRIPT_DIR%"
"%PYTHON%" -m PyInstaller "%SPEC%" --noconfirm

if errorlevel 1 (
    echo.
    echo [ERROR] 打包失败，请查看上方错误信息
    pause
    exit /b 1
)

echo.
echo ============================================================
echo  打包完成：%SCRIPT_DIR%dist\IMU_Viewer\
echo  运行方式：双击 dist\IMU_Viewer\IMU_Viewer.exe
echo ============================================================
pause
