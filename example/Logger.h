#pragma once
#include <memory>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

bool init_logger();
void flush_logger();
void shutdown_logger();
std::shared_ptr<spdlog::logger> get_logger();

// Returns the full path to the current log file, or empty string if none.
std::string get_log_file_path();

// Returns the file names contained in the 'log/imulinksdk' folder (not full paths).
std::vector<std::string> get_log_filenames();

// Helper to print the log filenames to console for demonstration purposes.
void print_log_filenames();
