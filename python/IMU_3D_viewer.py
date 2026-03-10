"""
3D Lines Viewer - 使用VTK显示N组起始点/终止点连线
所有库从本地 libs/ 目录加载
"""

import sys
import os
import math

# 将本地 python/ 和 libs/ 目录插入到Python路径最前面
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))   # python/
ROOT_DIR   = os.path.dirname(SCRIPT_DIR)                  # 项目根目录
LIBS_DIR   = os.path.join(ROOT_DIR, "libs")
sys.path.insert(0, SCRIPT_DIR)   # 辅助模块（box_actor 等）在同目录
sys.path.insert(0, LIBS_DIR)

import vtk
from vtk_axes import add_axes_to_renderer
from camera_control import bind_space_reset_camera
from draw_lines import add_lines_to_renderer
from box_actor import build_box_actor, set_box_quaternion
from print_help_message import print_help_message


# ─────────────────────────────────────────────
#  配置区：在此修改点组和参数
# ─────────────────────────────────────────────

# N组 (起始点, 终止点)，每个点为 (x, y, z)
POINT_GROUPS = [
    #((0.0,  0.0,  0.0), (1.0,  1.0,  1.0)),
]

# 长方体透明度（0.0=完全透明，1.0=完全不透明）
BOX_OPACITY = 0.9

# 窗口尺寸
WINDOW_WIDTH  = 1366
WINDOW_HEIGHT = 768

# ─────────────────────────────────────────────


def main():
    renderer = vtk.vtkRenderer()
    renderer.SetBackground(0.12, 0.12, 0.18)   # 深蓝灰背景

    # 字体路径
    _font_path = os.path.join(ROOT_DIR, "fonts", "HarmonyOS_Sans_SC_Regular.ttf")
    if not os.path.isfile(_font_path):
        print("[警告] 字体文件不存在: {}".format(_font_path))

    # 绘制初始点组
    add_lines_to_renderer(renderer, POINT_GROUPS)

    # ── 长方体 30×20×10mm，居中于原点 ──
    box_actor = build_box_actor(x_len=30.0, y_len=20.0, z_len=10.0, opacity=BOX_OPACITY)
    renderer.AddActor(box_actor)

    # ── 坐标轴 ──
    add_axes_to_renderer(renderer)

    # ── 渲染窗口 ──
    render_window = vtk.vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetSize(WINDOW_WIDTH, WINDOW_HEIGHT)
    render_window.SetWindowName("3D Lines Viewer  —  [鼠标左键旋转 | 右键缩放 | 中键平移]")

    # ── 交互器 ──
    interactor = vtk.vtkRenderWindowInteractor()
    interactor.SetRenderWindow(render_window)

    style = vtk.vtkInteractorStyleTrackballCamera()
    interactor.SetInteractorStyle(style)

    # 打印操作提示
    print_help_message()

    render_window.Render()
    renderer.ResetCamera()
    render_window.Render()

    # ── 空格键重置相机（在初始视角确定后绑定）──
    bind_space_reset_camera(interactor, renderer, render_window)

    # ── 演示：定时器驱动方块绕 Z 轴旋转 ──
    _angle = [0.0]
    _tick  = [0]

    def _on_updateCubeRot_timer(*_):
        _angle[0] += 1.0          # 每帧 +1°
        _tick[0]  += 1
        a = math.radians(_angle[0])
        # 绕 Z 轴旋转的四元数：(0, 0, sin(a/2), cos(a/2))
        set_box_quaternion(box_actor, 0.0, 0.0, math.sin(a / 2), math.cos(a / 2))
        print("  [timer] tick={:5d}  angle={:.1f}°".format(_tick[0], _angle[0]))
        render_window.Render()

    interactor.AddObserver("TimerEvent", _on_updateCubeRot_timer)
    interactor.Initialize()
    interactor.CreateRepeatingTimer(16)   # ~60fps，单位毫秒

    interactor.Start()


if __name__ == "__main__":
    main()
