#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transformation {
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);

    Transformation() {}

    Transformation(const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rot) : position(pos), scale(scale), rotation(rot) {}

    Transformation(const glm::vec3 &pos, const float scale, const glm::vec3 &rot) : position(pos), scale(scale), rotation(rot) {}

    static Transformation PositionX(float x) {
        return Transformation(glm::vec3(x, 0.0f, 0.0f), 1.0f, glm::vec3(0.0f));
    }

    static Transformation PositionY(float y) {
        return Transformation(glm::vec3(0.0f, y, 0.0f), 1.0f, glm::vec3(0.0f));
    }

    static Transformation PositionZ(float z) {
        return Transformation(glm::vec3(0.0f, 0.0f, z), 1.0f, glm::vec3(0.0f));
    }

    static Transformation Position(float x, float y, float z) {
        return Transformation(glm::vec3(x, y, z), 1.0f, glm::vec3(0.0f));
    }

    static Transformation Scale(float s) {
        return Transformation(glm::vec3(0.0f), s, glm::vec3(0.0f));
    }

    static Transformation Scale(float sx, float sy, float sz) {
        return Transformation(glm::vec3(0.0f), glm::vec3(sx, sy, sy), glm::vec3(0.0f));
    }

    static Transformation RotationX(float angle) {
        return Transformation(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(angle, 0.0f, 0.0f));
    }

    static Transformation RotationY(float angle) {
        return Transformation(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f, angle, 0.0f));
    }

    static Transformation RotationZ(float angle) {
        return Transformation(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, angle));
    }

    static Transformation Rotation(float angleX, float angleY, float angleZ) {
        return Transformation(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(angleX, angleY, angleZ));
    }
};

#endif // TRANSFORMATION_HPP
