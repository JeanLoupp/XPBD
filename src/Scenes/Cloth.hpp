#pragma once
#include "Scene.hpp"
#include "../utils.hpp"
#include "imgui.h"

class Cloth : public Scene {
public:
    // Parameters
    int w, h;
    float distance;
    bool bendingConstraints;
    float alphaDistance = 1e-8;
    float alphaBending = 1e-8;
    float alphaCollision = 1e-8;

    Cloth(int w = 64, int h = 64, float distance = 0.05f, bool bendingConstraints = false)
        : w(w), h(h), distance(distance), bendingConstraints(bendingConstraints) {

        std::vector<glm::vec3> pos;
        std::vector<Constraint *> constraints;

        plane = Mesh::createPlane();
        plane->applyTransform(utils::getTranslateY(-1.5) * utils::getScale(5));

        const std::vector<glm::vec3> &v = plane->getVertices();
        semiPlane = new SemiPlane(v[0], v[1], v[2]);

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                pos.push_back(glm::vec3(distance * x - distance * (w - 1) / 2, distance * h / 2, distance * y));

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
                constraints.push_back(new SemiPlaneConstraint(y * w + x, semiPlane, &alphaCollision));

                // Bending
                if (bendingConstraints) {
                    if (x != w - 1 && y < h - 2)
                        constraints.push_back(new BendingConstraint((y + 1) * w + x, (y + 1) * w + x + 1, y * w + x, (y + 2) * w + x + 1, M_PI, &alphaBending));
                    if (y != h - 1 && x < w - 2)
                        constraints.push_back(new BendingConstraint(y * w + x + 1, (y + 1) * w + x + 1, y * w + x, (y + 1) * w + x + 2, M_PI, &alphaBending));

                    //     if (y < h - 2)
                    //         constraints.push_back(new DistanceConstraint(y * w + x, (y + 2) * w + x, distance * 2, &alphaBending));
                    //     if (x < w - 2)
                    //         constraints.push_back(new DistanceConstraint(y * w + x, y * w + x + 2, distance * 2, &alphaBending));
                }
            }
        }

        mesh = Mesh::createPlane(pos, w, h);

        solver = new Solver(pos, constraints);

        solver->addFixedPoint(0, pos[0]);
        solver->addFixedPoint(w - 1, pos[w - 1]);
    }

    Cloth(const Cloth &scene) : Cloth(scene.w, scene.h, scene.distance, scene.bendingConstraints) {}

    void draw(ShaderProgram &shaderProgram) override {
        mesh->setVertices(solver->getPos());
        mesh->updateNormals();
        mesh->draw(shaderProgram, glm::vec3(0.7), glm::mat4(1.0));

        plane->draw(shaderProgram, glm::vec3(0.6), glm::mat4(1.0));
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
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Mesh> plane;
    SemiPlane *semiPlane;
};