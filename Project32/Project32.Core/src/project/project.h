#pragma once

#include "../common.h"
#include "../io/manifest.h"

namespace project32::io {

    struct project_error {
        std::string message;

        explicit project_error(std::string msg) : message(std::move(msg)) {}

        std::string to_string() const {
            return "Project Error: " + message;
        }
    };

    template<typename T>
    using project_result = std::variant<T, project_error>;

    /*
     *  @brief Manages project files and configuration
    */
    class project_manager {
    public:
        project_manager() = default;
        ~project_manager() = default;

        project_result<bool> load_project(const std::string& path);
        project_result<bool> save_project() const;
        project_result<bool> save_project_as(const std::string& path);
        project_result<bool> create_new_project(
            const std::string& path,
            const std::string& project_name,
            const std::string& version = "1.0"
        );

        bool is_project_loaded() const;
        void close_project();

        std::string get_project_path() const;
        std::string get_project_directory() const;
        std::string get_project_name(const std::string& default_val = "Untitled") const;
        void set_project_name(const std::string& name);

        std::string get_config_path(const std::string& default_val = "config.cfg") const;
        void set_config_path(const std::string& path);

        std::string get_version(const std::string& default_val = "1.0") const;
        void set_version(const std::string& version);

        std::vector<std::string> get_scenes() const;
        void set_scenes(const std::vector<std::string>& scenes);
        bool add_scene(const std::string& scene);
        bool remove_scene(const std::string& scene);
        bool has_scene(const std::string& scene) const;
        void clear_scenes();

        std::string get_string_property(const std::string& key, const std::string& default_val = "") const;
        void set_string_property(const std::string& key, const std::string& value);

        double get_number_property(const std::string& key, double default_val = 0.0) const;
        void set_number_property(const std::string& key, double value);

        bool get_bool_property(const std::string& key, bool default_val = false) const;
        void set_bool_property(const std::string& key, bool value);

        std::vector<std::string> get_list_property(const std::string& key) const;
        void set_list_property(const std::string& key, const std::vector<std::string>& value);

        std::vector<std::string> get_all_property_keys() const;
        bool has_property(const std::string& key) const;
        void remove_property(const std::string& key);

        const manifest* get_manifest() const { return manifest_.get(); }
        manifest* get_manifest() { return manifest_.get(); }

        static manifest_schema get_default_schema();
        project_result<bool> validate() const;

    private:
        std::unique_ptr<manifest> manifest_;
        std::string project_path_;

        void ensure_loaded() const;
    };

} // namespace project32::io