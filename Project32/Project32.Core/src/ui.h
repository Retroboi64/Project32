#pragma once

#include "common.h"

class UIX {
private:
	ImGuiContext* _imguiContext;
	bool _imguiInitialized;
public:
	UIX(GLFWwindow* window);
	~UIX();

	UIX(const UIX&) = delete;
	UIX& operator=(const UIX&) = delete;

	void Cleanup(GLFWwindow* window);

	void BeginImgui();
	void EndImgui();

	bool IsInitialized() const;
	ImGuiContext* GetContext() const;
};