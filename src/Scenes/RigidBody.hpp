#pragma once
#include "Scene.hpp"
#include "../utils.hpp"
#include "imgui.h"

#include "../RigidMesh.hpp"

#include <ctime>

class RigidBody : public Scene {
public:
    float alphaDistance = 1e-8;
    float alphaVolume = 1e-8;
    float alphaCollision = 1e-8;

    RigidBody() {

        std::vector<Constraint *> constraints;

        plane = Mesh::createPlane();
        plane->applyTransform(utils::getTranslateY(-1.5) * utils::getScale(500));

        const std::vector<glm::vec3> &v = plane->getVertices();
        semiPlane = new SemiPlane(v[0], v[1], v[2]);

        int res = 10;
        body = RigidMesh::createCube(res);

        sphere = Mesh::createSphere(1.5 / res);

        const std::vector<glm::vec3> &pos = body->getPos();

        for (int i = 0; i < pos.size(); i++) {
            constraints.push_back(new SemiPlaneConstraint(i, semiPlane, &alphaCollision));
        }

        solver = new Solver(pos, constraints);
    }

    RigidBody(const RigidBody &scene) : RigidBody() {
        this->alphaCollision = scene.alphaCollision;
        this->alphaVolume = scene.alphaVolume;
        this->alphaDistance = scene.alphaDistance;
    }

    ~RigidBody() override {
        delete semiPlane;
    }

    void draw(ShaderProgram &shaderProgram, ShaderProgram &checkerShaderProgram, ShadowMap &shadowMap) override {
        shadowMap.beginRender();
        shadowMap.addObject(body);
        shadowMap.endRender();

        shaderProgram.use();
        body->shapeMatch(solver->getPos());
        solver->setPos(body->getPos());

        if (!showSpheres)
            body->draw(shaderProgram, glm::vec3(0.7, 0, 0), glm::mat4(1.0));
        else
            for (const glm::vec3 &pos : body->getPos()) {
                sphere->draw(shaderProgram, glm::vec3(0.7, 0, 0), utils::getTranslate(pos));
            }

        checkerShaderProgram.use();
        shadowMap.sendShadowMap(checkerShaderProgram);
        plane->draw(checkerShaderProgram, glm::vec3(0.7), glm::mat4(1.0));
    }

    bool showUI() override {
        bool changed = false;

        ImGui::Checkbox("Show spheres", &showSpheres);

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

    std::shared_ptr<RigidMesh> body;
    std::shared_ptr<Mesh> sphere;

    bool showSpheres = false;
};