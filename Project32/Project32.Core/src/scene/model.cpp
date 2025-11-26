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
    if (meshes.empty()) {
        minBounds = glm::vec3(0.0f);
        maxBounds = glm::vec3(0.0f);
        center = glm::vec3(0.0f);
        size = glm::vec3(0.0f);
        return;
    }

    minBounds = glm::vec3(FLT_MAX);
    maxBounds = glm::vec3(-FLT_MAX);

    for (const auto& mesh : meshes) {
        const auto& bounds = mesh->GetBounds();
        minBounds = glm::min(minBounds, bounds.min);
        maxBounds = glm::max(maxBounds, bounds.max);
    }

    center = (minBounds + maxBounds) * 0.5f;
    size = maxBounds - minBounds;
}

std::unique_ptr<ModelImporter::LoadedModel> ModelImporter::LoadFromFile(
    const std::string& filePath,
    bool generateNormals,
    bool flipTextureCoords) {

    auto model = std::make_unique<LoadedModel>();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    spdlog::info("[ModelImporter] Loading model from: {}", filePath);

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str());

    if (!warn.empty()) {
        spdlog::warn("[ModelImporter] {}", warn);
    }

    if (!err.empty()) {
        spdlog::error("[ModelImporter] {}", err);
    }

    if (!ret) {
        spdlog::error("[ModelImporter] Failed to load model: {}", filePath);
        return nullptr;
    }

    spdlog::info("[ModelImporter] Loaded: {} shapes, {} materials",
        shapes.size(), materials.size());

    for (const auto& material : materials) {
        model->materialNames.push_back(material.name);
    }

    model->hasNormals = !attrib.normals.empty();
    model->hasTexCoords = !attrib.texcoords.empty();

    spdlog::debug("[ModelImporter] Model has normals: {}, texcoords: {}",
        model->hasNormals, model->hasTexCoords);

    for (size_t s = 0; s < shapes.size(); s++) {
        ProcessShape(attrib, shapes[s], model.get(), s,
            generateNormals, flipTextureCoords);
    }

    model->CalculateBounds();

    spdlog::info("[ModelImporter] Model loaded successfully");
    spdlog::info("[ModelImporter]   Bounds: min({:.2f}, {:.2f}, {:.2f}) max({:.2f}, {:.2f}, {:.2f})",
        model->minBounds.x, model->minBounds.y, model->minBounds.z,
        model->maxBounds.x, model->maxBounds.y, model->maxBounds.z);
    spdlog::info("[ModelImporter]   Center: ({:.2f}, {:.2f}, {:.2f})",
        model->center.x, model->center.y, model->center.z);
    spdlog::info("[ModelImporter]   Size: ({:.2f}, {:.2f}, {:.2f})",
        model->size.x, model->size.y, model->size.z);

    return model;
}

void ModelImporter::ProcessShape(
    const tinyobj::attrib_t& attrib,
    const tinyobj::shape_t& shape,
    LoadedModel* model,
    size_t shapeIndex,
    bool generateNormals,
    bool flipTextureCoords) {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<VertexKey, unsigned int, VertexKeyHash> vertexMap;

    size_t indexOffset = 0;
    size_t faceCount = shape.mesh.num_face_vertices.size();

    for (size_t f = 0; f < faceCount; f++) {
        int fv = shape.mesh.num_face_vertices[f];

        if (fv != 3) {
            spdlog::warn("[ModelImporter] Non-triangular face found in shape {}, skipping",
                shapeIndex);
            indexOffset += fv;
            continue;
        }

        // Process triangle vertices
        for (size_t v = 0; v < 3; v++) {
            tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];

            Vertex vertex;
            vertex.color = glm::vec3(1.0f); 

            if (idx.vertex_index >= 0) {
                vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
                vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
                vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];
            }

            if (idx.normal_index >= 0) {
                vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
                vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
                vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];
            }
            else {
                vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            if (idx.texcoord_index >= 0) {
                vertex.texCoord.x = attrib.texcoords[2 * idx.texcoord_index + 0];
                vertex.texCoord.y = flipTextureCoords ?
                    1.0f - attrib.texcoords[2 * idx.texcoord_index + 1] :
                    attrib.texcoords[2 * idx.texcoord_index + 1];
            }
            else {
                vertex.texCoord = glm::vec2(0.0f);
            }

            VertexKey key = CreateVertexKey(vertex);

            auto it = vertexMap.find(key);
            unsigned int vertexIndex;

            if (it != vertexMap.end()) {
                vertexIndex = it->second;
            }
            else {
                vertexIndex = static_cast<unsigned int>(vertices.size());
                vertices.push_back(vertex);
                vertexMap[key] = vertexIndex;
            }

            indices.push_back(vertexIndex);
        }

        indexOffset += fv;
    }

    if ((generateNormals && !model->hasNormals) || !model->hasNormals) {
        spdlog::debug("[ModelImporter] Generating normals for shape {}", shapeIndex);
        GenerateNormals(vertices, indices);
    }

    spdlog::debug("[ModelImporter] Generating tangent space for shape {}", shapeIndex);
    GenerateTangentSpace(vertices, indices);

    auto mesh = std::make_unique<Mesh>(shape.name.empty() ?
        "Shape_" + std::to_string(shapeIndex) : shape.name);
    mesh->LoadData(vertices, indices);

    spdlog::info("[ModelImporter] Shape {}: '{}' - {} vertices, {} triangles",
        shapeIndex, mesh->GetName(), vertices.size(), indices.size() / 3);

    model->meshes.push_back(std::move(mesh));
}

void ModelImporter::GenerateNormals(std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices) {
    for (auto& vertex : vertices) {
        vertex.normal = glm::vec3(0.0f);
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        const glm::vec3& p0 = vertices[i0].position;
        const glm::vec3& p1 = vertices[i1].position;
        const glm::vec3& p2 = vertices[i2].position;

        glm::vec3 faceNormal = CalculateNormal(p0, p1, p2);

        vertices[i0].normal += faceNormal;
        vertices[i1].normal += faceNormal;
        vertices[i2].normal += faceNormal;
    }

    for (auto& vertex : vertices) {
        float length = glm::length(vertex.normal);
        if (length > 0.0001f) {
            vertex.normal = glm::normalize(vertex.normal);
        }
        else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f); 
        }
    }
}

void ModelImporter::GenerateTangentSpace(std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices) {
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

        const glm::vec2& uv0 = vertices[i0].texCoord;
        const glm::vec2& uv1 = vertices[i1].texCoord;
        const glm::vec2& uv2 = vertices[i2].texCoord;

        glm::vec3 edge1 = p1 - p0;
        glm::vec3 edge2 = p2 - p0;

        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float det = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;

        if (std::abs(det) < 0.0001f) {
            for (unsigned int idx : {i0, i1, i2}) {
                const glm::vec3& n = vertices[idx].normal;

                glm::vec3 up = (std::abs(n.y) < 0.999f) ?
                    glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);

                glm::vec3 tangent = glm::normalize(glm::cross(up, n));
                glm::vec3 bitangent = glm::cross(n, tangent);

                vertices[idx].tangent += tangent;
                vertices[idx].bitangent += bitangent;
            }
            continue;
        }

        float f = 1.0f / det;

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // Accumulate tangents and bitangents
        vertices[i0].tangent += tangent;
        vertices[i1].tangent += tangent;
        vertices[i2].tangent += tangent;

        vertices[i0].bitangent += bitangent;
        vertices[i1].bitangent += bitangent;
        vertices[i2].bitangent += bitangent;
    }

    for (auto& vertex : vertices) {
        const glm::vec3& n = vertex.normal;
        glm::vec3& t = vertex.tangent;
        glm::vec3& b = vertex.bitangent;

        float tangentLength = glm::length(t);
        if (tangentLength > 0.0001f) {
            t = glm::normalize(t - n * glm::dot(n, t));

            float handedness = glm::dot(glm::cross(n, t), b);
            if (handedness < 0.0f) {
                t = -t;
            }

            b = glm::cross(n, t);
        }
        else {
            glm::vec3 up = (std::abs(n.y) < 0.999f) ?
                glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);

            t = glm::normalize(glm::cross(up, n));
            b = glm::cross(n, t);
        }
    }
}

glm::vec3 ModelImporter::CalculateNormal(const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2) {
    glm::vec3 edge1 = p1 - p0;
    glm::vec3 edge2 = p2 - p0;
    glm::vec3 normal = glm::cross(edge1, edge2);

    float length = glm::length(normal);
    if (length > 0.0001f) {
        return normal / length;
    }

    return glm::vec3(0.0f, 1.0f, 0.0f);
}

ModelImporter::VertexKey ModelImporter::CreateVertexKey(const Vertex& vertex) {
    VertexKey key;

    const float precision = 10000.0f;
    key.px = static_cast<int>(vertex.position.x * precision);
    key.py = static_cast<int>(vertex.position.y * precision);
    key.pz = static_cast<int>(vertex.position.z * precision);

    key.nx = static_cast<int>(vertex.normal.x * precision);
    key.ny = static_cast<int>(vertex.normal.y * precision);
    key.nz = static_cast<int>(vertex.normal.z * precision);

    key.u = static_cast<int>(vertex.texCoord.x * precision);
    key.v = static_cast<int>(vertex.texCoord.y * precision);

    return key;
}

Mesh* ModelImporter::LoadedModel::GetMeshByName(const std::string& name) {
    for (const auto& mesh : meshes) {
        if (mesh->GetName() == name) {
            return mesh.get();
        }
    }
    return nullptr;
}

Mesh* ModelImporter::LoadedModel::GetMeshByIndex(size_t index) {
    if (index < meshes.size()) {
        return meshes[index].get();
    }
    return nullptr;
}

size_t ModelImporter::LoadedModel::GetMeshCount() const {
    return meshes.size();
}

void ModelImporter::LoadedModel::Draw() const {
    for (const auto& mesh : meshes) {
        if (mesh && mesh->IsValid()) {
            mesh->Draw();
        }
    }
}

void ModelImporter::LoadedModel::DrawInstanced(unsigned int instanceCount) const {
    for (const auto& mesh : meshes) {
        if (mesh && mesh->IsValid()) {
            mesh->DrawInstanced(instanceCount);
        }
    }
}

std::vector<std::string> ModelImporter::LoadedModel::GetMeshNames() const {
    std::vector<std::string> names;
    names.reserve(meshes.size());

    for (const auto& mesh : meshes) {
        if (mesh) {
            names.push_back(mesh->GetName());
        }
    }

    return names;
}

ModelImporter::Stats ModelImporter::LoadedModel::GetStats() const {
    Stats stats;
    stats.meshCount = meshes.size();
    stats.totalVertices = 0;
    stats.totalIndices = 0;
    stats.totalTriangles = 0;
    stats.memoryUsage = 0;

    for (const auto& mesh : meshes) {
        if (mesh) {
            auto meshStats = mesh->GetStats();
            stats.totalVertices += meshStats.vertexCount;
            stats.totalIndices += meshStats.indexCount;
            stats.totalTriangles += meshStats.triangleCount;
            stats.memoryUsage += meshStats.memoryUsage;
        }
    }

    return stats;
}

void ModelImporter::LoadedModel::PrintInfo() const {
    spdlog::info("=== Model Information ===");
    spdlog::info("  Meshes: {}", meshes.size());
    spdlog::info("  Materials: {}", materialNames.size());
    spdlog::info("  Has Normals: {}", hasNormals);
    spdlog::info("  Has TexCoords: {}", hasTexCoords);

    auto stats = GetStats();
    spdlog::info("  Total Vertices: {}", stats.totalVertices);
    spdlog::info("  Total Triangles: {}", stats.totalTriangles);
    spdlog::info("  Memory Usage: {:.2f} KB", stats.memoryUsage / 1024.0f);

    spdlog::info("  Bounds: min({:.2f}, {:.2f}, {:.2f}) max({:.2f}, {:.2f}, {:.2f})",
        minBounds.x, minBounds.y, minBounds.z,
        maxBounds.x, maxBounds.y, maxBounds.z);
    spdlog::info("  Center: ({:.2f}, {:.2f}, {:.2f})",
        center.x, center.y, center.z);
    spdlog::info("  Size: ({:.2f}, {:.2f}, {:.2f})",
        size.x, size.y, size.z);
}