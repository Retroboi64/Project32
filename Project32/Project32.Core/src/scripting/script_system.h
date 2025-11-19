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
class EAUI;

class ScriptComponent {
private:
    Engine* m_engine;
    sol::state* m_lua;
    sol::table m_scriptTable;
    std::string m_scriptPath;
    bool m_isLoaded;
    std::filesystem::file_time_type m_lastWriteTime;
    int m_objectID;

public:
    ScriptComponent(sol::state* lua, const std::string& scriptPath, int objectID, Engine* engine = nullptr);
    ~ScriptComponent() = default;

    bool Load();
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
    std::unordered_map<int, std::unique_ptr<EAUI>> m_uiApps;

    bool m_hotReloadEnabled;
    float m_hotReloadCheckInterval;
    float m_timeSinceLastCheck;

    void BindMathTypes();
    void ExposeEngineSystems();
    void ExposeWindowSystem();
    void ExposeInputSystem();
    void ExposeUIAppSystem();
    void ExposeRendererSystem();
    void ExposeSceneSystem();

public:
    explicit ScriptSystem(Engine* engine);
    ~ScriptSystem();

    ScriptSystem(const ScriptSystem&) = delete;
    ScriptSystem& operator=(const ScriptSystem&) = delete;

    void Init();
    void Update(float dt);
    void FixedUpdate(float fixedDt);
    void Shutdown();

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

	// UI App Management (Using EAUI) for the future
    //int CreateUIApp(const std::string& title, int width, int height);
    //void RemoveUIApp(int appID);
    //EAUI* GetUIApp(int appID);
    //void UpdateUIApps(float dt);
};

#endif