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

#include "../GL_common.h"
#include "GL_shader.h"

// Shader implementation
bool Shader::CheckErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader compilation error (" << type << "): " << infoLog << std::endl;
            return false;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader linking error: " << infoLog << std::endl;
            return false;
        }
    }
    return true;
}

Shader::~Shader() {
    if (_ID != -1) {
        glDeleteProgram(_ID);
    }
}

void Shader::LoadFromString(const std::string& vertexSource, const std::string& fragmentSource) {
    const char* vShaderCode = vertexSource.c_str();
    const char* fShaderCode = fragmentSource.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    if (!CheckErrors(vertex, "VERTEX")) {
        glDeleteShader(vertex);
        return;
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    if (!CheckErrors(fragment, "FRAGMENT")) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return;
    }

    int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);
    glLinkProgram(tempID);

    if (CheckErrors(tempID, "PROGRAM")) {
        if (_ID != -1) glDeleteProgram(_ID);
        _uniformsLocations.clear();
        _ID = tempID;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::Load(const std::string& vertexPath, const std::string& fragmentPath) {
    try {
        std::string vertexSource = Util::ReadTextFromFile("res/shaders/" + vertexPath);
        std::string fragmentSource = Util::ReadTextFromFile("res/shaders/" + fragmentPath);
        LoadFromString(vertexSource, fragmentSource);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load shader: " << e.what() << std::endl;
    }
}

void Shader::Bind() const {
    if (_ID != -1) glUseProgram(_ID);
}

int Shader::GetUniformLocation(const std::string& name) {
    auto it = _uniformsLocations.find(name);
    if (it == _uniformsLocations.end()) {
        int location = glGetUniformLocation(_ID, name.c_str());
        _uniformsLocations[name] = location;
        // Don't spam warnings for optional uniforms like ambientStrength
        // if (location == -1) {
        //     std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
        // }
        return location;
    }
    return it->second;
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetFloat(const std::string& name, float value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::SetInt(const std::string& name, int value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::SetBool(const std::string& name, bool value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, (int)value);
    }
}

bool Shader::IsValid() const {
    return _ID != -1;
}

// ShaderManager implementation
bool ShaderManager::LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
    if (_shaders.find(name) != _shaders.end()) {
        std::cerr << "Shader with name '" << name << "' already exists." << std::endl;
        return false;
    }
    auto shader = std::make_unique<Shader>();
    shader->Load(vertexPath, fragmentPath);
    if (!shader->IsValid()) {
        std::cerr << "Failed to load shader: " << name << std::endl;
        return false;
    }
    _shaders[name] = std::move(shader);
    return true;
}

Shader* ShaderManager::GetShader(const std::string& name) {
    auto it = _shaders.find(name);
    if (it != _shaders.end()) {
        return it->second.get();
    }
    std::cerr << "Shader with name '" << name << "' not found." << std::endl;
    return nullptr;
}

int ShaderManager::GetShaderID(const std::string& name) {
    auto shader = GetShader(name);
    if (shader) {
        return shader->GetID();
    }
    return -1;
}

void ShaderManager::Clear() {
    _shaders.clear();
}