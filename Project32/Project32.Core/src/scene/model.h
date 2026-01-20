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

#ifndef MODEL_H
#define MODEL_H

#include "../common.h"
#include "../renderer/vertex.h"
#include "../../../Project32.Renderer/src/types.h"

namespace tinyobj {
    struct attrib_t;
    struct shape_t;
}

class ModelImporter {
public:
    struct Stats {
        size_t meshCount = 0;
        size_t totalVertices = 0;
        size_t totalIndices = 0;
        size_t totalTriangles = 0;
        size_t memoryUsage = 0;
    };

    struct LoadedModel {
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::string> materialNames;

        glm::vec3 minBounds{ FLT_MAX };
        glm::vec3 maxBounds{ -FLT_MAX };
        glm::vec3 center{ 0.0f };
        glm::vec3 size{ 0.0f };

        bool hasNormals = false;
        bool hasTexCoords = false;

        void CalculateBounds();

        Mesh* GetMeshByName(const std::string& name);
        Mesh* GetMeshByIndex(size_t index);
        size_t GetMeshCount() const;
        std::vector<std::string> GetMeshNames() const;

        void Draw() const;
        void DrawInstanced(unsigned int instanceCount) const;

        Stats GetStats() const;
        void PrintInfo() const;
    };

    static std::unique_ptr<LoadedModel> LoadFromFile(
        const std::string& filePath,
        bool generateNormals = true,
        bool flipTextureCoords = true);

private:
    struct VertexKey {
        int px, py, pz;  
        int nx, ny, nz;  
        int u, v;        

        bool operator==(const VertexKey& other) const {
            return px == other.px && py == other.py && pz == other.pz &&
                nx == other.nx && ny == other.ny && nz == other.nz &&
                u == other.u && v == other.v;
        }
    };

    struct VertexKeyHash {
        size_t operator()(const VertexKey& k) const {
            size_t h1 = std::hash<int>()(k.px);
            size_t h2 = std::hash<int>()(k.py);
            size_t h3 = std::hash<int>()(k.pz);
            size_t h4 = std::hash<int>()(k.nx);
            size_t h5 = std::hash<int>()(k.ny);
            size_t h6 = std::hash<int>()(k.nz);
            size_t h7 = std::hash<int>()(k.u);
            size_t h8 = std::hash<int>()(k.v);

            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^
                (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ^ (h8 << 7);
        }
    };

    static void ProcessShape(
        const tinyobj::attrib_t& attrib,
        const tinyobj::shape_t& shape,
        LoadedModel* model,
        size_t shapeIndex,
        bool generateNormals,
        bool flipTextureCoords);

    static void GenerateNormals(
        std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    static void GenerateTangentSpace(
        std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    static glm::vec3 CalculateNormal(
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2);

    static VertexKey CreateVertexKey(const Vertex& vertex);
};

using Model = ModelImporter::LoadedModel;
using ModelStats = ModelImporter::Stats;

#endif // MODEL_H