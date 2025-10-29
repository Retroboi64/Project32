#include "../common.h"

#include "project.h"
#include "../io/manifest.h"

namespace project32::io {
    bool project_manager::load_project(const std::string& path) {
        auto mf = manifest::load_from_file(path);
        if (!mf) {
            return false;
        }

        manifest_ = std::make_unique<manifest>(std::move(*mf));
        project_path_ = path;
        return true;
    }

    bool project_manager::save_project() const {
        if (!is_project_loaded() || project_path_.empty()) {
            return false;
        }

        return manifest_->save_to_file(project_path_);
    }

    bool project_manager::save_project_as(const std::string& path) {
        if (!is_project_loaded()) {
            return false;
        }

        if (manifest_->save_to_file(path)) {
            project_path_ = path;
            return true;
        }

        return false;
    }

    bool project_manager::create_new_project(const std::string& path, const std::string& project_name) {
        manifest_ = std::make_unique<manifest>();
        project_path_ = path;

        manifest_->set_string("ProjectName", project_name);
        manifest_->set_string("Config", "config.cfg");
        manifest_->set_list("Scenes", std::vector<std::string>());
        manifest_->set_number("Version", 1.0);

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

    std::optional<std::string> project_manager::get_project_name() const {
        if (!is_project_loaded()) {
            return std::nullopt;
        }
        return manifest_->get_string("ProjectName");
    }

    void project_manager::set_project_name(const std::string& name) {
        if (is_project_loaded()) {
            manifest_->set_string("ProjectName", name);
        }
    }

    std::optional<std::string> project_manager::get_config_path() const {
        if (!is_project_loaded()) {
            return std::nullopt;
        }
        return manifest_->get_string("Config");
    }

    void project_manager::set_config_path(const std::string& path) {
        if (is_project_loaded()) {
            manifest_->set_string("Config", path);
        }
    }

    std::optional<std::vector<std::string>> project_manager::get_scenes() const {
        if (!is_project_loaded()) {
            return std::nullopt;
        }
        return manifest_->get_list("Scenes");
    }

    void project_manager::set_scenes(const std::vector<std::string>& scenes) {
        if (is_project_loaded()) {
            manifest_->set_list("Scenes", scenes);
        }
    }

    void project_manager::add_scene(const std::string& scene) {
        if (!is_project_loaded()) {
            return;
        }

        auto scenes = manifest_->get_list("Scenes");
        std::vector<std::string> scene_list = scenes.value_or(std::vector<std::string>());

        if (std::find(scene_list.begin(), scene_list.end(), scene) == scene_list.end()) {
            scene_list.push_back(scene);
            manifest_->set_list("Scenes", scene_list);
        }
    }

    void project_manager::remove_scene(const std::string& scene) {
        if (!is_project_loaded()) {
            return;
        }

        auto scenes = manifest_->get_list("Scenes");
        if (!scenes) {
            return;
        }

        std::vector<std::string> scene_list = *scenes;
        scene_list.erase(
            std::remove(scene_list.begin(), scene_list.end(), scene),
            scene_list.end()
        );

        manifest_->set_list("Scenes", scene_list);
    }

    std::optional<double> project_manager::get_version() const {
        if (!is_project_loaded()) {
            return std::nullopt;
        }
        return manifest_->get_number("Version");
    }

    void project_manager::set_version(double version) {
        if (is_project_loaded()) {
            manifest_->set_number("Version", version);
        }
    }

    std::optional<std::string> project_manager::get_string_property(const std::string& key) const {
        if (!is_project_loaded()) {
            return std::nullopt;
        }
        return manifest_->get_string(key);
    }

    void project_manager::set_string_property(const std::string& key, const std::string& value) {
        if (is_project_loaded()) {
            manifest_->set_string(key, value);
        }
    }

    std::optional<double> project_manager::get_number_property(const std::string& key) const {
        if (!is_project_loaded()) {
            return std::nullopt;
        }
        return manifest_->get_number(key);
    }

    void project_manager::set_number_property(const std::string& key, double value) {
        if (is_project_loaded()) {
            manifest_->set_number(key, value);
        }
    }

    std::optional<std::vector<std::string>> project_manager::get_list_property(const std::string& key) const {
        if (!is_project_loaded()) {
            return std::nullopt;
        }
        return manifest_->get_list(key);
    }

    void project_manager::set_list_property(const std::string& key, const std::vector<std::string>& value) {
        if (is_project_loaded()) {
            manifest_->set_list(key, value);
        }
    }

    std::vector<std::string> project_manager::get_all_property_keys() const {
        if (!is_project_loaded()) {
            return {};
        }
        return manifest_->get_all_keys();
    }
}