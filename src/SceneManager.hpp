#pragma once

#include "ShaderProgram.hpp"
#include "Scenes/Scenes.hpp"
#include "Timer.hpp"

class SceneManager {
public:
    ~SceneManager() { delete scene; }

    void updateScene();
    void drawScene(ShaderProgram &shaderProgram, ShaderProgram &checkerShaderProgram);
    bool showSceneUI() { return scene->showUI(); }
    void showSceneConstraintUI() { return scene->showConstraintUI(); }

    int *getSolverIterations() { return &scene->solver->N_ITERATION; }

    void resetScene();

    void invertPlay() { play = !play; }
    void setPlay(bool play) { this->play = play; };
    bool getPlay() const { return play; }

    float getFPS() const { return 1.0f / dt; }

    SceneType getSceneType() const { return sceneType; }
    int getSceneTypeIndex() const { return static_cast<int>(sceneType); }
    void setSceneType(const SceneType type);
    void setSceneType(const int type) { setSceneType(static_cast<SceneType>(type)); }

    void grab(const glm::vec3 &direction, const glm::vec3 &camPos);
    void moveDragged(const glm::vec3 &direction, const glm::vec3 &camPos);
    void releaseGrabbed();
    bool isDragging() const { return grabbedIdx != -1; }

    bool getSaveVideo() { return saveVideo; }
    void setSaveVideo(bool value) { saveVideo = value; }

    char saveFilename[128] = "video";

private:
    Scene *scene;
    Timer timer;
    float dt = 1;

    bool play = true;

    bool saveVideo = false;

    SceneType sceneType = SceneType::CORD;

    // Grab
    float initialDistance;
    int grabbedIdx = -1;
};