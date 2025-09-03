#pragma once

#include "common.h"
#include <vector>

class Mesh {
private:
    GLuint _VAO = 0, _VBO = 0, _EBO = 0;
    GLuint _indexCount = 0;

public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void LoadData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Draw() const;
    bool IsValid() const;
};