#pragma once

#include <memory>
#include "../ShaderProgram.hpp"
#include "../Solver.hpp"
#include "../Mesh.hpp"

class Scene {
public:
    Solver *solver;

public:
    ~Scene() { delete solver; }

    virtual void draw(ShaderProgram &shaderProgram) = 0;
    virtual bool showUI() { return false; }

    void update(const float dt) { solver->update(dt); };

    const std::vector<glm::vec3> &getPos() { return solver->getPos(); }
};
