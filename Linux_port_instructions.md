# Ubuntu 移植工作清单

## 总结

| 工作项 | 难度 | 是否依赖 SDK 提供方 |
|--------|------|---------------------|
| 提供 `libimuLinkSDK.so` | — | **是**（必须，首要阻塞项） |
| 修改 CMakeLists.txt 支持 Linux | 低 | 否 |
| 修改 `_find_dll` 适配 `.so` 路径 | 低 | 否 |
| 修改 `add_dll_directory` 替换为 Linux 方式 | 低 | 否 |
| 提供 `setup_python_libs.sh` | 低 | 否 |
| 修改 `IMU_Viewer.spec` 适配 Linux | 低 | 否 |

---

## 1. 提供 Linux 版本的 imuLinkSDK 库（首要阻塞项）

**这是最核心的前提。** 当前 `out/install/` 下只有 Windows 的 `.dll` 和 `.lib`，Linux 需要 `.so`（共享库）。

- 需要 SDK 提供方编译并提供 `libimuLinkSDK.so`（linux-x64）
- 建议放置路径：

```
out/install/linux-x64/
├── bin/libimuLinkSDK.so
├── lib/libimuLinkSDK.so   （或 .a 静态库）
└── include/               （与 Windows 版相同的头文件）
```

---

## 2. 修改 CMakeLists.txt 移除 Windows 限制

`CMakeLists.txt` 第 26-28 行有硬性 Windows 检查：

```cmake
if(NOT WIN32)
  message(FATAL_ERROR "This CMakeLists currently supports Windows layout of out/install/*")
endif()
```

需要做的修改：

- 删除此 `FATAL_ERROR` 限制
- 添加 Linux 的 install 路径逻辑（`linux-x64/` 目录），参考现有 `x64-debug/x64-release` 查找逻辑
- 将 `IMPORTED_LOCATION` 从 `.dll` 改为 Linux 下的 `.so` 路径
- 去掉 `IMPORTED_IMPLIB`（Linux 不使用 `.lib` 导入库）
- 去掉 `copy_imu_runtime_dlls` 函数中复制 VC Runtime DLL 的部分（Linux 无此依赖）

示例（单配置生成器 Linux 路径）：

```cmake
if(NOT WIN32)
  set_target_properties(imuLinkSDK PROPERTIES
    IMPORTED_LOCATION "${_IMULINK_INSTALL_ROOT}/linux-x64/bin/libimuLinkSDK.so"
    INTERFACE_INCLUDE_DIRECTORIES "${_IMULINK_INSTALL_ROOT}/linux-x64/include"
  )
endif()
```

---

## 3. 修改 IMU_Viewer.py 的库文件查找逻辑

`IMU_Viewer.py` 中 `_find_dll` 只查找 `imuLinkSDK.dll`，Linux 下库名为 `libimuLinkSDK.so`。

需要修改 `_find_dll` 和 `_default_install_root`：

```python
def _default_install_root() -> str:
    here = _default_project_root()
    if sys.platform.startswith("win"):
        return os.path.join(here, "out", "install", "x64-debug")
    else:
        return os.path.join(here, "out", "install", "linux-x64")

def _find_dll(install_root: str) -> str:
    lib_name = "imuLinkSDK.dll" if sys.platform.startswith("win") else "libimuLinkSDK.so"
    # ... 其余查找逻辑中同样替换文件名
```

---

## 4. 修改 IMU_Viewer.py 的动态库目录注册方式

`IMU_Viewer.py` 中使用了 Windows 专有的 `os.add_dll_directory()`，Linux 不支持此 API。

当前代码（已有平台判断，无需大改）：

```python
if sys.platform.startswith("win") and hasattr(os, "add_dll_directory"):
    os.add_dll_directory(dll_dir)
```

Linux 下 `.so` 的搜索路径通过以下方式之一解决：
- 启动脚本中 `export LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH`
- 或在 Python 中：`os.environ["LD_LIBRARY_PATH"] = dll_dir`（需在 import 前设置，实际需通过包装脚本）
- 或将 `.so` 放在与可执行文件相同的目录，CMake `rpath` 设置 `$ORIGIN`

---

## 5. 安装 Linux 系统依赖

```bash
sudo apt update
sudo apt install python3 python3-pip cmake ninja-build build-essential
```

Python 依赖（pip 安装到系统，无需手动放 `libs/`）：

```bash
pip3 install vtk numpy matplotlib Pillow pyparsing python-dateutil \
    packaging fonttools contourpy cycler kiwisolver six
pip3 install pyinstaller pybind11
```

> Linux 下 VTK 通过 pip 安装后在系统 site-packages 中，`IMU_Viewer.py` 开头的 `sys.path.insert(0, LIBS_DIR)` 对 Linux 可能不需要。

---

## 6. 提供 setup_python_libs.sh

当前 `setup_python_libs.cmd` 只能在 Windows 运行，需额外提供 `setup_python_libs.sh`：

```bash
#!/bin/bash
set -e

echo "[1/2] 安装运行时依赖到 libs/ ..."
pip3 install vtk==9.6.0 numpy==2.4.2 matplotlib==3.10.8 Pillow==12.1.1 \
    pyparsing==3.3.2 python-dateutil==2.9.0.post0 packaging==26.0 \
    fonttools==4.61.1 contourpy==1.3.3 cycler==0.12.1 kiwisolver==1.4.9 six==1.17.0 \
    --target libs

echo "[2/2] 安装开发工具到系统 Python ..."
pip3 install pyinstaller==6.19.0 pybind11==2.13.6

echo "Done."
```

---

## 7. PyInstaller 打包（可选）

Linux 下 PyInstaller 可以正常运行：

```bash
python3 -m PyInstaller IMU_Viewer.spec --noconfirm
```

`IMU_Viewer.spec` 需要调整：

- `PYD_DIR` 路径适配 Linux（`.so` 而非 `.pyd`）
- 去掉 VC Runtime DLL（`concrt140.dll`、`msvcp140*.dll`、`vcruntime140*.dll`）的复制
- `binaries` 中 `imuLinkSDK.dll` 改为 `libimuLinkSDK.so`
- 输出为 ELF 可执行文件，不能跨平台使用

---

## CMake 构建命令（Linux）

```bash
# Debug
cmake -S . -B out/build/linux-x64-debug \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_PYTHON_BINDINGS=ON

cmake --build out/build/linux-x64-debug

# Release
cmake -S . -B out/build/linux-x64-release \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_PYTHON_BINDINGS=ON

cmake --build out/build/linux-x64-release
```
