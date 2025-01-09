#pragma once

#include <memory>
#include "render/ShaderProgram.hpp"
#include "simulation/Solver.hpp"
#include "mesh/Mesh.hpp"
#include "render/ShadowMap.hpp"
#include "imgui.h"

class Scene {
public:
    Solver *solver;

public:
    virtual ~Scene() { delete solver; }

    virtual void draw(ShaderProgram &shaderProgram, ShaderProgram &checkerShaderProgram, ShadowMap &shadowMap) = 0;
    virtual bool showUI() { return false; }
    virtual void showConstraintUI() {}

    void update(const float dt) { solver->update(dt); };

    const std::vector<glm::vec3> &getPos() { return solver->getPos(); }
};

inline void alphaSelector(const char *label, float &alpha) {
    ImGui::Text("%s", label);

    int powerOf10 = floorf(log10f(alpha));
    float multiplier = alpha / pow(10.0f, powerOf10);

    ImGui::PushID(label); // Ajouter un ID unique pour éviter les conflits de widgets

    // Limiter la largeur des champs pour que tout tienne dans la ligne
    float availableWidth = ImGui::GetContentRegionAvail().x; // Largeur disponible
    float multiplierWidth = availableWidth * 0.4f;           // 40% pour le multiplicateur
    float powerWidth = availableWidth * 0.2f;                // 20% pour la puissance de 10

    ImGui::SetNextItemWidth(multiplierWidth);
    if (ImGui::DragFloat("##multiplier", &multiplier, 0.1f, 0.1f, 10.0f, "%.2f")) {
        if (multiplier >= 10.0f) {
            multiplier /= 10.0f;
            powerOf10 += 1;
        } else if (multiplier < 1.0f) {
            multiplier *= 10.0f;
            powerOf10 -= 1;
        }
    }

    ImGui::SameLine();
    ImGui::Text("x 10^");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(powerWidth);
    ImGui::DragInt("##powerOf10", &powerOf10, 1, -16, 1);

    ImGui::PopID(); // Restaurer l'ID

    if (powerOf10 < -16) {
        alpha = 1e-16;
    } else if (powerOf10 > 0) {
        alpha = 10;
    } else {
        alpha = multiplier * pow(10.0f, powerOf10);
    }
}
