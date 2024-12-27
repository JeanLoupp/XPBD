#pragma once
#include "Scene.hpp"
#include "../utils.hpp"
#include "imgui.h"
#include <set>

class SoftBall : public Scene {
public:
    float alphaDistance = 1e-8;
    float alphaVolume = 1e-8;
    float alphaCollision = 1e-8;

    SoftBall() {
        std::vector<Constraint *> constraints;

        plane = Mesh::createPlane();
        plane->applyTransform(utils::getTranslateY(-1.5) * utils::getScale(500));

        const std::vector<glm::vec3> &v = plane->getVertices();
        semiPlane = new SemiPlane(v[0], v[1], v[2]);

        ball = Mesh::createFromOFF("data/mesh/sphere_one_sub.off");

        const std::vector<glm::vec3> &pos = ball->getVertices();
        const std::vector<uint> &indices = ball->getIndices();

        constraints.push_back(new MeshVolumeConstraint(indices, pos, &alphaVolume));

        // Compute unique edges
        std::set<std::pair<uint, uint>> edgeSet;

        for (int i = 0; i < indices.size(); i += 3) {
            uint a = indices[i];
            uint b = indices[i + 1];
            uint c = indices[i + 2];

            edgeSet.insert({std::min(a, b), std::max(a, b)});
            edgeSet.insert({std::min(a, c), std::max(a, c)});
            edgeSet.insert({std::min(b, c), std::max(b, c)});
        }

        for (const auto &edge : edgeSet) {
            const glm::vec3 &p1 = pos[edge.first];
            const glm::vec3 &p2 = pos[edge.second];
            constraints.push_back(new DistanceConstraint(edge.first, edge.second, glm::length(p1 - p2), &alphaDistance));
        }

        for (int i = 0; i < pos.size(); i++) {
            constraints.push_back(new SemiPlaneConstraint(i, semiPlane, &alphaCollision));
        }

        solver = new Solver(pos, constraints);
    }

    SoftBall(const SoftBall &scene) : SoftBall() {
        this->alphaCollision = scene.alphaCollision;
        this->alphaVolume = scene.alphaVolume;
        this->alphaDistance = scene.alphaDistance;
    }

    ~SoftBall() override {
        delete semiPlane;
    }

    void draw(ShaderProgram &shaderProgram, ShaderProgram &checkerShaderProgram, ShadowMap &shadowMap) override {
        shadowMap.beginRender();
        shadowMap.addObject(ball);
        shadowMap.endRender();

        shaderProgram.use();
        ball->setVertices(solver->getPos());
        ball->updateNormals();
        ball->draw(shaderProgram, glm::vec3(0, 0, 0.7), glm::mat4(1.0));

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
    std::shared_ptr<Mesh> ball;

    SemiPlane *semiPlane;
};