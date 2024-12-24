#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "utils.hpp"

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
    float l0;

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
    glm::vec3 x0;

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
    float dist;

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
    float angle;

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
    float l0;

    MinDistanceConstraint(uint i, uint j, float l0, const float *alpha) : l0(l0) {
        particles = {i, j};
        this->alpha = alpha;
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        return glm::length(pos[particles[0]] - pos[particles[1]]) - l0;
        return (pos[particles[0]] - pos[particles[1]]).length() - l0;
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

struct VolumeConstraint : public Constraint {
    float initialVolume;

    // Cache
    mutable std::vector<glm::vec3> gradients;

    VolumeConstraint(uint p1, uint p2, uint p3, uint p4, const std::vector<glm::vec3> &pos, const float *alpha) {
        particles = {p1, p2, p3, p4};
        gradients.resize(4, glm::vec3(0));
        initialVolume = calculateInitialVolume(pos);
        this->alpha = alpha;
    }

    float calculateInitialVolume(const std::vector<glm::vec3> &pos) const {
        const glm::vec3 &p1 = pos[particles[0]];
        const glm::vec3 &p2 = pos[particles[1]];
        const glm::vec3 &p3 = pos[particles[2]];
        const glm::vec3 &p4 = pos[particles[3]];

        return glm::dot(glm::cross(p2 - p1, p3 - p1), p4 - p1);
    }

    float eval(const std::vector<glm::vec3> &pos) const override {
        const glm::vec3 &p1 = pos[particles[0]];
        const glm::vec3 &p2 = pos[particles[1]];
        const glm::vec3 &p3 = pos[particles[2]];
        const glm::vec3 &p4 = pos[particles[3]];

        float volume = glm::dot(glm::cross(p2 - p1, p3 - p1), p4 - p1);
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

    bool isSatisfied(float val) const override {
        return fabs(val) < 1e-3;
    }
};
