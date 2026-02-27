#include "example2.h"
#include "include/IMULinkSDK.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <atomic>
#include <mutex>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "Logger.h"

#include <memory>

#include <sstream>
#include <iomanip>
#include <ctime>

/**
 * @brief Run open/close loop test which polls suit frames and logs output.
 * @brief-cn 运行开关测试：轮询套装帧并记录输出。
 * @param testRound Number of open/close iterations to run. / 运行的开关次数。
 * @return int 0 on success, non-zero on failure. / 成功返回 0，失败返回非零。
 */
static int IMULinkSDK_openCloseLoopTest(int testRound)
{
	int ret = -1;
	for (int i = 0; i < testRound; ++i)
	{
		std::cout << "Loop iteration " << (i + 1) << std::endl;
		spdlog::info("Loop iteration {}", (i + 1));
		/** @brief Initialize the SDK and start the listener. / 初始化 SDK 并启动监听器。 */
		int imuLinkRet = IMULinkSDK_Init();

		

		if (imuLinkRet != 0)
		{
			std::cerr << "IMULinkSDK_Init failed: " << imuLinkRet << std::endl;
			return imuLinkRet;
		}

		/** @brief Poll interval for GetSuitFrames. / 轮询间隔。 */
		const auto pollInterval = std::chrono::milliseconds(10);
		/** @brief Maximum runtime for each open/close iteration. / 每次迭代的最大运行时间。 */
		const auto maxRunTime = std::chrono::seconds(50);

		/** @brief Flag to stop the polling thread. / 停止轮询线程的标志。 */
		std::atomic<bool> stopPolling(false);
		/** @brief Mutex used to serialize printing to stdout. / 用于序列化输出打印的互斥量。 */
		std::mutex printMutex;

		auto pollStart = std::chrono::steady_clock::now();
		std::thread pollThread([&]() {
			while (!stopPolling.load())
			{
				auto now = std::chrono::steady_clock::now();
				if (now - pollStart >= maxRunTime)
				{
					break;
				}

				std::vector<IMULinkSuitFrames> frames;
				IMULinkSDK_GetSuitFrames(frames);

				if (!frames.empty())
				{
					std::lock_guard<std::mutex> lock(printMutex);
					for (auto itor = frames.begin(); itor != frames.end(); ++itor)
					{
						/** @brief Index within the suit frames to print (default head). / 要打印的索引（默认 头部）。 */
						KHS53_SkeletonIndex printIndex = KHS53_Head;

						PrintIMULinkFrame(std::cout, itor->rawFrames[printIndex]);
                        
					}
				}

				std::this_thread::sleep_for(pollInterval);
			}
		});

		/** @brief Let polling run up to maxRunTime. / 让轮询运行至最大运行时间。 */
		std::this_thread::sleep_for(maxRunTime);

		stopPolling.store(true);
		if (pollThread.joinable())
		{
			pollThread.join();
		}

		/** @brief Close the SDK and stop the listener. / 关闭 SDK 并停止监听器。 */
		int closeRet = IMULinkSDK_Close();
		if (closeRet != 0)
		{
			std::cerr << "IMULinkSDK_Close failed: " << closeRet << std::endl;
			return closeRet;
		}
		/** @brief Wait 1 second before next iteration. / 在下一次迭代前等待 1 秒。 */
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	ret = 0;
	return ret;
}



/**
 * @brief Print usage information for command-line options.
 * @brief-cn 打印命令行选项的使用信息。
 * @param progName Program name (argv[0]) or nullptr. / 程序名（argv[0]）或 nullptr。
 */
static void print_usage(const char* progName)
{
	std::cout << "Usage: " << (progName ? progName : "example2")
		<< " [options] [testCount]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  --help, -h          Show this help message and exit." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "Arguments:" << std::endl;
	std::cout << "  testCount           Optional positive integer specifying how many "
		<< "open/close iterations to run. Default is 5." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "Examples:" << std::endl;
	std::cout << "  " << (progName ? progName : "example2")
		<< "              Run with default 5 iterations." << std::endl;
	std::cout << "  " << (progName ? progName : "example2")
		<< " 10           Run with 10 iterations." << std::endl;
}

/** @brief Note: Logger is provided by example/Logger.{h,cpp} / 日志由 example/Logger.{h,cpp} 提供。 */

int main(int argc, char* argv[])
{
	/** @brief Handle help flag early. / 早期处理帮助标志。 */
	for (int i = 1; i < argc; ++i)
	{
		std::string a(argv[i]);
		if (a == "--help" || a == "-h")
		{
			print_usage(argv[0]);
			/** @brief Shutdown logger before exit. / 退出前关闭日志。 */
			flush_logger();
			shutdown_logger();
			return 0;
		}
	}

	/** @brief Initialize global logger with timestamped filename to avoid conflicts. / 初始化全局日志，使用带时间戳的文件名以避免冲突。 */
	if (!init_logger())
	{
		std::cerr << "Failed to initialize logger; continuing without file log." << std::endl;
	}
	else
	{
		spdlog::info("Example2 test log initialized");
		/** @brief Print existing log filenames. / 打印现有日志文件名。 */
		print_log_filenames();
	}



	/** @brief Default test count if not provided via argv. / 未提供时的默认测试次数。 */
	int testCount = 5; // default
	if (argc > 1)
	{
		try
		{
			testCount = std::stoi(std::string(argv[1]));
			if (testCount <= 0)
			{
				std::cerr << "Invalid test count: must be a positive integer." << std::endl;
				return -1;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "Failed to parse test count from argument '" << argv[1]
				<< "': " << e.what() << std::endl;
			return -1;
		}
	}

	std::cout << "Running open/close loop test with " << testCount << " iterations."
		<< std::endl;

	int ret = IMULinkSDK_openCloseLoopTest(testCount);
	if (ret != 0)
	{
		std::cerr << "IMULinkSDK_openCloseLoopTest failed: " << ret << std::endl;
		return -1;
	}
	std::cout << "Test end" << std::endl;
	// flush and shutdown shared logger
	flush_logger();
	shutdown_logger();
	std::cin.get(); // Wait for user input before exiting
	return 0;
}
