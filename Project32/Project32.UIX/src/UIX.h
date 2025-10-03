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

#ifdef UIEXPORT
#define UILIB_API __declspec(dllexport)
#else
#define UILIB_API __declspec(dllimport)
#endif

#include "imgui.h"
#include <string>
#include <vector>
#include <functional>


namespace UIX {
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
		Forest
	};

	class UILIB_API UIManager {
	public:
		~UIManager();

		static UIManager& Instance();

		void Initialize();
		void SetTheme(ThemePreset preset);
		void SetCustomTheme(const Theme& theme);
		void ApplyTheme();

		void BeginFrame();
		void EndFrame();

		bool Button(const char* label, const ImVec2& size = ImVec2(0, 0), bool glow = false);
		bool ImageButton(void* texture, const ImVec2& size, const char* tooltip = nullptr);
		bool ToggleButton(const char* label, bool* state);
		bool IconButton(const char* icon, const char* tooltip = nullptr);

		void ProgressBar(float fraction, const ImVec2& size, const char* overlay = nullptr);
		void GlowText(const char* text, const ImVec4& color);
		void HeaderText(const char* text);
		void SubText(const char* text);

		bool InputTextStyled(const char* label, char* buf, size_t bufSize);
		bool SliderFloatStyled(const char* label, float* v, float min, float max);
		bool ComboStyled(const char* label, int* current, const char* const items[], int count);

		bool BeginStyledWindow(const char* name, bool* open = nullptr, ImGuiWindowFlags flags = 0);
		void EndStyledWindow();

		bool BeginPanel(const char* label, const ImVec2& size = ImVec2(0, 0));
		void EndPanel();

		bool BeginCard(const char* title = nullptr);
		void EndCard();

		void BeginHorizontal();
		void EndHorizontal();
		void Space(float amount = -1.0f);
		void Separator();

		void ShowNotification(const char* message, float duration = 3.0f, ImVec4 color = ImVec4(0, 0, 0, 0));
		void UpdateNotifications();

		void ShowTooltip(const char* text);
		void ShowAdvancedTooltip(const char* title, const char* description);

		bool ShowConfirmDialog(const char* title, const char* message, bool* open);
		bool ShowInputDialog(const char* title, const char* label, char* buf, size_t bufSize, bool* open);

		bool BeginMenuBarStyled();
		void EndMenuBarStyled();
		bool MenuItemStyled(const char* label, const char* shortcut = nullptr);

		void SetNextWindowCenter();
		ImVec2 GetContentSize();
		float GetTextWidth(const char* text);

	private:
		UIManager();
		Theme currentTheme;

		struct Notification {
			std::string message;
			float timeRemaining;
			ImVec4 color;
		};
		std::vector<Notification> notifications;

		Theme GetThemePreset(ThemePreset preset);
		void PushButtonStyle(bool glow = false);
		void PopButtonStyle();
	};

	UILIB_API void SetupImGuiStyle();
	UILIB_API ImVec4 ColorWithAlpha(const ImVec4& color, float alpha);
	UILIB_API ImVec4 LerpColor(const ImVec4& a, const ImVec4& b, float t);
	UILIB_API void DrawGlow(ImDrawList* drawList, const ImVec2& pos, const ImVec2& size, const ImVec4& color, float intensity = 1.0f);
}