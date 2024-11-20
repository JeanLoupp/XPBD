#include "Camera.hpp"

#include <iostream>

Camera::Camera(int screenWidth, int screenHeight) {
    setViewport(screenWidth, screenHeight);
    updateView();
}

void Camera::mouse_button_callback(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseButtonPressed = true;
        firstMouse = true;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouseButtonPressed = false;
    }
}

void Camera::mouse_callback(double xpos, double ypos) {

    if (mouseButtonPressed) {
        m_hasMoved = true;

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (glm::abs(pitch) > 80.0f) pitch = 80.0f * glm::sign(pitch);

        cameraPos.x = radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraPos.y = radius * sin(glm::radians(pitch));
        cameraPos.z = radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        updateView();
    }
}

void Camera::scroll_callback(double xoffset, double yoffset) {
    m_hasMoved = true;

    radius -= (float)yoffset * 0.2;
    if (radius < 0.0f) radius = 0.0f;
    cameraPos.x = radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraPos.y = radius * sin(glm::radians(pitch));
    cameraPos.z = radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    updateView();
}

bool Camera::hasMoved() {
    if (m_hasMoved) {
        m_hasMoved = false;
        return true;
    }
    return false;
}

void Camera::setViewport(int screenWidth, int screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    projMat = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 100.0f);
}

void Camera::updateView() {
    direction = glm::normalize(-cameraPos);
    right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction));
    up = glm::cross(direction, right);

    viewMat = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), up);
}

glm::vec3 Camera::getRay(double x, double y) {
    float fovRad = glm::radians(fov);

    // Taille de la fenêtre dans l'espace caméra
    float viewportHeight = 2.0f * tan(fovRad / 2.0f);
    float viewportWidth = viewportHeight * screenWidth / screenHeight;

    // Coordonnées normalisées de l'écran (0,0 en bas à gauche, 1,1 en haut à droite)
    float u = x / screenWidth;
    float v = y / screenHeight;

    // Passer aux coordonnées dans l'espace caméra (-1 à +1)
    float px = (2.0f * u - 1.0f) * viewportWidth / 2.0f;
    float py = (1.0f - 2.0f * v) * viewportHeight / 2.0f;

    // Calculer le rayon dans l'espace monde
    glm::vec3 rayDirection = glm::normalize(direction - px * right + py * up);

    return rayDirection;
}