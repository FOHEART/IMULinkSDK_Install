#ifndef IMULINKFRAME_H
#define IMULINKFRAME_H

#include <cstdint>
#include "imulinksdk_typedefs.h"
#include <ostream>
#include <iomanip>

#/**
# * @brief Ensure 1-byte packing for cross-platform binary compatibility.
# * @brief-cn 保证 1 字节对齐以实现跨平台二进制兼容性。
# */
#if defined(_MSC_VER)
#pragma pack(push, 1)
#elif defined(__GNUC__)
#pragma pack(push, 1)
#else
#pragma pack(push, 1)
#endif

#define IMULINK_ACCEL_RAW_TO_FLOAT 4096.0f
#define IMULINK_GYRO_RAW_TO_FLOAT 16.4f

/**
 * @brief IMULinkFrame represents a single sensor frame.
 * @brief-cn IMULinkFrame：表示单个传感器帧。
 */
struct IMULinkFrame
{
   /** 
   @brief Suit serial number (e.g., 0x1403403E) /
   套装序列号，例如 0x1403403E 
   */
   uint32_t suitNumber;

   /** 
   @brief Sensor location in the suit / 
   传感器在套装中的位置 
   */
   KHS53_SkeletonIndex skeletonIndex;

   /** 
   @brief Frame index; may wrap (e.g., 0-255 or 0-65535) /
   帧序号，可能循环（例如 0-255 或 0-65535） 
   */
   uint32_t frameNumber;

   /** 
   @brief Sensor quaternion (w, x, y, z) /
   传感器四元数（w, x, y, z） 
   */
   float quat_wxyz[4];

   /** @brief Acceleration in g / 加速度，单位 g */
   float accel_g[3];

   /** @brief Gyroscope in degrees per second / 陀螺仪，单位 deg/s */
   float gyro_dps[3];

   /** @brief Magnetometer in milli-Gauss / 磁力计，单位 mGauss */
   float mag_mGauss[3];

   /** @brief Temperature in Celsius / 温度，单位摄氏度 */
   float temperature_C;
};

typedef struct IMULinkFrame IMULinkFrame;

/**
 * @brief IMULinkSuitFrames represents a batch of frames for a suit update.
 * @brief-cn IMULinkSuitFrames：表示一次套装更新的帧集合。
 *
 * The array is ordered by KHS53_SkeletonIndex, containing one frame per sensor.
 * / 数组按 KHS53_SkeletonIndex 顺序排列，每个传感器对应一帧数据。
 */
struct IMULinkSuitFrames
{
    /** 
    @brief Suit serial number (e.g., 0x1403403E) /
    套装序列号，例如 0x1403403E 
    */
    uint32_t suitNumber;

    /** 
    @brief Number of frames in this update, up to KHS53_Count /
    本次更新包含的帧数，最大为 KHS53_Count 
    */
    uint32_t frameCount;

    /**
    @brief One frame per sensor in KHS53_SkeletonIndex order /
    每个传感器对应一帧数据，按 KHS53_SkeletonIndex 顺序 
    */
    IMULinkFrame rawFrames[KHS53_Count];

    /** 
    @brief User extension variable (optional) /
    用户扩展变量（可选） 
    */
    uint32_t* userVar1;

    /**
    @brief User extension variable (optional) /
    用户扩展变量（可选） 
    */
    uint32_t* userVar2;
};

typedef struct IMULinkSuitFrames IMULinkSuitFrames;

#pragma pack(pop)

/**
 * @brief Print all elements of an IMULinkFrame to the provided output stream.
 * @brief-cn 将 IMULinkFrame 的所有元素打印到提供的输出流。
 * @param os Output stream to write to. / 输出流。
 * @param f The frame to print. / 要打印的帧。
 */
inline void PrintIMULinkFrame(std::ostream& os, const IMULinkFrame& f)
{
    os << "IMULinkFrame { ";

   /** 
   @brief Control whether to print suitNumber in hex /
   控制是否以十六进制打印 suitNumber 
   */
    bool isPrintSuitNumber = true;
    if (isPrintSuitNumber)
    {
      /** @brief Save stream state / 保存流状态 */
        auto oldFlagsSuit = os.flags();
        auto oldPrecSuit = os.precision();

        os << "suitNumber=" << std::showbase << std::hex << std::uppercase << f.suitNumber;

      /** @brief Restore stream state / 恢复流状态 */
        os.flags(oldFlagsSuit);
        os.precision(oldPrecSuit);

        os << ", ";
    }

   /** @brief Print sensor position / 打印传感器位置 */
   os << KHS53_SkeletonIndexToStdString(f.skeletonIndex);

   /** @brief Print frame number / 打印帧序号 */
    {
        auto oldFlagsFrame = os.flags();
        os << ", frameNumber=" << std::dec << f.frameNumber;
        os.flags(oldFlagsFrame);
    }

   /** @brief Control whether to print quaternion / 控制是否打印四元数 */
    bool isPrintQuat = true;
    if (isPrintQuat)
    {
      /** @brief Save stream state / 保存流状态 */
        auto oldFlagsQuat = os.flags();
        auto oldPrecQuat = os.precision();

        /* Align with sign, fixed format, 3 decimal places, width 8 for each value */
        os << ", quat_wxyz=[" << std::showpos << std::fixed << std::setprecision(4)
           << std::setw(8) << f.quat_wxyz[0]
           << "," << std::setw(8) << f.quat_wxyz[1]
           << "," << std::setw(8) << f.quat_wxyz[2]
           << "," << std::setw(8) << f.quat_wxyz[3]
           << std::noshowpos;

      /** @brief Restore stream state / 恢复流状态 */
        os.flags(oldFlagsQuat);
        os.precision(oldPrecQuat);

        os << "]";
    }

   /** @brief Control whether to print acceleration / 控制是否打印加速度 */
    bool isPrintAccel = false;
    if (isPrintAccel)
    {
      /** @brief Save stream state / 保存流状态 */
        auto oldFlags = os.flags();
        auto oldPrec = os.precision();

        /* Align with sign, fixed format, 3 decimal places, width 8 for each value */
        os << ", accel_g=[" << std::showpos << std::fixed << std::setprecision(3)
           << std::setw(8) << f.accel_g[0]
           << "," << std::setw(8) << f.accel_g[1]
           << "," << std::setw(8) << f.accel_g[2]
           << std::noshowpos;

      /** @brief Restore stream state / 恢复流状态 */
        os.flags(oldFlags);
        os.precision(oldPrec);

        os << "]";
    }

   /** @brief Control whether to print gyroscope / 控制是否打印陀螺仪 */
    bool isPrintGyro = false;
    if (isPrintGyro)
    {
      /** @brief Save stream state / 保存流状态 */
        auto oldFlags = os.flags();
        auto oldPrec = os.precision();

        /* Align with sign, fixed format, 3 decimal places, width 8 for each value
           以带符号对齐、固定格式、3位小数、每个值宽度为8打印 */
        os << ", gyro_dps=[" << std::showpos << std::fixed << std::setprecision(3)
           << std::setw(8) << f.gyro_dps[0]
           << "," << std::setw(8) << f.gyro_dps[1]
           << "," << std::setw(8) << f.gyro_dps[2]
           << std::noshowpos;

      /** @brief Restore stream state / 恢复流状态 */
        os.flags(oldFlags);
        os.precision(oldPrec);

        os << "]";
    }

   /** @brief Control whether to print magnetometer / 控制是否打印磁力计 */
    bool isPrintMag = false;
    if (isPrintMag)
    {
      /** @brief Save stream state / 保存流状态 */
        auto oldFlags = os.flags();
        auto oldPrec = os.precision();

        /* Align with sign, fixed format, 3 decimal places, width 8 for each value */
        os << ", mag_mGauss=[" << std::showpos << std::fixed << std::setprecision(3)
           << std::setw(8) << f.mag_mGauss[0]
           << "," << std::setw(8) << f.mag_mGauss[1]
           << "," << std::setw(8) << f.mag_mGauss[2]
           << std::noshowpos;

      /** @brief Restore stream state / 恢复流状态 */
        os.flags(oldFlags);
        os.precision(oldPrec);

        os << "]";
    }

   /** @brief Control whether to print temperature / 控制是否打印温度 */
   bool isPrintTemp = false;
    if (isPrintTemp)
    {
      /** @brief Unit: Celsius / 单位：摄氏度 */
        os << ", temperature_C=" << f.temperature_C;
    }
    os << " }\r\n";
}

/**
 * @brief Stream insertion operator for IMULinkFrame.
 * @brief-cn IMULinkFrame 的流插入运算符。
 * @param os Output stream. / 输出流。
 * @param f Frame to insert. / 要插入的帧。
 * @return std::ostream& The same output stream. / 返回相同的输出流。
 */
inline std::ostream& operator<<(std::ostream& os, const IMULinkFrame& f)
{
   PrintIMULinkFrame(os, f);
   return os;
}

#endif /* IMULINKFRAME_H */
