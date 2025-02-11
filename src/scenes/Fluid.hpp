// This scene does not work

#pragma once
#include "Scene.hpp"
#include "utils/utils.hpp"
#include "imgui.h"

class Fluid : public Scene {
public:
    // Parameters
    int size_x, size_y, size_z;
    float alphaDensity = 1e-3;
    float alphaPlaneCollision = 1e-8;

    Fluid(int size_x = 10, int size_y = 10, int size_z = 10) : size_x(size_x), size_y(size_y), size_z(size_z) {
        std::vector<glm::vec3> pos;
        std::vector<Constraint *> constraints;

        float pRadius = 0.01;
        DensityConstraint::d0 = 1000;
        DensityConstraint::h = 4.0f * pRadius;
        DensityConstraint::m = DensityConstraint::d0 * pow(DensityConstraint::h, 3);

        sphere = Mesh::createSphere(pRadius);
        box = Mesh::createBox();

        const std::vector<glm::vec3> &vertexBox = box->getVertices();

        planes = {
            new SemiPlane(vertexBox[0], vertexBox[1], vertexBox[2]),
            new SemiPlane(vertexBox[4], vertexBox[6], vertexBox[5]),
            new SemiPlane(vertexBox[8], vertexBox[9], vertexBox[10]),
            new SemiPlane(vertexBox[12], vertexBox[14], vertexBox[13]),
            new SemiPlane(vertexBox[16], vertexBox[18], vertexBox[17]),
            new SemiPlane(vertexBox[20], vertexBox[21], vertexBox[22]),
        };

        const float spacing = 2.0f * pRadius;
        uint counter = 0;
        for (int i = 0; i < size_x; i++) {
            for (int j = 0; j < size_y; j++) {
                for (int k = 0; k < size_z; k++) {

                    float offset_x = (size_x - 1) * spacing * 0.5f;
                    float offset_y = (size_y - 1) * spacing * 0.5f;
                    float offset_z = (size_z - 1) * spacing * 0.5f;

                    pos.emplace_back(i * spacing - offset_x, j * spacing - offset_y, k * spacing - offset_z);

                    constraints.push_back(new DensityConstraint(counter, &alphaDensity));
                    counter++;
                }
            }
        }

        for (int i = 0; i < pos.size(); i++) {
            for (SemiPlane *plane : planes) {
                constraints.push_back(new SemiPlaneConstraint(i, plane, &alphaPlaneCollision, pRadius));
            }
        }

        solver = new Solver(pos, constraints, DensityConstraint::m);
        solver->activateFluids();
    }

    Fluid(const Fluid &scene) : Fluid(size_x, size_y, size_z) {
        this->alphaDensity = scene.alphaDensity;
        this->alphaPlaneCollision = scene.alphaPlaneCollision;
    }

    ~Fluid() override {
    }

    void draw(ShaderProgram &shaderProgram, ShaderProgram &checkerShaderProgram, ShadowMap &shadowMap) override {
        shaderProgram.use();
        sphere->startDrawMultiple(shaderProgram);

        for (const glm::vec3 &pos : solver->getPos()) {
            sphere->addDrawMultiple(shaderProgram, glm::vec3(0.7), utils::getTranslate(pos));
        }

        sphere->endDrawMultiple();

        box->draw(shaderProgram, glm::vec3(0.7), glm::mat4(1.0));
    }

    bool showUI() override {
        bool changed = false;

        return changed;
    }

    void showConstraintUI() override {
        alphaSelector("Distance", alphaDensity);
    }

private:
    std::shared_ptr<Mesh> sphere;
    std::shared_ptr<Mesh> box;

    std::vector<SemiPlane *> planes;
};