#include "utils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace utils {

glm::mat4 getTransfoMat(const glm::vec3 &position,
                        const glm::vec3 &scale,
                        const glm::vec3 &rotation) {
    // Init
    glm::mat4 transformationMatrix = glm::mat4(1.0f);

    // Translation
    transformationMatrix = glm::translate(transformationMatrix, position);

    // Rotation
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // X
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // Y
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // Z

    // Scale
    transformationMatrix = glm::scale(transformationMatrix, scale);

    return transformationMatrix;
}

glm::mat4 getTransfoMat(const glm::vec3 &position,
                        float size,
                        const glm::vec3 &rotation) {
    glm::vec3 scale(size);

    // Init
    glm::mat4 transformationMatrix = glm::mat4(1.0f);

    // Scale
    transformationMatrix = glm::scale(transformationMatrix, scale);

    // Rotation
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // X
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // Y
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // Z

    // Translation
    transformationMatrix = glm::translate(transformationMatrix, position);

    return transformationMatrix;
}

glm::mat4 getTranslateX(float x) {
    return glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 0.0f));
}

glm::mat4 getTranslateY(float y) {
    return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y, 0.0f));
}

glm::mat4 getTranslateZ(float z) {
    return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, z));
}

glm::mat4 getTranslate(float x, float y, float z) {
    return glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

glm::mat4 getTranslate(const glm::vec3 &pos) {
    return glm::translate(glm::mat4(1.0f), pos);
}

glm::mat4 getScale(float s) {
    return glm::scale(glm::mat4(1.0f), glm::vec3(s));
}

glm::mat4 getScale(float sx, float sy, float sz) {
    return glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
}

glm::mat4 getRotateX(float angle) {
    return glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1, 0, 0));
}

glm::mat4 getRotateY(float angle) {
    return glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
}

glm::mat4 getRotateZ(float angle) {
    return glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1));
}

glm::mat4 getRotate(float angleX, float angleY, float angleZ) {
    glm::mat4 transformationMatrix(1.0f);
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(angleX), glm::vec3(1, 0, 0)); // X
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(angleY), glm::vec3(0, 1, 0)); // Y
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(angleZ), glm::vec3(0, 0, 1)); // Z
    return transformationMatrix;
}

} // namespace utils

std::ostream &operator<<(std::ostream &os, const glm::vec3 &vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}
