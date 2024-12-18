#pragma once
#include "Scene.hpp"
#include "../utils.hpp"
#include "imgui.h"

#include "../TetraMesh.hpp"

#include <ctime>

class RigidBody : public Scene {
public:
    RigidBody() {

        std::vector<Constraint *> constraints;

        sphere = Mesh::createSphere(0.1);

        plane = Mesh::createPlane();
        plane->applyTransform(utils::getTranslateY(-1.5) * utils::getScale(5));

        const std::vector<glm::vec3> &v = plane->getVertices();
        semiPlane = new SemiPlane(v[0], v[1], v[2]);

        float size = 1;
        // body = TetraMesh::createCube(size);
        body = TetraMesh::createBunny();

        const std::vector<glm::vec3> &pos = body->getPos();
        const std::vector<unsigned int> &edges = body->getEdges();
        const std::vector<unsigned int> &tets = body->getTets();

        for (int i = 0; i < edges.size(); i += 2) {
            constraints.push_back(new DistanceConstraint(edges[i], edges[i + 1], glm::length(pos[edges[i]] - pos[edges[i + 1]])));
        }

        for (int i = 0; i < tets.size(); i += 4) {
            constraints.push_back(new VolumeConstraint(tets[i], tets[i + 1], tets[i + 2], tets[i + 3], pos));
        }

        for (int i = 0; i < pos.size(); i++) {
            constraints.push_back(new SemiPlaneConstraint(i, semiPlane, 0));
        }

        solver = new Solver(pos, constraints);
    }

    RigidBody(const RigidBody &scene) : RigidBody() {
    }

    ~RigidBody() override {
    }

    void draw(ShaderProgram &shaderProgram) override {
        body->udpatePos(solver->getPos());
        body->draw(shaderProgram, glm::vec3(0.7), glm::mat4(1.0));

        plane->draw(shaderProgram, glm::vec3(0.7), glm::mat4(1.0));
    }

    bool showUI() override {
        bool changed = false;

        return changed;
    }

private:
    std::shared_ptr<Mesh> sphere;
    std::shared_ptr<Mesh> plane;

    SemiPlane *semiPlane;

    std::shared_ptr<TetraMesh> body;
};