#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

public:
    Camera(int screenWidth, int screenHeight);

    void mouse_button_callback(int button, int action);

    void mouse_callback(double xpos, double ypos);

    void scroll_callback(double xoffset, double yoffset);

    void setViewport(int screenWidth, int screenHeight);
    void updateView();

    const glm::mat4 &getViewMat() const { return viewMat; }
    const glm::mat4 &getProjMat() const { return projMat; }
    const glm::vec3 &getPos() const { return cameraPos; }

    float getAspectRatio() const { return (float)screenWidth / screenHeight; }
    float getNear() const { return near; }
    float getFar() const { return far; }

    glm::vec3 getRay(double x, double y);

    bool hasMoved();

private:
    float lastX = 0;
    float lastY = 0;
    float yaw = 90.0f;
    float pitch = 0.0f;
    float fov = 45.0f;
    float cameraSpeed = 2.5f;
    float radius = 10.0f;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, radius);
    bool firstMouse = true;
    bool mouseButtonPressed = false;
    bool m_hasMoved = true;

    float near = 0.1f;
    float far = 100.0f;

    int screenWidth, screenHeight;
    glm::mat4 projMat;
    glm::mat4 viewMat;

    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;
};

#endif // CAMERA_HPP