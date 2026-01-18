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
#include "../../r_common.h"

class Shader {
private:
    int _ID = -1;
	std::string _name;
    std::unordered_map<std::string, int> _uniformsLocations;

    bool CheckErrors(unsigned int shader, const std::string& type);

public:
    ~Shader();
    void LoadFromString(const std::string& vertexSource, const std::string& fragmentSource);
    void Load(const std::string& vertexPath, const std::string& fragmentPath);
    void Bind() const;
    int GetUniformLocation(const std::string& name);

    void SetMat4(const std::string& name, const glm::mat4& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetFloat(const std::string& name, float value);
    void SetInt(const std::string& name, int value);
    void SetBool(const std::string& name, bool value);

    bool IsValid() const;
    int GetID() const { return _ID; }
    std::string GetName() const { return _name; };
};

class ShaderManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> _shaders;

public:
    ShaderManager() = default;
    ~ShaderManager() = default;

    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    bool LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    Shader* GetShader(const std::string& name);
	int GetShaderID(const std::string& name);
    void Clear();
};