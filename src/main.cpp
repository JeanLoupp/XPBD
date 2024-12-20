#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <filesystem>

#include "ShaderProgram.hpp"
#include "ComputeShader.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Transformation.hpp"
#include "utils.hpp"
#include "Scenes/Scenes.hpp"
#include "Timer.hpp"
#include "UserInterface.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Constants and global variables
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;

Camera camera(SCR_WIDTH, SCR_HEIGHT);
GLFWwindow *window;
bool wireframeMode = false;

SceneManager sceneManager;

glm::vec3 lightPos(5.0f, 0.0f, 5.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

int frameCount = 0;

bool create_directories_if_needed(const std::string &filePath) {
    try {
        // Récupère uniquement le chemin du dossier sans le fichier
        std::filesystem::path path(filePath);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Erreur lors de la création des dossiers : " << e.what() << std::endl;
        return false;
    }
}

// Fonction pour sauvegarder l'écran
void SaveScreenshot(const char *filename) {
    std::vector<unsigned char> pixels(SCR_WIDTH * SCR_HEIGHT * 3); // RGB

    // Lire les pixels de la fenêtre actuelle
    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Inverser l'image verticalement
    for (int y = 0; y < SCR_HEIGHT / 2; ++y) {
        for (int x = 0; x < SCR_WIDTH * 3; ++x) {
            std::swap(pixels[y * SCR_WIDTH * 3 + x], pixels[(SCR_HEIGHT - y - 1) * SCR_WIDTH * 3 + x]);
        }
    }

    // Sauvegarder l'image au format PNG
    if (create_directories_if_needed(filename))
        if (stbi_write_png(filename, SCR_WIDTH, SCR_HEIGHT, 3, pixels.data(), SCR_WIDTH * 3)) {
            std::cout << "Screenshot saved to " << filename << std::endl;
        } else {
            std::cout << "Failed to save screenshot" << std::endl;
        }
}

// Callback functions
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    camera.setViewport(width, height);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) return;
    camera.mouse_button_callback(button, action);
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        sceneManager.grab(camera.getRay(xpos, ypos), camera.getPos());
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        sceneManager.releaseGrabbed();
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    camera.mouse_callback(xpos, ypos);
    if (sceneManager.isDragging()) {
        sceneManager.moveDragged(camera.getRay(xpos, ypos), camera.getPos());
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) return;
    camera.scroll_callback(xoffset, yoffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        wireframeMode = !wireframeMode;
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    } else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        SaveScreenshot("data/output/texture.png");
    }
}

GLuint genTexture(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    return texture;
}

// inits
void initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

bool initWindow() {
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Raytracing", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    return true;
}

bool initOpenGL() {
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    return true;
}

bool init() {
    initGLFW();
    if (!initWindow()) return false;
    if (!initOpenGL()) return false;
    return true;
}

void beginRender(ShaderProgram &shaderProgram) {

    shaderProgram.use();

    shaderProgram.set("view", camera.getViewMat());
    shaderProgram.set("projection", camera.getProjMat());
    shaderProgram.set("viewPos", camera.getPos());

    // Light
    shaderProgram.set("lightPos", lightPos);
    shaderProgram.set("lightColor", lightColor);
}

int main() {
    if (!init()) return -1;

    ShaderProgram shaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    ShaderProgram checkerShaderProgram("shaders/vertex_shader.glsl", "shaders/checker_frag.glsl");

    sceneManager.setSceneType(SceneType::CORD);

    UserInterface userInterface(&sceneManager, window);

    // Boucle de rendu
    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        beginRender(shaderProgram);
        beginRender(checkerShaderProgram);

        sceneManager.updateScene();
        sceneManager.drawScene(shaderProgram, checkerShaderProgram);

        userInterface.show();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (sceneManager.getSaveVideo() && sceneManager.getPlay()) {
            std::string filename = "data/output/" + std::string(sceneManager.saveFilename) + "/" + std::to_string(frameCount) + ".png";
            SaveScreenshot(filename.c_str());
            frameCount++;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
