#include "../common.h"
#include "project.h"

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

    void project_manager::ensure_loaded() const {
        if (!is_project_loaded()) {
            throw std::runtime_error("No project is currently loaded");
        }
    }

    project_result<bool> project_manager::load_project(const std::string& path) {
        auto result = manifest::load_from_file(path);

        if (is_error(result)) {
            auto err = get_error(result);
            return project_error("Failed to load project: " + err.message);
        }

        manifest_ = std::make_unique<manifest>(std::move(get_value(result)));
        project_path_ = path;

        auto schema = get_default_schema();
        schema.apply_defaults(*manifest_);

        auto validation = schema.validate(*manifest_);
        if (is_error(validation)) {
            auto err = get_error(validation);
            return project_error("Project validation failed: " + err.message);
        }

        return true;
    }

    project_result<bool> project_manager::save_project() const {
        if (!is_project_loaded()) {
            return project_error("No project loaded");
        }

        if (project_path_.empty()) {
            return project_error("Project path not set, use save_project_as()");
        }

        write_options options;
        options.create_directories = true;
        options.atomic_write = true;

        auto result = manifest_->save_to_file(project_path_, options);

        if (is_error(result)) {
            auto err = get_error(result);
            return project_error("Failed to save project: " + err.message);
        }

        return true;
    }

    project_result<bool> project_manager::save_project_as(const std::string& path) {
        if (!is_project_loaded()) {
            return project_error("No project loaded");
        }

        write_options options;
        options.create_directories = true;
        options.atomic_write = true;

        auto result = manifest_->save_to_file(path, options);

        if (is_ok(result)) {
            project_path_ = path;
            return true;
        }

        auto err = get_error(result);
        return project_error("Failed to save project: " + err.message);
    }

    project_result<bool> project_manager::create_new_project(
        const std::string& path,
        const std::string& project_name,
        const std::string& version
    ) {
        manifest_ = std::make_unique<manifest>();
        project_path_ = path;

        manifest_->set_string("ProjectName", project_name);
        manifest_->set_string("Version", version);
        manifest_->set_string("Config", "config.cfg");
        manifest_->set_list("Scenes", std::vector<std::string>());

        auto schema = get_default_schema();
        schema.apply_defaults(*manifest_);

        return save_project();
    }

    bool project_manager::is_project_loaded() const {
        return manifest_ != nullptr;
    }

    void project_manager::close_project() {
        manifest_.reset();
        project_path_.clear();
    }

    std::string project_manager::get_project_path() const {
        return project_path_;
    }

    std::string project_manager::get_project_directory() const {
        if (project_path_.empty()) return "";
        return filesystem::get_parent_path(project_path_);
    }

    std::string project_manager::get_project_name(const std::string& default_val) const {
        if (!is_project_loaded()) return default_val;
        return manifest_->get_string("ProjectName", default_val);
    }

    void project_manager::set_project_name(const std::string& name) {
        ensure_loaded();
        manifest_->set_string("ProjectName", name);
    }

    std::string project_manager::get_config_path(const std::string& default_val) const {
        if (!is_project_loaded()) return default_val;
        return manifest_->get_string("Config", default_val);
    }

    void project_manager::set_config_path(const std::string& path) {
        ensure_loaded();
        manifest_->set_string("Config", path);
    }

    std::string project_manager::get_version(const std::string& default_val) const {
        if (!is_project_loaded()) return default_val;
        return manifest_->get_string("Version", default_val);
    }

    void project_manager::set_version(const std::string& version) {
        ensure_loaded();
        manifest_->set_string("Version", version);
    }

    std::vector<std::string> project_manager::get_scenes() const {
        if (!is_project_loaded()) return {};
        return manifest_->get_list("Scenes");
    }

    void project_manager::set_scenes(const std::vector<std::string>& scenes) {
        ensure_loaded();
        manifest_->set_list("Scenes", scenes);
    }

    bool project_manager::add_scene(const std::string& scene) {
        ensure_loaded();

        auto scenes = get_scenes();

        if (std::find(scenes.begin(), scenes.end(), scene) != scenes.end()) {
            return false;
        }

        scenes.push_back(scene);
        manifest_->set_list("Scenes", scenes);
        return true;
    }

    bool project_manager::remove_scene(const std::string& scene) {
        ensure_loaded();

        auto scenes = get_scenes();
        auto it = std::find(scenes.begin(), scenes.end(), scene);

        if (it == scenes.end()) {
            return false;
        }

        scenes.erase(it);
        manifest_->set_list("Scenes", scenes);
        return true;
    }

    bool project_manager::has_scene(const std::string& scene) const {
        if (!is_project_loaded()) return false;

        auto scenes = get_scenes();
        return std::find(scenes.begin(), scenes.end(), scene) != scenes.end();
    }

    void project_manager::clear_scenes() {
        ensure_loaded();
        manifest_->set_list("Scenes", {});
    }

    std::string project_manager::get_string_property(const std::string& key, const std::string& default_val) const {
        if (!is_project_loaded()) return default_val;
        return manifest_->get_string(key, default_val);
    }

    void project_manager::set_string_property(const std::string& key, const std::string& value) {
        ensure_loaded();
        manifest_->set_string(key, value);
    }

    double project_manager::get_number_property(const std::string& key, double default_val) const {
        if (!is_project_loaded()) return default_val;
        return manifest_->get_number(key, default_val);
    }

    void project_manager::set_number_property(const std::string& key, double value) {
        ensure_loaded();
        manifest_->set_number(key, value);
    }

    bool project_manager::get_bool_property(const std::string& key, bool default_val) const {
        if (!is_project_loaded()) return default_val;
        return manifest_->get_bool(key, default_val);
    }

    void project_manager::set_bool_property(const std::string& key, bool value) {
        ensure_loaded();
        manifest_->set_bool(key, value);
    }

    std::vector<std::string> project_manager::get_list_property(const std::string& key) const {
        if (!is_project_loaded()) return {};
        return manifest_->get_list(key);
    }

    void project_manager::set_list_property(const std::string& key, const std::vector<std::string>& value) {
        ensure_loaded();
        manifest_->set_list(key, value);
    }

    std::vector<std::string> project_manager::get_all_property_keys() const {
        if (!is_project_loaded()) return {};
        return manifest_->get_all_keys();
    }

    bool project_manager::has_property(const std::string& key) const {
        if (!is_project_loaded()) return false;
        return manifest_->has_key(key);
    }

    void project_manager::remove_property(const std::string& key) {
        ensure_loaded();
        manifest_->remove_key(key);
    }

    manifest_schema project_manager::get_default_schema() {
        manifest_schema schema;

        schema.require_string("ProjectName")
            .string_not_empty("ProjectName");

        schema.optional_string("Version", "1.0");
        schema.optional_string("Config", "config.cfg");
        schema.optional_list("Scenes", {});

        return schema;
    }

    project_result<bool> project_manager::validate() const {
        if (!is_project_loaded()) {
            return project_error("No project loaded");
        }

        auto schema = get_default_schema();
        auto result = schema.validate(*manifest_);

        if (is_error(result)) {
            auto err = get_error(result);
            return project_error(err.to_string());
        }

        return true;
    }

} // namespace project32::io