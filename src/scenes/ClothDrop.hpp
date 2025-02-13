#pragma once
#include "Scene.hpp"
#include "utils/utils.hpp"
#include "imgui.h"

class ClothDrop : public Scene {
public:
    // Parameters
    int w;
    bool collisionConstraint;
    float alphaDistance = 1e-8;
    float alphaBending = 1e-8;
    float alphaPlaneCollision = 1e-8;
    float alphaCollision = 1e-8;

    bool drawLines = false;

    ClothDrop(int w = 64, bool collisionConstraint = false)
        : w(w), collisionConstraint(collisionConstraint) {

        sphere = Mesh::createSphere(1.0f, 32);

        std::vector<glm::vec3> pos;
        std::vector<Constraint *> constraints;

        const float distance = 4.0f / (w - 1);
        int rangeMax = glm::ceil((float)w / 32);
        if (w > 8) rangeMax = std::max(rangeMax, 2);

        for (int y = 0; y < w; y++) {
            for (int x = 0; x < w; x++) {
                pos.push_back(glm::vec3(distance * x - distance * (w - 1) / 2, 3, distance * y - distance * (w - 1) / 2));

                // Distance
                if (x != w - 1)
                    constraints.push_back(new DistanceConstraint(y * w + x, y * w + (x + 1), distance, &alphaDistance));
                if (y != w - 1)
                    constraints.push_back(new DistanceConstraint(y * w + x, (y + 1) * w + x, distance, &alphaDistance));
                if (x != w - 1 && y != w - 1) {
                    constraints.push_back(new DistanceConstraint(y * w + x, (y + 1) * w + (x + 1), distance * sqrt(2), &alphaDistance));
                    constraints.push_back(new DistanceConstraint((y + 1) * w + x, y * w + (x + 1), distance * sqrt(2), &alphaDistance));
                }

                // Collision
                // constraints.push_back(new SphereCollisionConstraint(y * w + x, &sphereCenter, sphereRad + 0.05, &alphaPlaneCollision));

                // Bending
                for (int range = rangeMax; range <= rangeMax; range++) {
                    if (y < w - range)
                        constraints.push_back(new DistanceConstraint(y * w + x, (y + range) * w + x, distance * range, &alphaBending));
                    if (x < w - range)
                        constraints.push_back(new DistanceConstraint(y * w + x, y * w + x + range, distance * range, &alphaBending));
                    if (y < w - range && x < w - range) {
                        constraints.push_back(new DistanceConstraint(y * w + x, (y + range) * w + x + range, distance * range * sqrt(2), &alphaBending));
                        constraints.push_back(new DistanceConstraint((y + range) * w + x, y * w + x + range, distance * range * sqrt(2), &alphaBending));
                    }
                }
            }
        }

        meshFront = Mesh::createPlane(pos, w, w);
        meshBack = Mesh::createPlane(pos, w, w, true);

        auto &indices = meshFront->getIndices();
        for (int i = 0; i < indices.size(); i += 3) {
            constraints.push_back(new SphereTriCollisionConstraint(indices[i], indices[i + 1], indices[i + 2], &sphereCenter, sphereRad + 0.05, &alphaPlaneCollision));
        }

        solver = new Solver(pos, constraints, 0.01 / (w * w));

        // solver->activateGlobalCollision(distance, &alphaPlaneCollision);
        // solver->setGlobalCollision(collisionConstraint);
    }

    ClothDrop(const ClothDrop &scene) : ClothDrop(scene.w, scene.collisionConstraint) {
        this->alphaCollision = scene.alphaCollision;
        this->alphaPlaneCollision = scene.alphaPlaneCollision;
        this->alphaDistance = scene.alphaDistance;
        this->alphaBending = scene.alphaBending;

        this->drawLines = scene.drawLines;
    }

    ~ClothDrop() override {
    }

    void draw(ShaderProgram &shaderProgram, ShaderProgram &checkerShaderProgram, ShadowMap &shadowMap) override {
        meshFront->setVertices(solver->getPos());
        meshFront->updateNormals();
        meshBack->setVertices(solver->getPos());
        meshBack->updateNormals();

        shadowMap.beginRender();
        shadowMap.addObject(meshFront);
        shadowMap.addObject(meshBack);
        shadowMap.endRender();

        shaderProgram.use();
        shadowMap.sendShadowMap(shaderProgram);
        meshFront->draw(shaderProgram, glm::vec3(0.1, 0.8, 0.9), glm::mat4(1.0));
        meshBack->draw(shaderProgram, glm::vec3(0.1, 0.8, 0.9), glm::mat4(1.0));

        if (drawLines) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(2.0f);
            meshFront->draw(shaderProgram, glm::vec3(0), glm::mat4(1.0));
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        sphere->draw(shaderProgram, glm::vec3(1.0, 0.9, 0.2), glm::mat4(1.0));
    }

    bool showUI() override {
        bool changed = false;

        // if (ImGui::Checkbox("Use self collision", &collisionConstraint)) {
        //     solver->setGlobalCollision(collisionConstraint);
        // }

        int newW = w;
        if (ImGui::InputInt("Width", &newW)) {
            if (newW < 1) {
                newW = 1;
            }
            if (newW != w) {
                w = newW;
                changed = true;
            }
        }

        ImGui::Checkbox("Draw lines", &drawLines);

        return changed;
    }

    void showConstraintUI() override {
        alphaSelector("Distance", alphaDistance);
        alphaSelector("Bending", alphaBending);
        alphaSelector("Collision", alphaCollision);
    }

private:
    std::shared_ptr<Mesh> meshFront;
    std::shared_ptr<Mesh> meshBack;
    std::shared_ptr<Mesh> sphere;

    glm::vec3 sphereCenter = glm::vec3(0);
    float sphereRad = 1.0f;
};