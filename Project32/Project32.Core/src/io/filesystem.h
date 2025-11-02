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

#endif // FILESYSTEM_H