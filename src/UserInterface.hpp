#pragma once

#include "SceneManager.hpp"
#include <GLFW/glfw3.h>

class UserInterface {
public:
    UserInterface(SceneManager *sceneManager, GLFWwindow *window);
    ~UserInterface();
    void show();

private:
    SceneManager *sceneManager;
};