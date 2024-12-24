#pragma once

#include "Mesh.hpp"

class TetraMesh : public Mesh {
public:
    TetraMesh(const std::vector<glm::vec3> &pos,
              const std::vector<uint> &meshToPos,
              const std::vector<uint> &edges,
              const std::vector<uint> &tets,
              const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<uint> &indices)
        : pos(pos), meshToPos(meshToPos), edges(edges), tets(tets), Mesh(vertices, normals, indices) {}

    const std::vector<glm::vec3> &getPos() { return pos; }
    const std::vector<uint> &getMeshToPos() const { return meshToPos; }
    const std::vector<uint> &getEdges() const { return edges; }
    const std::vector<uint> &getTets() const { return tets; }

    void udpatePos(const std::vector<glm::vec3> &pos);

    static std::shared_ptr<TetraMesh> createCube(float w = 1.0f);
    static std::shared_ptr<TetraMesh> createBunny();

private:
    std::vector<glm::vec3> pos;
    std::vector<uint> meshToPos;
    std::vector<uint> edges;
    std::vector<uint> tets;
};