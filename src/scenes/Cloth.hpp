#pragma once
#include "Scene.hpp"
#include "utils/utils.hpp"
#include "imgui.h"

class Cloth : public Scene {
public:
    // Parameters
    int w, h;
    float distance;
    bool bendingConstraints;
    bool collisionConstraint;
    bool spawnVertical;
    float alphaDistance = 1e-8;
    float alphaBending = 1e-8;
    float alphaPlaneCollision = 1e-8;
    float alphaCollision = 1e-8;

    Cloth(int w = 64, int h = 64, float distance = 0.05f, bool bendingConstraints = true, bool collisionConstraint = false, bool spawnVertical = false)
        : w(w), h(h), distance(distance), bendingConstraints(bendingConstraints), collisionConstraint(collisionConstraint), spawnVertical(spawnVertical) {

        std::vector<glm::vec3> pos;
        std::vector<Constraint *> constraints;

        plane = Mesh::createPlane();
        plane->applyTransform(utils::getTranslateY(-1.5) * utils::getScale(500));

        const std::vector<glm::vec3> &v = plane->getVertices();
        semiPlane = new SemiPlane(v[0], v[1], v[2]);

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (!spawnVertical)
                    pos.push_back(glm::vec3(distance * x - distance * (w - 1) / 2, distance * h / 2, distance * y));
                else
                    pos.push_back(glm::vec3(distance * x - distance * (w - 1) / 2, distance * (h - y), ((x * x + 3 * y) % 10 + 0.1) / 10000.0));

                // Distance
                if (x != w - 1)
                    constraints.push_back(new DistanceConstraint(y * w + x, y * w + (x + 1), distance, &alphaDistance));
                if (y != h - 1)
                    constraints.push_back(new DistanceConstraint(y * w + x, (y + 1) * w + x, distance, &alphaDistance));
                if (x != w - 1 && y != h - 1) {
                    constraints.push_back(new DistanceConstraint(y * w + x, (y + 1) * w + (x + 1), distance * sqrt(2), &alphaDistance));
                    constraints.push_back(new DistanceConstraint((y + 1) * w + x, y * w + (x + 1), distance * sqrt(2), &alphaDistance));
                }

                // Collision
                constraints.push_back(new SemiPlaneConstraint(y * w + x, semiPlane, &alphaPlaneCollision, 0.01));

                // Bending
                if (bendingConstraints) {
                    // if (x != w - 1 && y < h - 2)
                    //     constraints.push_back(new BendingConstraint((y + 1) * w + x, (y + 1) * w + x + 1, y * w + x, (y + 2) * w + x + 1, M_PI, &alphaBending));
                    // if (y != h - 1 && x < w - 2)
                    //     constraints.push_back(new BendingConstraint(y * w + x + 1, (y + 1) * w + x + 1, y * w + x, (y + 1) * w + x + 2, M_PI, &alphaBending));

                    if (y < h - 2)
                        constraints.push_back(new DistanceConstraint(y * w + x, (y + 2) * w + x, distance * 2, &alphaBending));
                    if (x < w - 2)
                        constraints.push_back(new DistanceConstraint(y * w + x, y * w + x + 2, distance * 2, &alphaBending));
                }
            }
        }

        meshFront = Mesh::createPlane(pos, w, h);
        meshBack = Mesh::createPlane(pos, w, h, true);

        solver = new Solver(pos, constraints);

        if (!spawnVertical) {
            solver->addFixedPoint(0, pos[0]);         // + glm::vec3(0.5, 0, 0));
            solver->addFixedPoint(w - 1, pos[w - 1]); // - glm::vec3(0.5, 0, 0));
        }

        solver->activateGlobalCollision(distance, &alphaPlaneCollision);
        solver->setGlobalCollision(collisionConstraint);
    }

    Cloth(const Cloth &scene) : Cloth(scene.w, scene.h, scene.distance, scene.bendingConstraints, scene.collisionConstraint, scene.spawnVertical) {
        this->alphaCollision = scene.alphaCollision;
        this->alphaPlaneCollision = scene.alphaPlaneCollision;
        this->alphaDistance = scene.alphaDistance;
        this->alphaBending = scene.alphaBending;
    }

    ~Cloth() override {
        delete semiPlane;
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
        meshFront->draw(shaderProgram, glm::vec3(0.7), glm::mat4(1.0));
        meshBack->draw(shaderProgram, glm::vec3(0.6), glm::mat4(1.0));

        checkerShaderProgram.use();
        shadowMap.sendShadowMap(checkerShaderProgram);
        plane->draw(checkerShaderProgram, glm::vec3(0.6), glm::mat4(1.0));
    }

    bool showUI() override {
        bool changed = false;

        if (ImGui::Checkbox("Use self collision", &collisionConstraint)) {
            solver->setGlobalCollision(collisionConstraint);
        }

        if (ImGui::Checkbox("Spawn vertical", &spawnVertical)) {
            changed = true;
        }

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

        int newH = h;
        if (ImGui::InputInt("Height", &newH)) {
            if (newH < 1) {
                newH = 1;
            }
            if (newH != h) {
                h = newH;
                changed = true;
            }
        }

        float newDistance = distance;
        if (ImGui::DragFloat("Distance", &newDistance, 0.01f, 0.0f, FLT_MAX)) {
            if (newDistance != distance) {
                distance = newDistance;
                changed = true;
            }
        }

        if (ImGui::Checkbox("Bending Constraint", &bendingConstraints))
            changed = true;

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
    std::shared_ptr<Mesh> plane;
    SemiPlane *semiPlane;
};