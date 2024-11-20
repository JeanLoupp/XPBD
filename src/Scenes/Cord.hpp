#pragma once
#include "Scene.hpp"
#include "../utils.hpp"
#include "imgui.h"

class Cord : public Scene {
public:
    // Parameters
    int nParticles;
    float distance;

    Cord(int nParticles = 10, float distance = 0.5f) : nParticles(nParticles), distance(distance) {

        std::vector<glm::vec3> pos;
        std::vector<Constraint *> constraints;

        for (int i = 0; i < nParticles; i++) {
            pos.push_back(glm::vec3(distance * i, 0, 0));

            if (i != nParticles - 1)
                constraints.push_back(new DistanceConstraint(i, i + 1, distance));
        }

        solver = new Solver(pos, constraints);
        solver->addFixedPoint(0, glm::vec3(0));
    }

    Cord(const Cord &scene) : Cord(scene.nParticles, scene.distance) {}

    void draw(ShaderProgram &shaderProgram) override {
        circle->startDrawMultiple(shaderProgram);

        for (const glm::vec3 &pos : solver->getPos()) {
            circle->addDrawMultiple(shaderProgram, glm::vec3(0.7), utils::getTranslate(pos));
        }

        circle->endDrawMultiple();
    }

    bool showUI() override {
        bool changed = false;

        int newNParticles = nParticles;
        if (ImGui::InputInt("nParticles", &newNParticles)) {
            if (newNParticles < 1) {
                newNParticles = 1;
            }
            if (newNParticles != nParticles) {
                nParticles = newNParticles;
                changed = true;
            }
        }

        float newDistance = distance;
        if (ImGui::DragFloat("Distance", &newDistance, 0.1f, 0.0f, FLT_MAX)) {
            if (newDistance != distance) {
                distance = newDistance;
                changed = true;
            }
        }

        return changed;
    }

private:
    std::shared_ptr<Mesh> circle = Mesh::createSphere(0.1, 8);
};