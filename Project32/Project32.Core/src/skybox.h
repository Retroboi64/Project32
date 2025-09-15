#pragma once
#include "common.h"

class Skybox {
private:
    unsigned int _VAO = 0;
    unsigned int _VBO = 0;
    unsigned int _textureID = 0;

    void CreateCube();
    unsigned int LoadCubemap(const std::vector<std::string>& faces);
public:
    ~Skybox();
    void Load(const std::vector<std::string>& faces);
    void Draw() const;
    unsigned int GetTextureID() const { return _textureID; }
};