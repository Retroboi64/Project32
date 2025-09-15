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
#include "textures.h"

class Mesh {
private:
    GLuint _VAO = 0, _VBO = 0, _EBO = 0;
    GLuint _indexCount = 0;

    std::string _name;

public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void LoadData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void SetName(std::string name) { _name = name; };
    void Draw() const;

    bool IsValid() const;
    int GetIndexCount() const { return _indexCount; }
    std::string GetName() const { return _name; }
};

namespace StaticMeshes {
    std::unique_ptr<Mesh> GetQuad();
    std::unique_ptr<Mesh> GetCube();
    std::unique_ptr<Mesh> GetCylinder(unsigned int segments, float height, float radius);
    std::unique_ptr<Mesh> GetSphere(unsigned int latitudeSegments, unsigned int longitudeSegments, float radius);
    std::unique_ptr<Mesh> GetCapsule(unsigned int segments, unsigned int rings, float height, float radius);
}