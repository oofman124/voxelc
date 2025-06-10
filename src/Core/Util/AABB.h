#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
#include <limits>
#include "vertex.h"

struct AABBRaycastResult
{
    bool hit = false;
    float tNear = 0.0f;
    float tFar = 0.0f;
    glm::vec3 normal = glm::vec3(0.0f);
};

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;

    // Returns true if this AABB overlaps with another
    bool intersects(const AABB &other) const
    {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    // Returns true if the point is inside the AABB
    bool contains(const glm::vec3 &point) const
    {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }


    // Ray-AABB intersection (returns true if hit, tNear is the distance)
    AABBRaycastResult raycast(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir) const
    {
        AABBRaycastResult result;
        result.hit = false;
        result.tNear = -std::numeric_limits<float>::infinity();
        result.tFar = std::numeric_limits<float>::infinity();
        glm::vec3 t1, t2;
        int hitAxis = -1;

        for (int i = 0; i < 3; ++i)
        {
            if (fabs(rayDir[i]) < 1e-8f)
            {
                if (rayOrigin[i] < min[i] || rayOrigin[i] > max[i])
                    return result;
            }
            else
            {
                t1[i] = (min[i] - rayOrigin[i]) / rayDir[i];
                t2[i] = (max[i] - rayOrigin[i]) / rayDir[i];
                if (t1[i] > t2[i])
                    std::swap(t1[i], t2[i]);
                if (t1[i] > result.tNear)
                {
                    result.tNear = t1[i];
                    hitAxis = i;
                }
                if (t2[i] < result.tFar)
                    result.tFar = t2[i];
                if (result.tNear > result.tFar)
                    return result;
                if (result.tFar < 0)
                    return result;
            }
        }

        result.hit = true;

        // Calculate normal
        result.normal = glm::vec3(0.0f);
        if (hitAxis >= 0)
        {
            float sign = (rayOrigin[hitAxis] < min[hitAxis]) ? -1.0f : 1.0f;
            if (fabs(rayDir[hitAxis]) > 1e-8f)
            {
                // Use .x, .y, .z directly based on hitAxis
                if (hitAxis == 0)
                    result.normal.x = -sign * glm::sign(rayDir.x);
                else if (hitAxis == 1)
                    result.normal.y = -sign * glm::sign(rayDir.y);
                else if (hitAxis == 2)
                    result.normal.z = -sign * glm::sign(rayDir.z);
            }
        }

        return result;
    }

    // Generate 8 corners as SpatialVertex for debug mesh
    std::vector<SpatialVertex> getVertices() const
    {
        
        std::vector<SpatialVertex> verts(8);
        verts[0].pos = {min.x, min.y, min.z};
        verts[1].pos = {max.x, min.y, min.z};
        verts[2].pos = {max.x, max.y, min.z};
        verts[3].pos = {min.x, max.y, min.z};
        verts[4].pos = {min.x, min.y, max.z};
        verts[5].pos = {max.x, min.y, max.z};
        verts[6].pos = {max.x, max.y, max.z};
        verts[7].pos = {min.x, max.y, max.z};

        // Assign normals for each corner (for debug: average of face normals at the corner)
        verts[0].normal = glm::normalize(glm::vec3(-1, -1, -1));
        verts[1].normal = glm::normalize(glm::vec3(1, -1, -1));
        verts[2].normal = glm::normalize(glm::vec3(1, 1, -1));
        verts[3].normal = glm::normalize(glm::vec3(-1, 1, -1));
        verts[4].normal = glm::normalize(glm::vec3(-1, -1, 1));
        verts[5].normal = glm::normalize(glm::vec3(1, -1, 1));
        verts[6].normal = glm::normalize(glm::vec3(1, 1, 1));
        verts[7].normal = glm::normalize(glm::vec3(-1, 1, 1));

        return verts;
    }
};

#endif // AABB_H