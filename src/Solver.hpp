#pragma once
#include "Constraint.hpp"

class Solver {
public:
    int N_ITERATION = 20;

    Solver(const std::vector<glm::vec3> &pos, const std::vector<Constraint *> &constraints);
    ~Solver();

    void update(const float dt);
    const std::vector<glm::vec3> &getPos() { return x; }

    void addFixedPoint(int index);
    void addFixedPoint(int index, const glm::vec3 &pos);
    void setPos(int index, const glm::vec3 &pos);

    void removeFixedPoint(int index);

private:
    unsigned int nParticles;
    unsigned int nConstraints;
    std::vector<glm::vec3> x;
    std::vector<glm::vec3> v;
    std::vector<Constraint *> C;
    std::vector<float> w; // inverse of mass
};