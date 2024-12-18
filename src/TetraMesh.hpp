#pragma once

#include "Mesh.hpp"

class TetraMesh : public Mesh {
public:
    TetraMesh(const std::vector<glm::vec3> &pos,
              const std::vector<unsigned int> &meshToPos,
              const std::vector<unsigned int> &edges,
              const std::vector<unsigned int> &tets,
              const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<unsigned int> &indices)
        : pos(pos), meshToPos(meshToPos), edges(edges), tets(tets), Mesh(vertices, normals, indices) {}

    const std::vector<glm::vec3> &getPos() { return pos; }
    const std::vector<unsigned int> &getMeshToPos() const { return meshToPos; }
    const std::vector<unsigned int> &getEdges() const { return edges; }
    const std::vector<unsigned int> &getTets() const { return tets; }

    void udpatePos(const std::vector<glm::vec3> &pos);

    static std::shared_ptr<TetraMesh> createCube(float w = 1.0f);
    static std::shared_ptr<TetraMesh> createBunny();

private:
    std::vector<glm::vec3> pos;
    std::vector<unsigned int> meshToPos;
    std::vector<unsigned int> edges;
    std::vector<unsigned int> tets;
};