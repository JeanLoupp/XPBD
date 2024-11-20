#include "SceneManager.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void SceneManager::resetScene() {
    Scene *newScene = Scenes::createScene(sceneType, scene);
    delete scene;
    scene = newScene;
    dt = timer.elapsed();
}

void SceneManager::updateScene() {
    dt = timer.elapsed();
    if (play) {
        scene->update(dt);
    }
}

void SceneManager::setSceneType(SceneType sceneType) {
    this->sceneType = sceneType;
    delete scene;
    scene = Scenes::createScene(sceneType);
    dt = timer.elapsed();
}

void SceneManager::drawScene(ShaderProgram &shaderProgram) {
    scene->draw(shaderProgram);
}

void SceneManager::grab(const glm::vec3 &direction, const glm::vec3 &camPos) {
    initialDistance = MAXFLOAT;
    grabbedIdx = -1;
    const std::vector<glm::vec3> &pos = scene->getPos();
    for (int i = 0; i < pos.size(); i++) {
        glm::vec3 pc = pos[i] - camPos;
        float proj = dot(pc, direction);
        float det = proj * proj - (dot(pc, pc) - 0.1);
        if (det > 0) {
            float dist = glm::length(pc);
            if (dist < initialDistance) {
                initialDistance = dist;
                grabbedIdx = i;
            }
        }
    }
    if (grabbedIdx != -1) scene->solver->addFixedPoint(grabbedIdx);
}

void SceneManager::moveDragged(const glm::vec3 &direction, const glm::vec3 &camPos) {
    if (grabbedIdx == -1) return;
    scene->solver->setPos(grabbedIdx, camPos + initialDistance * direction);
}

void SceneManager::releaseGrabbed() {
    if (grabbedIdx == -1) return;
    scene->solver->removeFixedPoint(grabbedIdx);
    grabbedIdx = -1;
}