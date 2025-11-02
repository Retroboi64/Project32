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

#ifndef MANIFEST_H
#define MANIFEST_H

#include "../common.h"

namespace project32::io {
    using manifest_value = std::variant<std::string, double, std::vector<std::string>>;

    class manifest {
    public:
        manifest() = default;

        static std::optional<manifest> parse(const std::string& content);
        static std::optional<manifest> load_from_file(const std::string& path);
        std::string serialize() const;
        bool save_to_file(const std::string& path) const;
        void set_string(const std::string& key, const std::string& value);
        std::optional<std::string> get_string(const std::string& key) const;
        void set_number(const std::string& key, double value);
        std::optional<double> get_number(const std::string& key) const;
        void set_list(const std::string& key, const std::vector<std::string>& value);
        std::optional<std::vector<std::string>> get_list(const std::string& key) const;
        bool has_key(const std::string& key) const;
        void remove_key(const std::string& key);
        std::vector<std::string> get_all_keys() const;
        void clear();

    private:
        std::map<std::string, manifest_value> data_;

        static std::string trim(const std::string& str);
        static std::string strip_quotes(const std::string& str);
        static std::vector<std::string> parse_list(const std::string& str);
    };
}

#endif // MANIFEST_H