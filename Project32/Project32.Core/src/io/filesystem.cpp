#include "../common.h"

#include "filesystem.h" 

namespace project32::io {
    std::optional<std::string> filesystem::read_text_file(const std::string& path) {
        std::ifstream file(path, std::ios::in | std::ios::ate);
        if (!file.is_open()) {
            return std::nullopt;
        }

        const auto size = file.tellg();
        if (size < 0) {
            return std::nullopt;
        }

        file.seekg(0, std::ios::beg);
        std::string content;
        content.resize(static_cast<size_t>(size));

        if (!file.read(content.data(), size)) {
            return std::nullopt;
        }

        return content;
    }

    std::optional<std::vector<char>> filesystem::read_binary_file(const std::string& path) {
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return std::nullopt;
        }

        const auto size = file.tellg();
        if (size < 0) {
            return std::nullopt;
        }

        file.seekg(0, std::ios::beg);
        std::vector<char> buffer;
        buffer.resize(static_cast<size_t>(size));

        if (!file.read(buffer.data(), size)) {
            return std::nullopt;
        }

        return buffer;
    }

    bool filesystem::write_text_file(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }

        file.write(content.data(), content.size());
        return file.good();
    }

    bool filesystem::write_binary_file(const std::string& path, const std::vector<char>& data) {
        std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }

        file.write(data.data(), data.size());
        return file.good();
    }

    bool filesystem::file_exists(const std::string& path) {
        std::error_code ec;
        return std::filesystem::exists(path, ec) && std::filesystem::is_regular_file(path, ec);
    }

    bool filesystem::directory_exists(const std::string& path) {
        std::error_code ec;
        return std::filesystem::exists(path, ec) && std::filesystem::is_directory(path, ec);
    }

    bool filesystem::create_directory(const std::string& path) {
        std::error_code ec;
        return std::filesystem::create_directories(path, ec);
    }

    std::string filesystem::get_extension(const std::string& path) {
        std::filesystem::path p(path);
        return p.extension().string();
    }

    std::string filesystem::get_filename_without_extension(const std::string& path) {
        std::filesystem::path p(path);
        return p.stem().string();
    }
}