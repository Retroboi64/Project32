#include "scene.h"
#include <iostream>
#include <algorithm>

// Scene implementation
void Scene::AddQuad(const std::string& name) {
    auto mesh = StaticMeshes::GetQuad();
    if (mesh) {
        _meshes.push_back(std::make_unique<SceneMesh>(std::move(mesh), name));
        std::cout << "Added quad mesh: " << name << std::endl;
    }
}

void Scene::AddCube(const std::string& name) {
    auto mesh = StaticMeshes::GetCube();
    if (mesh) {
        _meshes.push_back(std::make_unique<SceneMesh>(std::move(mesh), name));
        std::cout << "Added cube mesh: " << name << std::endl;
    }
}

void Scene::AddCylinder(const std::string& name, unsigned int segments, float height, float radius) {
    auto mesh = StaticMeshes::GetCylinder(segments, height, radius);
    if (mesh) {
        _meshes.push_back(std::make_unique<SceneMesh>(std::move(mesh), name));
        std::cout << "Added cylinder mesh: " << name << " (segments: " << segments
            << ", height: " << height << ", radius: " << radius << ")" << std::endl;
    }
}

void Scene::AddSphere(const std::string& name, unsigned int latSegments, unsigned int lonSegments, float radius) {
    auto mesh = StaticMeshes::GetSphere(latSegments, lonSegments, radius);
    if (mesh) {
        _meshes.push_back(std::make_unique<SceneMesh>(std::move(mesh), name));
        std::cout << "Added sphere mesh: " << name << " (lat: " << latSegments
            << ", lon: " << lonSegments << ", radius: " << radius << ")" << std::endl;
    }
}

void Scene::AddCapsule(const std::string& name, unsigned int segments, unsigned int rings, float height, float radius) {
    auto mesh = StaticMeshes::GetCapsule(segments, rings, height, radius);
    if (mesh) {
        _meshes.push_back(std::make_unique<SceneMesh>(std::move(mesh), name));
        std::cout << "Added capsule mesh: " << name << " (segments: " << segments
            << ", rings: " << rings << ", height: " << height << ", radius: " << radius << ")" << std::endl;
    }
}

bool Scene::LoadModel(const std::string& filePath, const std::string& namePrefix, bool generateNormals, bool flipTextureCoords) {
    auto loadedModel = ModelImporter::LoadFromFile(filePath, generateNormals, flipTextureCoords);
    if (!loadedModel) {
        std::cerr << "Failed to load model: " << filePath << std::endl;
        return false;
    }

    std::cout << "Successfully loaded model: " << filePath << std::endl;
    std::cout << "Model contains " << loadedModel->meshes.size() << " meshes" << std::endl;

    for (size_t i = 0; i < loadedModel->meshes.size(); ++i) {
        std::string meshName = namePrefix.empty() ?
            ("Model_Mesh_" + std::to_string(_meshes.size())) :
            (namePrefix + "_Mesh_" + std::to_string(i));

        auto& modelMesh = loadedModel->meshes[i];
        if (modelMesh) {
            _meshes.push_back(std::make_unique<SceneMesh>(std::move(modelMesh), meshName));
            std::cout << "Added mesh from model: " << meshName << std::endl;
        }
    }

    _loadedModels.push_back(std::move(loadedModel));

    return true;
}

SceneMesh* Scene::GetMesh(const std::string& name) {
    auto it = std::find_if(_meshes.begin(), _meshes.end(),
        [&name](const std::unique_ptr<SceneMesh>& sceneMesh) {
            return sceneMesh->name == name;
        });

    return (it != _meshes.end()) ? it->get() : nullptr;
}

SceneMesh* Scene::GetMesh(size_t index) {
    if (index < _meshes.size()) {
        return _meshes[index].get();
    }
    return nullptr;
}

void Scene::RemoveMesh(const std::string& name) {
    _meshes.erase(
        std::remove_if(_meshes.begin(), _meshes.end(),
            [&name](const std::unique_ptr<SceneMesh>& sceneMesh) {
                return sceneMesh->name == name;
            }),
        _meshes.end());
}

void Scene::RemoveMesh(size_t index) {
    if (index < _meshes.size()) {
        _meshes.erase(_meshes.begin() + index);
    }
}

void Scene::Clear() {
    _meshes.clear();
    _loadedModels.clear();
    std::cout << "Scene cleared" << std::endl;
}

void Scene::DrawAll() const {
    for (const auto& sceneMesh : _meshes) {
        if (sceneMesh->isVisible && sceneMesh->mesh) {
            sceneMesh->mesh->Draw();
        }
    }
}

void Scene::DrawMesh(const std::string& name) const {
    auto* sceneMesh = const_cast<Scene*>(this)->GetMesh(name);
    if (sceneMesh && sceneMesh->isVisible && sceneMesh->mesh) {
        sceneMesh->mesh->Draw();
    }
}

void Scene::DrawMesh(size_t index) const {
    auto* sceneMesh = const_cast<Scene*>(this)->GetMesh(index);
    if (sceneMesh && sceneMesh->isVisible && sceneMesh->mesh) {
        sceneMesh->mesh->Draw();
    }
}

void Scene::PrintMeshInfo() const {
    std::cout << "\n=== Scene Mesh Information ===" << std::endl;
    std::cout << "Total meshes: " << _meshes.size() << std::endl;

    for (size_t i = 0; i < _meshes.size(); ++i) {
        const auto& sceneMesh = _meshes[i];
        std::cout << "[" << i << "] " << sceneMesh->name;
        if (sceneMesh->mesh) {
            std::cout << " (Indices: " << sceneMesh->mesh->GetIndexCount() << ")";
        }
        std::cout << " - " << (sceneMesh->isVisible ? "Visible" : "Hidden") << std::endl;
        std::cout << "    Position: (" << sceneMesh->transform.position.x << ", " << sceneMesh->transform.position.y << ", " << sceneMesh->transform.position.z << ")" << std::endl;
        std::cout << "    Scale: (" << sceneMesh->transform.scale.x << ", " << sceneMesh->transform.scale.y << ", " << sceneMesh->transform.scale.z << ")" << std::endl;
    }
    std::cout << "==============================\n" << std::endl;
}

std::vector<std::string> Scene::GetMeshNames() const {
    std::vector<std::string> names;
    names.reserve(_meshes.size());

    for (const auto& sceneMesh : _meshes) {
        names.push_back(sceneMesh->name);
    }

    return names;
}

// SceneManager implementation
SceneManager::SceneManager() {
    _currentScene = std::make_unique<Scene>();
}

void SceneManager::CreateNewScene() {
    _currentScene = std::make_unique<Scene>();
    std::cout << "Created new scene" << std::endl;
}

void SceneManager::LoadSceneFromFile(const std::string& configFile) {
    if (_currentScene) {
        _currentScene->LoadModel(configFile, "LoadedModel");
    }
}