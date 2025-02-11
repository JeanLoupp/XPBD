// Handles initialization and reset of scenes, as well as updates and grabbing.

#pragma once

#include "render/ShaderProgram.hpp"
#include "scenes/Scenes.hpp"
#include "utils/Timer.hpp"
#include "render/ShadowMap.hpp"

class SceneManager {
public:
    ~SceneManager() { delete scene; }

    void updateScene();
    void drawScene(ShaderProgram &shaderProgram, ShaderProgram &checkerShaderProgram, ShadowMap &shadowMap);
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

    bool useSubsteps = false;

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