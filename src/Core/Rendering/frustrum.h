#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>
#include <array>

class Frustum {
public:
    void update(const glm::mat4& projView) {
        // Extract planes from projection-view matrix
        for (int i = 0; i < 6; i++) {
            planes[i] = extractPlane(projView, i);
        }
    }

    bool isInFrustum(const glm::vec3& position, float radius) const {
        // Check if sphere is inside all frustum planes
        for (const auto& plane : planes) {
            if (distanceToPoint(plane, position) < -radius) {
                return false;
            }
        }
        return true;
    }

private:
    std::array<glm::vec4, 6> planes; // Left, Right, Bottom, Top, Near, Far

    glm::vec4 extractPlane(const glm::mat4& m, int side) {
        glm::vec4 plane;
        switch(side) {
            case 0: // Left
                plane = m[3] + m[0];
                break;
            case 1: // Right
                plane = m[3] - m[0];
                break;
            case 2: // Bottom
                plane = m[3] + m[1];
                break;
            case 3: // Top
                plane = m[3] - m[1];
                break;
            case 4: // Near
                plane = m[3] + m[2];
                break;
            case 5: // Far
                plane = m[3] - m[2];
                break;
        }
        return glm::normalize(plane);
    }

    float distanceToPoint(const glm::vec4& plane, const glm::vec3& point) const {
        return glm::dot(glm::vec3(plane), point) + plane.w;
    }
};
#endif