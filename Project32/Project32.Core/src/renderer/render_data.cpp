#include "../common.h"

#include "render_data.h"

std::string RendererData::RenderDataManager::GetActiveSceneName() const {
	if (renderData._scenes.empty() || renderData.activeSceneIndex >= renderData._scenes.size()) {
			return "No Active Scene";
	}
	return renderData._scenes[renderData.activeSceneIndex].GetMetadata().name;
}

void RendererData::RenderDataManager::SetActiveSceneIndex(size_t index) {
	if (index < renderData._scenes.size()) {
		renderData.activeSceneIndex = index;
	} else {
		spdlog::warn("[RenderDataManager::SetActiveSceneIndex] Invalid scene index: {}", index);
	}
}