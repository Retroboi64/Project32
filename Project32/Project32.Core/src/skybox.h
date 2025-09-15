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