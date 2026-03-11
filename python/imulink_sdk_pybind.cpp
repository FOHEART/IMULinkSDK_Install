#include <algorithm>
#include <cstdint>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "include/IMULinkSDK.h"

namespace py = pybind11;

namespace {

py::dict FrameToDict(const IMULinkFrame& frame)
{
    py::dict result;
    result["suit_number"] = frame.suitNumber;
    result["skeleton_index"] = static_cast<std::uint32_t>(frame.skeletonIndex);
    result["frame_number"] = frame.frameNumber;
    result["quat_wxyz"] = py::make_tuple(
        frame.quat_wxyz[0], frame.quat_wxyz[1], frame.quat_wxyz[2], frame.quat_wxyz[3]);
    result["accel_g"] = py::make_tuple(frame.accel_g[0], frame.accel_g[1], frame.accel_g[2]);
    result["gyro_dps"] = py::make_tuple(frame.gyro_dps[0], frame.gyro_dps[1], frame.gyro_dps[2]);
    result["mag_mgauss"] = py::make_tuple(
        frame.mag_mGauss[0], frame.mag_mGauss[1], frame.mag_mGauss[2]);
    result["status_word"] = frame.status.statusWord;
    result["temperature_c"] = frame.temperature_C;
    return result;
}

py::dict SuitToDict(const IMULinkSuitFrames& suit)
{
    py::dict result;
    result["suit_number"] = suit.suitNumber;

    py::list frames;
    const std::uint32_t count = static_cast<std::uint32_t>(KHS53_Count);
    for (std::uint32_t i = 0; i < count; ++i)
    {
        frames.append(FrameToDict(suit.rawFrames[i]));
    }
    result["frames"] = std::move(frames);
    return result;
}

} // namespace

PYBIND11_MODULE(imulink_sdk_pybind, m)
{
    m.doc() = "pybind11 bindings for IMULinkSDK";

    m.def("version", &imulinksdk_version, "Return SDK version number");
    m.def("init", &IMULinkSDK_Init, "Initialize IMULinkSDK");
    m.def("close", &IMULinkSDK_Close, "Shutdown IMULinkSDK");

    m.def("get_suit_frames", []() {
        std::vector<IMULinkSuitFrames> frames;
        const unsigned int count = IMULinkSDK_GetSuitFrames(frames);

        py::list out;
        const std::size_t safe_count = std::min<std::size_t>(count, frames.size());
        for (std::size_t i = 0; i < safe_count; ++i)
        {
            out.append(SuitToDict(frames[i]));
        }
        return out;
    }, "Call IMULinkSDK_GetSuitFrames and return suit frame data");

    m.def("get_suit_frame_count", []() {
        std::vector<IMULinkSuitFrames> frames;
        return IMULinkSDK_GetSuitFrames(frames);
    }, "Call IMULinkSDK_GetSuitFrames and return suit count");

    m.def("add_to_whitelist", [](const std::vector<unsigned int>& suitNumbers) {
        return IMULinkSDK_AddToWhitelist(suitNumbers);
    }, "Add suit serial numbers to the whitelist");

    m.def("add_to_blacklist", [](const std::vector<unsigned int>& suitNumbers) {
        return IMULinkSDK_AddToBlacklist(suitNumbers);
    }, "Add suit serial numbers to the blacklist");
}
