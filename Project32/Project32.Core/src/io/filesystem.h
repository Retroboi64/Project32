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

#pragma once

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../common.h"
#include <system_error>
#include <span>

namespace project32::io {

    struct io_error {
        std::string path;
        std::string message;
        std::error_code error_code;

        io_error(std::string p, std::string msg, std::error_code ec = {})
            : path(std::move(p)), message(std::move(msg)), error_code(ec) {
        }

        std::string to_string() const {
            return "IO Error [" + path + "]: " + message +
                (error_code ? " (" + error_code.message() + ")" : "");
        }
    };

    template<typename T>
    using io_result = std::variant<T, io_error>;

    template<typename T>
    bool is_ok(const io_result<T>& result) {
        return std::holds_alternative<T>(result);
    }

    template<typename T>
    bool is_error(const io_result<T>& result) {
        return std::holds_alternative<io_error>(result);
    }

    template<typename T>
    T& get_value(io_result<T>& result) {
        if (auto* val = std::get_if<T>(&result)) {
            return *val;
        }
        throw std::runtime_error(std::get<io_error>(result).to_string());
    }

    template<typename T>
    const T& get_value(const io_result<T>& result) {
        if (auto* val = std::get_if<T>(&result)) {
            return *val;
        }
        throw std::runtime_error(std::get<io_error>(result).to_string());
    }

    template<typename T>
    const io_error& get_error(const io_result<T>& result) {
        return std::get<io_error>(result);
    }

    struct file_limits {
        static constexpr size_t max_text_file_size = 100 * 1024 * 1024;  // 100 MB
        static constexpr size_t max_binary_file_size = 500 * 1024 * 1024; // 500 MB
    };

    struct read_options {
        size_t max_size = file_limits::max_text_file_size;
        bool allow_empty = true;
        bool trim_whitespace = false;
    };

    struct write_options {
        bool create_directories = false;
        bool atomic_write = false;
        bool append = false;
    };

    class filesystem {
    public:
        static io_result<std::string> read_text_file(
            const std::string& path,
            const read_options& options = {}
        );

        static io_result<std::vector<std::string>> read_lines(
            const std::string& path,
            const read_options& options = {}
        );

        static io_result<bool> write_text_file(
            const std::string& path,
            std::string_view content,
            const write_options& options = {}
        );

        static io_result<std::vector<char>> read_binary_file(
            const std::string& path,
            size_t max_size = file_limits::max_binary_file_size
        );

        static io_result<bool> write_binary_file(
            const std::string& path,
            std::span<const char> data,
            const write_options& options = {}
        );

        static io_result<size_t> get_file_size(const std::string& path);
        static io_result<std::filesystem::file_time_type> get_last_write_time(const std::string& path);

        static bool file_exists(const std::string& path) noexcept;
        static bool directory_exists(const std::string& path) noexcept;
        static bool path_exists(const std::string& path) noexcept;

        static io_result<bool> create_directory(
            const std::string& path,
            bool create_parents = true
        );

        static io_result<bool> remove_file(const std::string& path);
        static io_result<bool> remove_directory(
            const std::string& path,
            bool recursive = false
        );

        static io_result<std::vector<std::string>> list_directory(
            const std::string& path,
            bool recursive = false
        );

        static std::string get_extension(const std::string& path) noexcept;
        static std::string get_filename(const std::string& path) noexcept;
        static std::string get_filename_without_extension(const std::string& path) noexcept;
        static std::string get_parent_path(const std::string& path) noexcept;
        static std::string get_absolute_path(const std::string& path) noexcept;
        static std::string normalize_path(const std::string& path) noexcept;

        static io_result<bool> copy_file(
            const std::string& from,
            const std::string& to,
            bool overwrite = false
        );

        static io_result<bool> move_file(
            const std::string& from,
            const std::string& to
        );

        static std::string get_temp_directory() noexcept;
        static std::string generate_temp_filename(std::string_view prefix = "tmp") noexcept;

    private:
        static io_error make_system_error(
            const std::string& path,
            const std::string& operation,
            const std::error_code& ec
        );

        static io_error make_error(
            const std::string& path,
            const std::string& message
        );

        static bool ensure_directory_exists(const std::filesystem::path& path);
    };

} // namespace project32::io

#endif // FILESYSTEM_H