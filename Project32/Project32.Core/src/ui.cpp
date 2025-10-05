#include "ui.h"

UIX::UIX(GLFWwindow* window)
	: _imguiContext(nullptr), _imguiInitialized(false)
{
	try {
		glfwMakeContextCurrent(window);

		IMGUI_CHECKVERSION();
		_imguiContext = ImGui::CreateContext();
		ImGui::SetCurrentContext(_imguiContext);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui::StyleColorsDark();

		float xscale, yscale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
		float main_scale = (xscale + yscale) * 0.5f;
		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(main_scale);
		style.FontScaleDpi = main_scale;

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");

		_imguiInitialized = true;
	}
	catch (const std::exception& e) {
		throw;
	}
}

UIX::~UIX() {
	_imguiContext = nullptr;
	_imguiInitialized = false;
}

void UIX::Cleanup(GLFWwindow* window) {
	if (_imguiInitialized && _imguiContext) {
		ImGui::SetCurrentContext(_imguiContext);
		glfwMakeContextCurrent(window);

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext(_imguiContext);
		_imguiContext = nullptr;
		_imguiInitialized = false;
	}
}

void UIX::BeginImgui() {
	if (_imguiInitialized && _imguiContext) {
		ImGui::SetCurrentContext(_imguiContext);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
}

void UIX::EndImgui() {
	if (_imguiInitialized && _imguiContext) {
		ImGui::SetCurrentContext(_imguiContext);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

bool UIX::IsInitialized() const {
	return _imguiInitialized;
}

ImGuiContext* UIX::GetContext() const {
	return _imguiContext;
}