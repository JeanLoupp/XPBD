#include "Mesh.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>

#define PI 3.14159265359

// Constructeur de la classe Mesh
Mesh::Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<uint> &indices, const std::string &name)
    : vertices(vertices), normals(normals), indices(indices), indexCount(indices.size()), name(name) {

    hasTextures = false;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &textures, const std::vector<uint> &indices, const std::string &name)
    : vertices(vertices), normals(normals), indices(indices), indexCount(indices.size()), name(name) {

    hasTextures = (textures.size() != 0);
    hasNormals = (normals.size() != 0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &TBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    if (hasNormals) {
        glBindBuffer(GL_ARRAY_BUFFER, NBO);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
    }

    if (hasTextures) {
        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(glm::vec2), textures.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

// Fonction pour dessiner le maillage
void Mesh::draw(uint shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat) {
    glUseProgram(shaderProgram);

    int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    uint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::draw(ShaderProgram &shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat) {
    shaderProgram.use();

    shaderProgram.set("objectColor", color);
    shaderProgram.set("model", modelMat);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::startDrawMultiple(ShaderProgram &shaderProgram) {
    shaderProgram.use();
    glBindVertexArray(VAO);
}

void Mesh::addDrawMultiple(ShaderProgram &shaderProgram, const glm::vec3 &color, const glm::mat4 &modelMat) {
    shaderProgram.set("objectColor", color);
    shaderProgram.set("model", modelMat);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::endDrawMultiple() {
    glBindVertexArray(0);
}

void Mesh::updateNormals() {
    normals.resize(vertices.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        uint i0 = indices[i];
        uint i1 = indices[i + 1];
        uint i2 = indices[i + 2];

        const glm::vec3 &v0 = vertices[i0];
        const glm::vec3 &v1 = vertices[i1];
        const glm::vec3 &v2 = vertices[i2];

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        normals[i0] += faceNormal;
        normals[i1] += faceNormal;
        normals[i2] += faceNormal;
    }

    for (glm::vec3 &normal : normals) {
        normal = glm::normalize(normal);
    }

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(glm::vec3), normals.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::setVertices(const std::vector<glm::vec3> &vertices) {
    if (this->vertices.size() != vertices.size()) std::cerr << "vertices must be the same size as current one in setVertices" << std::endl;
    this->vertices = vertices;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::updateVertices() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::applyTransform(const glm::mat4 &mat) {
    for (glm::vec3 &pos : vertices) {
        pos = glm::vec3(mat * glm::vec4(pos, 1.0f));
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Destructeur de la classe Mesh
Mesh::~Mesh() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    if (hasNormals) glDeleteBuffers(1, &NBO);
    if (hasTextures) glDeleteBuffers(1, &TBO);
    glDeleteVertexArrays(1, &VAO);
}

// Fonction statique pour créer un cube
std::shared_ptr<Mesh> Mesh::createCube(float w) {
    std::vector<glm::vec3> vertices = {
        // Arrière
        {-w, -w, -w},
        {w, -w, -w},
        {w, w, -w},
        {-w, w, -w},
        // Avant
        {-w, -w, w},
        {w, -w, w},
        {w, w, w},
        {-w, w, w},
        // Gauche
        {-w, -w, -w},
        {-w, w, -w},
        {-w, w, w},
        {-w, -w, w},
        // Droite
        {w, -w, -w},
        {w, w, -w},
        {w, w, w},
        {w, -w, w},
        // Bas
        {-w, -w, -w},
        {w, -w, -w},
        {w, -w, w},
        {-w, -w, w},
        // Haut
        {-w, w, -w},
        {w, w, -w},
        {w, w, w},
        {-w, w, w}};

    std::vector<glm::vec3> normals = {
        // Normales
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f}, // Arrière
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f}, // Avant
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f}, // Gauche
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}, // Droite
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f}, // Bas
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f} // Haut
    };

    std::vector<uint> indices = {
        // Face arrière
        0, 2, 1, 3, 2, 0,
        // Face avant
        4, 5, 6, 6, 7, 4,
        // Face gauche
        9, 8, 10, 11, 10, 8,
        // Face droite
        12, 13, 14, 14, 15, 12,
        // Face inférieure
        16, 17, 18, 18, 19, 16,
        // Face supérieure
        21, 20, 22, 23, 22, 20};

    return std::make_shared<Mesh>(vertices, normals, indices, "Cube");
}

std::shared_ptr<Mesh> Mesh::createSphere(float radius, int resolution) {

    int total = resolution * resolution;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint> indices;

    glm::vec3 pos;
    float phi, theta;

    int triShift[6][2] = {{0, 0}, {1, 1}, {0, 1}, {0, 0}, {1, 0}, {1, 1}};

    for (int i = 0; i <= resolution; i++) {
        for (int j = 0; j <= resolution; j++) {
            phi = -i * 2 * PI / resolution; // col
            theta = j * PI / resolution;    // li

            pos[0] = glm::sin(theta) * glm::cos(phi);
            pos[2] = glm::sin(theta) * glm::sin(phi);
            pos[1] = glm::cos(theta);

            vertices.push_back(radius * pos);
            normals.push_back(pos);

            if (i == resolution || j == resolution) continue;

            // Calculate triangle indices
            int nextRow = (i + 1);
            int nextCol = (j + 1);

            int current = j * (resolution + 1) + i;
            int next = j * (resolution + 1) + nextRow;
            int diagonal = nextCol * (resolution + 1) + i;
            int nextDiagonal = nextCol * (resolution + 1) + nextRow;

            // Triangle 1
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(diagonal);

            // Triangle 2
            indices.push_back(next);
            indices.push_back(nextDiagonal);
            indices.push_back(diagonal);
        }
    }

    return std::make_shared<Mesh>(vertices, normals, indices, "Sphere");
}

std::shared_ptr<Mesh> Mesh::createPlane() {
    std::vector<glm::vec3> vertices = {
        {-1.0f, 0, 1.0f},
        {1.0f, 0, 1.0f},
        {-1.0f, 0, -1.0f},
        {1.0f, 0, -1.0f},
    };

    std::vector<glm::vec3> normals = {
        {0, 1.0f, 0},
        {0, 1.0f, 0},
        {0, 1.0f, 0},
        {0, 1.0f, 0},
    };

    std::vector<uint> indices = {0, 1, 2, 3, 2, 1};

    return std::make_shared<Mesh>(vertices, normals, indices, "Plane");
}

std::shared_ptr<Mesh> Mesh::createPlane(const std::vector<glm::vec3> &pos, int w, int h) {
    std::vector<glm::vec3> vertices(pos);

    std::vector<glm::vec3> normals(pos.size(), glm::vec3(0));

    std::vector<uint> indices;

    for (int i = 0; i < w - 1; i++) {
        for (int j = 0; j < h - 1; j++) {
            indices.push_back(j * w + i);
            indices.push_back((j + 1) * w + i + 1);
            indices.push_back(j * w + i + 1);

            indices.push_back(j * w + i);
            indices.push_back((j + 1) * w + i);
            indices.push_back((j + 1) * w + i + 1);
        }
    }

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertices, normals, indices, "Plane");
    mesh->updateNormals();

    return mesh;
}

std::shared_ptr<Mesh> Mesh::createQuad() {
    std::vector<glm::vec3> vertices = {
        {-1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 0.0f},
        {1.0f, -1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f},
    };

    std::vector<glm::vec3> normals;

    std::vector<glm::vec2> textures = {
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
    };

    std::vector<uint> indices = {0, 2, 1, 0, 3, 2};

    return std::make_shared<Mesh>(vertices, normals, textures, indices, "Quad");
}

std::shared_ptr<Mesh> Mesh::createBox() {
    // cube but with faces toward inside

    std::vector<glm::vec3> vertices = {
        // Arrière
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f},
        // Avant
        {-1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f},
        // Gauche
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f},
        // Droite
        {1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        // Bas
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f},
        // Haut
        {-1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f}};

    std::vector<glm::vec3> normals = {
        // Normales
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f}, // Arrière
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f}, // Avant
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}, // Gauche
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f}, // Droite
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}, // Bas
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f} // Haut
    };

    std::vector<uint> indices = {
        // Face arrière
        1, 2, 0, 3, 0, 2,
        // Face avant
        5, 4, 6, 7, 6, 4,
        // Face gauche
        9, 10, 8, 11, 8, 10,
        // Face droite
        13, 12, 14, 15, 14, 12,
        // Face inférieure
        17, 16, 18, 19, 18, 16,
        // Face supérieure
        21, 22, 20, 23, 20, 22};

    return std::make_shared<Mesh>(vertices, normals, indices, "Box");
}

std::shared_ptr<Mesh> Mesh::createTore(int resolution) {

    int total = resolution * resolution;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint> indices;

    glm::vec3 pos;
    float phi, theta;

    int triShift[6][2] = {{0, 0}, {1, 1}, {0, 1}, {0, 0}, {1, 0}, {1, 1}};

    for (int i = 0; i <= resolution; i++) {
        for (int j = 0; j <= resolution; j++) {
            phi = -i * 2 * PI / resolution; // col
            theta = j * PI / resolution;    // li

            pos[0] = glm::sin(theta) * glm::cos(phi);
            pos[2] = glm::sin(theta) * glm::sin(phi);
            pos[1] = glm::cos(theta);

            vertices.push_back(pos);
            normals.push_back(pos);

            if (i == resolution || j == resolution) continue;

            // Calculate triangle indices
            int nextRow = (i + 1);
            int nextCol = (j + 1);

            int current = j * (resolution + 1) + i;
            int next = j * (resolution + 1) + nextRow;
            int diagonal = nextCol * (resolution + 1) + i;
            int nextDiagonal = nextCol * (resolution + 1) + nextRow;

            // Triangle 1
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(diagonal);

            // Triangle 2
            indices.push_back(next);
            indices.push_back(nextDiagonal);
            indices.push_back(diagonal);
        }
    }

    return std::make_shared<Mesh>(vertices, normals, indices, "Tore");
}

std::shared_ptr<Mesh> Mesh::createFromOFF(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::string line;
    // Read the header
    std::getline(file, line);
    if (line != "OFF") {
        throw std::runtime_error("File is not in OFF format.");
    }

    // Read the number of vertices, faces, and edges
    size_t numVertices = 0, numFaces = 0, numEdges = 0;
    std::getline(file, line);
    std::istringstream headerStream(line);
    headerStream >> numVertices >> numFaces >> numEdges;

    std::vector<glm::vec3> vertices;
    std::vector<uint> indices;

    // Read vertices
    for (size_t i = 0; i < numVertices; ++i) {
        float x, y, z;
        std::getline(file, line);
        std::istringstream vertexStream(line);
        vertexStream >> x >> y >> z;
        vertices.emplace_back(x, y, z);
    }

    // Read faces
    for (size_t i = 0; i < numFaces; ++i) {
        std::getline(file, line);
        std::istringstream faceStream(line);
        size_t faceSize;
        faceStream >> faceSize;
        if (faceSize != 3) {
            throw std::runtime_error("Only triangular faces are supported.");
        }
        uint v1, v2, v3;
        faceStream >> v1 >> v2 >> v3;
        indices.push_back(v1);
        indices.push_back(v2);
        indices.push_back(v3);
    }

    file.close();

    std::vector<glm::vec3> normals(numVertices);

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertices, normals, indices);
    mesh->updateNormals();

    return mesh;
}
