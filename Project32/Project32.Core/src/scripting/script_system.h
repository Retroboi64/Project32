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

#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H

#include "../common.h"
#include <sol/sol.hpp>

class Engine;
class Window;

struct UIElement {
    int id;
    int windowID;
    std::string type;
    std::string label;
    bool visible;

    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    float value = 0.0f;
    std::string text;

    float minValue = 0.0f;
    float maxValue = 1.0f;

    std::vector<std::string> items;
    int selectedIndex = -1;

    float colorR = 1.0f;
    float colorG = 1.0f;
    float colorB = 1.0f;
    float colorA = 1.0f;

    sol::function callback;
    std::vector<int> children;
};

class ScriptComponent {
private:
    Engine* m_engine;
    sol::state* m_lua;
    sol::table m_scriptTable;
    std::string m_scriptPath;
    bool m_isLoaded;
	bool m_hasChanged;
    std::filesystem::file_time_type m_lastWriteTime;
    int m_objectID;

public:
    ScriptComponent(sol::state* lua, const std::string& scriptPath, int objectID, Engine* engine = nullptr);
    ~ScriptComponent() = default;

    bool Load();
	bool Unload();
    bool Reload();
    void Update(float dt);
    void FixedUpdate(float fixedDt);

    void OnEvent(const std::string& eventName);
    template<typename... Args>
    void OnEvent(const std::string& eventName, Args&&... args);

    sol::table GetScriptTable() { return m_scriptTable; }

    bool IsLoaded() const { return m_isLoaded; }
    const std::string& GetScriptPath() const { return m_scriptPath; }
    int GetObjectID() const { return m_objectID; }
    bool HasChanged() const;

    template<typename... Args>
    void CallFunction(const std::string& funcName, Args&&... args);

    template<typename T>
    T GetVariable(const std::string& varName);

    template<typename T>
    void SetVariable(const std::string& varName, const T& value);
};

class ScriptSystem {
private:
    sol::state m_lua;
    Engine* m_engine;
    std::vector<std::unique_ptr<ScriptComponent>> m_scripts;
    std::unordered_map<int, ScriptComponent*> m_objectScripts;
    std::unordered_map<int, UIElement> m_uiElements;
    int m_nextUIID;

    bool m_hotReloadEnabled;
    float m_hotReloadCheckInterval;
    float m_timeSinceLastCheck;

    void BindMathTypes();
    void ExposeEngineSystems();
    void ExposeWindowSystem();
    void ExposeInputSystem();
    void ExposeUISystem();
    void ExposeRendererSystem();
    void ExposeSceneSystem();

    int CreateUIElement(int windowID, const std::string& type, const std::string& label);
    void RemoveUIElement(int elementID);
    UIElement* GetUIElement(int elementID);
    void ClearUIElements(int windowID);
    void RenderUIElement(int elementID);

public:
    explicit ScriptSystem(Engine* engine);
    ~ScriptSystem();

    ScriptSystem(const ScriptSystem&) = delete;
    ScriptSystem& operator=(const ScriptSystem&) = delete;

    void Init();
    void Update(float dt);
    void FixedUpdate(float fixedDt);
    void Reset();
    void Shutdown();

    void RenderUI(int windowID);

    void FindAndLoadScriptsInDirectory(const std::string& directoryPath);

    ScriptComponent* AttachScript(int objectID, const std::string& scriptPath);
    void DetachScript(int objectID);
    ScriptComponent* GetScript(int objectID);

    void TriggerEvent(int objectID, const std::string& eventName);

    std::vector<int> GetAllScriptedObjectIDs() const {
        std::vector<int> ids;
        ids.reserve(m_objectScripts.size());
        for (const auto& [objectID, script] : m_objectScripts) {
            ids.push_back(objectID);
        }
        return ids;
    }

    template<typename... Args>
    void TriggerEvent(int objectID, const std::string& eventName, Args&&... args);

    void SetHotReloadEnabled(bool enabled) { m_hotReloadEnabled = enabled; }
    bool IsHotReloadEnabled() const { return m_hotReloadEnabled; }
    void CheckForScriptChanges();
    void ReloadAllScripts();

    void ExecuteLua(const std::string& code);
    sol::state& GetLuaState() { return m_lua; }

    void RegisterFunction(const std::string& name);
    void ExecuteLuaFile(const std::string& name);
};

#endif