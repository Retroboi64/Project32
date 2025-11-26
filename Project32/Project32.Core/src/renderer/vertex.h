// vertex.h - Enhanced version with tangent space support

#ifndef VERTEX_H
#define VERTEX_H

#include "../common.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 color;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    Vertex()
        : position(0.0f)
        , normal(0.0f, 1.0f, 0.0f)
        , texCoord(0.0f)
        , color(1.0f)
        , tangent(1.0f, 0.0f, 0.0f)
        , bitangent(0.0f, 0.0f, 1.0f)
    {
    }

    Vertex(const glm::vec3& pos, const glm::vec3& norm,
        const glm::vec2& tex, const glm::vec3& col)
        : position(pos)
        , normal(norm)
        , texCoord(tex)
        , color(col)
        , tangent(1.0f, 0.0f, 0.0f)
        , bitangent(0.0f, 0.0f, 1.0f)
    {
    }
};

inline void CalculateTangents(std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices) {
    for (auto& vertex : vertices) {
        vertex.tangent = glm::vec3(0.0f);
        vertex.bitangent = glm::vec3(0.0f);
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
        glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    for (auto& vertex : vertices) {
        const glm::vec3& n = vertex.normal;
        const glm::vec3& t = vertex.tangent;

        vertex.tangent = glm::normalize(t - n * glm::dot(n, t));

        if (glm::dot(glm::cross(n, t), vertex.bitangent) < 0.0f) {
            vertex.tangent *= -1.0f;
        }

        vertex.bitangent = glm::cross(n, vertex.tangent);
    }
}

#endif // VERTEX_H