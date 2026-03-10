# Windows Compile

地址：git clone https://gitee.com/foheart_1_0/IMULinkSDK_Install.git

## 编译Debug版本

cmake -S . -B out/build/x64-Debug -G "Visual Studio 17 2022" -A x64 -DCMAKE_CONFIGURATION_TYPES="Debug"

cmake --build out/build/x64-Debug --config Debug

## 编译Release版本

cmake -S . -B out/build/x64-Release -G "Visual Studio 17 2022" -A x64 -DCMAKE_CONFIGURATION_TYPES="Release"

cmake --build out/build/x64-Release --config Release

## 头文件

## 库文件

Debug版本

out\install\x64-debug\bin\imuLinkSDK.dll

out\install\x64-debug\lib\imuLinkSDK.lib

Release版本

out\install\x64-release\bin\imuLinkSDK.dll

out\install\x64-release\lib\imuLinkSDK.lib

## 中间文件

中间文件在out/build文件夹下，编译错误时可以删除out/build文件夹重新编译。

---

## 项目结构

```
IMULinkSDK_Install/
├── IMU_Viewer.py              # 主入口：IMU 数据采集 + VTK 3D 实时可视化
├── CMakeLists.txt             # CMake 构建脚本（含 pybind11 Python 绑定）
├── example/                   # C++ 示例代码
│   ├── example1.cpp / .h      # 基础用法示例
│   ├── example2.cpp / .h      # 扩展用法示例
│   ├── example_BlackList.cpp  # 黑名单过滤示例
│   ├── example_WhiteList.cpp  # 白名单过滤示例
│   └── Logger.cpp / .h        # 日志工具
├── python/                    # Python 辅助模块与独立工具
│   ├── imulink_sdk_pybind.cpp # pybind11 绑定源文件（编译为 .pyd）
│   ├── IMU_3D_viewer.py       # 独立 3D 线段/长方体演示查看器
│   ├── box_actor.py           # VTK 长方体 Actor 构建 & 四元数旋转
│   ├── vtk_axes.py            # VTK 坐标轴 Actor 工具
│   ├── camera_control.py      # 相机控制（空格键重置视角）
│   ├── draw_lines.py          # VTK 连线/球体 Actor 工具
│   └── print_help_message.py  # 操作提示打印
├── fonts/
│   └── HarmonyOS_Sans_SC_Regular.ttf  # 中文字体（供 VTK 标签使用）
├── libs/                      # 第三方 Python 依赖（PIL、pyparsing 等）
├── spdlog/                    # spdlog 日志库头文件
└── out/                       # CMake 构建输出（.dll / .lib / .pyd）
    └── install/
        ├── x64-debug/bin/imuLinkSDK.dll
        ├── x64-debug/lib/imuLinkSDK.lib
        ├── x64-release/bin/imuLinkSDK.dll
        └── x64-release/lib/imuLinkSDK.lib
```

---

## Python 可视化功能说明

### IMU_Viewer.py

主运行入口，集成 SDK 数据采集与 VTK 3D 实时姿态可视化。

**运行方式：**

```
python IMU_Viewer.py [install_root]
```

**功能：**

- 通过 `imulink_sdk_pybind`（pybind11 模块）初始化 IMULink SDK
- 后台线程 `thread_poll_frame` 全速轮询 `sdk.get_suit_frames()`，获取指定骨骼节点（默认 `KHS53_T8`）的帧数据
- 每帧提取加速度 (`accel_g`)、陀螺仪 (`gyro_dps`)、磁力计 (`mag_mgauss`)、四元数 (`quat_wxyz`)
- 四元数通过共享变量传递给 VTK 主线程，驱动 3D 长方体实时旋转
- VTK 场景：30×20×10mm 彩色长方体 + 坐标轴，~60fps 定时器刷新
- 屏幕左上角显示当前 Suit 编号（16 进制格式，如 `Suit: 0x00000001`）
- 空格键重置相机视角；鼠标左键旋转、右键缩放、中键平移

**骨骼节点选择：**

修改 `IMU_Viewer.py` 中的 `skeletonForPrint` 变量可切换到其他骨骼节点（参考 `KHS53_SkeletonIndex` 枚举）。

### python/IMU_3D_viewer.py

独立的 VTK 3D 查看器，用于调试和演示，不依赖 SDK。

**功能：**

- 显示可配置的点组连线（`POINT_GROUPS`）
- 定时器驱动长方体绕 Z 轴自动旋转演示
- 支持鼠标交互（旋转/缩放/平移）、空格键重置视角

### Python 模块依赖

| 模块 | 说明 |
|------|------|
| `vtk` | 3D 渲染（需系统安装） |
| `imulink_sdk_pybind` | SDK Python 绑定（需先编译 CMake 目标） |
| `libs/` | PIL、pyparsing 等本地第三方依赖 |

---

## pybind11 Python 绑定编译

CMake 默认开启 `BUILD_PYTHON_BINDINGS=ON`，编译后生成：

```
out/build/x64-Debug/python/Debug/imulink_sdk_pybind.pyd
```

`IMU_Viewer.py` 启动时自动搜索该路径，无需手动配置 `PYTHONPATH`。

**pybind11 暴露的 API：**

| 函数 | 说明 |
|------|------|
| `sdk.version()` | 返回 SDK 版本字符串 |
| `sdk.init()` | 初始化 SDK，返回 0 表示成功 |
| `sdk.get_suit_frames()` | 获取所有 Suit 的最新帧列表 |
| `sdk.close()` | 关闭 SDK |

**帧数据字段（`frame` 字典）：**

| 字段 | 类型 | 说明 |
|------|------|------|
| `suit_number` | int | Suit 设备编号 |
| `skeleton_index` | int | 骨骼索引 |
| `frame_number` | int | 帧序号 |
| `quat_wxyz` | tuple(4) | 四元数 (w, x, y, z) |
| `accel_g` | tuple(3) | 加速度，单位 g |
| `gyro_dps` | tuple(3) | 角速度，单位 dps |
| `mag_mgauss` | tuple(3) | 磁力计，单位 mGauss |
| `status_word` | int | 状态字 |
| `temperature_c` | float | 温度，单位 °C |
