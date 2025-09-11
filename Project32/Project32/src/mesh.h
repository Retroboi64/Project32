#pragma once

#include "common.h"
#include "textures.h"

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

namespace StaticMeshes {
    std::unique_ptr<Mesh> GetQuad();
    std::unique_ptr<Mesh> GetCube();
    std::unique_ptr<Mesh> GetCylinder(unsigned int segments, float height, float radius);
    std::unique_ptr<Mesh> GetSphere(unsigned int latitudeSegments, unsigned int longitudeSegments, float radius);
    std::unique_ptr<Mesh> GetCapsule(unsigned int segments, unsigned int rings, float height, float radius);
}