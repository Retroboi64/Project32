#pragma once
#include "common.h"

class Shader {
private:
    int _ID = -1;
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
    void SetFloat(const std::string& name, float value);
    void SetInt(const std::string& name, int value);
    void SetBool(const std::string& name, bool value);
    bool IsValid() const;
	int GetID() const { return _ID; }
};