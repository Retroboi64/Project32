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