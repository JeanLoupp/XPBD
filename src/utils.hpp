#ifndef UTILS_HPP
#define UTILS_HPP

#include <glm/glm.hpp>
#include <iostream>
#include <ostream>

namespace utils {

glm::mat4 getTransfoMat(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
                        const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f),
                        const glm::vec3 &rotation = glm::vec3(0.0f, 0.0f, 0.0f));

glm::mat4 getTransfoMat(const glm::vec3 &position,
                        float size,
                        const glm::vec3 &rotation);

glm::mat4 getTranslateX(float x);
glm::mat4 getTranslateY(float y);
glm::mat4 getTranslateZ(float z);
glm::mat4 getTranslate(float x, float y, float z);
glm::mat4 getTranslate(const glm::vec3 &pos);

glm::mat4 getScale(float s);
glm::mat4 getScale(float sx, float sy, float sz);

glm::mat4 getRotateX(float angle);
glm::mat4 getRotateY(float angle);
glm::mat4 getRotateZ(float angle);
glm::mat4 getRotate(float angleX, float angleY, float angleZ);

} // namespace utils

std::ostream &operator<<(std::ostream &os, const glm::vec3 &vec);

#endif // UTILS_HPP
