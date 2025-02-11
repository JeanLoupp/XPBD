#include "Solver.hpp"
#include "utils/utils.hpp"
#include "utils/SpatialGrid.hpp"

Solver::Solver(const std::vector<glm::vec3> &pos, const std::vector<Constraint *> &constraints, float mass)
    : x(pos), nParticles(pos.size()), C(constraints), nConstraints(constraints.size()) {
    v = std::vector<glm::vec3>(nParticles, glm::vec3(0));
    w = std::vector<float>(pos.size(), 1.0f / mass);
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
    // TODO: put previous mass instead of default
    w[index] = 1.0f / 0.1f; // infinite mass
}

void Solver::setPos(int index, const glm::vec3 &p) {
    x[index] = p;
}

void Solver::setPos(const std::vector<glm::vec3> &p) {
    x = p;
}

void Solver::update(const float dt) {

    std::vector<glm::vec3> nextX(x.size());
    std::vector<float> lambda(nConstraints, 0);

    const glm::vec3 g(0, -9.81, 0);

    // Predict
    for (int i = 0; i < nParticles; i++) {
        if (w[i] != 0)
            nextX[i] = x[i] + dt * v[i] + dt * dt * g;
        else
            nextX[i] = x[i];
    }

    if (useRigid) {
        rigidMesh->shapeMatch(nextX);
        nextX = rigidMesh->getPos();
    }

    generateCollisionConstraints();
    generateFluidNeighbors();

    for (int n = 0; n < N_ITERATION; n++) {
        // Solve constraints
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

            if (useRigid) {
                rigidMesh->shapeMatch(nextX);
                nextX = rigidMesh->getPos();
            }
        }
    }

    // Update
    for (int i = 0; i < nParticles; i++) {
        v[i] = (nextX[i] - x[i]) / dt;
        x[i] = nextX[i];
    }

    cleanCollisionConstraints();
}

void Solver::updateSubsteps(const float dt_) {

    std::vector<glm::vec3> nextX(nParticles);

    const float dt = dt_ / N_ITERATION;

    const glm::vec3 g(0, -9.81, 0);

    float vmax = useGlobalCollision ? hCollision / 4.0f / dt : MAXFLOAT;

    generateCollisionConstraints();
    generateFluidNeighbors();

    for (int n = 0; n < N_ITERATION; n++) {
        // Predict
        for (int i = 0; i < nParticles; i++) {
            if (w[i] != 0)
                nextX[i] = x[i] + dt * v[i] + dt * dt * g;
            else
                nextX[i] = x[i];
        }

        const float beta = 0.05;

        // Solve constraints
        for (int j = 0; j < C.size(); j++) {
            float C_val = C[j]->eval(nextX);
            if (C[j]->isSatisfied(C_val)) continue; // constraint already satisfied

            std::vector<glm::vec3> grad = C[j]->evalGrad(nextX);
            float normGrad = C[j]->evalNorm2Grad(nextX, w);

            const float alpha = *(C[j]->alpha) / (dt * dt);

            // damping
            const float gamma = beta * alpha / dt;

            float correction = 0.0f;
            for (int i = 0; i < grad.size(); i++) {
                int index = C[j]->particles[i];
                correction += dot(grad[i], nextX[index] - x[index]);
            }

            float dlambda = (-C_val - gamma * correction) / ((1.0 + gamma) * normGrad + alpha);

            for (int i = 0; i < grad.size(); i++) {
                int index = C[j]->particles[i];
                nextX[index] += dlambda * w[index] * grad[i];
            }
        }

        applyFriction(nextX, dt);

        if (useRigid) {
            rigidMesh->shapeMatch(nextX);
            nextX = rigidMesh->getPos();
        }

        // Update
        for (int i = 0; i < nParticles; i++) {
            v[i] = (nextX[i] - x[i]) / dt;
            if (useGlobalCollision) {
                float norm = glm::length(v[i]);
                if (norm > vmax) {
                    v[i] *= vmax / norm;
                    x[i] += v[i] * dt;
                } else {
                    x[i] = nextX[i];
                }
            } else {
                x[i] = nextX[i];
            }
        }
    }

    cleanCollisionConstraints();
}

void Solver::generateCollisionConstraints() {
    if (!useGlobalCollision) return;

    SpatialGrid spatialGrid(hCollision);

    for (int i = 0; i < x.size(); ++i) {
        spatialGrid.addParticle(x[i], i);
    }

    auto &grid = spatialGrid.grid;

    for (const auto &[cell, particles] : grid) {
        // Go through 3x3x3 cells
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dz = -1; dz <= 1; ++dz) {
                    Coordinates3D neighborCell(cell.x + dx, cell.y + dy, cell.z + dz);

                    if (grid.find(neighborCell) != grid.end()) {
                        const auto &neighborParticles = grid[neighborCell];

                        for (uint p1 : particles) {
                            for (uint p2 : neighborParticles) {
                                // Add constraint once
                                if (p1 < p2) {
                                    C.push_back(new MinDistanceConstraint(p1, p2, hCollision, alphaCollision));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Solver::cleanCollisionConstraints() {
    if (!useGlobalCollision) return;

    for (int i = nConstraints; i < C.size(); i++) {
        delete C[i];
    }

    C.resize(nConstraints);
}

void Solver::activateGlobalCollision(float h, float *alphaCollision) {
    useGlobalCollision = true;
    hCollision = h;
    this->alphaCollision = alphaCollision;
}

void Solver::applyFriction(std::vector<glm::vec3> &nextX, const float dt) {
    if (!useGlobalCollision) return;

    float d = 20 * dt;

    for (int i = nConstraints; i < C.size(); i++) {
        int p1 = C[i]->particles[0];
        int p2 = C[i]->particles[1];
        glm::vec3 v1 = (nextX[p1] - x[p1]);
        glm::vec3 v2 = (nextX[p2] - x[p2]);

        glm::vec3 v_avg = (v1 + v2) / 2.0f;

        nextX[p1] = nextX[p1] + d * (v_avg - v1);
        nextX[p2] = nextX[p2] + d * (v_avg - v2);
    }
}

void Solver::activateFluids() {
    useFluids = true;
}

void Solver::generateFluidNeighbors() {
    if (!useFluids) return;

    SpatialGrid spatialGrid(DensityConstraint::h);

    for (uint i = 0; i < x.size(); ++i) {
        spatialGrid.addParticle(x[i], i);
        C[i]->particles = {i};
    }

    auto &grid = spatialGrid.grid;

    for (const auto &[cell, particles] : grid) {
        // Go through 3x3x3 cells
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dz = -1; dz <= 1; ++dz) {
                    Coordinates3D neighborCell(cell.x + dx, cell.y + dy, cell.z + dz);

                    if (grid.find(neighborCell) != grid.end()) {
                        const auto &neighborParticles = grid[neighborCell];

                        for (uint p1 : particles) {
                            for (uint p2 : neighborParticles) {
                                if (p1 == p2) continue;
                                C[p1]->particles.push_back(p2);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Solver::activateRigid(RigidMesh *mesh) {
    useRigid = true;
    rigidMesh = mesh;
}