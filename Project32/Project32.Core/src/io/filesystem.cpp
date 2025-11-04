/*
 * This file is part of Project32 - A compact yet powerful and flexible C++ Game Engine
 * Copyright (c) 2025 Patrick Reese (Retroboi64)
 *
 * Licensed under MIT with Attribution Requirements
 * See LICENSE file for full terms
 * GitHub: https://github.com/Retroboi64/Project32
 *
 * This header must not be removed from any source file.
 */

#include "../common.h"
#include "filesystem.h"
#include <random>
#include <sstream>
#include <chrono>

namespace project32::io {

    io_error filesystem::make_system_error(
        const std::string& path,
        const std::string& operation,
        const std::error_code& ec
    ) {
        return io_error(path, operation + " failed", ec);
    }

    io_error filesystem::make_error(
        const std::string& path,
        const std::string& message
    ) {
        return io_error(path, message, {});
    }

    bool filesystem::ensure_directory_exists(const std::filesystem::path& path) {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec)) {
            return std::filesystem::create_directories(path, ec);
        }
        return true;
    }

    io_result<std::string> filesystem::read_text_file(
        const std::string& path,
        const read_options& options
    ) {
        std::ifstream file(path, std::ios::in | std::ios::ate);

        if (!file.is_open()) {
            return make_error(path, "Failed to open file for reading");
        }

        const auto size = file.tellg();
        if (size < 0) {
            return make_error(path, "Failed to determine file size");
        }

        const size_t file_size = static_cast<size_t>(size);

        if (file_size > options.max_size) {
            return make_error(
                path,
                "File size (" + std::to_string(file_size) +
                " bytes) exceeds maximum allowed (" +
                std::to_string(options.max_size) + " bytes)"
            );
        }

        if (file_size == 0 && !options.allow_empty) {
            return make_error(path, "File is empty");
        }

        file.seekg(0, std::ios::beg);
        std::string content;
        content.resize(file_size);

        if (!file.read(content.data(), file_size)) {
            return make_error(path, "Failed to read file content");
        }

        if (options.trim_whitespace && !content.empty()) {
            const auto start = content.find_first_not_of(" \t\n\r");
            const auto end = content.find_last_not_of(" \t\n\r");

            if (start != std::string::npos && end != std::string::npos) {
                content = content.substr(start, end - start + 1);
            }
            else {
                content.clear();
            }
        }

        return content;
    }

    io_result<std::vector<std::string>> filesystem::read_lines(
        const std::string& path,
        const read_options& options
    ) {
        auto content_result = read_text_file(path, options);
        if (is_error(content_result)) {
            return get_error(content_result);
        }

        const std::string& content = get_value(content_result);
        std::vector<std::string> lines;
        std::istringstream stream(content);
        std::string line;

        while (std::getline(stream, line)) {
            if (options.trim_whitespace) {
                const auto start = line.find_first_not_of(" \t\r");
                const auto end = line.find_last_not_of(" \t\r");

                if (start != std::string::npos && end != std::string::npos) {
                    lines.push_back(line.substr(start, end - start + 1));
                }
                else if (options.allow_empty) {
                    lines.push_back("");
                }
            }
            else {
                lines.push_back(std::move(line));
            }
        }

        return lines;
    }

    io_result<bool> filesystem::write_text_file(
        const std::string& path,
        std::string_view content,
        const write_options& options
    ) {
        std::filesystem::path fs_path(path);

        if (options.create_directories) {
            if (fs_path.has_parent_path()) {
                if (!ensure_directory_exists(fs_path.parent_path())) {
                    return make_error(path, "Failed to create parent directories");
                }
            }
        }

        std::string write_path = path;
        std::string temp_path;

        if (options.atomic_write) {
            temp_path = path + ".tmp." + generate_temp_filename();
            write_path = temp_path;
        }

        std::ios_base::openmode mode = std::ios::out;
        if (options.append) {
            mode |= std::ios::app;
        }
        else {
            mode |= std::ios::trunc;
        }

        std::ofstream file(write_path, mode);
        if (!file.is_open()) {
            return make_error(path, "Failed to open file for writing");
        }

        file.write(content.data(), content.size());

        if (!file.good()) {
            file.close();
            if (!temp_path.empty()) {
                std::error_code ec;
                std::filesystem::remove(temp_path, ec);
            }
            return make_error(path, "Failed to write file content");
        }

        file.close();

        if (options.atomic_write) {
            std::error_code ec;
            std::filesystem::rename(temp_path, path, ec);
            if (ec) {
                std::filesystem::remove(temp_path, ec);
                return make_system_error(path, "Atomic write rename", ec);
            }
        }

        return true;
    }

    io_result<std::vector<char>> filesystem::read_binary_file(
        const std::string& path,
        size_t max_size
    ) {
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            return make_error(path, "Failed to open binary file for reading");
        }

        const auto size = file.tellg();
        if (size < 0) {
            return make_error(path, "Failed to determine file size");
        }

        const size_t file_size = static_cast<size_t>(size);

        if (file_size > max_size) {
            return make_error(
                path,
                "Binary file size (" + std::to_string(file_size) +
                " bytes) exceeds maximum allowed (" +
                std::to_string(max_size) + " bytes)"
            );
        }

        file.seekg(0, std::ios::beg);
        std::vector<char> buffer;
        buffer.resize(file_size);

        if (!file.read(buffer.data(), file_size)) {
            return make_error(path, "Failed to read binary file content");
        }

        return buffer;
    }

    io_result<bool> filesystem::write_binary_file(
        const std::string& path,
        std::span<const char> data,
        const write_options& options
    ) {
        std::filesystem::path fs_path(path);

        if (options.create_directories) {
            if (fs_path.has_parent_path()) {
                if (!ensure_directory_exists(fs_path.parent_path())) {
                    return make_error(path, "Failed to create parent directories");
                }
            }
        }

        std::string write_path = path;
        std::string temp_path;

        if (options.atomic_write) {
            temp_path = path + ".tmp." + generate_temp_filename();
            write_path = temp_path;
        }

        std::ios_base::openmode mode = std::ios::out | std::ios::binary;
        if (options.append) {
            mode |= std::ios::app;
        }
        else {
            mode |= std::ios::trunc;
        }

        std::ofstream file(write_path, mode);
        if (!file.is_open()) {
            return make_error(path, "Failed to open binary file for writing");
        }

        file.write(data.data(), data.size());

        if (!file.good()) {
            file.close();
            if (!temp_path.empty()) {
                std::error_code ec;
                std::filesystem::remove(temp_path, ec);
            }
            return make_error(path, "Failed to write binary file content");
        }

        file.close();

        if (options.atomic_write) {
            std::error_code ec;
            std::filesystem::rename(temp_path, path, ec);
            if (ec) {
                std::filesystem::remove(temp_path, ec);
                return make_system_error(path, "Atomic write rename", ec);
            }
        }

        return true;
    }

    io_result<size_t> filesystem::get_file_size(const std::string& path) {
        std::error_code ec;
        auto size = std::filesystem::file_size(path, ec);

        if (ec) {
            return make_system_error(path, "Get file size", ec);
        }

        return static_cast<size_t>(size);
    }

    io_result<std::filesystem::file_time_type> filesystem::get_last_write_time(
        const std::string& path
    ) {
        std::error_code ec;
        auto time = std::filesystem::last_write_time(path, ec);

        if (ec) {
            return make_system_error(path, "Get last write time", ec);
        }

        return time;
    }

    bool filesystem::file_exists(const std::string& path) noexcept {
        std::error_code ec;
        return std::filesystem::exists(path, ec) &&
            std::filesystem::is_regular_file(path, ec);
    }

    bool filesystem::directory_exists(const std::string& path) noexcept {
        std::error_code ec;
        return std::filesystem::exists(path, ec) &&
            std::filesystem::is_directory(path, ec);
    }

    bool filesystem::path_exists(const std::string& path) noexcept {
        std::error_code ec;
        return std::filesystem::exists(path, ec);
    }

    io_result<bool> filesystem::create_directory(
        const std::string& path,
        bool create_parents
    ) {
        std::error_code ec;
        bool result;

        if (create_parents) {
            result = std::filesystem::create_directories(path, ec);
        }
        else {
            result = std::filesystem::create_directory(path, ec);
        }

        if (ec) {
            return make_system_error(path, "Create directory", ec);
        }

        return result;
    }

    io_result<bool> filesystem::remove_file(const std::string& path) {
        if (!file_exists(path)) {
            return make_error(path, "File does not exist");
        }

        std::error_code ec;
        bool result = std::filesystem::remove(path, ec);

        if (ec) {
            return make_system_error(path, "Remove file", ec);
        }

        return result;
    }

    io_result<bool> filesystem::remove_directory(
        const std::string& path,
        bool recursive
    ) {
        if (!directory_exists(path)) {
            return make_error(path, "Directory does not exist");
        }

        std::error_code ec;

        if (recursive) {
            std::filesystem::remove_all(path, ec);
        }
        else {
            std::filesystem::remove(path, ec);
        }

        if (ec) {
            return make_system_error(path, "Remove directory", ec);
        }

        return true;
    }

    io_result<std::vector<std::string>> filesystem::list_directory(
        const std::string& path,
        bool recursive
    ) {
        if (!directory_exists(path)) {
            return make_error(path, "Directory does not exist");
        }

        std::vector<std::string> entries;
        std::error_code ec;

        try {
            if (recursive) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(path, ec)) {
                    if (ec) break;
                    entries.push_back(entry.path().string());
                }
            }
            else {
                for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
                    if (ec) break;
                    entries.push_back(entry.path().string());
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            return make_error(path, std::string("Directory iteration failed: ") + e.what());
        }

        if (ec) {
            return make_system_error(path, "List directory", ec);
        }

        return entries;
    }

    std::string filesystem::get_extension(const std::string& path) noexcept {
        std::filesystem::path p(path);
        return p.extension().string();
    }

    std::string filesystem::get_filename(const std::string& path) noexcept {
        std::filesystem::path p(path);
        return p.filename().string();
    }

    std::string filesystem::get_filename_without_extension(const std::string& path) noexcept {
        std::filesystem::path p(path);
        return p.stem().string();
    }

    std::string filesystem::get_parent_path(const std::string& path) noexcept {
        std::filesystem::path p(path);
        return p.parent_path().string();
    }

    std::string filesystem::get_absolute_path(const std::string& path) noexcept {
        std::error_code ec;
        auto abs_path = std::filesystem::absolute(path, ec);
        return ec ? path : abs_path.string();
    }

    std::string filesystem::normalize_path(const std::string& path) noexcept {
        std::filesystem::path p(path);
        return p.lexically_normal().string();
    }

    io_result<bool> filesystem::copy_file(
        const std::string& from,
        const std::string& to,
        bool overwrite
    ) {
        if (!file_exists(from)) {
            return make_error(from, "Source file does not exist");
        }

        std::error_code ec;
        auto options = overwrite ?
            std::filesystem::copy_options::overwrite_existing :
            std::filesystem::copy_options::none;

        std::filesystem::copy_file(from, to, options, ec);

        if (ec) {
            return make_system_error(from, "Copy file to " + to, ec);
        }

        return true;
    }

    io_result<bool> filesystem::move_file(
        const std::string& from,
        const std::string& to
    ) {
        if (!file_exists(from)) {
            return make_error(from, "Source file does not exist");
        }

        std::error_code ec;
        std::filesystem::rename(from, to, ec);

        if (ec) {
            return make_system_error(from, "Move file to " + to, ec);
        }

        return true;
    }

    std::string filesystem::get_temp_directory() noexcept {
        std::error_code ec;
        auto temp_path = std::filesystem::temp_directory_path(ec);
        return ec ? "" : temp_path.string();
    }

    std::string filesystem::generate_temp_filename(std::string_view prefix) noexcept {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<uint64_t> dis;

        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();

        std::ostringstream oss;
        oss << prefix << "_" << timestamp << "_" << std::hex << dis(gen);

        return oss.str();
    }

} // namespace project32::io