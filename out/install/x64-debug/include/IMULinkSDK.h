/**
 * @file IMULinkSDK.h
 * @brief IMULinkSDK 公共 API 头文件
 * @brief Public API header for IMULinkSDK
 */

#pragma once

#include "imulinksdk_typesconfig.h"
#include <vector>
#include "IMULinkFrame.h"

/**
 * @brief 返回 SDK 版本号
 * @brief Returns the SDK  version number
 *
 * @return 版本号 / Version number as integer
 */
IMULINKSDK_DLL_API int imulinksdk_version(void);

/**
 * @brief 初始化 SDK，开启 UDP 监听并启动帧处理线程
 * @brief Initializes the SDK, starts UDP listener and frame-processing thread
 *
 * @return 成功返回 0，失败返回非 0 / Returns 0 on success, non-zero on failure
 */
IMULINKSDK_DLL_API int IMULinkSDK_Init(void);

/**
 * @brief 关闭 SDK，停止所有后台线程并释放资源
 * @brief Shuts down the SDK, stops all background threads and releases resources
 *
 * @return 成功返回 0，失败返回非 0 / Returns 0 on success, non-zero on failure
 */
IMULINKSDK_DLL_API int IMULinkSDK_Close(void);

/**
 * @brief 获取所有已发现套装的最新帧数据
 * @brief Retrieves the latest frames for all discovered suits
 *
 * @param[out] frames 用于接收套装帧数据的向量 / Vector to receive suit frame data
 * @return 写入 frames 的套装数量 / Number of suits written into frames
 */
IMULINKSDK_DLL_API unsigned int IMULinkSDK_GetSuitFrames(std::vector<IMULinkSuitFrames>& frames);

/**
 * @brief 将套装序列号添加到黑名单，黑名单中的设备将被 SDK 忽略，不会出现在 IMULinkSDK_GetSuitFrames 的结果中
 * @brief Adds suit serial numbers to the blacklist; blacklisted devices are ignored by the SDK
 *        and will not appear in IMULinkSDK_GetSuitFrames results
 *
 * @param suitNumbers 要加入黑名单的套装序列号列表 / List of suit serial numbers to blacklist
 * @return 成功返回 0，失败返回非 0 / Returns 0 on success, non-zero on failure
 *
 * @par 示例 / Example
 * @code
 * // 忽略序列号为 0x1403413E 的套装 / Ignore suit with serial number 0x1403413E
 * std::vector<unsigned int> blacklist = { 0x1403413E };
 * IMULinkSDK_AddToBlacklist(blacklist);
 *
 * // 同时忽略多个套装 / Ignore multiple suits at once
 * std::vector<unsigned int> blacklist = { 0x1403413E, 0x1403403E };
 * IMULinkSDK_AddToBlacklist(blacklist);
 * @endcode
 */
IMULINKSDK_DLL_API int IMULinkSDK_AddToBlacklist(const std::vector<unsigned int>& suitNumbers);

/**
 * @brief 获取当前黑名单列表
 * @brief Returns a reference to the current blacklist of suit serial numbers
 *
 * @return 黑名单套装序列号向量的引用 / Reference to the vector of blacklisted suit serial numbers
 *
 * @par 示例 / Example
 * @code
 * std::vector<unsigned int>& currentBlacklist = IMULinkSDK_GetBlacklist();
 * for (const auto& sn : currentBlacklist)
 *     std::cout << std::showbase << std::hex << std::uppercase << sn << " ";
 * @endcode
 */
IMULINKSDK_DLL_API std::vector<unsigned int>& IMULinkSDK_GetBlacklist();

/**
 * @brief 清空黑名单列表
 * @brief Clears the blacklist
 *
 * @return 成功返回 0，失败返回非 0 / Returns 0 on success, non-zero on failure
 */
IMULINKSDK_DLL_API int IMULinkSDK_ClearBlacklist();

/**
 * @brief 将套装序列号添加到白名单；白名单非空时，仅白名单中的套装会出现在 IMULinkSDK_GetSuitFrames 的结果中
 * @brief Adds suit serial numbers to the whitelist; when the whitelist is non-empty,
 *        only whitelisted suits appear in IMULinkSDK_GetSuitFrames results
 *
 * @param suitNumbers 要加入白名单的套装序列号列表 / List of suit serial numbers to whitelist
 * @return 成功返回 0，失败返回非 0 / Returns 0 on success, non-zero on failure
 */
IMULINKSDK_DLL_API int IMULinkSDK_AddToWhitelist(const std::vector<unsigned int>& suitNumbers);

/**
 * @brief 获取当前白名单列表
 * @brief Returns a reference to the current whitelist of suit serial numbers
 *
 * @return 白名单套装序列号向量的引用 / Reference to the vector of whitelisted suit serial numbers
 */
IMULINKSDK_DLL_API std::vector<unsigned int>& IMULinkSDK_GetWhitelist();

/**
 * @brief 清空白名单列表；清空后恢复为接收所有套装（仍受黑名单影响）
 * @brief Clears the whitelist; after clearing, all suits are accepted again (blacklist still applies)
 *
 * @return 成功返回 0，失败返回非 0 / Returns 0 on success, non-zero on failure
 */
IMULINKSDK_DLL_API int IMULinkSDK_ClearWhitelist();
