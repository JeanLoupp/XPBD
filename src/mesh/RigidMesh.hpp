// Helper class that adds an original shape for an objet. Used for rigid bodies.

#pragma once

#include "Mesh.hpp"

class RigidMesh : public Mesh {
public:
    RigidMesh(const std::vector<glm::vec3> &pos, const std::vector<uint> &meshToPos, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<uint> &indices)
        : pos(pos), originalPos(pos), meshToPos(meshToPos), originalCOM(computeCOM(pos)), Mesh(vertices, normals, indices) {}

    void applyTransform(const glm::mat4 &mat);
    void shapeMatch(const std::vector<glm::vec3> &pos);

    const std::vector<glm::vec3> &getPos() { return pos; }
    const std::vector<uint> &getMeshToPos() const { return meshToPos; }

    std::vector<uint> generateEdges();

    static std::shared_ptr<RigidMesh> createCube(int resolution, float w = 1.0f);
    static std::shared_ptr<RigidMesh> createFromOFF(const std::string &filePath);

    static glm::vec3 computeCOM(const std::vector<glm::vec3> pos);

private:
    std::vector<glm::vec3> originalPos;
    std::vector<glm::vec3> pos;
    std::vector<uint> meshToPos;
    glm::vec3 originalCOM;
};