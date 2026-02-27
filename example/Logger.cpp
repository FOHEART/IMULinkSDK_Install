#include "Logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#endif

static std::shared_ptr<spdlog::logger> g_logger_instance;
static std::string g_log_file_path;
static std::string g_sdk_log_dir;

static std::string make_timestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_snapshot;
#ifdef _WIN32
    localtime_s(&tm_snapshot, &t);
#else
    localtime_r(&t, &tm_snapshot);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_snapshot, "%Y%m%d_%H%M%S");
    return oss.str();
}

bool init_logger()
{
    // Determine a directory next to the running executable and create a 'log' folder there.
    std::string exe_dir;
#ifdef _WIN32
    char path_buf[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, path_buf, MAX_PATH);
    if (len == 0)
    {
        exe_dir = ".";
    }
    else
    {
        std::string full(path_buf, path_buf + len);
        auto pos = full.find_last_of("\\/");
        exe_dir = (pos == std::string::npos) ? std::string(".") : full.substr(0, pos);
    }
#else
    char path_buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path_buf, sizeof(path_buf) - 1);
    if (len == -1)
    {
        exe_dir = ".";
    }
    else
    {
        path_buf[len] = '\0';
        std::string full(path_buf);
        auto pos = full.find_last_of('/');
        exe_dir = (pos == std::string::npos) ? std::string(".") : full.substr(0, pos);
    }
#endif

    std::string log_dir = exe_dir + "/log";
#ifdef _WIN32
    _mkdir(log_dir.c_str());
#else
    mkdir(log_dir.c_str(), 0755);
#endif

    // Create a subdirectory 'imulinksdk' inside the log folder
    g_sdk_log_dir = log_dir + "/imulinksdk";
#ifdef _WIN32
    _mkdir(g_sdk_log_dir.c_str());
#else
    mkdir(g_sdk_log_dir.c_str(), 0755);
#endif

    try
    {
        std::string ts = make_timestamp();
        std::string filename = g_sdk_log_dir + "/" + ts + ".log";
        std::string logger_name = std::string("file_logger_") + ts;
        g_logger_instance = spdlog::basic_logger_mt(logger_name, filename);
        spdlog::set_default_logger(g_logger_instance);
        // Set pattern to include timestamp and level, but omit the logger name
        g_logger_instance->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info);

        g_log_file_path = filename;
        return true;
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cerr << "spdlog init failed: " << ex.what() << std::endl;
        return false;
    }
}

void flush_logger()
{
    if (g_logger_instance)
        g_logger_instance->flush();
}

void shutdown_logger()
{
    spdlog::shutdown();
    g_logger_instance.reset();
    g_log_file_path.clear();
}

std::shared_ptr<spdlog::logger> get_logger()
{
    return g_logger_instance;
}

std::string get_log_file_path()
{
    return g_log_file_path;
}

// New helper: prints filenames returned by get_log_filenames()
 void print_log_filenames()
{
	auto names = get_log_filenames();
	if (names.empty())
	{
		std::cout << "No log files found in log/imulinksdk." << std::endl;
		return;
	}

	std::cout << "Log files in log/imulinksdk:" << std::endl;
	for (const auto& n : names)
	{
		std::cout << "  " << n << std::endl;
	}
}

std::vector<std::string> get_log_filenames()
{
    std::vector<std::string> result;
    if (g_sdk_log_dir.empty())
        return result;

#ifdef _WIN32
    // Ensure backslashes for Windows API
    std::string dir = g_sdk_log_dir;
    for (auto &c : dir) if (c == '/') c = '\\';
    std::string pattern = dir + "\\*";

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE)
        return result;
    do
    {
        const char *name = fd.cFileName;
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
        {
            // only add files (skip directories)
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                result.emplace_back(name);
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *dir = opendir(g_sdk_log_dir.c_str());
    if (!dir)
        return result;
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        std::string name(ent->d_name);
        if (name == "." || name == "..")
            continue;
        std::string full = g_sdk_log_dir + "/" + name;
        struct stat st;
        if (stat(full.c_str(), &st) == 0)
        {
            if (S_ISREG(st.st_mode))
                result.emplace_back(name);
        }
    }
    closedir(dir);
#endif

    return result;
}
