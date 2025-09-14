#pragma once

#include "common.h"
#include "mesh.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 26819)
#include <nlohmann/json.hpp>
#pragma warning(pop)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#include <nlohmann/json.hpp>
#pragma GCC diagnostic pop
#endif

using json = nlohmann::json;

// Forward declarations
class Scene;
class SceneNode;

// Improved SceneObject with better encapsulation
class SceneObject {
public:
    // Object types for better type safety
    enum class Type {
        Sphere,
        Cube,
        Cylinder,
        Capsule,
        Model,
        Custom
    };

    // Constructors
    SceneObject(std::string name, std::unique_ptr<Mesh> mesh, const Transform& transform, Type type = Type::Custom);
    SceneObject(const SceneObject&) = delete;
    SceneObject& operator=(const SceneObject&) = delete;
    SceneObject(SceneObject&&) = default;
    SceneObject& operator=(SceneObject&&) = default;

    // Getters
    const std::string& GetName() const noexcept { return _name; }
    const Transform& GetTransform() const noexcept { return _transform; }
    Transform& GetTransform() noexcept { return _transform; }
    const Mesh* GetMesh() const noexcept { return _mesh.get(); }
    Mesh* GetMesh() noexcept { return _mesh.get(); }
    Type GetType() const noexcept { return _type; }
    uint32_t GetId() const noexcept { return _id; }
    bool IsVisible() const noexcept { return _visible; }

    // Setters
    void SetTransform(const Transform& transform) { _transform = transform; }
    void SetVisible(bool visible) { _visible = visible; }
    void SetName(const std::string& name) { _name = name; }

    // Operations
    void Draw() const;
    json ToJSON() const;
    static std::unique_ptr<SceneObject> FromJSON(const json& objectJson);

private:
    std::string _name;
    std::unique_ptr<Mesh> _mesh;
    Transform _transform;
    Type _type;
    uint32_t _id;
    bool _visible = true;

    static uint32_t _nextId;

    // Type-specific parameters stored as JSON for flexibility
    json _parameters;
};

// Scene class with improved design
class Scene {
public:
    // Scene metadata
    struct Metadata {
        std::string name = "Untitled Scene";
        std::string description;
        std::string version = "1.1";
        std::string author;
        std::chrono::system_clock::time_point created = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point modified = std::chrono::system_clock::now();
    };

    // Event system for scene changes
    using ObjectAddedCallback = std::function<void(const SceneObject&)>;
    using ObjectRemovedCallback = std::function<void(uint32_t id, const std::string& name)>;
    using ObjectModifiedCallback = std::function<void(const SceneObject&)>;

    Scene() = default;
    explicit Scene(const std::string& name);
    ~Scene() = default;

    // Non-copyable but moveable
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;

    // Metadata operations
    const Metadata& GetMetadata() const noexcept { return _metadata; }
    void SetMetadata(const Metadata& metadata) { _metadata = metadata; UpdateModifiedTime(); }
    void SetName(const std::string& name) { _metadata.name = name; UpdateModifiedTime(); }

    // Object management
    template<typename... Args>
    SceneObject* AddSphere(const std::string& name, Args&&... args);
    SceneObject* AddCube(const std::string& name, const Transform& transform = {});
    template<typename... Args>
    SceneObject* AddCylinder(const std::string& name, Args&&... args);
    template<typename... Args>
    SceneObject* AddCapsule(const std::string& name, Args&&... args);
    SceneObject* AddMesh(const std::string& name, std::unique_ptr<Mesh> mesh, const Transform& transform = {});

    bool RemoveObject(const std::string& name);
    bool RemoveObject(uint32_t id);
    void Clear();

    // Object queries
    SceneObject* FindObject(const std::string& name) noexcept;
    const SceneObject* FindObject(const std::string& name) const noexcept;
    SceneObject* FindObject(uint32_t id) noexcept;
    const SceneObject* FindObject(uint32_t id) const noexcept;

    std::vector<SceneObject*> FindObjectsByType(SceneObject::Type type);
    std::vector<const SceneObject*> FindObjectsByType(SceneObject::Type type) const;

    // Rendering
    void Render() const;
    void RenderObject(const std::string& name) const;
    void RenderObject(uint32_t id) const;

    // Iteration support
    auto begin() noexcept { return _objects.begin(); }
    auto end() noexcept { return _objects.end(); }
    auto begin() const noexcept { return _objects.begin(); }
    auto end() const noexcept { return _objects.end(); }

    // Properties
    size_t GetObjectCount() const noexcept { return _objects.size(); }
    bool IsEmpty() const noexcept { return _objects.empty(); }

    // Serialization
    bool LoadFromFile(const std::string& filePath);
    bool SaveToFile(const std::string& filePath) const;
    bool LoadFromJSON(const json& sceneJson);
    json SaveToJSON() const;

    // Event callbacks
    void SetObjectAddedCallback(ObjectAddedCallback callback) { _onObjectAdded = std::move(callback); }
    void SetObjectRemovedCallback(ObjectRemovedCallback callback) { _onObjectRemoved = std::move(callback); }
    void SetObjectModifiedCallback(ObjectModifiedCallback callback) { _onObjectModified = std::move(callback); }

    // Debug
    void DebugPrint() const;
    void PrintStatistics() const;

private:
    std::vector<std::unique_ptr<SceneObject>> _objects;
    std::unordered_map<std::string, size_t> _nameToIndex;
    std::unordered_map<uint32_t, size_t> _idToIndex;
    Metadata _metadata;

    // Event callbacks
    ObjectAddedCallback _onObjectAdded;
    ObjectRemovedCallback _onObjectRemoved;
    ObjectModifiedCallback _onObjectModified;

    // Helper methods
    void UpdateModifiedTime() { _metadata.modified = std::chrono::system_clock::now(); }
    void UpdateIndices();
    void AddObjectInternal(std::unique_ptr<SceneObject> object);
    bool IsNameAvailable(const std::string& name) const;
    std::string GenerateUniqueName(const std::string& baseName) const;

    // JSON helpers
    //static Transform ParseTransform(const json& transformJson);
    //static json TransformToJSON(const Transform& transform);
    static json MetadataToJSON(const Metadata& metadata);
    static Metadata ParseMetadata(const json& metadataJson);
};

// Improved SceneManager with better resource management
class SceneManager {
public:
    static SceneManager& Instance() {
        static SceneManager instance;
        return instance;
    }

    // Scene management
    Scene* CreateScene(const std::string& name = "New Scene");
    bool LoadScene(const std::string& filePath);
    bool SaveScene(size_t index, const std::string& filePath) const;
    bool SaveCurrentScene(const std::string& filePath) const;
    bool RemoveScene(size_t index);
    void RemoveAllScenes();

    // Scene selection
    bool SetCurrentScene(size_t index);
    bool SetCurrentScene(const std::string& name);
    Scene* GetCurrentScene() noexcept;
    const Scene* GetCurrentScene() const noexcept;

    Scene* GetScene(size_t index) noexcept;
    const Scene* GetScene(size_t index) const noexcept;
    Scene* GetScene(const std::string& name) noexcept;
    const Scene* GetScene(const std::string& name) const noexcept;

    // Properties
    size_t GetSceneCount() const noexcept { return _scenes.size(); }
    std::optional<size_t> GetCurrentSceneIndex() const noexcept;
    std::vector<std::string> GetSceneNames() const;

    // Rendering
    void RenderCurrentScene() const;
    void RenderScene(size_t index) const;

    // Convenience methods for current scene
    template<typename... Args>
    SceneObject* AddSphereToCurrentScene(Args&&... args);
    template<typename... Args>
    SceneObject* AddCubeToCurrentScene(Args&&... args);
    bool RemoveObjectFromCurrentScene(const std::string& name);
    SceneObject* FindObjectInCurrentScene(const std::string& name);

    // Debug
    void PrintSceneList() const;

private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    std::vector<std::unique_ptr<Scene>> _scenes;
    std::optional<size_t> _currentSceneIndex;

    void UpdateCurrentSceneIndex();
};

// Template implementations
template<typename... Args>
SceneObject* Scene::AddSphere(const std::string& name, Args&&... args) {
    auto sphereMesh = StaticMeshes::GetSphere(std::forward<Args>(args)...);
    auto object = std::make_unique<SceneObject>(
        GenerateUniqueName(name),
        std::move(sphereMesh),
        Transform{},
        SceneObject::Type::Sphere
    );

    SceneObject* ptr = object.get();
    AddObjectInternal(std::move(object));
    return ptr;
}

template<typename... Args>
SceneObject* Scene::AddCylinder(const std::string& name, Args&&... args) {
    auto cylinderMesh = StaticMeshes::GetCylinder(std::forward<Args>(args)...);
    auto object = std::make_unique<SceneObject>(
        GenerateUniqueName(name),
        std::move(cylinderMesh),
        Transform{},
        SceneObject::Type::Cylinder
    );

    SceneObject* ptr = object.get();
    AddObjectInternal(std::move(object));
    return ptr;
}

template<typename... Args>
SceneObject* Scene::AddCapsule(const std::string& name, Args&&... args) {
    auto capsuleMesh = StaticMeshes::GetCapsule(std::forward<Args>(args)...);
    auto object = std::make_unique<SceneObject>(
        GenerateUniqueName(name),
        std::move(capsuleMesh),
        Transform{},
        SceneObject::Type::Capsule
    );

    SceneObject* ptr = object.get();
    AddObjectInternal(std::move(object));
    return ptr;
}

template<typename... Args>
SceneObject* SceneManager::AddSphereToCurrentScene(Args&&... args) {
    if (auto scene = GetCurrentScene()) {
        return scene->AddSphere(std::forward<Args>(args)...);
    }
    return nullptr;
}

template<typename... Args>
SceneObject* SceneManager::AddCubeToCurrentScene(Args&&... args) {
    if (auto scene = GetCurrentScene()) {
        return scene->AddCube(std::forward<Args>(args)...);
    }
    return nullptr;
}