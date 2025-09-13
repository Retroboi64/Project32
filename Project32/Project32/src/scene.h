#include "common.h"

class Scene {
private:
	struct SceneObject {
		std::string name;
		std::unique_ptr<Mesh> mesh;
		Transform transform;
	};
	std::vector<SceneObject> _objects;

public:
	Scene() = default;
	~Scene() = default;

	void DebugPrint() const {
		for (const auto& obj : _objects) {
			std::cout << "Object: " << obj.name << ", Position: ("
				<< obj.transform.position.x << ", "
				<< obj.transform.position.y << ", "
				<< obj.transform.position.z << ")\n";
		}
	}

	void AddSphere(const std::string& name, int sectorCount, int stackCount, float radius, const Transform& transform) {
		auto sphereMesh = StaticMeshes::GetSphere(sectorCount, stackCount, radius);
		_objects.push_back({ name, std::move(sphereMesh), transform });
	}

	void AddMesh(const std::string& name, std::unique_ptr<Mesh> mesh, const Transform& transform) {
		_objects.push_back({ name, std::move(mesh), transform });
	}

	void RemoveMesh(const std::string& name) {
		_objects.erase(std::remove_if(_objects.begin(), _objects.end(),
			[&](const SceneObject& obj) { return obj.name == name; }), _objects.end());
	}

	void DrawMeshes(Shader& shader) {
		if (_objects.empty()) return;

		const glm::vec3 Color(0.2f, 0.3f, 0.8f);

		shader.Bind();
		shader.SetBool("useTexture", false);
		shader.SetMat4("model", _objects[0].transform.ToMatrix());
		shader.SetVec3("color", Color);

		for (auto& obj : _objects) {
			if (!obj.mesh || !obj.mesh->IsValid()) continue;
			obj.mesh->Draw();
		}
	}

	void Clear() {
		_objects.clear();
	}

	void Render();

	const std::vector<SceneObject>& GetObjects() const { return _objects; }
};

class SceneManager {
private:
	std::vector<std::unique_ptr<Scene>> _scenes;
	int _currentSceneIndex = -1;

public:
	void CreateScene() {
		_scenes.push_back(std::make_unique<Scene>());
		if (_currentSceneIndex == -1) {
			_currentSceneIndex = 0;
		}
	}

	Scene* GetCurrentScene() {
		if (_currentSceneIndex >= 0 && _currentSceneIndex < static_cast<int>(_scenes.size())) {
			return _scenes[_currentSceneIndex].get();
		}
		return nullptr;
	}

	Scene* GetScene(int index) {
		if (index >= 0 && index < static_cast<int>(_scenes.size())) {
			return _scenes[index].get();
		}
		return nullptr;
	}

	int GetCurrentSceneIndex() const { return _currentSceneIndex; }
	int GetSceneCount() const { return static_cast<int>(_scenes.size()); }
};