#ifndef IMULINKFRAME_H
#define IMULINKFRAME_H

#include <cstdint>
#include "imulinksdk_typedefs.h"
#include <ostream>
#include <iomanip>
#include <algorithm>
#include <xscommon/xsqeconverter.h>
/**
 * @brief Ensure 1-byte packing for cross-platform binary compatibility.
 * @brief-cn 保证 1 字节对齐以实现跨平台二进制兼容性。
 */
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

	union
	{
		uint32_t statusWord;
		struct
		{
			uint32_t CalibrationAccuracy : 2;
			uint32_t _reserved : 30;
		} statusBits;
	} status;

	/** @brief Temperature in Celsius / 温度，单位摄氏度 */
	float temperature_C;
};

typedef struct IMULinkFrame IMULinkFrame;





/**
 * @brief IMULinkSuitFrames represents a batch of frames for a suit update.
 * @brief-cn IMULinkSuitFrames：表示一次套装更新的帧集合。
 *
 * The array is ordered in  KHS53_SkeletonIndex order, containing one frame per sensor.
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
 * @brief Initialize an IMULinkFrame by clearing all fields to zero.
 * @brief-cn 初始化 IMULinkFrame：将所有成员清零。
 */
inline void InitIMULinkFrame(IMULinkFrame& f)
{
	f.suitNumber = 0;
	f.skeletonIndex = static_cast<KHS53_SkeletonIndex>(0);
	f.frameNumber = 0;
	std::fill(std::begin(f.quat_wxyz), std::end(f.quat_wxyz), 0.0f);
	std::fill(std::begin(f.accel_g), std::end(f.accel_g), 0.0f);
	std::fill(std::begin(f.gyro_dps), std::end(f.gyro_dps), 0.0f);
	std::fill(std::begin(f.mag_mGauss), std::end(f.mag_mGauss), 0.0f);
	f.status.statusWord = 0;
	f.temperature_C = 0.0f;
}

/**
 * @brief Initialize an IMULinkSuitFrames by clearing all fields to zero.
 * @brief-cn 初始化 IMULinkSuitFrames：将所有成员清零。
 */
inline void InitIMULinkSuitFrames(IMULinkSuitFrames& sf)
{
	sf.suitNumber = 0;
	sf.userVar1 = nullptr;
	sf.userVar2 = nullptr;
	for (uint32_t i = 0; i < static_cast<uint32_t>(KHS53_Count); ++i)
	{
		InitIMULinkFrame(sf.rawFrames[i]);
		sf.rawFrames[i].skeletonIndex = static_cast<KHS53_SkeletonIndex>(i);
	}
}
/**
 * @brief Print all elements of an IMULinkFrame to the provided output stream.
 * @brief-cn 将 IMULinkFrame 的所有元素打印到提供的输出流。
 * @param os Output stream to write to. / 输出流。
 * @param f The frame to print. / 要打印的帧。
 */
inline void PrintIMULinkFrame(std::ostream& os, const IMULinkFrame& f)
{
	// os << "IMULinkFrame { ";

	/**
	@brief Control whether to print suitNumber in hex /
	控制是否以十六进制打印 suitNumber
	*/
	bool isPrintSuitNumber = false;
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
	bool isPrintSkeletonIndex = false;
	if (isPrintSkeletonIndex)
	{
		os << KHS53_SkeletonIndexToStdString(f.skeletonIndex);
	}

	/** @brief Print frame number / 打印帧序号 */
	bool isPrintFrameNumber = false;
	if (isPrintFrameNumber)
	{
		auto oldFlagsFrame = os.flags();
		os << ", frameNumber=" << std::dec << f.frameNumber;
		os.flags(oldFlagsFrame);
	}

	/** @brief Control whether to print quaternion / 控制是否打印四元数 */
	bool isPrintQuat = false;
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

	bool isPrintQuatEuler = false;
	if (isPrintQuatEuler)
	{
		/* Convert quaternion to Euler angles (roll, pitch, yaw) in degrees */
		float q_wxyz[4];
		std::copy(std::begin(f.quat_wxyz), std::end(f.quat_wxyz), q_wxyz);
		float euler[3];
		xsQEConverter converter;
		converter.QuatToEulerDegree(q_wxyz, 4, euler);
		/** @brief Save stream state / 保存流状态 */
		auto oldFlagsEuler = os.flags();
		auto oldPrecEuler = os.precision();
		/* Align with sign, fixed format, 3 decimal places, width 8 for each value */
		os << ",euler_deg:" << std::showpos << std::fixed << std::setprecision(2) << std::setw(6) << euler[0]
			<< " " << std::setw(6) << euler[1]
			<< " " << std::setw(6) << euler[2]
			<< std::noshowpos;
		/** @brief Restore stream state / 恢复流状态 */
		os.flags(oldFlagsEuler);
		os.precision(oldPrecEuler);
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

	/** @brief Control whether to print statusWord / 控制是否打印 statusWord */
	bool isPrintStatusWord = true;
	if (isPrintStatusWord)
	{
		auto oldFlagsStatus = os.flags();
		os << ", statusWord=" << std::dec << f.status.statusWord;
		os.flags(oldFlagsStatus);
	}

	os << " \r\n";
}

/**
 * @brief Print IMULinkFrame as numeric-only CSV.
 * @brief-cn 以仅包含数字的 CSV 格式打印 IMULinkFrame。
 *
 * Output columns:
 * suitNumber,skeletonIndex,frameNumber,
 * quat_w,quat_x,quat_y,quat_z,
 * accel_x,accel_y,accel_z,
 * gyro_x,gyro_y,gyro_z,
 * mag_x,mag_y,mag_z,
 * temperature_C
 */
inline void PrintIMULinkFrameCSV(std::ostream& os, const IMULinkFrame& f)
{
	auto oldFlags = os.flags();
	auto oldPrec = os.precision();

	os.setf(std::ios::fmtflags(0), std::ios::floatfield);
	os << std::fixed << std::setprecision(6);

	os << std::dec
		// << f.suitNumber << ','
		// << static_cast<uint32_t>(f.skeletonIndex) << ','
		// << f.frameNumber << ','
		<< f.quat_wxyz[0] << ',' << f.quat_wxyz[1] << ',' << f.quat_wxyz[2] << ',' << f.quat_wxyz[3] << ','
		<< f.accel_g[0] << ',' << f.accel_g[1] << ',' << f.accel_g[2] << ','
		<< f.gyro_dps[0] << ',' << f.gyro_dps[1] << ',' << f.gyro_dps[2] << ','
		// << f.mag_mGauss[0] << ',' << f.mag_mGauss[1] << ',' << f.mag_mGauss[2] << ','
		// <<f.status.statusWord << ','
		// << f.temperature_C
		<< "\n";

	os.flags(oldFlags);
	os.precision(oldPrec);
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
