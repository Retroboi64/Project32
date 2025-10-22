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
#include "BackEnd/common.h"

class ModelImporter {
private:
    static void GenerateNormals(std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    static void GenerateTangentSpace(std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    static glm::vec3 CalculateNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

public:
    struct LoadedModel {
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::string> materialNames;

        bool hasNormals = false;
        bool hasTexCoords = false;

        glm::vec3 minBounds = glm::vec3(FLT_MAX);
        glm::vec3 maxBounds = glm::vec3(-FLT_MAX);
        glm::vec3 center = glm::vec3(0.0f);
        glm::vec3 size = glm::vec3(0.0f);

        void CalculateBounds();
        std::string GetName();

		Mesh* GetMeshByName(const std::string& name);
    };

    static std::unique_ptr<LoadedModel> LoadFromFile(const std::string& filePath,
        bool generateNormals = true,
        bool flipTextureCoords = false);
};