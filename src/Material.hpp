#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>
#include <memory>

#include "Mesh.hpp"
#include "Transformation.hpp"
#include "utils.hpp"

class Material {
public:
    Material()
        : color(0.3f), emissionColor(0.0f), emissionStrength(0.0f), smoothness(0.0f), reflexivity(0.0f), pos(0.0f), rotation(0.0f), size(1.0f) { genModel(); }

    Material(glm::vec3 color, Transformation tranfo)
        : color(color), emissionColor(0.0f), emissionStrength(0.0f), smoothness(0.0f), reflexivity(0.0f), pos(tranfo.position), rotation(tranfo.rotation), size(tranfo.scale) { genModel(); };

    Material(glm::vec3 color, glm::vec3 emiColor, float emissionStrength, Transformation tranfo)
        : color(color), emissionColor(emiColor), emissionStrength(emissionStrength), smoothness(0.0f), reflexivity(0.0f), pos(tranfo.position), rotation(tranfo.rotation), size(tranfo.scale) { genModel(); };

    Material(glm::vec3 color, glm::vec3 emiColor, float emissionStrength, float smoothness, float reflexivity, Transformation tranfo)
        : color(color), emissionColor(emiColor), emissionStrength(emissionStrength), smoothness(smoothness), reflexivity(reflexivity), pos(tranfo.position), rotation(tranfo.rotation), size(tranfo.scale) { genModel(); };

    void draw(unsigned int shaderProgram) { mesh->draw(shaderProgram, color, modelMat); };

    void genModel() { modelMat = utils::getTransfoMat(pos, size, rotation); }

    void setPos(const glm::vec3 &pos) {
        this->pos = pos;
        genModel();
    }
    void setSize(const glm::vec3 &size) {
        this->size = size;
        genModel();
    }
    void setSize(const float size_) {
        size = glm::vec3(size_);
        genModel();
    }
    void setRotation(const glm::vec3 &rotation) {
        this->rotation = rotation;
        genModel();
    }

    void setColor(const glm::vec3 &color) { this->color = color; }
    void setEmiColor(const glm::vec3 &color) { emissionColor = color; }
    void setSmoothness(const float val) { smoothness = val; }
    void setReflexivity(const float ratio) { reflexivity = ratio; }
    void setEmissionStrength(const float strength) { emissionStrength = strength; }

    const glm::vec3 &getColor() const { return color; }
    const glm::vec3 &getEmiColor() const { return emissionColor; }
    const glm::vec3 &getPos() const { return pos; }
    const glm::vec3 &getSize() const { return size; }
    const glm::vec3 &getRotation() const { return rotation; }
    const glm::mat4 &getModel() const { return modelMat; }
    float getSmoothness() const { return smoothness; }
    float getReflexivity() const { return reflexivity; }
    float getEmissionStrength() const { return emissionStrength; }

private:
    std::shared_ptr<Mesh> mesh;

    glm::vec3 color;
    glm::vec3 emissionColor;
    glm::vec3 pos;
    glm::vec3 rotation;
    glm::vec3 size;
    float emissionStrength;
    float smoothness;
    float reflexivity;

    glm::mat4 modelMat;
};

#endif // MATERIAL_HPP