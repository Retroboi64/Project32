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
#include "script_system.h"
#include "../core/engine.h"
#include "../core/window.h"

ScriptComponent::ScriptComponent(sol::state* lua, const std::string& scriptPath, int objectID, Engine* engine)
    : m_lua(lua)
    , m_scriptPath(scriptPath)
    , m_objectID(objectID)
    , m_isLoaded(false)
    , m_engine(engine)
{
}

bool ScriptComponent::Load() {
    if (!m_lua) {
        spdlog::error("[ScriptComponent] Lua state is null");
        return false;
    }

    try {
        m_lua->script_file(m_scriptPath);
        m_scriptTable = (*m_lua)["Script"];

        if (!m_scriptTable.valid()) {
            spdlog::error("[ScriptComponent] Script file '{}' does not define a 'Script' table", m_scriptPath);
            return false;
        }

        if (std::filesystem::exists(m_scriptPath)) {
            m_lastWriteTime = std::filesystem::last_write_time(m_scriptPath);
        }

        if (m_scriptTable["Init"].valid()) {
            sol::protected_function init = m_scriptTable["Init"];
            auto result = init(m_scriptTable, m_objectID);
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("[ScriptComponent] Error in Init: {}", err.what());
                return false;
            }
        }

        m_isLoaded = true;
        spdlog::info("[ScriptComponent] Successfully loaded script: {}", m_scriptPath);
        return true;
    }
    catch (const sol::error& e) {
        spdlog::error("[ScriptComponent] Failed to load script '{}': {}", m_scriptPath, e.what());
        m_isLoaded = false;
        return false;
    }
}

bool ScriptComponent::Reload() {
    spdlog::info("[ScriptComponent] Reloading script: {}", m_scriptPath);

    if (m_isLoaded && m_scriptTable.valid()) {
        if (m_scriptTable["OnDestroy"].valid()) {
            sol::protected_function onDestroy = m_scriptTable["OnDestroy"];
            auto result = onDestroy(m_scriptTable, m_objectID);
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("[ScriptComponent] Error in OnDestroy during reload: {}", err.what());
            }
        }

        m_scriptTable = sol::lua_nil;
    }

    m_isLoaded = false;
    return Load();
}

void ScriptComponent::Update(float dt) {
    if (!m_isLoaded || !m_scriptTable.valid()) return;

    if (m_scriptTable["Update"].valid()) {
        sol::protected_function update = m_scriptTable["Update"];
        auto result = update(m_scriptTable, m_objectID, dt);

        if (!result.valid()) {
            sol::error err = result;
            spdlog::error("[ScriptComponent] Error in Update ({}): {}", m_scriptPath, err.what());
        }
    }
}

void ScriptComponent::FixedUpdate(float fixedDt) {
    if (!m_isLoaded || !m_scriptTable.valid()) return;

    if (m_scriptTable["FixedUpdate"].valid()) {
        sol::protected_function fixedUpdate = m_scriptTable["FixedUpdate"];
        auto result = fixedUpdate(m_scriptTable, m_objectID, fixedDt);

        if (!result.valid()) {
            sol::error err = result;
            spdlog::error("[ScriptComponent] Error in FixedUpdate ({}): {}", m_scriptPath, err.what());
        }
    }
}

void ScriptComponent::OnEvent(const std::string& eventName) {
    if (!m_isLoaded || !m_scriptTable.valid()) return;

    if (m_scriptTable[eventName].valid()) {
        sol::protected_function eventFunc = m_scriptTable[eventName];
        auto result = eventFunc(m_scriptTable, m_objectID);

        if (!result.valid()) {
            sol::error err = result;
            spdlog::error("[ScriptComponent] Error in {} ({}): {}", eventName, m_scriptPath, err.what());
        }
    }
}

bool ScriptComponent::HasChanged() const {
    if (!std::filesystem::exists(m_scriptPath)) {
        return false;
    }

    auto currentWriteTime = std::filesystem::last_write_time(m_scriptPath);
    return currentWriteTime != m_lastWriteTime;
}

ScriptSystem::ScriptSystem(Engine* engine)
    : m_engine(engine)
    , m_hotReloadEnabled(true)
    , m_hotReloadCheckInterval(1.0f)
    , m_timeSinceLastCheck(0.0f)
    , m_nextUIID(1)
{
}

ScriptSystem::~ScriptSystem() {
    Shutdown();
}

void ScriptSystem::Init() {
    spdlog::info("[ScriptSystem] Initializing Lua scripting system");

    m_lua.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::math,
        sol::lib::string,
        sol::lib::table
    );

    BindMathTypes();
    ExposeEngineSystems();
    ExposeWindowSystem();
    ExposeInputSystem();
    ExposeUISystem();
    ExposeRendererSystem();
    ExposeSceneSystem();

    spdlog::info("[ScriptSystem] Lua scripting system initialized successfully");
}

void ScriptSystem::BindMathTypes() {
    m_lua.new_usertype<glm::vec2>("Vec2",
        sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
        "x", &glm::vec2::x,
        "y", &glm::vec2::y,
        sol::meta_function::addition, [](const glm::vec2& a, const glm::vec2& b) { return a + b; },
        sol::meta_function::subtraction, [](const glm::vec2& a, const glm::vec2& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const glm::vec2& v, float s) { return v * s; },
            [](float s, const glm::vec2& v) { return v * s; }
        ),
        "length", [](const glm::vec2& v) { return glm::length(v); },
        "normalize", [](const glm::vec2& v) { return glm::normalize(v); },
        "dot", [](const glm::vec2& a, const glm::vec2& b) { return glm::dot(a, b); }
    );

    m_lua.new_usertype<glm::vec3>("Vec3",
        sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z,
        sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
        sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const glm::vec3& v, float s) { return v * s; },
            [](float s, const glm::vec3& v) { return v * s; }
        ),
        "length", [](const glm::vec3& v) { return glm::length(v); },
        "normalize", [](const glm::vec3& v) { return glm::normalize(v); },
        "dot", [](const glm::vec3& a, const glm::vec3& b) { return glm::dot(a, b); },
        "cross", [](const glm::vec3& a, const glm::vec3& b) { return glm::cross(a, b); }
    );

    m_lua.new_usertype<glm::vec4>("Vec4",
        sol::constructors<glm::vec4(), glm::vec4(float, float, float, float)>(),
        "x", &glm::vec4::x,
        "y", &glm::vec4::y,
        "z", &glm::vec4::z,
        "w", &glm::vec4::w
    );

    m_lua["Math"] = m_lua.create_table_with(
        "PI", glm::pi<float>(),
        "Lerp", [](float a, float b, float t) { return glm::mix(a, b, t); },
        "Clamp", [](float v, float min, float max) { return glm::clamp(v, min, max); },
        "Abs", [](float v) { return glm::abs(v); },
        "Sin", [](float v) { return glm::sin(v); },
        "Cos", [](float v) { return glm::cos(v); },
        "Tan", [](float v) { return glm::tan(v); },
        "Sqrt", [](float v) { return glm::sqrt(v); },
        "Radians", [](float degrees) { return glm::radians(degrees); },
        "Degrees", [](float radians) { return glm::degrees(radians); },
        "Min", sol::overload(
            [](float a, float b) { return glm::min(a, b); },
            [](int a, int b) { return glm::min(a, b); }
        ),
        "Max", sol::overload(
            [](float a, float b) { return glm::max(a, b); },
            [](int a, int b) { return glm::max(a, b); }
        )
    );
}

void ScriptSystem::ExposeEngineSystems() {
    m_lua["Log"] = [](const std::string& msg) {
        spdlog::info("[Lua] {}", msg);
        };

    m_lua["LogWarn"] = [](const std::string& msg) {
        spdlog::warn("[Lua] {}", msg);
        };

    m_lua["LogError"] = [](const std::string& msg) {
        spdlog::error("[Lua] {}", msg);
        };

    m_lua["Time"] = m_lua.create_table_with(
        "GetDeltaTime", [this]() { return m_engine ? m_engine->GetDeltaTime() : 0.0f; },
        "GetFrameCount", [this]() { return m_engine ? m_engine->GetFrameCount() : 0; }
    );

    m_lua["GetScript"] = [this](int objectID) -> sol::optional<sol::table> {
        auto* script = GetScript(objectID);
        if (script && script->IsLoaded()) {
            return script->GetScriptTable();
        }
        return sol::nullopt;
        };

    m_lua["Random"] = m_lua.create_table_with(
        "Range", [](float min, float max) {
            return min + static_cast<float>(rand()) /
                (static_cast<float>(RAND_MAX / (max - min)));
        },
        "Value", []() {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        }
    );

    m_lua["Engine"] = m_lua.create_table_with(
        "GetVersion", [this]() { return P32::Engine::GetVersionString(); },
        "IsPaused", [this]() { return m_engine ? m_engine->IsPaused() : false; },
        "SetPaused", [this](bool paused) {
            if (m_engine) m_engine->SetPaused(paused);
        },
        "GetID", [this]() { return m_engine ? m_engine->GetID() : -1; },
        "Quit", [this]() {
            if (m_engine) m_engine->Shutdown();
        }
    );
}

void ScriptSystem::ExposeWindowSystem() {
    m_lua["Window"] = m_lua.create_table_with(
        "Create", [this](int width, int height, const std::string& title) -> int {
            if (!m_engine) return -1;
            return m_engine->GetWindowManager()->AddWindow(width, height, title);
        },

        "CreateWindow", [this](int width, int height, const std::string& title) -> int {
            if (!m_engine) return -1;
            return m_engine->GetWindowManager()->AddWindow(width, height, title);
        },

        "Remove", [this](int windowID) {
            if (!m_engine) return;
            m_engine->GetWindowManager()->RemoveWindow(windowID);
        },

        "GetMainWindowID", [this]() -> int {
            return m_engine ? m_engine->GetMainWindowID() : -1;
        },

        "SetMainWindow", [this](int windowID) {
            if (m_engine) m_engine->SetMainWindow(windowID);
        },

        "GetWindowCount", [this]() -> int {
            return m_engine ? m_engine->GetWindowManager()->Count() : 0;
        },

        "GetWindowByID", [this](int windowID) -> Window* {
            return m_engine ? m_engine->GetWindowManager()->GetWindowByID(windowID) : nullptr;
        },

        "GetWindowTitle", [this](int windowID) -> std::string {
            if (!m_engine) return "";
            return m_engine->GetWindowManager()->GetWindowTitle(windowID);
        },

        "SetWindowTitle", [this](int windowID, const std::string& title) {
            if (!m_engine) return;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (window) window->SetTitle(title);
        },

        "GetWindowSize", [this](int windowID) -> sol::optional<std::tuple<int, int>> {
            if (!m_engine) return sol::nullopt;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (!window) return sol::nullopt;
            return std::make_tuple(window->GetWidth(), window->GetHeight());
        },

        "SetWindowSize", [this](int windowID, int width, int height) {
            if (!m_engine) return;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (window) window->SetSize(width, height);
        },

        "GetWindowPosition", [this](int windowID) -> sol::optional<std::tuple<int, int>> {
            if (!m_engine) return sol::nullopt;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (!window) return sol::nullopt;
            glm::ivec2 pos = window->GetPosition();
            return std::make_tuple(pos.x, pos.y);
        },

        "SetWindowPosition", [this](int windowID, int x, int y) {
            if (!m_engine) return;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (window) window->SetPosition(x, y);
        },

        "IsWindowOpen", [this](int windowID) -> bool {
            if (!m_engine) return false;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            return window ? window->IsOpen() : false;
        },

        "SetVSync", [this](int windowID, bool enabled) {
            if (!m_engine) return;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (window) window->SetVSync(enabled);
        },

        "ToggleFullscreen", [this](int windowID) {
            if (!m_engine) return;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (window) window->ToggleFullscreen();
        }
    );
}

void ScriptSystem::ExposeInputSystem() {
    m_lua["Input"] = m_lua.create_table_with(
        "LockMouse", [this](sol::variadic_args args) {
            if (!m_engine) return;

            int windowID = 0;
            bool lock = false;

            if (args.size() == 1) {
                lock = args.get<bool>(0);
                windowID = m_engine->GetMainWindowID();
            }
            else if (args.size() >= 2) {
                windowID = args.get<int>(0);
                lock = args.get<bool>(1);
            }
            else {
                return;
            }

            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (window) window->MouseLocked(lock);
        },

        "GetMousePosition", [this](sol::variadic_args args) -> std::tuple<float, float> {
            if (!m_engine) return std::make_tuple(0.0f, 0.0f);

            int windowID = 0;
            if (args.size() >= 1) {
                if (args[0].is<int>()) {
                    windowID = args.get<int>(0);
                }
                else {
                    windowID = m_engine->GetMainWindowID();
                }
            }
            else {
                windowID = m_engine->GetMainWindowID();
            }

            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (!window) return std::make_tuple(0.0f, 0.0f);
            glm::vec2 pos = window->GetMousePosition();
            return std::make_tuple(pos.x, pos.y);
        },

        "SetMousePosition", [this](sol::variadic_args args) {
            if (!m_engine) return;

            int windowID = 0;
            float x = 0.0f, y = 0.0f;

            if (args.size() == 2) {
                x = args.get<float>(0);
                y = args.get<float>(1);
                windowID = m_engine->GetMainWindowID();
            }
            else if (args.size() >= 3) {
                windowID = args.get<int>(0);
                x = args.get<float>(1);
                y = args.get<float>(2);
            }
            else {
                return;
            }

            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (window) window->SetMousePosition(x, y);
        },

        "GetKeyDown", [this](sol::variadic_args args) -> bool {
            if (!m_engine) return false;

            int windowID = 0;
            std::string key;

            if (args.size() == 1) {
                key = args.get<std::string>(0);
                windowID = m_engine->GetMainWindowID();
            }
            else if (args.size() >= 2) {
                windowID = args.get<int>(0);
                key = args.get<std::string>(1);
            }
            else {
                return false;
            }

            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            if (!window) return false;

            if (key.length() == 1) {
                char c = key[0];
                int keyCode = static_cast<int>(toupper(c));
                return window->IsKeyPressed(keyCode);
            }

            if (key == "Space") return window->IsKeyPressed(32);
            if (key == "LeftShift") return window->IsKeyPressed(340);
            if (key == "LeftControl") return window->IsKeyPressed(341);
            if (key == "Escape") return window->IsKeyPressed(256);
            if (key == "Enter") return window->IsKeyPressed(257);
            if (key == "Tab") return window->IsKeyPressed(258);

            spdlog::warn("[ScriptSystem] GetKeyDown: Invalid key '{}'", key);
            return false;
        },

        "IsMouseButtonPressed", [this](int windowID, int button) -> bool {
            if (!m_engine) return false;
            Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
            return window ? window->IsMouseButtonPressed(button) : false;
        }
    );
}

void ScriptSystem::ExposeUISystem() {
    m_lua.new_usertype<UIElement>("UIElement",
        "id", sol::readonly(&UIElement::id),
        "windowID", &UIElement::windowID,
        "type", &UIElement::type,
        "label", &UIElement::label,
        "visible", &UIElement::visible,
        "x", &UIElement::x,
        "y", &UIElement::y,
        "width", &UIElement::width,
        "height", &UIElement::height,
        "value", &UIElement::value,
        "text", &UIElement::text,
        "minValue", &UIElement::minValue,
        "maxValue", &UIElement::maxValue,
        "items", &UIElement::items,
        "selectedIndex", &UIElement::selectedIndex,
        "callback", &UIElement::callback,
        "children", &UIElement::children
    );

    m_lua["UI"] = m_lua.create_table_with(
        "CreateWindow", [this](sol::variadic_args args) -> int {
            std::string title = "UI Window";
            int windowID = m_engine ? m_engine->GetMainWindowID() : 0;

            if (args.size() >= 1) title = args.get<std::string>(0);
            if (args.size() >= 2) windowID = args.get<int>(1);

            return CreateUIElement(windowID, "Window", title);
        },

        "CreateButton", [this](int windowID, const std::string& label) -> int {
            return CreateUIElement(windowID, "Button", label);
        },

        "CreateText", [this](int windowID, const std::string& text) -> int {
            int id = CreateUIElement(windowID, "Text", "");
            auto* elem = GetUIElement(id);
            if (elem) elem->text = text;
            return id;
        },

        "CreateSlider", [this](int windowID, const std::string& label, float min, float max, float value) -> int {
            int id = CreateUIElement(windowID, "Slider", label);
            auto* elem = GetUIElement(id);
            if (elem) {
                elem->minValue = min;
                elem->maxValue = max;
                elem->value = value;
            }
            return id;
        },

        "CreateInputText", [this](int windowID, const std::string& label) -> int {
            return CreateUIElement(windowID, "InputText", label);
        },

        "CreateCheckbox", [this](int windowID, const std::string& label, bool checked) -> int {
            int id = CreateUIElement(windowID, "Checkbox", label);
            auto* elem = GetUIElement(id);
            if (elem) elem->value = checked ? 1.0f : 0.0f;
            return id;
        },

        "CreateComboBox", [this](int windowID, const std::string& label, const std::vector<std::string>& items) -> int {
            int id = CreateUIElement(windowID, "ComboBox", label);
            auto* elem = GetUIElement(id);
            if (elem) elem->items = items;
            return id;
        },

        "CreateColorPicker", [this](int windowID, const std::string& label) -> int {
            return CreateUIElement(windowID, "ColorPicker", label);
        },

        "CreateSeparator", [this](int windowID) -> int {
            return CreateUIElement(windowID, "Separator", "");
        },

        "Remove", [this](int elementID) {
            RemoveUIElement(elementID);
        },

        "SetVisible", [this](int elementID, bool visible) {
            auto* elem = GetUIElement(elementID);
            if (elem) elem->visible = visible;
        },

        "SetPosition", [this](int elementID, float x, float y) {
            auto* elem = GetUIElement(elementID);
            if (elem) {
                elem->x = x;
                elem->y = y;
            }
        },

        "SetSize", [this](int elementID, float width, float height) {
            auto* elem = GetUIElement(elementID);
            if (elem) {
                elem->width = width;
                elem->height = height;
            }
        },

        "SetValue", [this](int elementID, float value) {
            auto* elem = GetUIElement(elementID);
            if (elem) elem->value = value;
        },

        "GetValue", [this](int elementID) -> float {
            auto* elem = GetUIElement(elementID);
            return elem ? elem->value : 0.0f;
        },

        "SetText", [this](int elementID, const std::string& text) {
            auto* elem = GetUIElement(elementID);
            if (elem) elem->text = text;
        },

        "GetText", [this](int elementID) -> std::string {
            auto* elem = GetUIElement(elementID);
            return elem ? elem->text : "";
        },

        "SetCallback", [this](int elementID, sol::function callback) {
            auto* elem = GetUIElement(elementID);
            if (elem) elem->callback = callback;
        },

        "AddChild", [this](int parentID, int childID) {
            auto* parent = GetUIElement(parentID);
            if (parent) parent->children.push_back(childID);
        },

        "ClearAll", [this](int windowID) {
            ClearUIElements(windowID);
        }
    );
}

void ScriptSystem::ExposeRendererSystem() {
    m_lua["Renderer"] = m_lua.create_table_with(
        "LoadSkybox", [this](const std::vector<std::string>& faces) {
            if (!m_engine) return;
        },
        "ClearColor", [this](float r, float g, float b, float a) {
        },
        "SetViewport", [this](glm::vec2, int width, int height) {
        },
        "DrawScene", [this]() {
        }
    );
}

void ScriptSystem::ExposeSceneSystem() {
    m_lua["Scene"] = m_lua.create_table_with(
        "GetObjectCount", [this]() -> int {
            return static_cast<int>(m_objectScripts.size());
        },

        "CreateObject", [this](const std::string& scriptPath) -> int {
            static int nextObjectID = 1000;
            int objectID = nextObjectID++;
            auto* script = AttachScript(objectID, scriptPath);
            return script ? objectID : -1;
        },

        "DestroyObject", [this](int objectID) {
            DetachScript(objectID);
        },

        "HasObject", [this](int objectID) -> bool {
            return GetScript(objectID) != nullptr;
        }
    );
}

int ScriptSystem::CreateUIElement(int windowID, const std::string& type, const std::string& label) {
    UIElement element;
    element.id = m_nextUIID++;
    element.windowID = windowID;
    element.type = type;
    element.label = label;
    element.visible = true;

    m_uiElements[element.id] = element;
    spdlog::info("[ScriptSystem] Created UI element '{}' with ID {} for window {}", type, element.id, windowID);

    return element.id;
}

void ScriptSystem::RemoveUIElement(int elementID) {
    auto it = m_uiElements.find(elementID);
    if (it != m_uiElements.end()) {
        m_uiElements.erase(it);
        spdlog::info("[ScriptSystem] Removed UI element with ID {}", elementID);
    }
}

UIElement* ScriptSystem::GetUIElement(int elementID) {
    auto it = m_uiElements.find(elementID);
    return (it != m_uiElements.end()) ? &it->second : nullptr;
}

void ScriptSystem::ClearUIElements(int windowID) {
    for (auto it = m_uiElements.begin(); it != m_uiElements.end();) {
        if (it->second.windowID == windowID) {
            it = m_uiElements.erase(it);
        }
        else {
            ++it;
        }
    }
    spdlog::info("[ScriptSystem] Cleared all UI elements for window {}", windowID);
}

void ScriptSystem::RenderUI(int windowID) {
    if (!m_engine) return;

    Window* window = m_engine->GetWindowManager()->GetWindowByID(windowID);
    if (!window || !window->GetUI() || !window->GetUI()->IsInitialized()) return;

    for (auto& [id, elem] : m_uiElements) {
        if (elem.windowID != windowID || !elem.visible) continue;

        if (elem.type == "Window") {
            ImGui::SetNextWindowPos(ImVec2(elem.x, elem.y), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(elem.width > 0 ? elem.width : 300, elem.height > 0 ? elem.height : 200), ImGuiCond_FirstUseEver);

            if (ImGui::Begin(elem.label.c_str())) {
                for (int childID : elem.children) {
                    RenderUIElement(childID);
                }
            }
            ImGui::End();
        }
        else {
            RenderUIElement(id);
        }
    }
}

void ScriptSystem::RenderUIElement(int elementID) {
    auto* elem = GetUIElement(elementID);
    if (!elem || !elem->visible) return;

    if (elem->type == "Button") {
        if (ImGui::Button(elem->label.c_str())) {
            if (elem->callback.valid()) {
                elem->callback(elem->id);
            }
        }
    }
    else if (elem->type == "Text") {
        ImGui::Text("%s", elem->text.c_str());
    }
    else if (elem->type == "Slider") {
        if (ImGui::SliderFloat(elem->label.c_str(), &elem->value, elem->minValue, elem->maxValue)) {
            if (elem->callback.valid()) {
                elem->callback(elem->id, elem->value);
            }
        }
    }
    else if (elem->type == "InputText") {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "%s", elem->text.c_str());

        if (ImGui::InputText(elem->label.c_str(), buffer, sizeof(buffer))) {
            elem->text = std::string(buffer);
            if (elem->callback.valid()) {
                elem->callback(elem->id, elem->text);
            }
        }
    }
    else if (elem->type == "Checkbox") {
        bool checked = elem->value > 0.5f;
        if (ImGui::Checkbox(elem->label.c_str(), &checked)) {
            elem->value = checked ? 1.0f : 0.0f;
            if (elem->callback.valid()) {
                elem->callback(elem->id, checked);
            }
        }
    }
    else if (elem->type == "ComboBox") {
        if (!elem->items.empty()) {
            const char* preview = elem->selectedIndex >= 0 && elem->selectedIndex < static_cast<int>(elem->items.size())
                ? elem->items[elem->selectedIndex].c_str()
                : "Select...";

            if (ImGui::BeginCombo(elem->label.c_str(), preview)) {
                for (int i = 0; i < static_cast<int>(elem->items.size()); i++) {
                    bool selected = (elem->selectedIndex == i);
                    if (ImGui::Selectable(elem->items[i].c_str(), selected)) {
                        elem->selectedIndex = i;
                        if (elem->callback.valid()) {
                            elem->callback(elem->id, i, elem->items[i]);
                        }
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
    }
    else if (elem->type == "ColorPicker") {
        float color[4] = { elem->colorR, elem->colorG, elem->colorB, elem->colorA };
        if (ImGui::ColorEdit4(elem->label.c_str(), color)) {
            elem->colorR = color[0];
            elem->colorG = color[1];
            elem->colorB = color[2];
            elem->colorA = color[3];
            if (elem->callback.valid()) {
                elem->callback(elem->id, color[0], color[1], color[2], color[3]);
            }
        }
    }
    else if (elem->type == "Separator") {
        ImGui::Separator();
    }
    else if (elem->type == "SameLine") {
        ImGui::SameLine();
    }
    else if (elem->type == "Spacing") {
        ImGui::Spacing();
    }
}

void ScriptSystem::Update(float dt) {
    if (m_hotReloadEnabled) {
        m_timeSinceLastCheck += dt;
        if (m_timeSinceLastCheck >= m_hotReloadCheckInterval) {
            CheckForScriptChanges();
            m_timeSinceLastCheck = 0.0f;
        }
    }

    for (auto& [objectID, script] : m_objectScripts) {
        if (script && script->IsLoaded()) {
            script->Update(dt);
        }
    }
}

void ScriptSystem::FixedUpdate(float fixedDt) {
    for (auto& [objectID, script] : m_objectScripts) {
        if (script && script->IsLoaded()) {
            script->FixedUpdate(fixedDt);
        }
    }
}

void ScriptSystem::Shutdown() {
    spdlog::info("[ScriptSystem] Shutting down");

    for (auto& [objectID, script] : m_objectScripts) {
        if (script && script->IsLoaded()) {
            script->OnEvent("OnDestroy");
        }
    }

    m_uiElements.clear();
    m_objectScripts.clear();
    m_scripts.clear();
}

void ScriptSystem::FindAndLoadScriptsInDirectory(const std::string& directoryPath) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".lua") {
            if (entry.path().filename() == "init.lua") {
                spdlog::info("[ScriptSystem] Found main file: {}", entry.path().string());
            }
            spdlog::info("[ScriptSystem] Found script file: {}", entry.path().string());
            AttachScript(-1, entry.path().string());
        }
    }
}

ScriptComponent* ScriptSystem::AttachScript(int objectID, const std::string& scriptPath) {
    if (m_objectScripts.find(objectID) != m_objectScripts.end()) {
        spdlog::warn("[ScriptSystem] Object {} already has a script attached, detaching old one", objectID);
        DetachScript(objectID);
    }

    auto script = std::make_unique<ScriptComponent>(&m_lua, scriptPath, objectID, m_engine);
    ScriptComponent* scriptPtr = script.get();

    if (script->Load()) {
        m_objectScripts[objectID] = scriptPtr;
        m_scripts.push_back(std::move(script));
        spdlog::info("[ScriptSystem] Attached script '{}' to object {}", scriptPath, objectID);
        return scriptPtr;
    }

    spdlog::error("[ScriptSystem] Failed to attach script '{}'", scriptPath);
    return nullptr;
}

void ScriptSystem::DetachScript(int objectID) {
    auto it = m_objectScripts.find(objectID);
    if (it != m_objectScripts.end()) {
        ScriptComponent* script = it->second;

        if (script && script->IsLoaded()) {
            script->OnEvent("OnDestroy");
        }

        m_scripts.erase(
            std::remove_if(m_scripts.begin(), m_scripts.end(),
                [script](const std::unique_ptr<ScriptComponent>& s) {
                    return s.get() == script;
                }),
            m_scripts.end()
        );

        m_objectScripts.erase(it);
        spdlog::info("[ScriptSystem] Detached script from object {}", objectID);
    }
}

ScriptComponent* ScriptSystem::GetScript(int objectID) {
    auto it = m_objectScripts.find(objectID);
    return (it != m_objectScripts.end()) ? it->second : nullptr;
}

void ScriptSystem::TriggerEvent(int objectID, const std::string& eventName) {
    auto script = GetScript(objectID);
    if (script) {
        script->OnEvent(eventName);
    }
}

template<typename... Args>
void ScriptSystem::TriggerEvent(int objectID, const std::string& eventName, Args&&... args) {
    auto script = GetScript(objectID);
    if (script) {
        script->OnEvent(eventName);
    }
}

void ScriptSystem::CheckForScriptChanges() {
    for (auto& [objectID, script] : m_objectScripts) {
        if (script && script->IsLoaded() && script->HasChanged()) {
            spdlog::info("[ScriptSystem] Hot reloading: {}", script->GetScriptPath());
            script->Reload();
        }
    }
}

void ScriptSystem::ReloadAllScripts() {
    spdlog::info("[ScriptSystem] Reloading all scripts");
    for (auto& [objectID, script] : m_objectScripts) {
        if (script && script->IsLoaded()) {
            script->Reload();
        }
    }
}

void ScriptSystem::ExecuteLua(const std::string& code) {
    try {
        m_lua.script(code);
    }
    catch (const sol::error& e) {
        spdlog::error("[ScriptSystem] Lua execution error: {}", e.what());
    }
}

void ScriptSystem::RegisterFunction(const std::string& name) {
    try {
        m_lua[name] = [name]() {
            spdlog::info("[Lua] Called registered function: {}", name);
            };
    }
    catch (const sol::error& e) {
        spdlog::error("[ScriptSystem] Failed to register function '{}': {}", name, e.what());
    }
}

void ScriptSystem::ExecuteLuaFile(const std::string& filePath) {
    try {
        m_lua.script_file(filePath);
    }
    catch (const sol::error& e) {
        spdlog::error("[ScriptSystem] Lua file execution error ({}): {}", filePath, e.what());
    }
}