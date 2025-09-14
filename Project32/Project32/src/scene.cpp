#define _CRT_SECURE_NO_WARNINGS 

#include "scene.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <map>

uint32_t SceneObject::_nextId = 1;

// SceneUtils implementation
namespace SceneUtils {
    Transform ParseTransform(const json& transformJson) {
        Transform transform;

        if (transformJson.contains("position") && transformJson["position"].is_array() &&
            transformJson["position"].size() >= 3) {
            const auto& pos = transformJson["position"];
            transform.position = glm::vec3(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
        }

        if (transformJson.contains("rotation") && transformJson["rotation"].is_array() &&
            transformJson["rotation"].size() >= 3) {
            const auto& rot = transformJson["rotation"];
            transform.rotation = glm::vec3(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>());
        }

        if (transformJson.contains("scale")) {
            const auto& scale = transformJson["scale"];
            if (scale.is_array() && scale.size() >= 3) {
                transform.scale = glm::vec3(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
            }
            else if (scale.is_number()) {
                float uniformScale = scale.get<float>();
                transform.scale = glm::vec3(uniformScale);
            }
        }

        return transform;
    }

    json TransformToJSON(const Transform& transform) {
        json transformJson;
        transformJson["position"] = { transform.position.x, transform.position.y, transform.position.z };
        transformJson["rotation"] = { transform.rotation.x, transform.rotation.y, transform.rotation.z };
        transformJson["scale"] = { transform.scale.x, transform.scale.y, transform.scale.z };
        return transformJson;
    }
}

// SceneObject implementation
SceneObject::SceneObject(std::string name, std::unique_ptr<Mesh> mesh, const Transform& transform, Type type)
    : _name(std::move(name))
    , _mesh(std::move(mesh))
    , _transform(transform)
    , _type(type)
    , _id(_nextId++)
{
    if (!_mesh) {
        throw std::invalid_argument("SceneObject: mesh cannot be null");
    }
}

void SceneObject::Draw() const {
    if (_visible && _mesh) {
        _mesh->Draw();
    }
}

json SceneObject::ToJSON() const {
    json objectJson;
    objectJson["id"] = _id;
    objectJson["name"] = _name;
    objectJson["visible"] = _visible;

    switch (_type) {
    case Type::Sphere: objectJson["type"] = "sphere"; break;
    case Type::Cube: objectJson["type"] = "cube"; break;
    case Type::Cylinder: objectJson["type"] = "cylinder"; break;
    case Type::Capsule: objectJson["type"] = "capsule"; break;
    case Type::Model: objectJson["type"] = "model"; break;
    case Type::Custom: objectJson["type"] = "custom"; break;
    }

    objectJson["transform"] = SceneUtils::TransformToJSON(_transform);

    if (!_parameters.empty()) {
        objectJson["parameters"] = _parameters;
    }

    return objectJson;
}

std::unique_ptr<SceneObject> SceneObject::FromJSON(const json& objectJson) {
    if (!objectJson.contains("name") || !objectJson.contains("type")) {
        std::cerr << "SceneObject::FromJSON: Invalid object JSON - missing name or type" << std::endl;
        return nullptr;
    }

    std::string name = objectJson["name"].get<std::string>();
    std::string typeStr = objectJson["type"].get<std::string>();

    Type type = Type::Custom;
    if (typeStr == "sphere") type = Type::Sphere;
    else if (typeStr == "cube" || typeStr == "box") type = Type::Cube;
    else if (typeStr == "cylinder") type = Type::Cylinder;
    else if (typeStr == "capsule") type = Type::Capsule;
    else if (typeStr == "model") type = Type::Model;

    Transform transform;
    if (objectJson.contains("transform")) {
        transform = SceneUtils::ParseTransform(objectJson["transform"]);
    }

    std::unique_ptr<Mesh> mesh;

    switch (type) {
    case Type::Sphere: {
        int sectorCount = objectJson.value("sectorCount", 16);
        int stackCount = objectJson.value("stackCount", 16);
        float radius = objectJson.value("radius", 1.0f);
        mesh = StaticMeshes::GetSphere(sectorCount, stackCount, radius);
        break;
    }
    case Type::Cube: {
        mesh = StaticMeshes::GetCube();
        break;
    }
    case Type::Cylinder: {
        unsigned int segments = objectJson.value("segments", 16);
        float height = objectJson.value("height", 2.0f);
        float radius = objectJson.value("radius", 0.5f);
        mesh = StaticMeshes::GetCylinder(segments, height, radius);
        break;
    }
    case Type::Capsule: {
        unsigned int segments = objectJson.value("segments", 16);
        unsigned int rings = objectJson.value("rings", 8);
        float height = objectJson.value("height", 2.0f);
        float radius = objectJson.value("radius", 0.5f);
        mesh = StaticMeshes::GetCapsule(segments, rings, height, radius);
        break;
    }
    case Type::Model: {
        std::cout << "Warning: Model loading not implemented yet" << std::endl;
        return nullptr;
    }
    case Type::Custom: {
        std::cout << "Warning: Cannot recreate custom mesh from JSON" << std::endl;
        return nullptr;
    }
    }

    if (!mesh) {
        return nullptr;
    }

    auto object = std::make_unique<SceneObject>(std::move(name), std::move(mesh), transform, type);

    if (objectJson.contains("visible")) {
        object->SetVisible(objectJson["visible"].get<bool>());
    }

    if (objectJson.contains("parameters")) {
        object->_parameters = objectJson["parameters"];
    }

    return object;
}

// Scene implementation
Scene::Scene(const std::string& name) {
    _metadata.name = name;
}

SceneObject* Scene::AddCube(const std::string& name, const Transform& transform) {
    auto cubeMesh = StaticMeshes::GetCube();
    auto object = std::make_unique<SceneObject>(
        GenerateUniqueName(name),
        std::move(cubeMesh),
        transform,
        SceneObject::Type::Cube
    );

    SceneObject* ptr = object.get();
    AddObjectInternal(std::move(object));
    return ptr;
}

SceneObject* Scene::AddMesh(const std::string& name, std::unique_ptr<Mesh> mesh, const Transform& transform) {
    if (!mesh) {
        std::cerr << "Scene::AddMesh: Cannot add null mesh" << std::endl;
        return nullptr;
    }

    auto object = std::make_unique<SceneObject>(
        GenerateUniqueName(name),
        std::move(mesh),
        transform,
        SceneObject::Type::Custom
    );

    SceneObject* ptr = object.get();
    AddObjectInternal(std::move(object));
    return ptr;
}

bool Scene::RemoveObject(const std::string& name) {
    auto it = _nameToIndex.find(name);
    if (it == _nameToIndex.end()) {
        return false;
    }

    size_t index = it->second;
    uint32_t id = _objects[index]->GetId();

    if (_onObjectRemoved) {
        _onObjectRemoved(id, name);
    }

    _objects.erase(_objects.begin() + index);
    UpdateIndices();
    UpdateModifiedTime();
    return true;
}

bool Scene::RemoveObject(uint32_t id) {
    auto it = _idToIndex.find(id);
    if (it == _idToIndex.end()) {
        return false;
    }

    size_t index = it->second;
    std::string name = _objects[index]->GetName();

    if (_onObjectRemoved) {
        _onObjectRemoved(id, name);
    }

    _objects.erase(_objects.begin() + index);
    UpdateIndices();
    UpdateModifiedTime();
    return true;
}

void Scene::Clear() {
    for (const auto& object : _objects) {
        if (_onObjectRemoved) {
            _onObjectRemoved(object->GetId(), object->GetName());
        }
    }

    _objects.clear();
    _nameToIndex.clear();
    _idToIndex.clear();
    UpdateModifiedTime();
}

SceneObject* Scene::FindObject(const std::string& name) noexcept {
    auto it = _nameToIndex.find(name);
    return (it != _nameToIndex.end()) ? _objects[it->second].get() : nullptr;
}

const SceneObject* Scene::FindObject(const std::string& name) const noexcept {
    auto it = _nameToIndex.find(name);
    return (it != _nameToIndex.end()) ? _objects[it->second].get() : nullptr;
}

SceneObject* Scene::FindObject(uint32_t id) noexcept {
    auto it = _idToIndex.find(id);
    return (it != _idToIndex.end()) ? _objects[it->second].get() : nullptr;
}

const SceneObject* Scene::FindObject(uint32_t id) const noexcept {
    auto it = _idToIndex.find(id);
    return (it != _idToIndex.end()) ? _objects[it->second].get() : nullptr;
}

std::vector<SceneObject*> Scene::FindObjectsByType(SceneObject::Type type) {
    std::vector<SceneObject*> result;
    for (auto& object : _objects) {
        if (object->GetType() == type) {
            result.push_back(object.get());
        }
    }
    return result;
}

std::vector<const SceneObject*> Scene::FindObjectsByType(SceneObject::Type type) const {
    std::vector<const SceneObject*> result;
    for (const auto& object : _objects) {
        if (object->GetType() == type) {
            result.push_back(object.get());
        }
    }
    return result;
}

void Scene::Render() const {
    for (const auto& object : _objects) {
        object->Draw();
    }
}

void Scene::RenderObject(const std::string& name) const {
    if (const auto* object = FindObject(name)) {
        object->Draw();
    }
    else {
        std::cerr << "Scene::RenderObject: Object '" << name << "' not found" << std::endl;
    }
}

void Scene::RenderObject(uint32_t id) const {
    if (const auto* object = FindObject(id)) {
        object->Draw();
    }
    else {
        std::cerr << "Scene::RenderObject: Object with ID " << id << " not found" << std::endl;
    }
}

bool Scene::LoadFromFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Scene::LoadFromFile: Failed to open file: " << filePath << std::endl;
            return false;
        }

        json sceneJson;
        file >> sceneJson;
        return LoadFromJSON(sceneJson);
    }
    catch (const std::exception& e) {
        std::cerr << "Scene::LoadFromFile: Error loading scene: " << e.what() << std::endl;
        return false;
    }
}

bool Scene::SaveToFile(const std::string& filePath) const {
    try {
        json sceneJson = SaveToJSON();
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Scene::SaveToFile: Failed to create file: " << filePath << std::endl;
            return false;
        }

        file << sceneJson.dump(4);
        std::cout << "Scene saved to: " << filePath << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Scene::SaveToFile: Error saving scene: " << e.what() << std::endl;
        return false;
    }
}

bool Scene::LoadFromJSON(const json& sceneJson) {
    try {
        if (!sceneJson.contains("scene")) {
            std::cerr << "Scene::LoadFromJSON: Invalid scene format - missing 'scene' field" << std::endl;
            return false;
        }

        const auto& scene = sceneJson["scene"];

        if (scene.contains("metadata")) {
            _metadata = ParseMetadata(scene["metadata"]);
        }

        Clear();

        if (scene.contains("objects") && scene["objects"].is_array()) {
            const auto& objects = scene["objects"];
            size_t successCount = 0;

            for (const auto& objectJson : objects) {
                auto object = SceneObject::FromJSON(objectJson);
                if (object) {
                    AddObjectInternal(std::move(object));
                    successCount++;
                }
                else {
                    std::cerr << "Failed to load object from JSON" << std::endl;
                }
            }

            std::cout << "Successfully loaded " << successCount << "/" << objects.size()
                << " objects into scene '" << _metadata.name << "'" << std::endl;
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Scene::LoadFromJSON: Error parsing JSON: " << e.what() << std::endl;
        return false;
    }
}

json Scene::SaveToJSON() const {
    json sceneJson;

    sceneJson["scene"]["metadata"] = MetadataToJSON(_metadata);

    json objectsArray = json::array();
    for (const auto& object : _objects) {
        objectsArray.push_back(object->ToJSON());
    }

    sceneJson["scene"]["objects"] = objectsArray;

    return sceneJson;
}

void Scene::DebugPrint() const {
    std::cout << "=== Scene: " << _metadata.name << " ===" << std::endl;
    std::cout << "Object count: " << _objects.size() << std::endl;

    for (const auto& object : _objects) {
        const auto& pos = object->GetTransform().position;
        std::cout << "  [" << object->GetId() << "] " << object->GetName()
            << " (" << (object->IsVisible() ? "visible" : "hidden") << ")"
            << " at (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    }
}

void Scene::PrintStatistics() const {
    std::cout << "=== Scene Statistics ===" << std::endl;
    std::cout << "Name: " << _metadata.name << std::endl;
    std::cout << "Total objects: " << _objects.size() << std::endl;

    // Count by type
    std::map<SceneObject::Type, int> typeCounts;
    int visibleCount = 0;

    for (const auto& object : _objects) {
        typeCounts[object->GetType()]++;
        if (object->IsVisible()) visibleCount++;
    }

    std::cout << "Visible objects: " << visibleCount << std::endl;
    std::cout << "Objects by type:" << std::endl;

    const char* typeNames[] = { "Sphere", "Cube", "Cylinder", "Capsule", "Model", "Custom" };
    for (const auto& [type, count] : typeCounts) {
        std::cout << "  " << typeNames[static_cast<int>(type)] << ": " << count << std::endl;
    }
}

void Scene::UpdateIndices() {
    _nameToIndex.clear();
    _idToIndex.clear();

    for (size_t i = 0; i < _objects.size(); ++i) {
        _nameToIndex[_objects[i]->GetName()] = i;
        _idToIndex[_objects[i]->GetId()] = i;
    }
}

void Scene::AddObjectInternal(std::unique_ptr<SceneObject> object) {
    size_t index = _objects.size();
    const std::string& name = object->GetName();
    uint32_t id = object->GetId();

    _objects.push_back(std::move(object));
    _nameToIndex[name] = index;
    _idToIndex[id] = index;

    if (_onObjectAdded) {
        _onObjectAdded(*_objects.back());
    }

    UpdateModifiedTime();
}

bool Scene::IsNameAvailable(const std::string& name) const {
    return _nameToIndex.find(name) == _nameToIndex.end();
}

std::string Scene::GenerateUniqueName(const std::string& baseName) const {
    if (IsNameAvailable(baseName)) {
        return baseName;
    }

    int counter = 1;
    std::string uniqueName;
    do {
        uniqueName = baseName + "_" + std::to_string(counter++);
    } while (!IsNameAvailable(uniqueName));

    return uniqueName;
}

json Scene::MetadataToJSON(const Metadata& metadata) {
    json metadataJson;
    metadataJson["name"] = metadata.name;
    metadataJson["description"] = metadata.description;
    metadataJson["version"] = metadata.version;
    metadataJson["author"] = metadata.author;

    auto timeToString = [](const std::chrono::system_clock::time_point& tp) -> std::string {
        auto time_t = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
        };

    metadataJson["created"] = timeToString(metadata.created);
    metadataJson["modified"] = timeToString(metadata.modified);

    return metadataJson;
}

// Metadata parsing with basic ISO 8601 support
Scene::Metadata Scene::ParseMetadata(const json& metadataJson) {
    Metadata metadata;

    if (metadataJson.contains("name")) {
        metadata.name = metadataJson["name"].get<std::string>();
    }
    if (metadataJson.contains("description")) {
        metadata.description = metadataJson["description"].get<std::string>();
    }
    if (metadataJson.contains("version")) {
        metadata.version = metadataJson["version"].get<std::string>();
    }
    if (metadataJson.contains("author")) {
        metadata.author = metadataJson["author"].get<std::string>();
    }

    // Parse ISO 8601 timestamps - simplified
    auto parseTime = [](const std::string& timeStr) -> std::chrono::system_clock::time_point {
        // Simple ISO 8601 parser - TODO: A more robust implementation
        std::tm tm = {};
        std::istringstream ss(timeStr);

        // Try to parse ISO 8601 format: YYYY-MM-DDTHH:MM:SSZ
        if (ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ")) {
            std::time_t time_t = std::mktime(&tm);
            return std::chrono::system_clock::from_time_t(time_t);
        }

        return std::chrono::system_clock::now();
        };

    if (metadataJson.contains("created")) {
        try {
            std::string createdStr = metadataJson["created"].get<std::string>();
            metadata.created = parseTime(createdStr);
        }
        catch (const std::exception&) {
            metadata.created = std::chrono::system_clock::now();
        }
    }

    if (metadataJson.contains("modified")) {
        try {
            std::string modifiedStr = metadataJson["modified"].get<std::string>();
            metadata.modified = parseTime(modifiedStr);
        }
        catch (const std::exception&) {
            metadata.modified = std::chrono::system_clock::now();
        }
    }

    return metadata;
}

// SceneManager implementation
Scene* SceneManager::CreateScene(const std::string& name) {
    _scenes.push_back(std::make_unique<Scene>(name));
    if (!_currentSceneIndex.has_value()) {
        _currentSceneIndex = _scenes.size() - 1;
    }
    return _scenes.back().get();
}

bool SceneManager::LoadScene(const std::string& filePath) {
    auto scene = std::make_unique<Scene>();
    if (scene->LoadFromFile(filePath)) {
        _scenes.push_back(std::move(scene));
        _currentSceneIndex = _scenes.size() - 1;
        return true;
    }
    return false;
}

bool SceneManager::SaveScene(size_t index, const std::string& filePath) const {
    if (const auto* scene = GetScene(index)) {
        return scene->SaveToFile(filePath);
    }
    std::cerr << "SceneManager::SaveScene: Invalid scene index " << index << std::endl;
    return false;
}

bool SceneManager::SaveCurrentScene(const std::string& filePath) const {
    if (const auto* scene = GetCurrentScene()) {
        return scene->SaveToFile(filePath);
    }
    std::cerr << "SceneManager::SaveCurrentScene: No current scene selected" << std::endl;
    return false;
}

bool SceneManager::RemoveScene(size_t index) {
    if (index >= _scenes.size()) {
        return false;
    }

    _scenes.erase(_scenes.begin() + index);
    UpdateCurrentSceneIndex();
    return true;
}

void SceneManager::RemoveAllScenes() {
    _scenes.clear();
    _currentSceneIndex.reset();
}

bool SceneManager::SetCurrentScene(size_t index) {
    if (index >= _scenes.size()) {
        return false;
    }
    _currentSceneIndex = index;
    return true;
}

bool SceneManager::SetCurrentScene(const std::string& name) {
    for (size_t i = 0; i < _scenes.size(); ++i) {
        if (_scenes[i]->GetMetadata().name == name) {
            _currentSceneIndex = i;
            return true;
        }
    }
    return false;
}

Scene* SceneManager::GetCurrentScene() noexcept {
    if (_currentSceneIndex.has_value() && _currentSceneIndex.value() < _scenes.size()) {
        return _scenes[_currentSceneIndex.value()].get();
    }
    return nullptr;
}

const Scene* SceneManager::GetCurrentScene() const noexcept {
    if (_currentSceneIndex.has_value() && _currentSceneIndex.value() < _scenes.size()) {
        return _scenes[_currentSceneIndex.value()].get();
    }
    return nullptr;
}

Scene* SceneManager::GetScene(size_t index) noexcept {
    return (index < _scenes.size()) ? _scenes[index].get() : nullptr;
}

const Scene* SceneManager::GetScene(size_t index) const noexcept {
    return (index < _scenes.size()) ? _scenes[index].get() : nullptr;
}

Scene* SceneManager::GetScene(const std::string& name) noexcept {
    for (auto& scene : _scenes) {
        if (scene->GetMetadata().name == name) {
            return scene.get();
        }
    }
    return nullptr;
}

const Scene* SceneManager::GetScene(const std::string& name) const noexcept {
    for (const auto& scene : _scenes) {
        if (scene->GetMetadata().name == name) {
            return scene.get();
        }
    }
    return nullptr;
}

std::optional<size_t> SceneManager::GetCurrentSceneIndex() const noexcept {
    return _currentSceneIndex;
}

std::vector<std::string> SceneManager::GetSceneNames() const {
    std::vector<std::string> names;
    names.reserve(_scenes.size());

    for (const auto& scene : _scenes) {
        names.push_back(scene->GetMetadata().name);
    }

    return names;
}

void SceneManager::RenderCurrentScene() const {
    if (const auto* scene = GetCurrentScene()) {
        scene->Render();
    }
}

void SceneManager::RenderScene(size_t index) const {
    if (const auto* scene = GetScene(index)) {
        scene->Render();
    }
}

bool SceneManager::RemoveObjectFromCurrentScene(const std::string& name) {
    if (auto* scene = GetCurrentScene()) {
        return scene->RemoveObject(name);
    }
    return false;
}

SceneObject* SceneManager::FindObjectInCurrentScene(const std::string& name) {
    if (auto* scene = GetCurrentScene()) {
        return scene->FindObject(name);
    }
    return nullptr;
}

void SceneManager::UpdateCurrentSceneIndex() {
    if (_scenes.empty()) {
        _currentSceneIndex.reset();
    }
    else if (_currentSceneIndex.has_value() && _currentSceneIndex.value() >= _scenes.size()) {
        _currentSceneIndex = _scenes.size() - 1;
    }
}

void SceneManager::PrintSceneList() const {
    std::cout << "=== Scene Manager ===" << std::endl;
    std::cout << "Total scenes: " << _scenes.size() << std::endl;

    if (_currentSceneIndex.has_value()) {
        std::cout << "Current scene index: " << _currentSceneIndex.value() << std::endl;
    }
    else {
        std::cout << "No current scene selected" << std::endl;
    }

    for (size_t i = 0; i < _scenes.size(); ++i) {
        const auto& scene = _scenes[i];
        std::cout << "  [" << i << "] " << scene->GetMetadata().name
            << " (" << scene->GetObjectCount() << " objects)";
        if (_currentSceneIndex.has_value() && _currentSceneIndex.value() == i) {
            std::cout << " <- current";
        }
        std::cout << std::endl;
    }
}