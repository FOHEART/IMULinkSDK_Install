import importlib
import os
import sys
import threading

# ── VTK / 3D 辅助模块路径 ──
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PYTHON_DIR = os.path.join(SCRIPT_DIR, "python")
LIBS_DIR = os.path.join(SCRIPT_DIR, "libs")
sys.path.insert(0, PYTHON_DIR)   # box_actor, vtk_axes, camera_control 在 python/ 目录
sys.path.insert(0, LIBS_DIR)

import vtk
from vtk_axes import add_axes_to_renderer
from camera_control import bind_space_reset_camera
from box_actor import build_box_actor, set_box_quaternion


def _default_project_root() -> str:
    return os.path.dirname(os.path.abspath(__file__))


def _default_install_root() -> str:
    here = _default_project_root()
    return os.path.join(here, "out", "install", "x64-debug")


def _find_dll(install_root: str) -> str:
    # PyInstaller 打包后 DLL 直接放在 _internal/（sys._MEIPASS）
    if hasattr(sys, "_MEIPASS"):
        meipass_dll = os.path.join(sys._MEIPASS, "imuLinkSDK.dll")
        if os.path.isfile(meipass_dll):
            return meipass_dll

    dll = os.path.join(install_root, "bin", "imuLinkSDK.dll")
    if os.path.isfile(dll):
        return dll

    # Fallbacks for alternate folder casing/layout.
    candidates = [
        os.path.join(os.path.dirname(install_root), "x64-Debug", "bin", "imuLinkSDK.dll"),
        os.path.join(os.path.dirname(install_root), "x64-Release", "bin", "imuLinkSDK.dll"),
    ]
    for c in candidates:
        if os.path.isfile(c):
            return c

    raise FileNotFoundError(
        "Could not find imuLinkSDK.dll. Looked in: \n  - " + "\n  - ".join([dll] + candidates)
    )


def _register_module_search_paths(project_root: str) -> None:
    # PyInstaller 打包后模块在 _internal/（sys._MEIPASS）
    if hasattr(sys, "_MEIPASS") and sys._MEIPASS not in sys.path:
        sys.path.insert(0, sys._MEIPASS)

    # Prefer local build outputs first so running this script after a build works directly.
    candidates = [
        os.path.join(project_root, "out", "build", "x64-Debug", "python"),
        os.path.join(project_root, "out", "build", "x64-Release", "python"),
        os.path.join(project_root, "out", "build", "x64-debug", "python"),
        os.path.join(project_root, "out", "build", "x64-release", "python"),
        os.path.join(project_root, "build", "python"),
    ]

    for path in candidates:
        if not os.path.isdir(path):
            continue

        config_dirs = [
            path,
            os.path.join(path, "Debug"),
            os.path.join(path, "Release"),
            os.path.join(path, "RelWithDebInfo"),
            os.path.join(path, "MinSizeRel"),
        ]
        for d in config_dirs:
            if os.path.isdir(d) and d not in sys.path:
                sys.path.insert(0, d)


def _import_bindings():
    try:
        return importlib.import_module("imulink_sdk_pybind")
    except ModuleNotFoundError as exc:
        raise ModuleNotFoundError(
            "Cannot import 'imulink_sdk_pybind'. Build CMake target 'imulink_sdk_pybind' first."
        ) from exc


def main(argv: list[str]) -> int:
    project_root = _default_project_root()
    install_root = argv[1] if len(argv) > 1 else _default_install_root()
    dll_path = _find_dll(install_root)

    # Ensure the dependent runtime DLLs living next to imuLinkSDK.dll can be located.
    dll_dir = os.path.dirname(dll_path)
    if sys.platform.startswith("win") and hasattr(os, "add_dll_directory"):
        os.add_dll_directory(dll_dir)

    _register_module_search_paths(project_root)
    sdk = _import_bindings()

    ver = sdk.version()
    print(f"version: {ver}")

    ret = sdk.init()
    print(f"IMULinkSDK_Init: {ret}")
    if ret != 0:
        return ret

    sdk.add_to_whitelist([0x1403413E])
    stop_event = threading.Event()

    # 共享四元数：[qw, qx, qy, qz]，由轮询线程写入，VTK 定时器读取
    _current_quat = [1.0, 0.0, 0.0, 0.0]
    # 共享 suit_number，由轮询线程写入
    _current_suit = [None]

    # imulinksdk_typedefs.h: enum KHS53_SkeletonIndex_s
    KHS53_SkeletonIndex = {name: i for i, name in enumerate([
        "KHS53_Pelvis", "KHS53_L5", "KHS53_L3", "KHS53_T12", "KHS53_T8",
        "KHS53_Neck", "KHS53_Head",
        "KHS53_RightShoulder", "KHS53_RightUpperArm", "KHS53_RightForeArm", "KHS53_RightHand",
        "KHS53_RightHandThumb1", "KHS53_RightHandThumb2", "KHS53_RightHandThumb3",
        "KHS53_RightHandIndex1", "KHS53_RightHandIndex2", "KHS53_RightHandIndex3",
        "KHS53_RightHandMiddle1", "KHS53_RightHandMiddle2", "KHS53_RightHandMiddle3",
        "KHS53_RightHandRing1", "KHS53_RightHandRing2", "KHS53_RightHandRing3",
        "KHS53_RightHandPinky1", "KHS53_RightHandPinky2", "KHS53_RightHandPinky3",
        "KHS53_LeftShoulder", "KHS53_LeftUpperArm", "KHS53_LeftForeArm", "KHS53_LeftHand",
        "KHS53_LeftHandThumb1", "KHS53_LeftHandThumb2", "KHS53_LeftHandThumb3",
        "KHS53_LeftHandIndex1", "KHS53_LeftHandIndex2", "KHS53_LeftHandIndex3",
        "KHS53_LeftHandMiddle1", "KHS53_LeftHandMiddle2", "KHS53_LeftHandMiddle3",
        "KHS53_LeftHandRing1", "KHS53_LeftHandRing2", "KHS53_LeftHandRing3",
        "KHS53_LeftHandPinky1", "KHS53_LeftHandPinky2", "KHS53_LeftHandPinky3",
        "KHS53_RightUpperLeg", "KHS53_RightLowerLeg", "KHS53_RightFoot", "KHS53_RightToe",
        "KHS53_LeftUpperLeg", "KHS53_LeftLowerLeg", "KHS53_LeftFoot", "KHS53_LeftToe",
    ])}
    #修改这里可以选择打印不同骨骼的数据
    skeletonForPrint = ["KHS53_Pelvis"]

    def thread_poll_frame() -> None:
        import time
        while not stop_event.is_set():
            suits = sdk.get_suit_frames()
            if not suits:
                time.sleep(0.001)
                continue
            for suit in suits:
                suit_number = suit["suit_number"]
                idx = KHS53_SkeletonIndex[skeletonForPrint[0]]
                frames = suit["frames"]
                if idx < len(frames):
                    frame = frames[idx]
                    frameNumber = frame["frame_number"]
                    ax, ay, az = frame["accel_g"]
                    gx, gy, gz = frame["gyro_dps"]       # noqa: F841  (备用，当前未打印)
                    mx, my, mz = frame["mag_mgauss"]     # noqa: F841  (备用，当前未打印)
                    qw, qx, qy, qz = frame["quat_wxyz"]
                    # 将最新四元数和 suit_number 写入共享变量，供 VTK 定时器读取
                    _current_quat[0] = qw
                    _current_quat[1] = qx
                    _current_quat[2] = qy
                    _current_quat[3] = qz
                    _current_suit[0] = suit_number
                    #print(f"[0x{suit_number:08X}][{skeletonForPrint[0]}] frame_number: {frameNumber}  accel_g: x={ax:.4f}  y={ay:.4f}  z={az:.4f}")
                    #print(f"[0x{suit_number:08X}][{skeletonForPrint}] gyro_dps: x={gx:.4f}  y={gy:.4f}  z={gz:.4f}")
                    #print(f"[0x{suit_number:08X}][{skeletonForPrint}] mag_mgauss: x={mx:.4f}  y={my:.4f}  z={mz:.4f}")
                    print(f"[0x{suit_number:08X}][{skeletonForPrint}] quat: w={qw:.4f}  x={qx:.4f}  y={qy:.4f}  z={qz:.4f}")

    poll_thread = threading.Thread(target=thread_poll_frame, daemon=True)
    poll_thread.start()

    # ── VTK 3D 场景 ──
    renderer = vtk.vtkRenderer()
    renderer.SetBackground(0.12, 0.12, 0.18)

    box_actor = build_box_actor(x_len=30.0, y_len=20.0, z_len=10.0, opacity=0.9)
    renderer.AddActor(box_actor)
    add_axes_to_renderer(renderer)

    # ── 中文字体路径 ──
    _FONT_PATH = os.path.join(SCRIPT_DIR, "fonts", "HarmonyOS_Sans_SC_Regular.ttf")

    # ── 左上角 suit_number 文字标签 ──
    suit_label = vtk.vtkTextActor()
    suit_label.SetInput("Suit: --")
    suit_label.GetPositionCoordinate().SetCoordinateSystemToDisplay()
    suit_label.SetPosition(10, 768 - 30)
    suit_label.GetTextProperty().SetFontSize(20)
    suit_label.GetTextProperty().SetColor(1.0, 1.0, 1.0)
    suit_label.GetTextProperty().BoldOn()
    suit_label.GetTextProperty().SetFontFamily(vtk.VTK_FONT_FILE)
    suit_label.GetTextProperty().SetFontFile(_FONT_PATH)
    renderer.AddViewProp(suit_label)

    # ── 左上角骨骼选择按钮（从上到下：胯 / 后背 / 头）──
    _BTN_W, _BTN_H = 80, 28
    _BTN_X = 10
    _buttons = [
        ("胯",   "KHS53_Pelvis", 768 - 70),
        ("后背", "KHS53_T8",     768 - 100),
        ("头",   "KHS53_Head",   768 - 130),
        ("左肩", "KHS53_LeftShoulder", 768 - 188),
        ("左上臂", "KHS53_LeftUpperArm", 768 - 218),
        ("左小臂", "KHS53_LeftForeArm", 768 - 248),
        ("左手", "KHS53_LeftHand", 768 - 278),
        ("右肩", "KHS53_RightShoulder", 768 - 336),
        ("右上臂", "KHS53_RightUpperArm", 768 - 366),
        ("右小臂", "KHS53_RightForeArm", 768 - 396),
        ("右手", "KHS53_RightHand", 768 - 426),
        ("左大腿", "KHS53_LeftUpperLeg", 768 - 484),
        ("左小腿", "KHS53_LeftLowerLeg", 768 - 514),
        ("左脚", "KHS53_LeftFoot", 768 - 544),
        ("右大腿", "KHS53_RightUpperLeg", 768 - 602),
        ("右小腿", "KHS53_RightLowerLeg", 768 - 632),
        ("右脚", "KHS53_RightFoot", 768 - 662),
    ]
    _COLOR_ACTIVE   = (1.0, 0.85, 0.0)   # 黄色：当前选中
    _COLOR_INACTIVE = (0.7, 0.7, 0.7)    # 灰色：未选中

    btn_actors = []
    for label_text, bone_name, y_pos in _buttons:
        actor = vtk.vtkTextActor()
        actor.SetInput(f"[{label_text}]")
        actor.GetPositionCoordinate().SetCoordinateSystemToDisplay()
        actor.SetPosition(_BTN_X, y_pos)
        actor.GetTextProperty().SetFontSize(18)
        actor.GetTextProperty().BoldOn()
        actor.GetTextProperty().SetFontFamily(vtk.VTK_FONT_FILE)
        actor.GetTextProperty().SetFontFile(_FONT_PATH)
        is_active = (bone_name == skeletonForPrint[0])
        actor.SetInput(f"-->[{label_text}]" if is_active else f"[{label_text}]")
        c = _COLOR_ACTIVE if is_active else _COLOR_INACTIVE
        actor.GetTextProperty().SetColor(*c)
        renderer.AddViewProp(actor)
        btn_actors.append((actor, bone_name, label_text, y_pos))

    def _refresh_btn_colors():
        for actor, bone_name, label_text, _ in btn_actors:
            is_active = bone_name == skeletonForPrint[0]
            actor.SetInput(f"-->[{label_text}]" if is_active else f"[{label_text}]")
            c = _COLOR_ACTIVE if is_active else _COLOR_INACTIVE
            actor.GetTextProperty().SetColor(*c)

    def _on_left_click(obj, _event):
        x, y = obj.GetEventPosition()
        for _, bone_name, _, y_pos in btn_actors:
            if _BTN_X <= x <= _BTN_X + _BTN_W and y_pos <= y <= y_pos + _BTN_H:
                skeletonForPrint[0] = bone_name
                _refresh_btn_colors()
                break

    render_window = vtk.vtkRenderWindow()
    render_window.SetWindowName("IMU 3D Viewer")
    render_window.AddRenderer(renderer)
    render_window.SetSize(1366, 768)

    interactor = vtk.vtkRenderWindowInteractor()
    interactor.SetRenderWindow(render_window)
    interactor.SetInteractorStyle(vtk.vtkInteractorStyleTrackballCamera())

    render_window.Render()
    renderer.ResetCamera()
    renderer.GetActiveCamera().Elevation(-45)
    renderer.ResetCameraClippingRange()
    render_window.Render()

    bind_space_reset_camera(interactor, renderer, render_window)

    # VTK 定时器回调：从共享变量读取四元数并更新长方体姿态，更新 suit_number 标签
    def _on_timer(_obj, _event):
        qw, qx, qy, qz = _current_quat
        set_box_quaternion(box_actor, qx, qy, qz, qw)
        sn = _current_suit[0]
        suit_label.SetInput(f"Suit: 0x{sn:08X}" if sn is not None else "Suit: --")
        render_window.Render()

    interactor.Initialize()
    interactor.AddObserver("TimerEvent", _on_timer)
    interactor.AddObserver("LeftButtonPressEvent", _on_left_click)
    interactor.CreateRepeatingTimer(16)   # 16 ms 重复定时器，~60 fps

    print("按关闭窗口退出。")
    interactor.Start()

    stop_event.set()
    poll_thread.join()

    ret = sdk.close()
    print(f"IMULinkSDK_Close: {ret}")

    return 0

if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
