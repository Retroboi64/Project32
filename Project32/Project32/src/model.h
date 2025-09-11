#pragma once

#include "common.h"
#include "mesh.h"
#include <cfloat>

#include <tiny_obj_loader.h>

class ModelImporter {
public:
    struct LoadedModel {
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::string> materialNames;
        bool hasNormals = false;
        bool hasTexCoords = false;

        // Bounding box info
        glm::vec3 minBounds = glm::vec3(FLT_MAX);
        glm::vec3 maxBounds = glm::vec3(-FLT_MAX);
        glm::vec3 center = glm::vec3(0.0f);
        glm::vec3 size = glm::vec3(0.0f);

        void CalculateBounds();
    };

    static std::unique_ptr<LoadedModel> LoadFromFile(const std::string& filePath,
        bool generateNormals = true,
        bool flipTextureCoords = false);

private:
    static void GenerateNormals(std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    static void GenerateTangentSpace(std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    static glm::vec3 CalculateNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
};