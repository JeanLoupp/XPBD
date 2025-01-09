#pragma once
#include "Scene.hpp"
#include "utils/utils.hpp"
#include "imgui.h"

#include "mesh/TetraMesh.hpp"

#include <ctime>

class SoftBody : public Scene {
public:
    float alphaDistance = 1e-8;
    float alphaVolume = 1e-8;
    float alphaCollision = 1e-8;

    SoftBody() {

        std::vector<Constraint *> constraints;

        plane = Mesh::createPlane();
        plane->applyTransform(utils::getTranslateY(-1.5) * utils::getScale(500));

        const std::vector<glm::vec3> &v = plane->getVertices();
        semiPlane = new SemiPlane(v[0], v[1], v[2]);

        float size = 1;
        // body = TetraMesh::createCube(size);
        body = TetraMesh::createBunny();

        const std::vector<glm::vec3> &pos = body->getPos();
        const std::vector<uint> &edges = body->getEdges();
        const std::vector<uint> &tets = body->getTets();

        for (int i = 0; i < edges.size(); i += 2) {
            constraints.push_back(new DistanceConstraint(edges[i], edges[i + 1], glm::length(pos[edges[i]] - pos[edges[i + 1]]), &alphaDistance));
        }

        for (int i = 0; i < tets.size(); i += 4) {
            constraints.push_back(new VolumeConstraint(tets[i], tets[i + 1], tets[i + 2], tets[i + 3], pos, &alphaVolume));
        }

        for (int i = 0; i < pos.size(); i++) {
            constraints.push_back(new SemiPlaneConstraint(i, semiPlane, &alphaCollision, 0));
        }

        solver = new Solver(pos, constraints);
    }

    SoftBody(const SoftBody &scene) : SoftBody() {
        this->alphaCollision = scene.alphaCollision;
        this->alphaVolume = scene.alphaVolume;
        this->alphaDistance = scene.alphaDistance;
    }

    ~SoftBody() override {
        delete semiPlane;
    }

    void draw(ShaderProgram &shaderProgram, ShaderProgram &checkerShaderProgram, ShadowMap &shadowMap) override {
        shadowMap.beginRender();
        shadowMap.addObject(body);
        shadowMap.endRender();

        shaderProgram.use();
        body->udpatePos(solver->getPos());
        body->draw(shaderProgram, glm::vec3(0.7, 0, 0), glm::mat4(1.0));

        checkerShaderProgram.use();
        shadowMap.sendShadowMap(checkerShaderProgram);
        plane->draw(checkerShaderProgram, glm::vec3(0.7), glm::mat4(1.0));
    }

    bool showUI() override {
        bool changed = false;

        return changed;
    }

    void showConstraintUI() override {
        alphaSelector("Distance", alphaDistance);
        alphaSelector("Volume", alphaVolume);
        alphaSelector("Collision", alphaCollision);
    }

private:
    std::shared_ptr<Mesh> plane;

    SemiPlane *semiPlane;

    std::shared_ptr<TetraMesh> body;
};