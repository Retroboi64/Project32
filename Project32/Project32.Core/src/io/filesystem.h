#pragma once

#include "../common.h"

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace project32::io {
    class filesystem {
    public:
        static std::optional<std::string> read_text_file(const std::string& path);
        static std::optional<std::vector<char>> read_binary_file(const std::string& path);

        static bool write_text_file(const std::string& path, const std::string& content);
        static bool write_binary_file(const std::string& path, const std::vector<char>& data);
        static bool file_exists(const std::string& path);
        static bool directory_exists(const std::string& path);
        static bool create_directory(const std::string& path);

        static std::string get_extension(const std::string& path);
        static std::string get_filename_without_extension(const std::string& path);
    };
} 