#include "../common.h"

#include "filesystem.h"
#include "manifest.h"

namespace project32::io {
    std::string manifest::trim(const std::string& str) {
        const auto start = std::find_if(str.begin(), str.end(), [](unsigned char c) {
            return !std::isspace(c);
            });

        const auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char c) {
            return !std::isspace(c);
            }).base();

        return (start < end) ? std::string(start, end) : std::string();
    }

    std::string manifest::strip_quotes(const std::string& str) {
        std::string trimmed = trim(str);
        if (trimmed.size() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
            return trimmed.substr(1, trimmed.size() - 2);
        }
        return trimmed;
    }

    std::vector<std::string> manifest::parse_list(const std::string& str) {
        std::vector<std::string> result;
        std::string trimmed = trim(str);

        if (trimmed.size() < 2 || trimmed.front() != '[' || trimmed.back() != ']') {
            return result;
        }

        std::string content = trimmed.substr(1, trimmed.size() - 2);

        std::istringstream iss(content);
        std::string token;

        while (std::getline(iss, token, ',')) {
            std::string item = strip_quotes(token);
            if (!item.empty()) {
                result.push_back(item);
            }
        }

        return result;
    }

    std::optional<manifest> manifest::parse(const std::string& content) {
        manifest mf;
        std::istringstream stream(content);
        std::string line;

        while (std::getline(stream, line)) {
            line = trim(line);

            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }

            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            if (key.empty()) {
                continue;
            }

            if (value.front() == '[') {
                auto list = parse_list(value);
                mf.set_list(key, list);
            }
            else if (value.front() == '"') {
                mf.set_string(key, strip_quotes(value));
            }
            else {
                try {
                    size_t idx;
                    double num = std::stod(value, &idx);
                    if (idx == value.size()) {
                        mf.set_number(key, num);
                    }
                    else {
                        mf.set_string(key, value);
                    }
                }
                catch (...) {
                    mf.set_string(key, value);
                }
            }
        }

        return mf;
    }

    std::optional<manifest> manifest::load_from_file(const std::string& path) {
        auto content = filesystem::read_text_file(path);
        if (!content) {
            return std::nullopt;
        }
        return parse(*content);
    }

    std::string manifest::serialize() const {
        std::ostringstream oss;

        for (const auto& [key, value] : data_) {
            oss << key << " = ";

            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << '"' << arg << '"';
                }
                else if constexpr (std::is_same_v<T, double>) {
                    std::ostringstream num_stream;
                    num_stream << arg;
                    std::string num_str = num_stream.str();
                    if (num_str.find('.') == std::string::npos && arg == static_cast<int>(arg)) {
                        oss << static_cast<int>(arg);
                    }
                    else {
                        oss << arg;
                    }
                }
                else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << '[';
                    for (size_t i = 0; i < arg.size(); ++i) {
                        oss << '"' << arg[i] << '"';
                        if (i < arg.size() - 1) {
                            oss << ", ";
                        }
                    }
                    oss << ']';
                }
                }, value);

            oss << '\n';
        }

        return oss.str();
    }

    bool manifest::save_to_file(const std::string& path) const {
        std::string content = serialize();
        return filesystem::write_text_file(path, content);
    }

    void manifest::set_string(const std::string& key, const std::string& value) {
        data_[key] = value;
    }

    std::optional<std::string> manifest::get_string(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) {
            return std::nullopt;
        }

        if (auto* str = std::get_if<std::string>(&it->second)) {
            return *str;
        }

        return std::nullopt;
    }

    void manifest::set_number(const std::string& key, double value) {
        data_[key] = value;
    }

    std::optional<double> manifest::get_number(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) {
            return std::nullopt;
        }

        if (auto* num = std::get_if<double>(&it->second)) {
            return *num;
        }

        return std::nullopt;
    }

    void manifest::set_list(const std::string& key, const std::vector<std::string>& value) {
        data_[key] = value;
    }

    std::optional<std::vector<std::string>> manifest::get_list(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) {
            return std::nullopt;
        }

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

    void manifest::clear() {
        data_.clear();
    }
}