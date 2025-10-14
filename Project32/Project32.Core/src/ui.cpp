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

#include "common.h"
#include "ui.h"

ImVec4 ColorWithAlpha(const ImVec4& color, float alpha) {
    return ImVec4(color.x, color.y, color.z, alpha);
}

UIX::UIX(GLFWwindow* window)
    : _imguiContext(nullptr), _imguiInitialized(false)
{
    try {
        std::cout << "[UIX] Initializing ImGui..." << std::endl;

        glfwMakeContextCurrent(window);

        IMGUI_CHECKVERSION();
        _imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(_imguiContext);
        std::cout << "[UIX] ImGui context created: " << _imguiContext << std::endl;

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

        SetTheme(ThemePreset::Dark);
        std::cout << "[UIX] Initialization complete!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[UIX ERROR] Initialization failed: " << e.what() << std::endl;
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

void UIX::SetTheme(ThemePreset preset) {
    ApplyTheme(GetThemePreset(preset));
}

void UIX::SetCustomTheme(const Theme& theme) {
    ApplyTheme(theme);
}

Theme UIX::GetThemePreset(ThemePreset preset) {
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

    case ThemePreset::Dracula:
        theme.primary = ImVec4(0.74f, 0.58f, 0.98f, 1.0f);    
        theme.secondary = ImVec4(0.16f, 0.16f, 0.21f, 1.0f);  
        theme.accent = ImVec4(1.0f, 0.47f, 0.78f, 1.0f);      
        theme.background = ImVec4(0.11f, 0.11f, 0.15f, 1.0f); 
        theme.text = ImVec4(0.97f, 0.97f, 0.94f, 1.0f);       
        theme.textDim = ImVec4(0.62f, 0.62f, 0.68f, 1.0f);
        theme.rounding = 4.0f;
        theme.borderSize = 1.0f;
        break;

    case ThemePreset::Nord:
        theme.primary = ImVec4(0.51f, 0.63f, 0.76f, 1.0f);    
        theme.secondary = ImVec4(0.23f, 0.26f, 0.32f, 1.0f);  
        theme.accent = ImVec4(0.55f, 0.75f, 0.68f, 1.0f);    
        theme.background = ImVec4(0.18f, 0.20f, 0.25f, 1.0f); 
        theme.text = ImVec4(0.92f, 0.94f, 0.95f, 1.0f);      
        theme.textDim = ImVec4(0.60f, 0.65f, 0.70f, 1.0f);
        theme.rounding = 3.0f;
        theme.borderSize = 1.0f;
        break;

    case ThemePreset::Gruvbox:
        theme.primary = ImVec4(0.98f, 0.74f, 0.40f, 1.0f);    
        theme.secondary = ImVec4(0.20f, 0.18f, 0.16f, 1.0f);  
        theme.accent = ImVec4(0.72f, 0.73f, 0.15f, 1.0f);     
        theme.background = ImVec4(0.16f, 0.15f, 0.13f, 1.0f); 
        theme.text = ImVec4(0.92f, 0.86f, 0.70f, 1.0f);       
        theme.textDim = ImVec4(0.66f, 0.60f, 0.53f, 1.0f);
        theme.rounding = 2.0f;
        theme.borderSize = 1.0f;
        break;

    case ThemePreset::Monokai:
        theme.primary = ImVec4(0.40f, 0.85f, 0.94f, 1.0f);    
        theme.secondary = ImVec4(0.16f, 0.16f, 0.14f, 1.0f);  
        theme.accent = ImVec4(0.98f, 0.96f, 0.45f, 1.0f);    
        theme.background = ImVec4(0.13f, 0.13f, 0.11f, 1.0f); 
        theme.text = ImVec4(0.97f, 0.97f, 0.95f, 1.0f);       
        theme.textDim = ImVec4(0.58f, 0.58f, 0.56f, 1.0f);
        theme.rounding = 2.0f;
        theme.borderSize = 0.0f;
        break;

    case ThemePreset::SolarizedDark:
        theme.primary = ImVec4(0.15f, 0.55f, 0.82f, 1.0f);    
        theme.secondary = ImVec4(0.03f, 0.21f, 0.26f, 1.0f);  
        theme.accent = ImVec4(0.71f, 0.54f, 0.0f, 1.0f);      
        theme.background = ImVec4(0.0f, 0.17f, 0.21f, 1.0f);  
        theme.text = ImVec4(0.51f, 0.58f, 0.59f, 1.0f);       
        theme.textDim = ImVec4(0.36f, 0.43f, 0.44f, 1.0f);
        theme.rounding = 4.0f;
        theme.borderSize = 1.0f;
        break;

    case ThemePreset::TokyoNight:
        theme.primary = ImVec4(0.45f, 0.68f, 1.0f, 1.0f);     
        theme.secondary = ImVec4(0.11f, 0.13f, 0.20f, 1.0f);  
        theme.accent = ImVec4(0.73f, 0.57f, 1.0f, 1.0f);      
        theme.background = ImVec4(0.09f, 0.10f, 0.15f, 1.0f); 
        theme.text = ImVec4(0.79f, 0.82f, 0.92f, 1.0f);      
        theme.textDim = ImVec4(0.56f, 0.60f, 0.72f, 1.0f);
        theme.rounding = 5.0f;
        theme.borderSize = 1.0f;
        break;
    }

    return theme;
}

void UIX::ApplyTheme(const Theme& theme) {
    if (!ImGui::GetCurrentContext()) {
        currentTheme = theme;
        return;
    }

    currentTheme = theme;

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = theme.rounding;
    style.WindowBorderSize = theme.borderSize;
    style.WindowPadding = ImVec2(12, 12);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Right;

    style.FrameRounding = theme.rounding;
    style.FrameBorderSize = 0.0f;
    style.FramePadding = ImVec2(8, 6);

    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = theme.rounding;

    style.GrabRounding = theme.rounding;
    style.GrabMinSize = 12.0f;

    style.TabRounding = theme.rounding;
    style.TabBorderSize = 0.0f;

    style.ChildRounding = theme.rounding;
    style.ChildBorderSize = 1.0f;

    style.PopupRounding = theme.rounding;
    style.PopupBorderSize = theme.borderSize;

    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.IndentSpacing = 20.0f;

    style.SeparatorTextBorderSize = 1.0f;
    style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
    style.SeparatorTextPadding = ImVec2(20.0f, 3.0f);

    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = theme.text;
    colors[ImGuiCol_TextDisabled] = theme.textDim;
    colors[ImGuiCol_TextSelectedBg] = ColorWithAlpha(theme.primary, 0.35f);

    colors[ImGuiCol_WindowBg] = theme.background;
    colors[ImGuiCol_ChildBg] = ColorWithAlpha(theme.background, 0.0f);
    colors[ImGuiCol_PopupBg] = ColorWithAlpha(theme.secondary, 0.95f);

    colors[ImGuiCol_Border] = ColorWithAlpha(theme.primary, 0.3f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    colors[ImGuiCol_FrameBg] = ColorWithAlpha(theme.secondary, 0.8f);
    colors[ImGuiCol_FrameBgHovered] = ColorWithAlpha(theme.secondary, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ColorWithAlpha(theme.primary, 0.3f);

    colors[ImGuiCol_TitleBg] = ColorWithAlpha(theme.secondary, 0.9f);
    colors[ImGuiCol_TitleBgActive] = theme.secondary;
    colors[ImGuiCol_TitleBgCollapsed] = ColorWithAlpha(theme.secondary, 0.7f);

    colors[ImGuiCol_MenuBarBg] = theme.secondary;

    colors[ImGuiCol_ScrollbarBg] = ColorWithAlpha(theme.background, 0.5f);
    colors[ImGuiCol_ScrollbarGrab] = ColorWithAlpha(theme.primary, 0.5f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ColorWithAlpha(theme.primary, 0.7f);
    colors[ImGuiCol_ScrollbarGrabActive] = theme.primary;

    colors[ImGuiCol_CheckMark] = theme.accent;

    colors[ImGuiCol_SliderGrab] = theme.primary;
    colors[ImGuiCol_SliderGrabActive] = theme.accent;

    colors[ImGuiCol_Button] = ColorWithAlpha(theme.primary, 0.6f);
    colors[ImGuiCol_ButtonHovered] = ColorWithAlpha(theme.primary, 0.8f);
    colors[ImGuiCol_ButtonActive] = theme.primary;

    colors[ImGuiCol_Header] = ColorWithAlpha(theme.primary, 0.5f);
    colors[ImGuiCol_HeaderHovered] = ColorWithAlpha(theme.primary, 0.7f);
    colors[ImGuiCol_HeaderActive] = ColorWithAlpha(theme.primary, 0.9f);

    colors[ImGuiCol_Separator] = ColorWithAlpha(theme.primary, 0.4f);
    colors[ImGuiCol_SeparatorHovered] = ColorWithAlpha(theme.primary, 0.6f);
    colors[ImGuiCol_SeparatorActive] = ColorWithAlpha(theme.primary, 0.8f);

    colors[ImGuiCol_ResizeGrip] = ColorWithAlpha(theme.primary, 0.4f);
    colors[ImGuiCol_ResizeGripHovered] = ColorWithAlpha(theme.primary, 0.6f);
    colors[ImGuiCol_ResizeGripActive] = ColorWithAlpha(theme.primary, 0.8f);

    colors[ImGuiCol_Tab] = ColorWithAlpha(theme.secondary, 0.8f);
    colors[ImGuiCol_TabHovered] = ColorWithAlpha(theme.primary, 0.8f);
    colors[ImGuiCol_TabActive] = ColorWithAlpha(theme.primary, 0.6f);
    colors[ImGuiCol_TabUnfocused] = ColorWithAlpha(theme.secondary, 0.6f);
    colors[ImGuiCol_TabUnfocusedActive] = ColorWithAlpha(theme.secondary, 0.8f);

    colors[ImGuiCol_DockingPreview] = ColorWithAlpha(theme.primary, 0.7f);
    colors[ImGuiCol_DockingEmptyBg] = ColorWithAlpha(theme.background, 1.0f);

    colors[ImGuiCol_PlotLines] = theme.primary;
    colors[ImGuiCol_PlotLinesHovered] = theme.accent;
    colors[ImGuiCol_PlotHistogram] = theme.primary;
    colors[ImGuiCol_PlotHistogramHovered] = theme.accent;

    colors[ImGuiCol_TableHeaderBg] = ColorWithAlpha(theme.secondary, 0.8f);
    colors[ImGuiCol_TableBorderStrong] = ColorWithAlpha(theme.primary, 0.5f);
    colors[ImGuiCol_TableBorderLight] = ColorWithAlpha(theme.primary, 0.3f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
    colors[ImGuiCol_TableRowBgAlt] = ColorWithAlpha(theme.secondary, 0.2f);

    colors[ImGuiCol_DragDropTarget] = ColorWithAlpha(theme.accent, 0.9f);

    colors[ImGuiCol_NavHighlight] = theme.accent;
    colors[ImGuiCol_NavWindowingHighlight] = ColorWithAlpha(theme.accent, 0.7f);
    colors[ImGuiCol_NavWindowingDimBg] = ColorWithAlpha(theme.background, 0.2f);

    colors[ImGuiCol_ModalWindowDimBg] = ColorWithAlpha(theme.background, 0.35f);
}