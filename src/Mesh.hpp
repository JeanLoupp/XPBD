#ifndef MESH_HPP
#define MESH_HPP

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include "ShaderProgram.hpp"

class Mesh {
public:
    Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<unsigned int> &indices, std::string name = "");
    Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &textures, const std::vector<unsigned int> &indices, std::string name = "");
    ~Mesh();

    void draw(unsigned int shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat);
    void draw(ShaderProgram &shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat);

    void startDrawMultiple(ShaderProgram &shaderProgram);
    void addDrawMultiple(ShaderProgram &shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat);
    void endDrawMultiple();

    void updateNormals();

    void setVertices(const std::vector<glm::vec3> &vertices);
    void applyTransform(const glm::mat4 &mat);

    static std::shared_ptr<Mesh> createCube(float w = 1.0f);
    static std::shared_ptr<Mesh> createSphere(float radius = 1.0f, int resolution = 16);
    static std::shared_ptr<Mesh> createPlane();
    static std::shared_ptr<Mesh> createPlane(const std::vector<glm::vec3> &pos, int w, int h);
    static std::shared_ptr<Mesh> createBox();
    static std::shared_ptr<Mesh> createQuad();
    static std::shared_ptr<Mesh> createTore(int resolution = 16);

    const unsigned int getVAO() const { return VAO; }
    const unsigned int getIndexCount() const { return indexCount; }

    void setName(std::string newName) { name = newName; }
    std::string getName() const { return name; }

    const std::vector<glm::vec3> &getVertices() const { return vertices; }
    const std::vector<glm::vec3> &getNormals() const { return normals; }
    const std::vector<unsigned int> &getIndices() const { return indices; }

protected:
    unsigned int VAO, VBO, NBO, TBO, EBO;
    bool hasNormals, hasTextures;
    size_t indexCount;
    std::string name;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
};

#endif // MESH_HPP
