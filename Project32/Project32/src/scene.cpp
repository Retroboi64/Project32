#include "scene.h"

// Scene implementation
void Scene::DebugPrint() const {
	for (const auto& obj : _objects) {
		std::cout << "Object: " << obj.name << ", Position: ("
			<< obj.transform.position.x << ", "
			<< obj.transform.position.y << ", "
			<< obj.transform.position.z << ")\n";
	}
}

void Scene::AddSphere(const std::string& name, int sectorCount, int stackCount, float radius, const Transform& transform) {
	auto sphereMesh = StaticMeshes::GetSphere(sectorCount, stackCount, radius);
	_objects.push_back({ name, std::move(sphereMesh), transform });
}

void Scene::AddMesh(const std::string& name, std::unique_ptr<Mesh> mesh, const Transform& transform) {
	_objects.push_back({ name, std::move(mesh), transform });
}

void Scene::RemoveMesh(const std::string& name) {
	_objects.erase(std::remove_if(_objects.begin(), _objects.end(),
		[&](const SceneObject& obj) { return obj.name == name; }), _objects.end());
}

void Scene::DrawMeshes() {
	if (_objects.empty()) return;

	for (auto& obj : _objects) {
		obj.mesh->Draw();
	}
}

void Scene::DrawMeshByName(const std::string& name) {
	for (auto& obj : _objects) {
		if (obj.name == name) {
			obj.mesh->Draw();
			return;
		}
	}
	std::cerr << "Mesh with name '" << name << "' not found in scene.\n";
}

// TODO: THIS LOOKS WRONG
Transform Scene::GetMeshTransform(const std::string& name) const {
	for (const auto& obj : _objects) {
		if (obj.name == name) {
			return obj.transform;
		}
	}

	std::cerr << "Mesh with name '" << name << "' not found in scene.\n";

	return Transform{};
}

// SceneManager implementation
void SceneManager::CreateScene() {
	_scenes.push_back(std::make_unique<Scene>());
	if (_currentSceneIndex == -1) {
		_currentSceneIndex = 0;
	}
}

void SceneManager::DrawCurrentScene() {
	if (auto scene = GetCurrentScene()) {
		scene->DrawMeshes();
	}
}

Transform SceneManager::GetMeshTransformInCurrentScene(const std::string& name) const {
	if (_currentSceneIndex >= 0 && _currentSceneIndex < static_cast<int>(_scenes.size())) {
		return _scenes[_currentSceneIndex]->GetMeshTransform(name);
	}
	else {
		std::cerr << "No current scene selected.\n";
		return Transform{};
	}
	return Transform{};
}

Scene* SceneManager::GetCurrentScene() {
	if (_currentSceneIndex >= 0 && _currentSceneIndex < static_cast<int>(_scenes.size())) {
		return _scenes[_currentSceneIndex].get();
	}
	return nullptr;
}

Scene* SceneManager::GetScene(int index) {
	if (index >= 0 && index < static_cast<int>(_scenes.size())) {
		return _scenes[index].get();
	}
	return nullptr;
}
