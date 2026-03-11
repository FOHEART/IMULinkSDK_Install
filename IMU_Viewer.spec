# -*- mode: python ; coding: utf-8 -*-
"""
IMU_Viewer.spec  —  PyInstaller spec for IMU_Viewer.py
生成目录：dist/IMU_Viewer/
运行：pyinstaller IMU_Viewer.spec
"""

import os
import glob

SCRIPT_DIR = os.path.dirname(os.path.abspath(SPEC))          # spec 文件所在目录
LIBS_DIR   = os.path.join(SCRIPT_DIR, "libs")
PYTHON_DIR = os.path.join(SCRIPT_DIR, "python")
PYD_DIR    = os.path.join(SCRIPT_DIR, "out", "build", "x64-Debug", "python")

# ── 收集 vtkmodules 下所有 .pyd 和 .py ──
vtk_datas = []
vtkmodules_src = os.path.join(LIBS_DIR, "vtkmodules")
for root, dirs, files in os.walk(vtkmodules_src):
    rel = os.path.relpath(root, LIBS_DIR)
    for f in files:
        if f.endswith((".py", ".pyd", ".pyi", ".typed")):
            vtk_datas.append((os.path.join(root, f), rel))

# vtk.py 顶层入口
vtk_datas.append((os.path.join(LIBS_DIR, "vtk.py"), "."))

# vtk.libs DLL
vtk_libs_src = os.path.join(LIBS_DIR, "vtk.libs")
for f in os.listdir(vtk_libs_src):
    vtk_datas.append((os.path.join(vtk_libs_src, f), "vtk.libs"))

# ── 字体 ──
font_datas = [
    (os.path.join(SCRIPT_DIR, "fonts", "HarmonyOS_Sans_SC_Regular.ttf"), "fonts"),
]

# ── python/ 辅助模块 ──
py_datas = []
for f in os.listdir(PYTHON_DIR):
    if f.endswith(".py"):
        py_datas.append((os.path.join(PYTHON_DIR, f), "python"))

# ── imulink_sdk_pybind.pyd + SDK DLL + VC Runtime ──
sdk_binaries = []
for pat in [
    os.path.join(PYD_DIR, "imulink_sdk_pybind*.pyd"),
    os.path.join(PYD_DIR, "imuLinkSDK.dll"),
    os.path.join(PYD_DIR, "concrt140.dll"),
    os.path.join(PYD_DIR, "msvcp140*.dll"),
    os.path.join(PYD_DIR, "vcruntime140*.dll"),
]:
    for path in glob.glob(pat):
        sdk_binaries.append((path, "."))

# ── example2.exe（数据源进程）──
example2_path = os.path.join(SCRIPT_DIR, "out", "build", "x64-Debug", "example2.exe")
example2_data = [(example2_path, ".")] if os.path.isfile(example2_path) else []

# ────────────────────────────────────────────────
a = Analysis(
    [os.path.join(SCRIPT_DIR, "IMU_Viewer.py")],
    pathex=[SCRIPT_DIR, LIBS_DIR, PYTHON_DIR, PYD_DIR],
    binaries=sdk_binaries,
    datas=vtk_datas + font_datas + py_datas + example2_data,
    hiddenimports=[
        # vtk 动态导入
        "vtkmodules",
        "vtkmodules.all",
        "vtkmodules.vtkRenderingOpenGL2",
        "vtkmodules.vtkInteractionStyle",
        "vtkmodules.vtkRenderingFreeType",
        "vtkmodules.vtkRenderingAnnotation",
        "vtkmodules.util",
        "vtkmodules.util.misc",
        "vtkmodules.util.numpy_support",
        # 辅助模块
        "vtk_axes",
        "camera_control",
        "box_actor",
        # pybind 模块
        "imulink_sdk_pybind",
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[
        "tkinter", "unittest", "email", "http", "urllib",
        "xml", "pydoc", "doctest", "argparse", "sqlite3",
        "multiprocessing",
    ],
    noarchive=False,
    optimize=0,
)

pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name="IMU_Viewer",
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=False,
    console=True,       # 保留控制台窗口，方便查看日志
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)

coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=False,
    upx_exclude=[],
    name="IMU_Viewer",
)
