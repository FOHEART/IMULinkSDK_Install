# Linux 移植详细说明

来源：Linux_port_instructions.md（完整内容备份）

## 总结表

| 工作项 | 难度 | 是否依赖 SDK 提供方 |
|--------|------|---------------------|
| 提供 `libimuLinkSDK.so` | — | **是**（必须，首要阻塞项） |
| 修改 CMakeLists.txt 支持 Linux | 低 | 否 |
| 修改 `_find_dll` 适配 `.so` 路径 | 低 | 否 |
| 修改 `add_dll_directory` 替换为 Linux 方式 | 低 | 否 |
| 提供 `setup_python_libs.sh` | 低 | 否 |
| 修改 `IMU_Viewer.spec` 适配 Linux | 低 | 否 |

---

## 1. libimuLinkSDK.so（首要阻塞项）

需 SDK 提供方编译提供，建议放置路径：
```
out/install/linux-x64/
├── bin/libimuLinkSDK.so
├── lib/libimuLinkSDK.so
└── include/
```

---

## 2. CMakeLists.txt 修改

第 26-28 行有硬性 WIN32 检查需删除：
```cmake
if(NOT WIN32)
  message(FATAL_ERROR "This CMakeLists currently supports Windows layout of out/install/*")
endif()
```

修改要点：
- 删除 FATAL_ERROR 限制
- 添加 linux-x64/ 路径逻辑
- IMPORTED_LOCATION 改为 .so 路径
- 去掉 IMPORTED_IMPLIB（Linux 无 .lib）
- 去掉 copy_imu_runtime_dlls 中复制 VC Runtime DLL 部分

Linux 路径示例：
```cmake
if(NOT WIN32)
  set_target_properties(imuLinkSDK PROPERTIES
    IMPORTED_LOCATION "${_IMULINK_INSTALL_ROOT}/linux-x64/bin/libimuLinkSDK.so"
    INTERFACE_INCLUDE_DIRECTORIES "${_IMULINK_INSTALL_ROOT}/linux-x64/include"
  )
endif()
```

---

## 3. IMU_Viewer.py _find_dll 修改

```python
def _default_install_root() -> str:
    here = _default_project_root()
    if sys.platform.startswith("win"):
        return os.path.join(here, "out", "install", "x64-debug")
    else:
        return os.path.join(here, "out", "install", "linux-x64")

def _find_dll(install_root: str) -> str:
    lib_name = "imuLinkSDK.dll" if sys.platform.startswith("win") else "libimuLinkSDK.so"
    # 其余查找逻辑中同样替换文件名
```

---

## 4. add_dll_directory 处理

当前代码已有平台判断（无需大改）：
```python
if sys.platform.startswith("win") and hasattr(os, "add_dll_directory"):
    os.add_dll_directory(dll_dir)
```

Linux 解决方案（选一）：
- 启动脚本：`export LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH`
- Python 中：`os.environ["LD_LIBRARY_PATH"] = dll_dir`（需在 import 前，须包装脚本）
- CMake rpath 设 `$ORIGIN`

---

## 5. setup_python_libs.sh 内容

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

## 6. 系统依赖

```bash
sudo apt update
sudo apt install python3 python3-pip cmake ninja-build build-essential
```

---

## 7. IMU_Viewer.spec 修改要点

- PYD_DIR 路径适配 Linux（.so 而非 .pyd）
- 去掉 VC Runtime DLL（concrt140.dll、msvcp140*.dll、vcruntime140*.dll）
- binaries 中 imuLinkSDK.dll 改为 libimuLinkSDK.so

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
