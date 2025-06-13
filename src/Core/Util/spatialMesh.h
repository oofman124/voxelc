#ifndef SPATIAL_MESH_H
#define SPATIAL_MESH_H

#include <vector>
#include <memory>
#include <set>
#include <map>
#include <glm/glm.hpp>
#include "vertex.h"
#include "AABB.h"
#include "../transform.h"
#include "../Rendering/mesh.h"
#include <algorithm>


struct SpatialRaycastResult {
    float t = 0.0f;                // Distance along the ray to the hit
    glm::vec3 position{0.0f};      // World or local space intersection point
    glm::vec3 normal{0.0f};        // Surface normal at the hit
};


class SpatialMesh {
public:
    std::vector<SpatialVertex> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<Transform> transform;
    std::vector<AABB> aabbs; // One or more AABBs for collision/raycasting

    SpatialMesh() = default;

    // Load from a UV_Mesh (assumes blocky mesh, can approximate otherwise)
    void loadFromMesh(const UV_Mesh& mesh, std::shared_ptr<Transform> transformPtr = nullptr) {
        vertices.clear();
        indices = mesh.indices;
        transform = transformPtr;

        // Convert Vertex to SpatialVertex (no normals yet)
        for (const auto& v : mesh.vertices) {
            SpatialVertex sv;
            sv.pos = glm::vec3(v.x, v.y, v.z);
            sv.normal = glm::vec3(0.0f); // Will be calculated
            vertices.push_back(sv);
        }
        calculateNormals();
        calculateBoundsAABB();
    }

    void loadFromVertices(const std::vector<glm::vec3> verts, const std::vector<unsigned int>& inds, std::shared_ptr<Transform> transformPtr = nullptr) {
        vertices.clear();
        indices = inds;
        transform = transformPtr;

        // Convert glm::vec3 to SpatialVertex
        for (const auto& v : verts) {
            SpatialVertex sv;
            sv.pos = v;
            sv.normal = glm::vec3(0.0f); // Will be calculated
            vertices.push_back(sv);
        }
        calculateNormals();
        calculateBoundsAABB();
    }

    // Calculate per-vertex normals (simple, for blocky meshes)
    void calculateNormals() {
        // Zero out normals
        for (auto& v : vertices) v.normal = glm::vec3(0.0f);

        // For each triangle, add face normal to each vertex
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            auto& v0 = vertices[indices[i]];
            auto& v1 = vertices[indices[i+1]];
            auto& v2 = vertices[indices[i+2]];
            glm::vec3 normal = glm::normalize(glm::cross(v1.pos - v0.pos, v2.pos - v0.pos));
            v0.normal += normal;
            v1.normal += normal;
            v2.normal += normal;
        }
        // Normalize all normals
        for (auto& v : vertices) v.normal = glm::normalize(v.normal);
    }

    // Calculate AABBs for the mesh (one big AABB, or per-block if blocky)
    void calculateBoundsAABB() {
        aabbs.clear();
        if (vertices.empty()) return;

        // Simple: one big AABB
        glm::vec3 min = vertices[0].pos, max = vertices[0].pos;
        for (const auto& v : vertices) {
            min = glm::min(min, v.pos);
            max = glm::max(max, v.pos);
        }
        aabbs.push_back(AABB{min, max});
    }


    // Optionally, create per-block AABBs if mesh is blocky
    void calculateBlockAABBs(float blockSize = 1.0f) {
        aabbs.clear();
        if (vertices.empty()) return;

        // Map of block positions
        auto ivec3_less = [](const glm::ivec3 &a, const glm::ivec3 &b)
        {
            if (a.x != b.x)
                return a.x < b.x;
            if (a.y != b.y)
                return a.y < b.y;
            return a.z < b.z;
        };
        std::set<glm::ivec3, decltype(ivec3_less)> blocks(ivec3_less);
        for (const auto &v : vertices)
        {
            glm::ivec3 blockPos = glm::floor(v.pos / blockSize);
            blocks.insert(blockPos);
        }

        // Group by YZ, then merge X
        std::map<std::pair<int, int>, std::vector<int>> yzToXs;
        for (const auto& b : blocks) {
            yzToXs[{b.y, b.z}].push_back(b.x);
        }

        for (const auto& [yz, xs] : yzToXs) {
            if (xs.empty()) continue;
            std::vector<int> sortedXs = xs;
            std::sort(sortedXs.begin(), sortedXs.end());

            // Greedy merge consecutive Xs
            int startX = sortedXs[0];
            int prevX = sortedXs[0];
            for (size_t i = 1; i <= sortedXs.size(); ++i) {
                if (i == sortedXs.size() || sortedXs[i] != prevX + 1) {
                    // Create merged AABB for [startX, prevX] at (y, z)
                    glm::vec3 min = glm::vec3(startX, yz.first, yz.second) * blockSize;
                    glm::vec3 max = glm::vec3(prevX + 1, yz.first + 1, yz.second + 1) * blockSize;
                    aabbs.push_back(AABB{min, max});
                    if (i < sortedXs.size()) startX = sortedXs[i];
                }
                if (i < sortedXs.size()) prevX = sortedXs[i];
            }
        }
    }

    // Raycast against all AABBs (returns nearest hit)
    bool raycast(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir, SpatialRaycastResult &result) const
    {
        bool hit = false;
        float closestT = std::numeric_limits<float>::max();
        glm::vec3 hitNormal(0.0f);
        glm::vec3 hitPos(0.0f);
        for (const auto &aabb : aabbs)
        {
            auto aabbResult = aabb.raycast(rayOrigin, rayDir);
            if (aabbResult.hit && aabbResult.tNear >= 0 && aabbResult.tNear < closestT)
            {
                hit = true;
                closestT = aabbResult.tNear;
                hitNormal = aabbResult.normal;
                hitPos = rayOrigin + rayDir * aabbResult.tNear;
            }
        }
        if (hit)
        {
            result.t = closestT;
            result.normal = hitNormal;
            result.position = hitPos;
        }
        return hit;
    }
    // Raycast against all AABBs in world space (applies transform if present)
    bool raycastWorld(const glm::vec3& rayOrigin, const glm::vec3& rayDir, SpatialRaycastResult &result) const {
        if (!transform) {
            // No transform, use local space raycast
            return raycast(rayOrigin, rayDir, result);
        }

        SpatialRaycastResult tempResult;
        // Transform ray into local space
        glm::mat4 invMat = glm::inverse(transform->getMatrix());
        glm::vec3 localOrigin = glm::vec3(invMat * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localDir = glm::normalize(glm::vec3(invMat * glm::vec4(rayDir, 0.0f)));

        bool hit = raycast(localOrigin, localDir, tempResult);
        if (hit) {
            // Transform normal back to world space
            glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(transform->getMatrix())));
            result.normal = glm::normalize(normalMat * tempResult.normal);
            result.position = glm::vec3(transform->getMatrix() * glm::vec4(tempResult.position, 1.0f));
            result.t = tempResult.t; // Distance along the ray
        }
        return hit;
    }
};

#endif // SPATIAL_MESH_H