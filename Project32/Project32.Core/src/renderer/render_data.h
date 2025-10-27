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
		std::vector<Scene> _scenes;
	};
}

#endif // RENDER_DATA_H
