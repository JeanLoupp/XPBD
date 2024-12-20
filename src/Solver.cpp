#include "Solver.hpp"
#include "utils.hpp"

Solver::Solver(const std::vector<glm::vec3> &pos, const std::vector<Constraint *> &constraints)
    : x(pos), nParticles(pos.size()), C(constraints), nConstraints(constraints.size()) {
    v = std::vector<glm::vec3>(nParticles, glm::vec3(0));
    w = std::vector<float>(pos.size(), 1.0f / 0.1f);
}

Solver::~Solver() {
    for (Constraint *constraint : C) {
        delete constraint;
    }
    C.clear();
}

void Solver::addFixedPoint(int index) {
    w[index] = 0; // infinite mass
}

void Solver::addFixedPoint(int index, const glm::vec3 &pos) {
    w[index] = 0; // infinite mass
    x[index] = pos;
}

void Solver::removeFixedPoint(int index) {
    w[index] = 1.0f / 0.1f; // infinite mass
}

void Solver::setPos(int index, const glm::vec3 &p) {
    x[index] = p;
}

void Solver::update(const float dt) {

    std::vector<glm::vec3> nextX(x.size());
    std::vector<float> lambda(nConstraints, 0);

    const glm::vec3 g(0, -9.81, 0);

    for (int i = 0; i < nParticles; i++) {
        if (w[i] != 0)
            nextX[i] = x[i] + dt * v[i] + dt * dt * g;
        else
            nextX[i] = x[i];
    }

    for (int i = 0; i < N_ITERATION; i++) {
        for (int j = 0; j < nConstraints; j++) {
            float C_val = C[j]->eval(nextX);
            if (C[j]->isSatisfied(C_val)) continue; // constraint already satisfied

            std::vector<glm::vec3> grad = C[j]->evalGrad(nextX);
            float normGrad = C[j]->evalNorm2Grad(nextX, w);

            const float alpha = *(C[j]->alpha) / (dt * dt);

            float dlambda = (-C_val - alpha * lambda[j]) / (normGrad + alpha);

            lambda[j] += dlambda;

            for (int i = 0; i < grad.size(); i++) {
                int index = C[j]->particles[i];
                nextX[index] += dlambda * w[index] * grad[i];
            }
        }
    }

    for (int i = 0; i < nParticles; i++) {
        v[i] = (nextX[i] - x[i]) / dt;
        x[i] = nextX[i];
    }
}