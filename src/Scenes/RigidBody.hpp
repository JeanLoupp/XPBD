#pragma once
#include "Scene.hpp"
#include "../utils.hpp"
#include "imgui.h"

class RigidBody : public Scene {
public:
    RigidBody(int w = 64, int h = 64, float distance = 0.05f) {

        std::vector<glm::vec3> pos;
        std::vector<Constraint *> constraints;

        cube = Mesh::createCube(0.1);
        sphere = Mesh::createSphere(0.1);
        box = Mesh::createBox();

        pos.insert(pos.end(), cube->getVertices().begin(), cube->getVertices().end());

        for (glm::vec3 &p : pos) {
        }

        solver = new Solver(pos, constraints);
    }

    RigidBody(const RigidBody &scene) : RigidBody() {}

    void draw(ShaderProgram &shaderProgram) override {
    }

private:
    std::shared_ptr<Mesh> cube;
    std::shared_ptr<Mesh> sphere;
    std::shared_ptr<Mesh> box;
};