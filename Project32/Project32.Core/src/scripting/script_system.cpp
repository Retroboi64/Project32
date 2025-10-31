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

ScriptComponent::ScriptComponent(sol::state* lua, const std::string& scriptPath, int objectID)
    : m_lua(lua)
    , m_scriptPath(scriptPath)
    , m_objectID(objectID)
    , m_isLoaded(false)
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

    if (m_isLoaded && m_scriptTable["OnDestroy"].valid()) {
        sol::protected_function onDestroy = m_scriptTable["OnDestroy"];
        onDestroy(m_scriptTable, m_objectID);
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

    m_objectScripts.clear();
    m_scripts.clear();
}

ScriptComponent* ScriptSystem::AttachScript(int objectID, const std::string& scriptPath) {
    if (m_objectScripts.find(objectID) != m_objectScripts.end()) {
        spdlog::warn("[ScriptSystem] Object {} already has a script attached, detaching old one", objectID);
        DetachScript(objectID);
    }

    auto script = std::make_unique<ScriptComponent>(&m_lua, scriptPath, objectID);
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