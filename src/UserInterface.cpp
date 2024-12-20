#include "UserInterface.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

UserInterface::UserInterface(SceneManager *sceneManager, GLFWwindow *window) : sceneManager(sceneManager) {
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

UserInterface::~UserInterface() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UserInterface::show() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("XPBD");

    ImGui::Text("FPS: %d", (int)sceneManager->getFPS());

    int currentIndex = sceneManager->getSceneTypeIndex();
    if (ImGui::Combo("Scene Type", &currentIndex, Scenes::sceneNames.data(), Scenes::sceneNames.size())) {
        sceneManager->setSceneType(currentIndex);
    }

    if (ImGui::Button(sceneManager->getPlay() ? "Pause" : "Play")) {
        sceneManager->invertPlay();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        sceneManager->resetScene();
    }

    bool saveVideo = sceneManager->getSaveVideo();
    if (ImGui::Checkbox("Save video", &saveVideo)) {
        sceneManager->setSaveVideo(saveVideo);
    }

    ImGuiInputTextFlags flags = (saveVideo && sceneManager->getPlay()) ? ImGuiInputTextFlags_ReadOnly : 0;
    ImGui::InputText("Output name", sceneManager->saveFilename, IM_ARRAYSIZE(sceneManager->saveFilename), flags);

    if (ImGui::CollapsingHeader("Solver parameters")) {
        ImGui::InputInt("Iterations", sceneManager->getSolverIterations(), 1, 10);
        if (*sceneManager->getSolverIterations() < 1) {
            *sceneManager->getSolverIterations() = 1;
        }
    }

    ImGui::Text("Scene Parameters");

    if (sceneManager->showSceneUI()) {
        sceneManager->resetScene();
    }

    if (ImGui::CollapsingHeader("Constraints"))
        sceneManager->showSceneConstraintUI();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}