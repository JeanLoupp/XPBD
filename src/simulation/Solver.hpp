#pragma once
#include "simulation/Constraint.hpp"

class Solver {
public:
    int N_ITERATION = 20;

    Solver(const std::vector<glm::vec3> &pos, const std::vector<Constraint *> &constraints);
    ~Solver();

    void activateGlobalCollision(float h, float *alphaCollision);

    void update(const float dt);
    const std::vector<glm::vec3> &getPos() { return x; }

    void addFixedPoint(int index);
    void addFixedPoint(int index, const glm::vec3 &pos);
    void setPos(int index, const glm::vec3 &pos);
    void setPos(const std::vector<glm::vec3> &pos);

    void removeFixedPoint(int index);

private:
    uint nParticles;
    uint nConstraints;
    std::vector<glm::vec3> x;
    std::vector<glm::vec3> v;
    std::vector<Constraint *> C;
    std::vector<float> w; // inverse of mass

    void generateCollisionConstraints();
    void cleanCollisionConstraints();
    void applyFriction(std::vector<glm::vec3> &nextX, const float dt);

    bool useGlobalCollision = false;
    float hCollision = 1.0f;
    float *alphaCollision;
};