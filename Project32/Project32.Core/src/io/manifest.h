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

#include "filesystem.h"

namespace project32::io {

    class manifest;
    class manifest_schema;

    using manifest_value = std::variant<std::string, double, bool, std::vector<std::string>>;

    struct manifest_error {
        std::string key;
        std::string message;

        manifest_error(std::string k, std::string msg)
            : key(std::move(k)), message(std::move(msg)) {
        }

        std::string to_string() const {
            return "Manifest Error [" + key + "]: " + message;
        }
    };

    template<typename T>
    using manifest_result = std::variant<T, manifest_error>;

    template<typename T>
    struct is_manifest_type : std::false_type {};

    template<> struct is_manifest_type<std::string> : std::true_type {};
    template<> struct is_manifest_type<double> : std::true_type {};
    template<> struct is_manifest_type<bool> : std::true_type {};
    template<> struct is_manifest_type<int> : std::true_type {};
    template<> struct is_manifest_type<std::vector<std::string>> : std::true_type {};

    using validation_func = std::function<bool(const manifest_value&)>;

    /*
     * @brief Schema for validating manifest files
     */
    class manifest_schema {
    public:
        manifest_schema() = default;

        manifest_schema& require_string(const std::string& key);
        manifest_schema& require_number(const std::string& key);
        manifest_schema& require_bool(const std::string& key);
        manifest_schema& require_list(const std::string& key);

        manifest_schema& optional_string(const std::string& key, std::string default_val);
        manifest_schema& optional_number(const std::string& key, double default_val);
        manifest_schema& optional_bool(const std::string& key, bool default_val);
        manifest_schema& optional_list(const std::string& key, std::vector<std::string> default_val);

        manifest_schema& add_validator(const std::string& key, validation_func validator);

        manifest_schema& number_range(const std::string& key, double min, double max);

        manifest_schema& string_not_empty(const std::string& key);
        manifest_schema& string_one_of(const std::string& key, std::vector<std::string> allowed);

        manifest_schema& list_not_empty(const std::string& key);
        manifest_schema& list_min_size(const std::string& key, size_t min_size);
        manifest_schema& list_max_size(const std::string& key, size_t max_size);

        manifest_result<bool> validate(const manifest& mf) const;

        void apply_defaults(manifest& mf) const;

    private:
        struct field_spec {
            std::string key;
            std::optional<manifest_value> default_value;
            bool required = false;
            std::vector<validation_func> validators;
        };

        std::map<std::string, field_spec> fields_;

        bool validate_field(const std::string& key, const manifest_value& value) const;
    };

    /**
     * @brief Configuration manifest parser and serializer
     */
    class manifest {
    public:
        manifest() = default;

        static manifest_result<manifest> parse(std::string_view content);
        static manifest_result<manifest> load_from_file(const std::string& path);

        std::string serialize() const;
        io_result<bool> save_to_file(const std::string& path, const write_options& options = {}) const;

        void set_string(const std::string& key, std::string value);
        void set_number(const std::string& key, double value);
        void set_bool(const std::string& key, bool value);
        void set_list(const std::string& key, std::vector<std::string> value);

        template<typename T>
        void set(const std::string& key, T&& value) {
            static_assert(is_manifest_type<std::decay_t<T>>::value,
                "Type not supported by manifest");

            if constexpr (std::is_same_v<std::decay_t<T>, int>) {
                data_[key] = static_cast<double>(value);
            }
            else {
                data_[key] = std::forward<T>(value);
            }
        }

        std::string get_string(const std::string& key, std::string_view default_val = "") const;
        double get_number(const std::string& key, double default_val = 0.0) const;
        bool get_bool(const std::string& key, bool default_val = false) const;
        std::vector<std::string> get_list(const std::string& key,
            const std::vector<std::string>& default_val = {}) const;

        std::optional<std::string> try_get_string(const std::string& key) const;
        std::optional<double> try_get_number(const std::string& key) const;
        std::optional<bool> try_get_bool(const std::string& key) const;
        std::optional<std::vector<std::string>> try_get_list(const std::string& key) const;

        template<typename T>
        std::optional<T> try_get(const std::string& key) const {
            auto it = data_.find(key);
            if (it == data_.end()) {
                return std::nullopt;
            }

            if constexpr (std::is_same_v<T, int>) {
                if (auto* num = std::get_if<double>(&it->second)) {
                    return static_cast<int>(*num);
                }
            }
            else if (auto* val = std::get_if<T>(&it->second)) {
                return *val;
            }

            return std::nullopt;
        }

        bool has_key(const std::string& key) const;
        void remove_key(const std::string& key);
        std::vector<std::string> get_all_keys() const;

        bool is_string(const std::string& key) const;
        bool is_number(const std::string& key) const;
        bool is_bool(const std::string& key) const;
        bool is_list(const std::string& key) const;

        void clear();
        size_t size() const { return data_.size(); }
        bool empty() const { return data_.empty(); }

        void merge(const manifest& other, bool overwrite = true);

        manifest clone() const { return *this; }

    private:
        std::map<std::string, manifest_value> data_;

        static std::string_view trim(std::string_view str);
        static std::string strip_quotes(std::string_view str);
        static manifest_result<std::vector<std::string>> parse_list(std::string_view str);
        static manifest_result<bool> parse_bool(std::string_view str);

        static std::string escape_string(std::string_view str);
        static std::string serialize_value(const manifest_value& value);
    };

} // namespace project32::io

#endif // MANIFEST_H