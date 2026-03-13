# IMULinkSDK 示例工程

IMULink 惯性动捕套装的 SDK 示例工程，包含 C++ 示例程序与 Python 3D 实时可视化查看器。

仓库地址：`git clone https://gitee.com/foheart_1_0/IMULinkSDK_Install.git`

---

## 功能概览

| 功能 | 说明 |
|------|------|
| SDK C++ 接口 | 初始化/关闭 SDK、获取套装帧数据、白名单/黑名单管理 |
| Python 绑定 | 通过 pybind11 将 C++ SDK 暴露为 Python 模块 |
| 3D 实时查看器 | VTK 渲染窗口实时展示单个骨骼节点的姿态（四元数 → 长方体旋转） |
| 骨骼节点切换 | 窗口左上角点击按钮，切换当前显示的骨骼节点 |
| 相机控制 | 轨迹球相机、按空格键重置视角 |

---

## 项目结构

```
IMULinkSDK_Install/
├── CMakeLists.txt                  # CMake 构建配置（支持 Windows / Linux）
├── CMakeSettings.json              # Visual Studio CMake 多配置设置
├── IMU_Viewer.py                   # 主程序：Python 3D 实时查看器（跨平台）
├── scripts/
│   ├── [Windows]build_dist.cmd     # Windows 打包脚本（生成绿色可运行目录）
│   └── [Windows]setup_python_libs.cmd  # Windows：安装 Python 依赖库
├── setup_python_libs.sh            # Linux：安装 Python 依赖库
├── git_pull_latest.sh              # Linux：拉取最新代码脚本
│
├── example/                        # C++ 示例代码
│   ├── example1.h                  # 基础数据获取示例
│   ├── example2.h                  # 套装帧轮询示例
│   ├── example_BlackList.h         # 黑名单过滤示例
│   └── Logger.h / Logger.cpp       # 日志工具
│
├── python/                         # Python 辅助模块
│   ├── imulink_sdk_pybind.cpp      # pybind11 绑定源码（编译为 .pyd/.so）
│   ├── box_actor.py                # VTK 长方体 Actor（四元数旋转）
│   ├── camera_control.py           # 相机空格键重置绑定
│   ├── vtk_axes.py                 # 坐标轴渲染
│   ├── draw_lines.py               # 线段绘制工具
│   ├── IMU_3D_viewer.py            # 独立 3D 查看器（不依赖 SDK，用于调试）
│   └── print_help_message.py       # 帮助信息打印
│
├── fonts/
│   └── HarmonyOS_Sans_SC_Regular.ttf   # 中文字体（VTK 文字渲染使用）
│
├── libs/                           # Python 依赖库（由 setup_python_libs 安装，不入库）
│
├── out/
│   ├── build/                      # 编译中间文件（不入库）
│   │   ├── x64-Debug/              # Windows Debug 编译输出
│   │   ├── x64-Release/            # Windows Release 编译输出
│   │   ├── linux-Debug/            # Linux Debug 编译输出
│   │   └── linux-Release/          # Linux Release 编译输出
│   └── install/
│       ├── x64-debug/              # Windows Debug SDK
│       │   ├── bin/imuLinkSDK.dll
│       │   ├── lib/imuLinkSDK.lib
│       │   └── include/
│       ├── x64-release/            # Windows Release SDK
│       ├── linux-Debug/            # Linux Debug SDK
│       │   ├── lib/libimuLinkSDK.so
│       │   └── include/
│       └── linux-Release/          # Linux Release SDK
│
└── spdlog/                         # spdlog 日志库头文件
```

---

## 构建步骤

### Windows

#### 前置条件

- Windows 10/11 x64
- Visual Studio 2022（含 C++ 桌面开发组件）
- CMake 3.20+
- Python 3.10+ x64（安装到 `C:\python\`）

#### 1. 安装 Python 依赖

```cmd
scripts\[Windows]setup_python_libs.cmd
```

#### 2. 编译 Debug 版本

```cmd
cmake -S . -B out/build/x64-Debug -G "Visual Studio 17 2022" -A x64 -DCMAKE_CONFIGURATION_TYPES="Debug"
cmake --build out/build/x64-Debug --config Debug
```

#### 3. 编译 Release 版本

```cmd
cmake -S . -B out/build/x64-Release -G "Visual Studio 17 2022" -A x64 -DCMAKE_CONFIGURATION_TYPES="Release"
cmake --build out/build/x64-Release --config Release
```

编译输出：

| 文件 | 路径 |
|------|------|
| SDK DLL | `out/install/x64-debug/bin/imuLinkSDK.dll` |
| SDK 静态库 | `out/install/x64-debug/lib/imuLinkSDK.lib` |
| Python 扩展 | `out/build/x64-Debug/python/imulink_sdk_pybind.*.pyd` |
| 示例程序 | `out/build/x64-Debug/example2.exe` |

CMake 默认开启 `BUILD_PYTHON_BINDINGS=ON`，会自动下载 pybind11 v2.13.6（若系统未安装）。

#### 4. 运行查看器

```cmd
C:\python\python.exe IMU_Viewer.py
```

---

### Linux（Ubuntu 22.04+）

#### 前置条件

```bash
sudo apt install python3 python3-pip python3-dev cmake ninja-build build-essential
```

> 系统 Python 须为 3.10+（Ubuntu 22.04 默认为 Python 3.10）。

#### 1. 安装 Python 依赖

```bash
bash setup_python_libs.sh
```

#### 2. 编译 Debug 版本

```bash
cmake -S . -B out/build/linux-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_PYTHON_BINDINGS=ON
cmake --build out/build/linux-Debug
```

#### 3. 编译 Release 版本

```bash
cmake -S . -B out/build/linux-Release -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_PYTHON_BINDINGS=ON
cmake --build out/build/linux-Release
```

编译输出：

| 文件 | 路径 |
|------|------|
| SDK 共享库 | `out/install/linux-Release/lib/libimuLinkSDK.so` |
| Python 扩展 | `out/build/linux-Release/python/imulink_sdk_pybind.*.so` |
| 示例程序 | `out/build/linux-Release/example2` |

#### 4. 运行查看器

```bash
python3 IMU_Viewer.py
```

`IMU_Viewer.py` 启动时自动搜索 `out/build/` 下的 pybind 模块路径，无需手动设置 `PYTHONPATH`。

---

## SDK 核心接口

### C++ 接口（`IMULinkSDK.h`）

| 函数 | 说明 |
|------|------|
| `imulinksdk_version()` | 返回 SDK 版本号 |
| `IMULinkSDK_Init()` | 初始化 SDK，返回 0 表示成功 |
| `IMULinkSDK_Close()` | 关闭 SDK |
| `IMULinkSDK_GetSuitFrames(frames)` | 获取所有在线套装的当前帧数据 |
| `IMULinkSDK_AddToWhitelist(suits)` | 将指定序列号加入白名单 |
| `IMULinkSDK_AddToBlacklist(suits)` | 将指定序列号加入黑名单 |

### Python 绑定接口（`imulink_sdk_pybind`）

```python
import imulink_sdk_pybind as sdk

sdk.version()                         # 返回版本号
sdk.init()                            # 初始化，返回 0 表示成功
sdk.close()                           # 关闭
sdk.get_suit_frames()                 # 返回套装帧数据列表
sdk.add_to_whitelist([0x1403413E])    # 白名单过滤
sdk.add_to_blacklist([0x1403413E])    # 黑名单过滤
```

`get_suit_frames()` 返回格式：

```python
[
    {
        "suit_number": 0x1403413E,
        "frames": [
            {
                "skeleton_index": 0,        # KHS53_SkeletonIndex 枚举值
                "frame_number":   42,
                "quat_wxyz":      (w, x, y, z),
                "accel_g":        (ax, ay, az),   # 加速度，单位 g
                "gyro_dps":       (gx, gy, gz),   # 角速度，单位 deg/s
                "mag_mgauss":     (mx, my, mz),   # 磁力计，单位 mGauss
                "status_word":    0,
                "temperature_c":  25.0,            # 温度，单位 °C
            },
            # ... 共 KHS53_Count 个节点
        ]
    }
]
```

### 骨骼节点枚举（`KHS53_SkeletonIndex`，共 53 个节点）

| 区域 | 节点 |
|------|------|
| 躯干 | Pelvis, L5, L3, T12, T8, Neck, Head |
| 右臂 | RightShoulder, RightUpperArm, RightForeArm, RightHand |
| 右手指 | RightHandThumb1~3, Index1~3, Middle1~3, Ring1~3, Pinky1~3 |
| 左臂 | LeftShoulder, LeftUpperArm, LeftForeArm, LeftHand |
| 左手指 | LeftHandThumb1~3, Index1~3, Middle1~3, Ring1~3, Pinky1~3 |
| 右腿 | RightUpperLeg, RightLowerLeg, RightFoot, RightToe |
| 左腿 | LeftUpperLeg, LeftLowerLeg, LeftFoot, LeftToe |

---

## IMU_Viewer.py — 3D 实时查看器

### 窗口操作

| 操作 | 功能 |
|------|------|
| 鼠标左键拖拽 | 旋转相机（轨迹球模式） |
| 鼠标右键拖拽 | 缩放 |
| 鼠标中键拖拽 | 平移 |
| 空格键 | 重置相机到初始视角 |
| 左上角按钮点击 | 切换显示的骨骼节点 |
| 关闭窗口 | 退出程序 |

### 骨骼节点按钮（左上角，从上到下）

当前选中节点显示为**黄色**并带 `-->` 前缀，未选中为灰色。

```
-->[胯]        KHS53_Pelvis
  [后背]       KHS53_T8
  [头]         KHS53_Head
               ← 空隙
  [左肩]       KHS53_LeftShoulder
  [左上臂]     KHS53_LeftUpperArm
  [左小臂]     KHS53_LeftForeArm
  [左手]       KHS53_LeftHand
               ← 空隙
  [右肩]       KHS53_RightShoulder
  [右上臂]     KHS53_RightUpperArm
  [右小臂]     KHS53_RightForeArm
  [右手]       KHS53_RightHand
               ← 空隙
  [左大腿]     KHS53_LeftUpperLeg
  [左小腿]     KHS53_LeftLowerLeg
  [左脚]       KHS53_LeftFoot
               ← 空隙
  [右大腿]     KHS53_RightUpperLeg
  [右小腿]     KHS53_RightLowerLeg
  [右脚]       KHS53_RightFoot
```

### 数据流

```
IMU 套装
   │  UDP 数据包（端口 5010）
   ▼
SocketListener（SDK 内部）
   │
   ▼
imuLinkSDK（.dll / .so）  →  IMULinkSDK_GetSuitFrames()
   │
   ▼
imulink_sdk_pybind（.pyd / .so）  →  sdk.get_suit_frames()
   │
   ▼
thread_poll_frame（后台线程）  →  写入共享四元数 _current_quat[4]
   │
   ▼
_on_timer（VTK 定时器，16ms / ~60 fps）  →  set_box_quaternion()
   │
   ▼
VTK 渲染窗口（3D 长方体实时姿态显示）
```

---

## 打包为独立可执行文件（PyInstaller）

### Windows（EXE）

使用 PyInstaller 将 `IMU_Viewer.py` 打包为独立的 `IMU_Viewer.exe`，用户无需安装 Python 即可运行。

#### 前置条件

- Python 3.10+ x64 安装在 `C:\python\`
- PyInstaller 已安装（通过 `scripts\\[Windows]setup_python_libs.cmd` 安装）
- CMake 工程已编译（存在 `out/build/x64-Debug/python/imulink_sdk_pybind*.pyd`）

#### 打包步骤

直接双击运行：

```cmd
scripts\[Windows]build_dist.cmd
```

脚本会自动完成以下操作：
1. 检查 Python、PyInstaller、pybind 模块是否就绪
2. 执行 `python -m PyInstaller IMU_Viewer.spec --noconfirm`
3. 打包完成后提示输出路径

#### 输出结果

```
dist\IMU_Viewer\
├── IMU_Viewer.exe        ← 启动文件（双击运行）
└── _internal\            ← 所有依赖（VTK DLL、pyd、字体、SDK 等，勿删除）
```

将整个 `dist\IMU_Viewer\` 文件夹复制到目标机器，双击 `IMU_Viewer.exe` 即可运行。

> **注意**：`_internal\` 目录与 `IMU_Viewer.exe` 必须保持在同一文件夹下，不可单独移动 exe 文件。

#### 打包说明文件

| 文件 | 说明 |
|------|------|
| `IMU_Viewer.spec` | PyInstaller spec 配置文件，控制打包内容 |
| `scripts\\[Windows]build_dist.cmd` | 一键打包脚本 |
| `build\IMU_Viewer\` | PyInstaller 中间文件，可删除后重新打包 |
| `dist\IMU_Viewer\` | 打包输出目录 |

---

## 中间文件说明

编译中间文件位于 `out/build/`，编译出错时可删除该目录后重新编译。
