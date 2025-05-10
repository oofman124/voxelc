#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class Transform : public std::enable_shared_from_this<Transform> {
public:
    Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
        matrixNeedsUpdate = true;
    };
    Transform(const glm::mat4& matrix) {
        this->matrix = matrix;
        matrixNeedsUpdate = false;
        decomposeMatrix();
    };
    Transform() {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        scale = glm::vec3(1.0f, 1.0f, 1.0f);
        matrixNeedsUpdate = true;
    };
    void decomposeMatrix() {
        // Extract the translation
        position = glm::vec3(matrix[3]);

        // Extract the scale
        scale = glm::vec3(
            glm::length(glm::vec3(matrix[0])),
            glm::length(glm::vec3(matrix[1])),
            glm::length(glm::vec3(matrix[2]))
        );

        // Extract the rotation
        glm::mat3 rotationMatrix = glm::mat3(
            matrix[0] / scale.x,
            matrix[1] / scale.y,
            matrix[2] / scale.z
        );
        rotation = glm::quat_cast(rotationMatrix);

        matrixNeedsUpdate = false;
    };
    void setPosition(const glm::vec3& position) {
        this->position = position;
        matrixNeedsUpdate = true;
    };
    void setRotation(const glm::quat& rotation) {
        this->rotation = rotation;
        matrixNeedsUpdate = true;
    };
    void setScale(const glm::vec3& scale) {
        this->scale = scale;
        matrixNeedsUpdate = true;
    };
    void setMatrix(const glm::mat4& matrix) {
        this->matrix = matrix;
        matrixNeedsUpdate = false;
    };

    glm::vec3 getPosition() const {
        return position;
    };
    glm::quat getRotation() const {
        return rotation;
    };
    glm::vec3 getScale() const {
        return scale;
    };
    glm::mat4 getMatrix() {
        if (matrixNeedsUpdate) {
            updateMatrix();
        }
        return matrix;
    };

private:
    void updateMatrix() {
        matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = matrix * glm::mat4_cast(rotation);
        matrix = glm::scale(matrix, scale);
        matrixNeedsUpdate = false;
    };
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    glm::mat4 matrix;
    bool matrixNeedsUpdate;
};

#endif // TRANSFORM_H