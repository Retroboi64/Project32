/*
 * This file is part of Project32 - A compact yet powerful and flexible C++ Game Engine
 * Copyright (c) 2025 Patrick Reese (Retroboi64)
 *
 * Licensed under MIT with Attribution Requirements
 * See LICENSE file for full terms
 * GitHub: https://github.com/Retroboi64/Project32
 *
 * This header must not be removed from any source file.
 */

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
		float nearPlane{ 0.1f };
		float farPlane{ 100.0f };
		bool renderScene{ true };
		bool wireframeMode{ false };
		bool showDebugInfo{ true };
		bool showImGuiDemo{ false };
		bool showSettingsWindow{ true };
		bool vsyncEnabled{ true };
		int msaaSamples{ 4 };
		float gamma{ 2.2f };
		float exposure{ 1.0f };
	};

	struct RenderData {
		std::string name;
		std::string version{ "1.0.0" };
		RendererSettings settings;

		std::vector<Scene> _scenes;
		size_t activeSceneIndex = 0;
	};

	class RenderDataManager {
	private:
		RenderData renderData;
		std::string loadedFilePath;

	public:
		RenderDataManager() = default;
		~RenderDataManager() = default;

		std::string GetProjectName() const { return renderData.name; }
		std::string GetActiveSceneName() const;
		int GetActiveSceneIndex() const { return static_cast<int>(renderData.activeSceneIndex); }
		RendererSettings& GetSettings() { return renderData.settings; }
		const RendererSettings& GetSettings() const { return renderData.settings; }
		const RenderData& GetRenderData() const { return renderData; }
		RenderData& GetRenderData() { return renderData; }
		std::string GetLoadedFilePath() const { return loadedFilePath; }

		void SetActiveSceneIndex(size_t index);
		void SetProjectName(const std::string& name) { renderData.name = name; }

		bool LoadFromFile(const std::string& filepath);
		bool SaveToFile(const std::string& filepath) const;
	};
}

#endif // RENDER_DATA_H