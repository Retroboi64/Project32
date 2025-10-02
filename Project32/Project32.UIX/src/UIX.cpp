#include "UIX.h"
#include <algorithm>

namespace UIX {
	UIManager& UIManager::Instance() {
		static UIManager instance;
		return instance;
	}

	void UIManager::Initialize() {
		SetTheme(ThemePreset::Dark);
		ApplyTheme();
	}

	Theme UIManager::GetThemePreset(ThemePreset preset) {
		Theme theme;

		switch (preset) {
		case ThemePreset::Dark:
			theme.primary = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
			theme.secondary = ImVec4(0.20f, 0.22f, 0.27f, 1.0f);
			theme.accent = ImVec4(0.40f, 0.70f, 1.0f, 1.0f);
			theme.background = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
			theme.text = ImVec4(0.95f, 0.96f, 0.98f, 1.0f);
			theme.textDim = ImVec4(0.60f, 0.62f, 0.65f, 1.0f);
			theme.rounding = 6.0f;
			theme.borderSize = 1.0f;
			break;

		case ThemePreset::Cyberpunk:
			theme.primary = ImVec4(1.0f, 0.0f, 0.8f, 1.0f);
			theme.secondary = ImVec4(0.1f, 0.1f, 0.15f, 1.0f);
			theme.accent = ImVec4(0.0f, 0.9f, 0.9f, 1.0f);
			theme.background = ImVec4(0.05f, 0.05f, 0.08f, 1.0f);
			theme.text = ImVec4(0.0f, 1.0f, 0.8f, 1.0f);
			theme.textDim = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);
			theme.rounding = 0.0f;
			theme.borderSize = 2.0f;
			break;

		case ThemePreset::Ocean:
			theme.primary = ImVec4(0.2f, 0.6f, 0.8f, 1.0f);
			theme.secondary = ImVec4(0.15f, 0.25f, 0.35f, 1.0f);
			theme.accent = ImVec4(0.3f, 0.8f, 0.9f, 1.0f);
			theme.background = ImVec4(0.08f, 0.15f, 0.22f, 1.0f);
			theme.text = ImVec4(0.9f, 0.95f, 1.0f, 1.0f);
			theme.textDim = ImVec4(0.5f, 0.65f, 0.75f, 1.0f);
			theme.rounding = 8.0f;
			theme.borderSize = 1.0f;
			break;

		case ThemePreset::Forest:
			theme.primary = ImVec4(0.4f, 0.7f, 0.3f, 1.0f);
			theme.secondary = ImVec4(0.2f, 0.3f, 0.2f, 1.0f);
			theme.accent = ImVec4(0.6f, 0.9f, 0.4f, 1.0f);
			theme.background = ImVec4(0.12f, 0.18f, 0.12f, 1.0f);
			theme.text = ImVec4(0.95f, 0.98f, 0.95f, 1.0f);
			theme.textDim = ImVec4(0.6f, 0.7f, 0.6f, 1.0f);
			theme.rounding = 5.0f;
			theme.borderSize = 1.0f;
			break;

		case ThemePreset::Light:
			theme.primary = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
			theme.secondary = ImVec4(0.9f, 0.9f, 0.92f, 1.0f);
			theme.accent = ImVec4(0.20f, 0.50f, 0.90f, 1.0f);
			theme.background = ImVec4(0.95f, 0.95f, 0.97f, 1.0f);
			theme.text = ImVec4(0.1f, 0.1f, 0.12f, 1.0f);
			theme.textDim = ImVec4(0.45f, 0.45f, 0.50f, 1.0f);
			theme.rounding = 6.0f;
			theme.borderSize = 1.0f;
			break;
		}

		return theme;
	}

	void UIManager::SetTheme(ThemePreset preset) {
		currentTheme = GetThemePreset(preset);
		ApplyTheme();
	}

	void UIManager::SetCustomTheme(const Theme& theme) {
		currentTheme = theme;
		ApplyTheme();
	}

	void UIManager::ApplyTheme() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding = currentTheme.rounding;
		style.WindowBorderSize = currentTheme.borderSize;
		style.WindowPadding = ImVec2(12, 12);

		style.FrameRounding = currentTheme.rounding;
		style.FrameBorderSize = 0.0f;
		style.FramePadding = ImVec2(8, 6);

		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = currentTheme.rounding;

		style.GrabRounding = currentTheme.rounding;
		style.GrabMinSize = 12.0f;

		style.TabRounding = currentTheme.rounding;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = currentTheme.text;
		colors[ImGuiCol_TextDisabled] = currentTheme.textDim;
		colors[ImGuiCol_WindowBg] = currentTheme.background;
		colors[ImGuiCol_ChildBg] = currentTheme.background;
		colors[ImGuiCol_PopupBg] = ColorWithAlpha(currentTheme.secondary, 0.95f);
		colors[ImGuiCol_Border] = ColorWithAlpha(currentTheme.primary, 0.3f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_FrameBg] = ColorWithAlpha(currentTheme.secondary, 0.8f);
		colors[ImGuiCol_FrameBgHovered] = ColorWithAlpha(currentTheme.secondary, 1.0f);
		colors[ImGuiCol_FrameBgActive] = ColorWithAlpha(currentTheme.primary, 0.3f);
		colors[ImGuiCol_TitleBg] = ColorWithAlpha(currentTheme.secondary, 0.9f);
		colors[ImGuiCol_TitleBgActive] = currentTheme.secondary;
		colors[ImGuiCol_TitleBgCollapsed] = ColorWithAlpha(currentTheme.secondary, 0.7f);
		colors[ImGuiCol_MenuBarBg] = currentTheme.secondary;
		colors[ImGuiCol_ScrollbarBg] = ColorWithAlpha(currentTheme.background, 0.5f);
		colors[ImGuiCol_ScrollbarGrab] = ColorWithAlpha(currentTheme.primary, 0.5f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ColorWithAlpha(currentTheme.primary, 0.7f);
		colors[ImGuiCol_ScrollbarGrabActive] = currentTheme.primary;
		colors[ImGuiCol_CheckMark] = currentTheme.accent;
		colors[ImGuiCol_SliderGrab] = currentTheme.primary;
		colors[ImGuiCol_SliderGrabActive] = currentTheme.accent;
		colors[ImGuiCol_Button] = ColorWithAlpha(currentTheme.primary, 0.6f);
		colors[ImGuiCol_ButtonHovered] = ColorWithAlpha(currentTheme.primary, 0.8f);
		colors[ImGuiCol_ButtonActive] = currentTheme.primary;
		colors[ImGuiCol_Header] = ColorWithAlpha(currentTheme.primary, 0.5f);
		colors[ImGuiCol_HeaderHovered] = ColorWithAlpha(currentTheme.primary, 0.7f);
		colors[ImGuiCol_HeaderActive] = ColorWithAlpha(currentTheme.primary, 0.9f);
		colors[ImGuiCol_Separator] = ColorWithAlpha(currentTheme.primary, 0.4f);
		colors[ImGuiCol_SeparatorHovered] = ColorWithAlpha(currentTheme.primary, 0.6f);
		colors[ImGuiCol_SeparatorActive] = ColorWithAlpha(currentTheme.primary, 0.8f);
		colors[ImGuiCol_ResizeGrip] = ColorWithAlpha(currentTheme.primary, 0.4f);
		colors[ImGuiCol_ResizeGripHovered] = ColorWithAlpha(currentTheme.primary, 0.6f);
		colors[ImGuiCol_ResizeGripActive] = ColorWithAlpha(currentTheme.primary, 0.8f);
		colors[ImGuiCol_Tab] = ColorWithAlpha(currentTheme.secondary, 0.8f);
		colors[ImGuiCol_TabHovered] = ColorWithAlpha(currentTheme.primary, 0.8f);
		colors[ImGuiCol_TabActive] = ColorWithAlpha(currentTheme.primary, 0.6f);
		colors[ImGuiCol_TabUnfocused] = ColorWithAlpha(currentTheme.secondary, 0.6f);
		colors[ImGuiCol_TabUnfocusedActive] = ColorWithAlpha(currentTheme.secondary, 0.8f);
	}

	void UIManager::BeginFrame() {
		UpdateNotifications();
	}

	void UIManager::EndFrame() {}

	bool UIManager::Button(const char* label, const ImVec2& size, bool glow) {
		PushButtonStyle(glow);
		bool result = ImGui::Button(label, size);
		PopButtonStyle();
		return result;
	}

	bool UIManager::ToggleButton(const char* label, bool* state) {
		ImVec4 activeColor = currentTheme.accent;
		ImVec4 inactiveColor = ColorWithAlpha(currentTheme.secondary, 0.8f);

		ImGui::PushStyleColor(ImGuiCol_Button, *state ? activeColor : inactiveColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, *state ? ColorWithAlpha(activeColor, 0.8f) : ColorWithAlpha(inactiveColor, 1.0f));

		bool clicked = ImGui::Button(label);
		if (clicked) *state = !*state;

		ImGui::PopStyleColor(2);
		return clicked;
	}

	void UIManager::ProgressBar(float fraction, const ImVec2& size, const char* overlay) {
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, currentTheme.primary);
		ImGui::ProgressBar(fraction, size, overlay);
		ImGui::PopStyleColor();
	}

	void UIManager::GlowText(const char* text, const ImVec4& color) {
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::Text("%s", text);
		ImGui::PopStyleColor();
	}

	void UIManager::HeaderText(const char* text) {
		ImGui::PushFont(ImGui::GetFont());
		ImGui::PushStyleColor(ImGuiCol_Text, currentTheme.accent);
		ImGui::Text("%s", text);
		ImGui::PopStyleColor();
		ImGui::PopFont();
	}

	void UIManager::SubText(const char* text) {
		ImGui::PushStyleColor(ImGuiCol_Text, currentTheme.textDim);
		ImGui::Text("%s", text);
		ImGui::PopStyleColor();
	}

	bool UIManager::BeginStyledWindow(const char* name, bool* open, ImGuiWindowFlags flags) {
		return ImGui::Begin(name, open, flags);
	}

	void UIManager::EndStyledWindow() {
		ImGui::End();
	}

	bool UIManager::BeginPanel(const char* label, const ImVec2& size) {
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ColorWithAlpha(currentTheme.secondary, 0.5f));
		bool result = ImGui::BeginChild(label, size, true);
		return result;
	}

	void UIManager::EndPanel() {
		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void UIManager::ShowNotification(const char* message, float duration, ImVec4 color) {
		if (color.w == 0.0f) color = currentTheme.accent;
		notifications.push_back({ message, duration, color });
	}

	void UIManager::UpdateNotifications() {
		float yOffset = 10.0f;
		ImGuiIO& io = ImGui::GetIO();

		for (auto it = notifications.begin(); it != notifications.end();) {
			it->timeRemaining -= io.DeltaTime;

			if (it->timeRemaining <= 0.0f) {
				it = notifications.erase(it);
			}
			else {
				ImVec2 windowPos(io.DisplaySize.x - 310.0f, yOffset);
				ImGui::SetNextWindowPos(windowPos);
				ImGui::SetNextWindowSize(ImVec2(300, 0));

				ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorWithAlpha(it->color, 0.9f));
				ImGui::Begin("##notification", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
				ImGui::TextWrapped("%s", it->message.c_str());
				ImGui::End();
				ImGui::PopStyleColor();

				yOffset += 70.0f;
				++it;
			}
		}
	}

	void UIManager::ShowTooltip(const char* text) {
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", text);
		}
	}

	void UIManager::PushButtonStyle(bool glow) {
		if (glow) {
			ImGui::PushStyleColor(ImGuiCol_Button, ColorWithAlpha(currentTheme.accent, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentTheme.accent);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorWithAlpha(currentTheme.accent, 1.2f));
		}
	}

	void UIManager::PopButtonStyle() {}

	// TODO: Replace with glm
	ImVec4 ColorWithAlpha(const ImVec4& color, float alpha) {
		return ImVec4(color.x, color.y, color.z, alpha);
	}

	ImVec4 LerpColor(const ImVec4& a, const ImVec4& b, float t) {
		return ImVec4(
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t,
			a.w + (b.w - a.w) * t
		);
	}

}