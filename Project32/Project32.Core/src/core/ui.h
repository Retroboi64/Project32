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

#ifndef UI_H
#define UI_H

#include "../common.h"

struct Theme {
	ImVec4 primary;
	ImVec4 secondary;
	ImVec4 accent;
	ImVec4 background;
	ImVec4 text;
	ImVec4 textDim;
	float rounding;
	float borderSize;
};

enum class ThemePreset {
	Dark,
	Light,
	Cyberpunk,
	Ocean,
	Forest,
	Dracula,
	Nord,
	Gruvbox,
	Monokai,
	SolarizedDark,
	TokyoNight
};

class UIX {
private:
	ImGuiContext* _imguiContext;
	bool _imguiInitialized;

	Theme currentTheme;
	Theme GetThemePreset(ThemePreset preset);

public:
	UIX(GLFWwindow* window);
	~UIX();

	UIX(const UIX&) = delete;
	UIX& operator=(const UIX&) = delete;

	void SetTheme(ThemePreset preset);
	void SetCustomTheme(const Theme& theme);
	void ApplyTheme(const Theme& theme);
	void Cleanup(GLFWwindow* window);

	void BeginImgui();
	void EndImgui();

	bool IsInitialized() const;
	ImGuiContext* GetContext() const;
};

#endif // UI_H