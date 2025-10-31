#pragma once

#ifndef RENDER_DATA_H
#define RENDER_DATA_H

#include "../common.h"
#include "../scene/scene.h"

namespace RendererData
{
	struct RendererSettings {
		glm::vec3 backgroundColor{ 0.05f, 0.05f, 0.1f };
		glm::vec3 lightPosition{ 10.0f, 10.0f, 10.0f };
		float fov{ 90.0f };
		bool renderScene{ true };
		bool wireframeMode{ false };
		bool showDebugInfo{ true };
		bool showImGuiDemo{ false };
		bool showSettingsWindow{ true };
	};	

	struct RenderData {
		std::string name;
		RendererSettings settings;

		std::vector<Scene> _scenes;
		size_t activeSceneIndex = 0;
	};

	class RenderDataManager {
	private:
		RenderData renderData;

	public:
		RenderDataManager() = default;
		~RenderDataManager() = default;

		std::string GetProjectName() const { return renderData.name; }
		std::string GetActiveSceneName() const;
		int GetActiveSceneIndex() const { return static_cast<int>(renderData.activeSceneIndex); }
		void SetActiveSceneIndex(size_t index);
		void SetProjectName(const std::string& name) { renderData.name = name; }
		RendererSettings& GetSettings() { return renderData.settings; }

	};
}

#endif // RENDER_DATA_H
