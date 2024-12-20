#pragma once
#include "Scene.hpp"
#include "../utils.hpp"
#include "imgui.h"

#include <ctime>

class Spheres : public Scene {
public:
    int spawnParticles;
    float pRadius;

    float alphaPlaneCollision = 1e-8;
    float alphaCollision = 1e-8;

    Spheres(int totalParticles = 300, float pRadius = 0.1) : spawnParticles(totalParticles), pRadius(pRadius) {

        std::vector<glm::vec3> pos;
        std::vector<Constraint *> constraints;

        sphere = Mesh::createSphere(pRadius);
        box = Mesh::createBox();

        const std::vector<glm::vec3> &vertexBox = box->getVertices();

        planes = {
            new SemiPlane(vertexBox[0], vertexBox[1], vertexBox[2]),
            new SemiPlane(vertexBox[4], vertexBox[6], vertexBox[5]),
            new SemiPlane(vertexBox[8], vertexBox[9], vertexBox[10]),
            new SemiPlane(vertexBox[12], vertexBox[14], vertexBox[13]),
            new SemiPlane(vertexBox[16], vertexBox[18], vertexBox[17]),
            // new SemiPlane(vertexBox[20], vertexBox[21], vertexBox[22]),
        };

        std::srand(static_cast<unsigned>(std::time(nullptr)));

        const float bound = 1.0f - pRadius;

        for (int i = 0; i < totalParticles; i++) {
            float x = -bound + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 2.0f)) * bound;
            float y = -bound + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 2.0f)) * bound;
            float z = -bound + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 2.0f)) * bound;
            pos.push_back(glm::vec3(x, y, z));

            for (SemiPlane *plane : planes) {
                constraints.push_back(new SemiPlaneConstraint(i, plane, &alphaPlaneCollision, pRadius));
            }

            for (int j = 0; j < i; j++) {
                constraints.push_back(new MinDistanceConstraint(i, j, 2 * pRadius, &alphaCollision));
            }
        }

        solver = new Solver(pos, constraints);
    }

    Spheres(const Spheres &scene) : Spheres(scene.spawnParticles, scene.pRadius) {
    }

    ~Spheres() override {
        for (SemiPlane *plane : planes) {
            delete plane;
        }
        planes.clear();
    }

    void draw(ShaderProgram &shaderProgram) override {
        sphere->startDrawMultiple(shaderProgram);

        for (const glm::vec3 &pos : solver->getPos()) {
            sphere->addDrawMultiple(shaderProgram, glm::vec3(0.7), utils::getTranslate(pos));
        }

        sphere->endDrawMultiple();

        box->draw(shaderProgram, glm::vec3(0.7), glm::mat4(1.0));
    }

    bool showUI() override {
        bool changed = false;

        int n = spawnParticles;
        if (ImGui::InputInt("Spawn Particles", &n)) {
            if (n < 1) {
                n = 1;
            }
            if (n != spawnParticles) {
                spawnParticles = n;
                changed = true;
            }
        }

        float newRad = pRadius;
        if (ImGui::DragFloat("Distance", &newRad, 0.01f, 0.0f, FLT_MAX)) {
            if (newRad != pRadius) {
                pRadius = newRad;
                changed = true;
            }
        }

        return changed;
    }

    void showConstraintUI() override {
        alphaSelector("Collision", alphaCollision);
        alphaSelector("Plane collision", alphaPlaneCollision);
    }

private:
    std::shared_ptr<Mesh> sphere;
    std::shared_ptr<Mesh> box;

    std::vector<SemiPlane *> planes;
};