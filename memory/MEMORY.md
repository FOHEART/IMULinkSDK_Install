# IMULinkSDK_Install 项目记忆

## 项目概述
Windows-first IMU SDK 可视化工具，正在向 Linux/Ubuntu 移植。

## 关键文件
- [IMU_Viewer.py](../IMU_Viewer.py) — 主程序，Python + VTK 3D 可视化
- [CMakeLists.txt](../CMakeLists.txt) — 构建配置，当前有 WIN32 硬限制
- [Linux_port_instructions.md](../Linux_port_instructions.md) — Linux 移植完整工作清单
- [[Windows]setup_python_libs.cmd](../scripts/[Windows]setup_python_libs.cmd) — Windows Python 依赖安装脚本
- [git_pull_latest.sh](../git_pull_latest.sh) — Linux 拉取最新代码脚本（已创建）
- [git_pull_latest_windows.cmd](../git_pull_latest_windows.cmd) — Windows 拉取最新代码脚本

## Linux 移植状态
详见 [linux_port.md](linux_port.md)

### 已完成
- CMakeLists.txt 已支持 Linux（`linux-Debug`/`linux-Release`），保留 Windows 路径不变
- pybind 模块构建成功：`out/build/linux-Debug/python/imulink_sdk_pybind.cpython-310-x86_64-linux-gnu.so`
- `libimuLinkSDK.so` 自动复制到 pybind 模块旁，RPATH=$ORIGIN 已设置

### 构建命令（Linux）
```bash
# Debug（含 pybind）
cmake -S . -B out/build/linux-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_PYTHON_BINDINGS=ON
cmake --build out/build/linux-Debug

# Release
cmake -S . -B out/build/linux-Release -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_PYTHON_BINDINGS=ON
cmake --build out/build/linux-Release
```

### 首要阻塞项（已解决）
~~SDK 提供方需提供 `libimuLinkSDK.so`~~ — 已放入 `out/install/linux-Debug/lib/` 和 `out/install/linux-Release/lib/`

### 已完成（续）
- IMU_Viewer.py 适配 Linux：`_default_install_root`、`_find_dll`、`_register_module_search_paths` 均按平台分支，Windows 逻辑不变
- Linux 下用 `LD_LIBRARY_PATH` 替代 `add_dll_directory`
- setup_python_libs.sh 创建完成（Python 3.10 兼容版本：numpy==2.2.6、contourpy==1.3.2）
- **工程已在 Linux 上正常运行验证通过**

### 待完成
| # | 工作项 | 状态 |
|---|--------|------|
| 1 | 修改 `IMU_Viewer.spec` 去掉 VC Runtime，改 `.so` | 待做（打包用） |
