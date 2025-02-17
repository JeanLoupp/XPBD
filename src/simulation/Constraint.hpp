#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "utils/utils.hpp"

// Virtual class to handle constraints
// Children must implement:
//   - eval: evaluate the constraint
//   - evalGrad: evaluate the gradient of the constraint
//   - evalNorm2Grad: compute the denominator of lambda in the solver

struct Constraint {
    std::vector<uint> particles;
    const float *alpha;
    virtual float eval(const std::vector<glm::vec3> &pos) const = 0;
    virtual std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const = 0;
    virtual float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const = 0;
    virtual bool isSatisfied(float val) const {
        return fabs(val) < 1e-3;
    }
};

// Distance between two particles is fixed
struct DistanceConstraint : public Constraint {
    const float l0;

    DistanceConstraint(uint i, uint j, float l0, const float *alpha) : l0(l0) {
        particles = {i, j};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        return glm::length(pos[particles[0]] - pos[particles[1]]) - l0;
        return (pos[particles[0]] - pos[particles[1]]).length() - l0;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        return {glm::normalize(pos[particles[0]] - pos[particles[1]]),
                glm::normalize(pos[particles[1]] - pos[particles[0]])};
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        return 1.0f * w[particles[0]] + 1.0f * w[particles[1]];
    }
};

// Particle pos is fixed
struct PositionConstraint : public Constraint {
    const glm::vec3 x0;

    PositionConstraint(uint i, const glm::vec3 &x0, const float *alpha) : x0(x0) {
        particles = {i};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        return glm::length(pos[particles[0]] - x0);
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        return {glm::normalize(pos[particles[0]] - x0)};
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        return 1.0f * w[particles[0]];
    }
};

// p: point of the plane
// n: normal of the plane
struct SemiPlane {
    glm::vec3 p, n;
    SemiPlane(const glm::vec3 &p, const glm::vec3 &n) : p(p), n(n) {}
    SemiPlane(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
        p = a;
        n = glm::normalize(glm::cross(b - a, c - a));
    }
};

// Stay above semi-plane
// dist: distance above plane (by default: 0)
struct SemiPlaneConstraint : public Constraint {
    SemiPlane *plane;
    const float dist;

    SemiPlaneConstraint(uint i, SemiPlane *plane, const float *alpha, float dist = 0.05f) : plane(plane), dist(dist) {
        particles = {i};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        return dot(pos[particles[0]] - plane->p, plane->n) - dist;
    }

    bool isSatisfied(float val) const override {
        return val >= 0;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        return {plane->n};
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        return 1.0f * w[particles[0]];
    }
};

struct BendingConstraint : public Constraint {
    const float angle;

    // Cache
    // mutable glm::vec3 n1, n2;
    // mutable float d;
    mutable glm::vec3 q1, q2, q3, q4;

    BendingConstraint(uint p1, uint p2, uint p3, uint p4, float angle, const float *alpha) : angle(angle) {
        particles = {p1, p2, p3, p4};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        const glm::vec3 &p1 = pos[particles[0]];
        const glm::vec3 &p2 = pos[particles[1]];
        const glm::vec3 &p3 = pos[particles[2]];
        const glm::vec3 &p4 = pos[particles[3]];

        glm::vec3 n1 = glm::cross(p2 - p1, p3 - p1);
        glm::vec3 n2 = glm::cross(p2 - p1, p4 - p1);

        float d = glm::dot(n1, n2);

        float norms = glm::length(n1) * glm::length(n2);

        if (norms > 1e-8) {
            d /= norms;
        }

        d = glm::clamp(d, -1.0f, 1.0f);

        return glm::acos(d) - angle;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        const glm::vec3 &p1 = pos[particles[0]];
        const glm::vec3 p2 = pos[particles[1]] - p1;
        const glm::vec3 p3 = pos[particles[2]] - p1;
        const glm::vec3 p4 = pos[particles[3]] - p1;

        glm::vec3 n1 = glm::normalize(glm::cross(p2, p3));
        glm::vec3 n2 = glm::normalize(glm::cross(p2, p4));

        float d = glm::dot(n1, n2);
        d = glm::clamp(d, -1.0f, 1.0f);

        if (d * d > 1 - 1e-8) return {glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0)};

        const float factor = 1.0f / sqrt(1.0f - d * d);

        q3 = factor * (glm::cross(p2, n2) + glm::cross(n1, p2) * d) / glm::length(glm::cross(p2, p3));
        q4 = factor * (glm::cross(p2, n1) + glm::cross(n2, p2) * d) / glm::length(glm::cross(p2, p4));
        q2 = -factor * ((glm::cross(p3, n2) + glm::cross(n1, p3) * d) / glm::length(glm::cross(p2, p3)) +
                        (glm::cross(p4, n1) + glm::cross(n2, p4) * d) / glm::length(glm::cross(p2, p4)));
        q1 = -q2 - q3 - q4;

        return {q1, q2, q3, q4};
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        return w[particles[0]] * glm::length2(q1) +
               w[particles[1]] * glm::length2(q2) +
               w[particles[2]] * glm::length2(q3) +
               w[particles[3]] * glm::length2(q4);
    }
};

// Distance between two particles is greater than l0
struct MinDistanceConstraint : public Constraint {
    const float l0;

    MinDistanceConstraint(uint i, uint j, float l0, const float *alpha) : l0(l0) {
        particles = {i, j};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        return glm::length(pos[particles[0]] - pos[particles[1]]) - l0;
    }

    bool isSatisfied(float val) const override {
        return val >= 0;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        return {glm::normalize(pos[particles[0]] - pos[particles[1]]),
                glm::normalize(pos[particles[1]] - pos[particles[0]])};
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        return 1.0f * w[particles[0]] + 1.0f * w[particles[1]];
    }
};

// Distance from p0 is greater than l0
struct SphereCollisionConstraint : public Constraint {
    glm::vec3 *p0;
    const float l0;

    SphereCollisionConstraint(uint i, glm::vec3 *p0, float l0, const float *alpha) : l0(l0), p0(p0) {
        particles = {i};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        return glm::length(pos[particles[0]] - *p0) - l0;
    }

    bool isSatisfied(float val) const override {
        return val >= 0;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        return {glm::normalize(pos[particles[0]] - *p0)};
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        return 1.0f * w[particles[0]];
    }
};

// dir: direction of cylinder
// p: point belonging to axis
// r: radius
struct Cylinder {
    glm::vec3 dir, p;
    float r;

    Cylinder(const glm::vec3 &dir, const glm::vec3 &p, float r) : dir(dir), p(p), r(r) {}
};

struct CylinderCollisionConstraint : public Constraint {
    Cylinder *cylinder;

    CylinderCollisionConstraint(uint i, Cylinder *cylinder, const float *alpha) : cylinder(cylinder) {
        particles = {i};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        float t = -glm::dot(cylinder->p - pos[particles[0]], cylinder->dir);
        return glm::length((cylinder->p + t * cylinder->dir) - pos[particles[0]]) - cylinder->r;
    }

    bool isSatisfied(float val) const override {
        return val >= 0;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        float t = -glm::dot(cylinder->p - pos[particles[0]], cylinder->dir);
        return {glm::normalize(pos[particles[0]] - (cylinder->p + t * cylinder->dir))};
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        return 1.0f * w[particles[0]];
    }
};

// Distance from triangle abc from p0 is greater than l0
struct SphereTriCollisionConstraint : public Constraint {
    glm::vec3 *p0;
    const float l0;

    mutable glm::vec3 hitPoint;

    SphereTriCollisionConstraint(uint a, uint b, uint c, glm::vec3 *p0, float l0, const float *alpha) : l0(l0), p0(p0) {
        particles = {a, b, c};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        const glm::vec3 &a = pos[particles[0]];
        const glm::vec3 &b = pos[particles[1]];
        const glm::vec3 &c = pos[particles[2]];

        glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));

        glm::vec3 Pproj = *p0 - glm::dot(n, *p0 - a) * n;

        // Test if projected point is outside triangle
        for (int i = 0; i < 3; i++) {
            const glm::vec3 &orig = pos[particles[i]];
            const glm::vec3 &dest = pos[particles[(i + 1) % 3]];

            bool inside = glm::dot(glm::cross(dest - orig, Pproj - orig), n) > 0;

            if (!inside) {
                // Return closest distance to segment or point
                float t = -glm::dot(orig - *p0, dest - orig) / glm::length2(dest - orig);

                // If not in [0, 1]: closest to point
                t = glm::clamp(t, 0.0f, 1.0f);

                glm::vec3 closest = orig + t * (dest - orig);

                hitPoint = closest;

                return glm::length(*p0 - closest) - l0;
            }
        }

        // Projected point is inside triangle
        hitPoint = Pproj;
        return glm::length(*p0 - Pproj) - l0;
    }

    bool isSatisfied(float val) const override {
        return val >= 0;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        glm::vec3 dir = glm::normalize(hitPoint - *p0);
        return {dir, dir, dir};
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        return 1.0f * w[particles[0]] + 1.0f * w[particles[1]] + 1.0f * w[particles[2]];
    }
};

struct VolumeConstraint : public Constraint {
    float initialVolume;

    // Cache
    mutable std::vector<glm::vec3> gradients;

    VolumeConstraint(uint p1, uint p2, uint p3, uint p4, const std::vector<glm::vec3> &pos, const float *alpha) {
        particles = {p1, p2, p3, p4};
        gradients.resize(4, glm::vec3(0));
        initialVolume = calculateVolume(pos);
        this->alpha = alpha;
    }

    float calculateVolume(const std::vector<glm::vec3> &pos) const {
        const glm::vec3 &p1 = pos[particles[0]];
        const glm::vec3 &p2 = pos[particles[1]];
        const glm::vec3 &p3 = pos[particles[2]];
        const glm::vec3 &p4 = pos[particles[3]];

        return glm::dot(glm::cross(p2 - p1, p3 - p1), p4 - p1);
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        float volume = calculateVolume(pos);
        return (volume - initialVolume);
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        const glm::vec3 &p1 = pos[particles[0]];
        const glm::vec3 &p2 = pos[particles[1]];
        const glm::vec3 &p3 = pos[particles[2]];
        const glm::vec3 &p4 = pos[particles[3]];

        glm::vec3 v1 = p2 - p1;
        glm::vec3 v2 = p3 - p1;
        glm::vec3 v3 = p4 - p1;

        gradients[0] = glm::cross(p4 - p2, p3 - p2);
        gradients[1] = glm::cross(v2, v3);
        gradients[2] = glm::cross(v3, v1);
        gradients[3] = glm::cross(v1, v2);

        return gradients;
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        float norm2 = 0.0f;
        for (size_t i = 0; i < gradients.size(); ++i) {
            norm2 += w[particles[i]] * glm::length2(gradients[i]);
        }
        return norm2;
    }
};

// TODO: only works with one object
struct MeshVolumeConstraint : public Constraint {
    float initialVolume;
    const float *k; // pressure
    std::vector<uint> indices;
    uint startIndex;

    // Cache
    mutable std::vector<glm::vec3> gradients;

    // Base pressure: 1.0f
    MeshVolumeConstraint(const std::vector<uint> &indices, const std::vector<glm::vec3> &pos, float *pressure, const float *alpha, uint startIndex = 0)
        : indices(indices), k(pressure), startIndex(startIndex) {
        particles.reserve(pos.size());
        for (int i = 0; i < pos.size(); i++) {
            particles[i] = i + startIndex;
        }

        gradients.resize(pos.size(), glm::vec3(0));
        initialVolume = calculateVolume(pos);

        for (int i = 0; i < indices.size(); i++) {
            this->indices[i] += startIndex;
        }
        this->alpha = alpha;
    }

    float calculateVolume(const std::vector<glm::vec3> &pos) const {
        float V = 0;

        for (int i = 0; i < indices.size(); i += 3) {
            V += glm::dot(glm::cross(pos[indices[i]], pos[indices[i + 1]]), pos[indices[i + 2]]);
        }
        return V;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        float volume = calculateVolume(pos);
        return volume - (*k) * initialVolume;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        std::fill(gradients.begin(), gradients.end(), glm::vec3(0));

        for (int i = 0; i < indices.size(); i += 3) {
            const glm::vec3 &p1 = pos[indices[i]];
            const glm::vec3 &p2 = pos[indices[i + 1]];
            const glm::vec3 &p3 = pos[indices[i + 2]];

            gradients[indices[i] - startIndex] += glm::cross(p2, p3);
            gradients[indices[i + 1] - startIndex] += glm::cross(p3, p1);
            gradients[indices[i + 2] - startIndex] += glm::cross(p1, p2);
        }

        return gradients;
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        float norm2 = 0.0f;
        for (size_t i = 0; i < gradients.size(); ++i) {
            norm2 += w[i] * glm::length2(gradients[i]);
        }
        return norm2;
    }
};

struct DensityConstraint : public Constraint {
    inline static float d0 = 1000;
    inline static float h = 0.02;
    inline static float m = 0.001;

    uint p0;

    // Cache
    mutable std::vector<glm::vec3> gradients;

    DensityConstraint(uint p0, const float *alpha)
        : p0(p0) {
        this->alpha = alpha;
    }

    static float W_poly6(const glm::vec3 &p1, const glm::vec3 &p2) {
        float r = glm::length(p1 - p2);
        if (r > h) return 0;

        return 315.0f / (64.0f * M_PI * pow(h, 9)) * pow(h * h - r * r, 3);
    }

    static glm::vec3 gW_spiky(const glm::vec3 &p1, const glm::vec3 &p2) {
        float r = glm::length(p1 - p2);
        if (r > h) return glm::vec3(0);

        return (float)(-45.0f / (M_PI * pow(h, 6)) * pow(h - r, 2)) * (p1 - p2) / r;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        float d = 0;

        for (int j : particles) {
            d += m * W_poly6(pos[p0], pos[j]);
        }

        return d - d0;
    }

    bool isSatisfied(float val) const override {
        return val <= 0;
    }

    std::vector<glm::vec3> evalGrad(const std::vector<glm::vec3> &pos) const override {
        if (gradients.size() != particles.size()) {
            gradients.resize(particles.size());
        }

        gradients[0] = glm::vec3(0);

        for (int i = 1; i < particles.size(); i++) {
            gradients[i] = -m * gW_spiky(pos[p0], pos[particles[i]]);
            gradients[0] += -gradients[i];
        }

        return gradients;
    }

    float evalNorm2Grad(const std::vector<glm::vec3> &pos, const std::vector<float> &w) const override {
        float norm2 = 0.0f;
        for (size_t i = 0; i < gradients.size(); ++i) {
            norm2 += w[particles[i]] * glm::length2(gradients[i]);
        }
        return norm2;
    }
};
