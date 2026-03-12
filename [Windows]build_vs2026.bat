@echo off
chcp 65001 >nul
cd /d "%~dp0"

echo [0/4] 安装 Python 依赖...
call "%~dp0[Windows]setup_python_libs.cmd"
if %errorlevel% neq 0 (
    echo Python 依赖安装失败，退出码: %errorlevel%
    pause
    exit /b %errorlevel%
)

echo [1/4] 生成 Visual Studio 2026 项目 (Debug)...
cmake -S . -B out/build/x64-Debug -G "Visual Studio 18 2026" -A x64 -DCMAKE_CONFIGURATION_TYPES="Debug"
if %errorlevel% neq 0 (
    echo CMake 配置失败，退出码: %errorlevel%
    pause
    exit /b %errorlevel%
)

echo [2/4] 编译 Debug...
cmake --build out/build/x64-Debug --config Debug
if %errorlevel% neq 0 (
    echo 编译失败，退出码: %errorlevel%
    pause
    exit /b %errorlevel%
)

echo [3/4] 生成 Visual Studio 2026 项目 (Release)...
cmake -S . -B out/build/x64-Release -G "Visual Studio 18 2026" -A x64 -DCMAKE_CONFIGURATION_TYPES="Release"
if %errorlevel% neq 0 (
    echo CMake 配置失败，退出码: %errorlevel%
    pause
    exit /b %errorlevel%
)

echo [4/4] 编译 Release...
cmake --build out/build/x64-Release --config Release
if %errorlevel% neq 0 (
    echo 编译失败，退出码: %errorlevel%
    pause
    exit /b %errorlevel%
)

echo 全部编译完成。
pause
