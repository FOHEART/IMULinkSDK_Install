import importlib
import os
import sys
import threading


def _default_project_root() -> str:
    return os.path.dirname(os.path.abspath(__file__))


def _default_install_root() -> str:
    here = _default_project_root()
    return os.path.join(here, "out", "install", "x64-debug")


def _find_dll(install_root: str) -> str:
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

    stop_event = threading.Event()

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
    skeletonForPrint = "KHS53_T8"
    def thread_poll_frame() -> None:
        while not stop_event.is_set():
            suits = sdk.get_suit_frames()
            for suit in suits:
                suit_number = suit["suit_number"]
                for frame in suit["frames"]:
                    if frame["skeleton_index"] == KHS53_SkeletonIndex[skeletonForPrint]:
                        ax, ay, az = frame["accel_g"]
                        print(f"[0x{suit_number:08X}][{skeletonForPrint}] accel_g: x={ax:.4f}  y={ay:.4f}  z={az:.4f}")
            stop_event.wait(0.01)

    poll_thread = threading.Thread(target=thread_poll_frame, daemon=True)
    poll_thread.start()

    input("Press Enter to stop...\n")

    stop_event.set()
    poll_thread.join()

    ret = sdk.close()
    print(f"IMULinkSDK_Close: {ret}")

    return 0

if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
