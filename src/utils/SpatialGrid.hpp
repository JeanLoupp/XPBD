// Helper class to create a spatial hash to accelerate collision detection
// To use:
//    - Define h as the dimension of each cell
//    - Add particles to the grid with addParticles
//    - Use grid to get the neighbors

#pragma once

#include <unordered_map>
#include <tuple>
#include <vector>
#include <glm/glm.hpp>

struct Coordinates3D {
    int x, y, z;

    Coordinates3D(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
    Coordinates3D(glm::vec3 pos) : x(pos.x), y(pos.y), z(pos.z) {}
    // Comparison operator for 3D coordinates
    bool operator==(const Coordinates3D &other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

namespace std {
template <>
struct hash<Coordinates3D> {
    size_t operator()(const Coordinates3D &coord) const {
        return std::hash<int>()(coord.x) ^ std::hash<int>()(coord.y) ^ std::hash<int>()(coord.z);
    }
};
} // namespace std

class SpatialGrid {
private:
    float h;

public:
    std::unordered_map<Coordinates3D, std::vector<uint>> grid;

    SpatialGrid(float h_) : h(h_) {}

    void clear() { grid.clear(); }
    void addParticle(const glm::vec3 &pos, uint index) {
        Coordinates3D coord(pos / h);
        grid[coord].push_back(index);
    }
};