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
#include "manifest.h"

#include <sstream>
#include <iostream>

namespace project32::io {

    template<typename T>
    bool is_error(const manifest_result<T>& result) {
        return std::holds_alternative<manifest_error>(result);
    }

    template<typename T>
    bool is_ok(const manifest_result<T>& result) {
        return std::holds_alternative<T>(result);
    }

    template<typename T>
    const manifest_error& get_error(const manifest_result<T>& result) {
        return std::get<manifest_error>(result);
    }

    template<typename T>
    T& get_value(manifest_result<T>& result) {
        if (auto* val = std::get_if<T>(&result)) {
            return *val;
        }
        throw std::runtime_error(std::get<manifest_error>(result).to_string());
    }

    template<typename T>
    const T& get_value(const manifest_result<T>& result) {
        if (auto* val = std::get_if<T>(&result)) {
            return *val;
        }
        throw std::runtime_error(std::get<manifest_error>(result).to_string());
    }

    // manifest_schema implementation
    manifest_schema& manifest_schema::require_string(const std::string& key) {
        fields_[key] = field_spec{ key, std::nullopt, true };
        return *this;
    }

    manifest_schema& manifest_schema::require_number(const std::string& key) {
        fields_[key] = field_spec{ key, std::nullopt, true };
        return *this;
    }

    manifest_schema& manifest_schema::require_bool(const std::string& key) {
        fields_[key] = field_spec{ key, std::nullopt, true };
        return *this;
    }

    manifest_schema& manifest_schema::require_list(const std::string& key) {
        fields_[key] = field_spec{ key, std::nullopt, true };
        return *this;
    }

    manifest_schema& manifest_schema::optional_string(const std::string& key, std::string default_val) {
        fields_[key] = field_spec{ key, std::move(default_val), false };
        return *this;
    }

    manifest_schema& manifest_schema::optional_number(const std::string& key, double default_val) {
        fields_[key] = field_spec{ key, default_val, false };
        return *this;
    }

    manifest_schema& manifest_schema::optional_bool(const std::string& key, bool default_val) {
        fields_[key] = field_spec{ key, default_val, false };
        return *this;
    }

    manifest_schema& manifest_schema::optional_list(const std::string& key, std::vector<std::string> default_val) {
        fields_[key] = field_spec{ key, std::move(default_val), false };
        return *this;
    }

    manifest_schema& manifest_schema::add_validator(const std::string& key, validation_func validator) {
        fields_[key].validators.push_back(std::move(validator));
        return *this;
    }

    manifest_schema& manifest_schema::number_range(const std::string& key, double min, double max) {
        return add_validator(key, [min, max](const manifest_value& val) {
            if (auto* num = std::get_if<double>(&val)) {
                return *num >= min && *num <= max;
            }
            return false;
            });
    }

    manifest_schema& manifest_schema::string_not_empty(const std::string& key) {
        return add_validator(key, [](const manifest_value& val) {
            if (auto* str = std::get_if<std::string>(&val)) {
                return !str->empty();
            }
            return false;
            });
    }

    manifest_schema& manifest_schema::string_one_of(const std::string& key, std::vector<std::string> allowed) {
        return add_validator(key, [allowed = std::move(allowed)](const manifest_value& val) {
            if (auto* str = std::get_if<std::string>(&val)) {
                return std::find(allowed.begin(), allowed.end(), *str) != allowed.end();
            }
            return false;
            });
    }

    manifest_schema& manifest_schema::list_not_empty(const std::string& key) {
        return add_validator(key, [](const manifest_value& val) {
            if (auto* list = std::get_if<std::vector<std::string>>(&val)) {
                return !list->empty();
            }
            return false;
            });
    }

    manifest_schema& manifest_schema::list_min_size(const std::string& key, size_t min_size) {
        return add_validator(key, [min_size](const manifest_value& val) {
            if (auto* list = std::get_if<std::vector<std::string>>(&val)) {
                return list->size() >= min_size;
            }
            return false;
            });
    }

    manifest_schema& manifest_schema::list_max_size(const std::string& key, size_t max_size) {
        return add_validator(key, [max_size](const manifest_value& val) {
            if (auto* list = std::get_if<std::vector<std::string>>(&val)) {
                return list->size() <= max_size;
            }
            return false;
            });
    }

    bool manifest_schema::validate_field(const std::string& key, const manifest_value& value) const {
        auto it = fields_.find(key);
        if (it == fields_.end()) return true;

        for (const auto& validator : it->second.validators) {
            if (!validator(value)) return false;
        }
        return true;
    }

    manifest_result<bool> manifest_schema::validate(const manifest& mf) const {
        for (const auto& [key, spec] : fields_) {
            if (spec.required && !mf.has_key(key)) {
                return manifest_error(key, "Required field missing");
            }
        }

        for (const auto& key : mf.get_all_keys()) {
            auto it = fields_.find(key);
            if (it != fields_.end()) {
                bool valid = false;

                if (mf.is_string(key)) {
                    auto val = mf.try_get_string(key);
                    if (val) valid = validate_field(key, *val);
                }
                else if (mf.is_number(key)) {
                    auto val = mf.try_get_number(key);
                    if (val) valid = validate_field(key, *val);
                }
                else if (mf.is_bool(key)) {
                    auto val = mf.try_get_bool(key);
                    if (val) valid = validate_field(key, *val);
                }
                else if (mf.is_list(key)) {
                    auto val = mf.try_get_list(key);
                    if (val) valid = validate_field(key, *val);
                }

                if (!valid) {
                    return manifest_error(key, "Validation failed");
                }
            }
        }

        return true;
    }

    void manifest_schema::apply_defaults(manifest& mf) const {
        for (const auto& [key, spec] : fields_) {
            if (!mf.has_key(key) && spec.default_value.has_value()) {
                std::visit([&mf, &key](auto&& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        mf.set_string(key, val);
                    }
                    else if constexpr (std::is_same_v<T, double>) {
                        mf.set_number(key, val);
                    }
                    else if constexpr (std::is_same_v<T, bool>) {
                        mf.set_bool(key, val);
                    }
                    else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        mf.set_list(key, val);
                    }
                    }, *spec.default_value);
            }
        }
    }

    // Manifest Implementation
    std::string_view manifest::trim(std::string_view str) {
        const auto start = str.find_first_not_of(" \t\n\r");
        if (start == std::string_view::npos) return {};

        const auto end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }

    std::string manifest::strip_quotes(std::string_view str) {
        auto trimmed = trim(str);
        if (trimmed.size() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
            return std::string(trimmed.substr(1, trimmed.size() - 2));
        }
        return std::string(trimmed);
    }

    manifest_result<std::vector<std::string>> manifest::parse_list(std::string_view str) {
        auto trimmed = trim(str);

        if (trimmed.size() < 2 || trimmed.front() != '[' || trimmed.back() != ']') {
            return manifest_error("", "Invalid list format");
        }

        std::vector<std::string> result;
        auto content = trimmed.substr(1, trimmed.size() - 2);

        size_t pos = 0;
        bool in_quotes = false;
        std::string current;

        for (size_t i = 0; i < content.size(); ++i) {
            char c = content[i];

            if (c == '"' && (i == 0 || content[i - 1] != '\\')) {
                in_quotes = !in_quotes;
                current += c;
            }
            else if (c == ',' && !in_quotes) {
                auto item = strip_quotes(current);
                if (!item.empty()) {
                    result.push_back(std::move(item));
                }
                current.clear();
            }
            else {
                current += c;
            }
        }

        if (!current.empty()) {
            auto item = strip_quotes(current);
            if (!item.empty()) {
                result.push_back(std::move(item));
            }
        }

        return result;
    }

    manifest_result<bool> manifest::parse_bool(std::string_view str) {
        auto lower = std::string(trim(str));
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        if (lower == "true" || lower == "yes" || lower == "1" || lower == "on") {
            return true;
        }
        else if (lower == "false" || lower == "no" || lower == "0" || lower == "off") {
            return false;
        }

        return manifest_error("", "Invalid boolean value: " + std::string(str));
    }

    std::string manifest::escape_string(std::string_view str) {
        std::string result;
        result.reserve(str.size());

        for (char c : str) {
            switch (c) {
            case '\\': result += "\\\\"; break;
            case '"':  result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:   result += c; break;
            }
        }

        return result;
    }

    std::string manifest::serialize_value(const manifest_value& value) {
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::string>) {
                return '"' + escape_string(arg) + '"';
            }
            else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << std::setprecision(15) << arg;
                return oss.str();
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            }
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::string result = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    result += '"' + escape_string(arg[i]) + '"';
                    if (i < arg.size() - 1) result += ", ";
                }
                result += "]";
                return result;
            }
            return "";
            }, value);
    }

    manifest_result<manifest> manifest::parse(std::string_view content) {
        manifest mf;
        std::string content_str(content);  
        std::istringstream stream(content_str);
        std::string line;
        int line_number = 0;

        while (std::getline(stream, line)) {
            line_number++;
            auto trimmed = trim(line);

            if (trimmed.empty() || trimmed[0] == '#') continue;

            size_t pos = trimmed.find('=');
            if (pos == std::string_view::npos) {
                return manifest_error(
                    "line_" + std::to_string(line_number),
                    "Missing '=' separator"
                );
            }

            auto key = std::string(trim(trimmed.substr(0, pos)));
            auto value = trim(trimmed.substr(pos + 1));

            if (key.empty()) {
                return manifest_error(
                    "line_" + std::to_string(line_number),
                    "Empty key"
                );
            }

            if (value.empty()) {
                mf.set_string(key, "");
            }
            else if (value.front() == '[') {
                auto list_result = parse_list(value);
                if (is_error(list_result)) {
                    auto err = get_error(list_result);
                    return manifest_error(key, err.message);
                }
                mf.set_list(key, get_value(list_result));
            }
            else if (value.front() == '"') {
                mf.set_string(key, strip_quotes(value));
            }
            else if (value == "true" || value == "false" ||
                value == "yes" || value == "no" ||
                value == "on" || value == "off") {
                auto bool_result = parse_bool(value);
                if (is_ok(bool_result)) {
                    mf.set_bool(key, get_value(bool_result));
                }
                else {
                    mf.set_string(key, std::string(value));
                }
            }
            else {
                try {
                    size_t idx;
                    double num = std::stod(std::string(value), &idx);
                    if (idx == value.size()) {
                        mf.set_number(key, num);
                    }
                    else {
                        mf.set_string(key, std::string(value));
                    }
                }
                catch (...) {
                    mf.set_string(key, std::string(value));
                }
            }
        }

        return mf;
    }

    manifest_result<manifest> manifest::load_from_file(const std::string& path) {
        auto content = filesystem::read_text_file(path);
        if (is_error(content)) {
            const auto& err = get_error(content);
            return manifest_error(path, err.message);
        }
        return parse(get_value(content));
    }

    std::string manifest::serialize() const {
        std::ostringstream oss;

        for (const auto& [key, value] : data_) {
            oss << key << " = " << serialize_value(value) << '\n';
        }

        return oss.str();
    }

    io_result<bool> manifest::save_to_file(const std::string& path, const write_options& options) const {
        return filesystem::write_text_file(path, serialize(), options);
    }

    void manifest::set_string(const std::string& key, std::string value) {
        data_[key] = std::move(value);
    }

    void manifest::set_number(const std::string& key, double value) {
        data_[key] = value;
    }

    void manifest::set_bool(const std::string& key, bool value) {
        data_[key] = value;
    }

    void manifest::set_list(const std::string& key, std::vector<std::string> value) {
        data_[key] = std::move(value);
    }

    std::string manifest::get_string(const std::string& key, std::string_view default_val) const {
        return try_get_string(key).value_or(std::string(default_val));
    }

    double manifest::get_number(const std::string& key, double default_val) const {
        return try_get_number(key).value_or(default_val);
    }

    bool manifest::get_bool(const std::string& key, bool default_val) const {
        return try_get_bool(key).value_or(default_val);
    }

    std::vector<std::string> manifest::get_list(const std::string& key, const std::vector<std::string>& default_val) const {
        return try_get_list(key).value_or(default_val);
    }

    std::optional<std::string> manifest::try_get_string(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) return std::nullopt;

        if (auto* str = std::get_if<std::string>(&it->second)) {
            return *str;
        }
        return std::nullopt;
    }

    std::optional<double> manifest::try_get_number(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) return std::nullopt;

        if (auto* num = std::get_if<double>(&it->second)) {
            return *num;
        }
        return std::nullopt;
    }

    std::optional<bool> manifest::try_get_bool(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) return std::nullopt;

        if (auto* b = std::get_if<bool>(&it->second)) {
            return *b;
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string>> manifest::try_get_list(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) return std::nullopt;

        if (auto* list = std::get_if<std::vector<std::string>>(&it->second)) {
            return *list;
        }
        return std::nullopt;
    }

    bool manifest::has_key(const std::string& key) const {
        return data_.find(key) != data_.end();
    }

    void manifest::remove_key(const std::string& key) {
        data_.erase(key);
    }

    std::vector<std::string> manifest::get_all_keys() const {
        std::vector<std::string> keys;
        keys.reserve(data_.size());

        for (const auto& [key, _] : data_) {
            keys.push_back(key);
        }

        return keys;
    }

    bool manifest::is_string(const std::string& key) const {
        auto it = data_.find(key);
        return it != data_.end() && std::holds_alternative<std::string>(it->second);
    }

    bool manifest::is_number(const std::string& key) const {
        auto it = data_.find(key);
        return it != data_.end() && std::holds_alternative<double>(it->second);
    }

    bool manifest::is_bool(const std::string& key) const {
        auto it = data_.find(key);
        return it != data_.end() && std::holds_alternative<bool>(it->second);
    }

    bool manifest::is_list(const std::string& key) const {
        auto it = data_.find(key);
        return it != data_.end() && std::holds_alternative<std::vector<std::string>>(it->second);
    }

    void manifest::clear() {
        data_.clear();
    }

    void manifest::merge(const manifest& other, bool overwrite) {
        for (const auto& [key, value] : other.data_) {
            if (overwrite || !has_key(key)) {
                data_[key] = value;
            }
        }
    }

} // namespace project32::io