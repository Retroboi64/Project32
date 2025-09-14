#include "common.h"
#include "mesh.h"

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

	void DebugPrint() const;
	void AddSphere(const std::string& name, int sectorCount, int stackCount, float radius, const Transform& transform);
	void AddMesh(const std::string& name, std::unique_ptr<Mesh> mesh, const Transform& transform);
	void RemoveMesh(const std::string& name);
	void DrawMeshes();
	void DrawMeshByName(const std::string& name);
	void Clear() { _objects.clear(); }
	void Render();

	Transform GetMeshTransform(const std::string& name) const;

	const std::vector<SceneObject>& GetObjects() const { return _objects; }
};

class SceneManager {
private:
	std::vector<std::unique_ptr<Scene>> _scenes;
	int _currentSceneIndex = -1;

public:
	void CreateScene();
	void DrawCurrentScene();

	int GetCurrentSceneIndex() const { return _currentSceneIndex; }
	int GetSceneCount() const { return static_cast<int>(_scenes.size()); }

	Scene* GetCurrentScene();
	Scene* GetScene(int index);

	Transform GetMeshTransformInCurrentScene(const std::string& name) const;
};