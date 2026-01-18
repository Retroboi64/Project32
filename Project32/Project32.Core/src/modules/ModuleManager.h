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

#include "../common.h"

class ModuleManager {
public:
    struct LoadResult {
        bool success;
        std::string message;
        DWORD errorCode;

        operator bool() const { return success; }
    };

    struct LoadConfig {
        bool searchSystemPath = false;
        bool searchCurrentDir = true;
        DWORD flags = 0; 
    };

    static ModuleManager& Instance() {
        static ModuleManager instance;
        return instance;
    }

    LoadResult TryLoadModule(const std::string& name, const std::string& dllPath,
        const LoadConfig& config = LoadConfig{}) {
        if (IsModuleLoaded(name)) {
            return { false, "Module already loaded", 0 };
        }

        if (!std::filesystem::exists(dllPath)) {
            spdlog::warn("[Module] {} not found at: {}", name, dllPath);
            return { false, "File not found", ERROR_FILE_NOT_FOUND };
        }

        std::string fullPath = std::filesystem::absolute(dllPath).string();
        HMODULE handle = LoadModuleInternal(fullPath, config);

        if (!handle) {
            DWORD error = GetLastError();
            spdlog::error("[Module] {} failed to load. Error: 0x{:X}", name, error);
            return { false, GetErrorMessage(error), error };
        }

        modules[name] = { handle, fullPath };
        spdlog::info("[Module] {} loaded successfully from: {}", name, fullPath);
        return { true, "Success", 0 };
    }

    LoadResult LoadModuleAuto(const std::string& name,
        const std::vector<std::string>& searchPaths = {}) {
        std::vector<std::string> paths = searchPaths;

        paths.push_back(name);
        paths.push_back(name + ".dll");
        paths.push_back("modules/" + name + ".dll");
        paths.push_back("plugins/" + name + ".dll");

        for (const auto& path : paths) {
            if (std::filesystem::exists(path)) {
                return TryLoadModule(name, path);
            }
        }

        return { false, "Module not found in any search path", ERROR_FILE_NOT_FOUND };
    }

    template<typename T>
    std::optional<T> GetFunction(const std::string& moduleName, const std::string& functionName) {
        auto it = modules.find(moduleName);
        if (it == modules.end()) {
            spdlog::warn("[Module] Cannot get function '{}': module '{}' not loaded",
                functionName, moduleName);
            return std::nullopt;
        }

        T func = reinterpret_cast<T>(GetProcAddress(it->second.handle, functionName.c_str()));
        if (!func) {
            spdlog::warn("[Module] Function '{}' not found in module '{}'",
                functionName, moduleName);
            return std::nullopt;
        }

        return func;
    }

    template<typename T>
    T GetFunctionOr(const std::string& moduleName, const std::string& functionName, T fallback) {
        return GetFunction<T>(moduleName, functionName).value_or(fallback);
    }

    template<typename Ret, typename... Args>
    std::optional<Ret> CallFunction(const std::string& moduleName,
        const std::string& functionName,
        Args&&... args) {
        using FuncType = Ret(*)(Args...);
        auto func = GetFunction<FuncType>(moduleName, functionName);

        if (!func) {
            return std::nullopt;
        }

        return (*func)(std::forward<Args>(args)...);
    }

    bool IsModuleLoaded(const std::string& name) const {
        return modules.find(name) != modules.end();
    }

    std::optional<HMODULE> GetModuleHandle(const std::string& name) const {
        auto it = modules.find(name);
        if (it == modules.end()) {
            return std::nullopt;
        }
        return it->second.handle;
    }

    std::optional<std::string> GetModulePath(const std::string& name) const {
        auto it = modules.find(name);
        if (it == modules.end()) {
            return std::nullopt;
        }
        return it->second.path;
    }

    bool UnloadModule(const std::string& name) {
        auto it = modules.find(name);
        if (it == modules.end()) {
            return false;
        }

        FreeLibrary(it->second.handle);
        spdlog::info("[Module] {} unloaded.", name);
        modules.erase(it);
        return true;
    }

    void UnloadAll() {
        for (auto& [name, info] : modules) {
            FreeLibrary(info.handle);
            spdlog::info("[Module] {} unloaded.", name);
        }
        modules.clear();
    }

    std::vector<std::string> GetLoadedModules() const {
        std::vector<std::string> names;
        names.reserve(modules.size());
        for (const auto& [name, _] : modules) {
            names.push_back(name);
        }
        return names;
    }

    struct ModuleInfo {
        std::string name;
        std::string path;
        HMODULE handle;
    };

    std::vector<ModuleInfo> GetModuleInfos() const {
        std::vector<ModuleInfo> infos;
        infos.reserve(modules.size());
        for (const auto& [name, info] : modules) {
            infos.push_back({ name, info.path, info.handle });
        }
        return infos;
    }

    ~ModuleManager() {
        UnloadAll();
    }

private:
    struct ModuleData {
        HMODULE handle = nullptr;
        std::string path;
    };

    std::unordered_map<std::string, ModuleData> modules;

    HMODULE LoadModuleInternal(const std::string& fullPath, const LoadConfig& config) {
#ifdef UNICODE
        std::wstring wFullPath(fullPath.begin(), fullPath.end());
        if (config.flags != 0) {
            return LoadLibraryExW(wFullPath.c_str(), NULL, config.flags);
        }
        return LoadLibraryW(wFullPath.c_str());
#else
        if (config.flags != 0) {
            return LoadLibraryExA(fullPath.c_str(), NULL, config.flags);
        }
        return LoadLibraryA(fullPath.c_str());
#endif
    }

    std::string GetErrorMessage(DWORD error) {
        char* msgBuf = nullptr;
        size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&msgBuf, 0, NULL);

        std::string message(msgBuf, size);
        LocalFree(msgBuf);
        return message;
    }
};