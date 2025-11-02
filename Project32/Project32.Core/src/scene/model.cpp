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

#include "../common.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "model.h"

void ModelImporter::LoadedModel::CalculateBounds() {
    minBounds = glm::vec3(FLT_MAX);
    maxBounds = glm::vec3(-FLT_MAX);

    center = (minBounds + maxBounds) * 0.5f;
    size = maxBounds - minBounds;
}

std::unique_ptr<ModelImporter::LoadedModel> ModelImporter::LoadFromFile(const std::string& filePath,
    bool generateNormals,
    bool flipTextureCoords) {
    auto model = std::make_unique<LoadedModel>();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str());

    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load model: " << filePath << std::endl;
        return nullptr;
    }

    std::cout << "Loading model: " << filePath << std::endl;
    std::cout << "Shapes: " << shapes.size() << ", Materials: " << materials.size() << std::endl;

    for (const auto& material : materials) {
        model->materialNames.push_back(material.name);
    }

    model->hasNormals = !attrib.normals.empty();
    model->hasTexCoords = !attrib.texcoords.empty();

    for (size_t s = 0; s < shapes.size(); s++) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::unordered_map<std::string, unsigned int> vertexMap;

        size_t indexOffset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            if (fv != 3) {
                std::cerr << "Warning: Non-triangular face found in shape " << s << std::endl;
                indexOffset += fv;
                continue;
            }

            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];

				glm::vec3 position;
				glm::vec3 normal;
				glm::vec2 uv;
				glm::vec2 texCoord;

                if (idx.vertex_index >= 0) {
                    position.x = attrib.vertices[3 * idx.vertex_index + 0];
                    position.y = attrib.vertices[3 * idx.vertex_index + 1];
                    position.z = attrib.vertices[3 * idx.vertex_index + 2];

                    model->minBounds = glm::min(model->minBounds, position);
                    model->maxBounds = glm::max(model->maxBounds, position);
                }

                if (idx.normal_index >= 0) {
                    normal.x = attrib.normals[3 * idx.normal_index + 0];
                    normal.y = attrib.normals[3 * idx.normal_index + 1];
                    normal.z = attrib.normals[3 * idx.normal_index + 2];
                }
                else {
                    normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default normal
                }

                if (idx.texcoord_index >= 0) {
                    uv.x = attrib.texcoords[2 * idx.texcoord_index + 0];
                    uv.y = flipTextureCoords ?
                        1.0f - attrib.texcoords[2 * idx.texcoord_index + 1] :
                        attrib.texcoords[2 * idx.texcoord_index + 1];
                    texCoord = uv;
                }
                else {
                    uv = glm::vec2(0.0f);
                    texCoord = glm::vec2(0.0f);
                }

                Vertex vertex{
					position,
					normal,
					texCoord
                };

                for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                    vertex.m_BoneIDs[i] = 0;
                    vertex.m_Weights[i] = 0.0f;
                }

                std::string vertexKey = std::to_string(vertex.position.x) + "_" +
                    std::to_string(vertex.position.y) + "_" +
                    std::to_string(vertex.position.z) + "_" +
                    std::to_string(vertex.normal.x) + "_" +
                    std::to_string(vertex.normal.y) + "_" +
                    std::to_string(vertex.normal.z) + "_" +
                    std::to_string(vertex.uv.x) + "_" +
                    std::to_string(vertex.uv.y);

                unsigned int vertexIndex;
                auto it = vertexMap.find(vertexKey);
                if (it != vertexMap.end()) {
                    vertexIndex = it->second;
                }
                else {
                    vertexIndex = static_cast<unsigned int>(vertices.size());
                    vertices.push_back(vertex);
                    vertexMap[vertexKey] = vertexIndex;
                }

                indices.push_back(vertexIndex);
            }

            indexOffset += fv;
        }

        if (generateNormals && !model->hasNormals) {
            GenerateNormals(vertices, indices);
        }

        GenerateTangentSpace(vertices, indices);

        auto mesh = std::make_unique<Mesh>();
        mesh->LoadData(vertices, indices);
        model->meshes.push_back(std::move(mesh));

        std::cout << "Shape " << s << ": " << vertices.size() << " vertices, "
            << indices.size() / 3 << " triangles" << std::endl;
    }

    model->center = (model->minBounds + model->maxBounds) * 0.5f;
    model->size = model->maxBounds - model->minBounds;

    std::cout << "Model bounds: min(" << model->minBounds.x << ", " << model->minBounds.y << ", " << model->minBounds.z
        << ") max(" << model->maxBounds.x << ", " << model->maxBounds.y << ", " << model->maxBounds.z << ")" << std::endl;
    std::cout << "Model center: (" << model->center.x << ", " << model->center.y << ", " << model->center.z << ")" << std::endl;
    std::cout << "Model size: (" << model->size.x << ", " << model->size.y << ", " << model->size.z << ")" << std::endl;

    return model;
}

void ModelImporter::GenerateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    for (auto& vertex : vertices) {
        vertex.normal = glm::vec3(0.0f);
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3& p1 = vertices[indices[i]].position;
        const glm::vec3& p2 = vertices[indices[i + 1]].position;
        const glm::vec3& p3 = vertices[indices[i + 2]].position;

        glm::vec3 faceNormal = CalculateNormal(p1, p2, p3);

        vertices[indices[i]].normal += faceNormal;
        vertices[indices[i + 1]].normal += faceNormal;
        vertices[indices[i + 2]].normal += faceNormal;
    }

    for (auto& vertex : vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }
}

void ModelImporter::GenerateTangentSpace(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    for (auto& vertex : vertices) {
        vertex.tangent = glm::vec3(0.0f);
        vertex.bitangent = glm::vec3(0.0f);
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        const glm::vec3& p0 = vertices[i0].position;
        const glm::vec3& p1 = vertices[i1].position;
        const glm::vec3& p2 = vertices[i2].position;

        const glm::vec2& uv0 = vertices[i0].uv;
        const glm::vec2& uv1 = vertices[i1].uv;
        const glm::vec2& uv2 = vertices[i2].uv;

        glm::vec3 edge1 = p1 - p0;
        glm::vec3 edge2 = p2 - p0;

        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        if (std::isfinite(f)) {
            glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
            glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

            vertices[i0].tangent += tangent;
            vertices[i1].tangent += tangent;
            vertices[i2].tangent += tangent;

            vertices[i0].bitangent += bitangent;
            vertices[i1].bitangent += bitangent;
            vertices[i2].bitangent += bitangent;
        }
    }

    for (auto& vertex : vertices) {
        if (glm::length(vertex.tangent) > 0.001f) {
            vertex.tangent = glm::normalize(vertex.tangent - vertex.normal * glm::dot(vertex.normal, vertex.tangent));

            if (glm::dot(glm::cross(vertex.normal, vertex.tangent), vertex.bitangent) < 0.0f) {
                vertex.tangent = -vertex.tangent;
            }

            vertex.bitangent = glm::normalize(glm::cross(vertex.normal, vertex.tangent));
        }
    }
}

glm::vec3 ModelImporter::CalculateNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    glm::vec3 u = p2 - p1;
    glm::vec3 v = p3 - p1;
    return glm::normalize(glm::cross(u, v));
}

Mesh* ModelImporter::LoadedModel::GetMeshByName(const std::string& name) {
    for (const auto& mesh : meshes) {
        if (mesh->GetName() == name) {
            return mesh.get();
        }
    }
    return nullptr;
}