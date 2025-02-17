#pragma once
#include "Scene.hpp"
#include "utils/utils.hpp"
#include "imgui.h"

#include <algorithm>
#include <random>

class ClothTurn : public Scene {
public:
    // Parameters
    int w;
    float alphaDistance = 1e-8;
    float alphaBending = 1e-8;
    float alphaPlaneCollision = 1e-8;
    float alphaCollision = 1e-8;

    float cylinderDist;
    float cylinderTheta;

    bool drawLines = false;

    ClothTurn(int w = 16, float cylinderDist = 1, float cylinderTheta = 0)
        : w(w), cylinderDist(cylinderDist), cylinderTheta(cylinderTheta) {

        int h = 64;

        cylinderMesh1 = Mesh::createCylinder(1.0f, cylRad);
        cylinderMesh2 = Mesh::createCylinder(1.0f, cylRad);

        cylinder1 = new Cylinder({1, 0, 0}, {0, cylinderDist / 2, 0}, cylRad + 0.02);
        cylinder2 = new Cylinder({1, 0, 0}, {0, -cylinderDist / 2, 0}, cylRad + 0.02);

        std::vector<glm::vec3> pos;
        std::vector<Constraint *> constraints;

        const float distance = 4.0f / (h - 1) * M_PI_2;

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {

                float theta = 2 * y * M_PI / h;

                pos.push_back(glm::vec3(distance * x - distance * (w - 1) / 2,
                                        cos(theta),
                                        sin(theta)));

                // Distance
                constraints.push_back(new DistanceConstraint(y * w + x, (y + 1) % h * w + x, distance, &alphaDistance));

                if (x != w - 1) {
                    constraints.push_back(new DistanceConstraint(y * w + x, y * w + (x + 1), distance, &alphaDistance));
                    constraints.push_back(new DistanceConstraint(y * w + x, (y + 1) % h * w + (x + 1), distance * sqrt(2), &alphaDistance));
                    constraints.push_back(new DistanceConstraint((y + 1) % h * w + x, y * w + (x + 1), distance * sqrt(2), &alphaDistance));
                }

                // Collision
                constraints.push_back(new CylinderCollisionConstraint(y * w + x, cylinder1, &alphaPlaneCollision));
                constraints.push_back(new CylinderCollisionConstraint(y * w + x, cylinder2, &alphaPlaneCollision));

                // Bending
                // int range = 2;
                // constraints.push_back(new DistanceConstraint(y * w + x, (y + range) % h * w + x, distance, &alphaDistance));

                // if (x < w - range) {
                //     constraints.push_back(new DistanceConstraint(y * w + x, y * w + (x + range), distance, &alphaDistance));
                //     constraints.push_back(new DistanceConstraint(y * w + x, (y + range) % h * w + (x + range), distance * sqrt(2), &alphaDistance));
                //     constraints.push_back(new DistanceConstraint((y + range) % h * w + x, y * w + (x + range), distance * sqrt(2), &alphaDistance));
                // }
            }
        }

        std::default_random_engine engine(42);
        std::shuffle(constraints.begin(), constraints.end(), engine);

        meshFront = Mesh::createPlane(pos, w, h, false, true);
        meshBack = Mesh::createPlane(pos, w, h, true, true);

        solver = new Solver(pos, constraints, 0.01 / (w * h));

        solver->activateGlobalCollision(distance, &alphaPlaneCollision);
    }

    ClothTurn(const ClothTurn &scene) : ClothTurn(scene.w) {
        this->alphaCollision = scene.alphaCollision;
        this->alphaPlaneCollision = scene.alphaPlaneCollision;
        this->alphaDistance = scene.alphaDistance;
        this->alphaBending = scene.alphaBending;

        this->drawLines = scene.drawLines;
    }

    ~ClothTurn() override {
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

        cylinderMesh1->draw(shaderProgram, glm::vec3(1.0, 0.9, 0.2), utils::getTransfoMat({0, cylinderDist / 2, 0}, {5, 1, 1}, {0, cylinderTheta, 0}));
        cylinderMesh2->draw(shaderProgram, glm::vec3(1.0, 0.9, 0.2), utils::getTransfoMat({0, -cylinderDist / 2, 0}, {5, 1, 1}, {0, -cylinderTheta, 0}));
    }

    bool showUI() override {
        bool changed = false;

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

        // cylinder1 = new Cylinder({1, 0, 0}, {0, cylinderDist / 2, 0}, cylRad + 0.02);
        // cylinder2 = new Cylinder({1, 0, 0}, {0, -cylinderDist / 2, 0}, cylRad + 0.02);

        if (ImGui::SliderFloat("Cylinder Angle", &cylinderTheta, 0, 360)) {
            float theta = glm::radians(cylinderTheta);
            cylinder1->dir = {cos(theta), 0, -sin(theta)};
            cylinder2->dir = {cos(theta), 0, sin(theta)};
        }

        if (ImGui::SliderFloat("Cylinder distance", &cylinderDist, 0, 10)) {
            cylinder1->p.y = cylinderDist / 2;
            cylinder2->p.y = -cylinderDist / 2;
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
    std::shared_ptr<Mesh> cylinderMesh1;
    std::shared_ptr<Mesh> cylinderMesh2;

    Cylinder *cylinder1;
    Cylinder *cylinder2;

    float cylRad = 0.1f;
};