// Helper class to load, send to GPU and transform a mesh

#ifndef MESH_HPP
#define MESH_HPP

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include "render/ShaderProgram.hpp"

class Mesh {
public:
    Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<uint> &indices, const std::string &name = "");
    Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &textures, const std::vector<uint> &indices, const std::string &name = "");
    ~Mesh();

    void draw(uint shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat);
    void draw(ShaderProgram &shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat);

    void startDrawMultiple(ShaderProgram &shaderProgram);
    void addDrawMultiple(ShaderProgram &shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat);
    void endDrawMultiple();

    void updateNormals();
    void updateVertices();

    void setVertices(const std::vector<glm::vec3> &vertices);
    void applyTransform(const glm::mat4 &mat);

    static std::shared_ptr<Mesh> createCube(float w = 1.0f);
    static std::shared_ptr<Mesh> createSphere(float radius = 1.0f, int resolution = 16);
    static std::shared_ptr<Mesh> createPlane();
    static std::shared_ptr<Mesh> createPlane(const std::vector<glm::vec3> &pos, int w, int h, bool flipNormal = false, bool closePlane = false);
    static std::shared_ptr<Mesh> createBox();
    static std::shared_ptr<Mesh> createQuad();
    static std::shared_ptr<Mesh> createTore(int resolution = 16);
    static std::shared_ptr<Mesh> createCylinder(float L, float r, uint resolution = 16);
    static std::shared_ptr<Mesh> createFromOFF(const std::string &filePath);

    const uint getVAO() const { return VAO; }
    const uint getIndexCount() const { return indexCount; }

    void setName(std::string newName) { name = newName; }
    std::string getName() const { return name; }

    const std::vector<glm::vec3> &getVertices() const { return vertices; }
    const std::vector<glm::vec3> &getNormals() const { return normals; }
    const std::vector<uint> &getIndices() const { return indices; }

protected:
    uint VAO, VBO, NBO, TBO, EBO;
    bool hasNormals, hasTextures;
    size_t indexCount;
    std::string name;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint> indices;
};

#endif // MESH_HPP
