#pragma once

#include "../common.h"

namespace project32::io {
    class manifest;

    class project_manager {
    public:
        project_manager() = default;
        ~project_manager() = default;

        bool load_project(const std::string& path);
        bool save_project() const;
        bool save_project_as(const std::string& path);
        bool create_new_project(const std::string& path, const std::string& project_name);
        bool is_project_loaded() const;
        void close_project();
        std::string get_project_path() const;
        std::optional<std::string> get_project_name() const;
        void set_project_name(const std::string& name);

        std::optional<std::string> get_config_path() const;
        void set_config_path(const std::string& path);

        std::optional<std::vector<std::string>> get_scenes() const;
        void set_scenes(const std::vector<std::string>& scenes);
        void add_scene(const std::string& scene);
        void remove_scene(const std::string& scene);

        std::optional<double> get_version() const;
        void set_version(double version);

        std::optional<std::string> get_string_property(const std::string& key) const;
        void set_string_property(const std::string& key, const std::string& value);

        std::optional<double> get_number_property(const std::string& key) const;
        void set_number_property(const std::string& key, double value);

        std::optional<std::vector<std::string>> get_list_property(const std::string& key) const;
        void set_list_property(const std::string& key, const std::vector<std::string>& value);

        std::vector<std::string> get_all_property_keys() const;

    private:
        std::unique_ptr<manifest> manifest_;
        std::string project_path_;
    };
}