#include "RigidMesh.hpp"
#include <Eigen/Dense>

glm::mat3 tensorProduct(const glm::vec3 &a, const glm::vec3 &b) {
    return {a.x * b.x, a.x * b.y, a.x * b.z,
            a.y * b.x, a.y * b.y, a.y * b.z,
            a.z * b.x, a.z * b.y, a.z * b.z};
}

glm::mat3 polarDecomposition(const glm::mat3 &M) {
    Eigen::Matrix3f A;
    A << M[0][0], M[0][1], M[0][2], M[1][0], M[1][1], M[1][2], M[2][0], M[2][1], M[2][2];
    Eigen::JacobiSVD<Eigen::Matrix3f> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const Eigen::Matrix3f R = svd.matrixU() * (svd.matrixV().transpose());

    return {R(0, 0), R(0, 1), R(0, 2), R(1, 0), R(1, 1), R(1, 2), R(2, 0), R(2, 1), R(2, 2)};
}

void RigidMesh::applyTransform(const glm::mat4 &mat) {
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i] = originalPos[i];
    }
    for (int i = 0; i < pos.size(); i++) {
        pos[i] = glm::vec3(mat * glm::vec4(pos[i], 1.0f));
        originalPos[i] = glm::vec3(mat * glm::vec4(originalPos[i], 1.0f));
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::vec3 RigidMesh::computeCOM(const std::vector<glm::vec3> pos) {
    glm::vec3 com(0);

    for (int i = 0; i < pos.size(); i++) {
        com += pos[i];
    }
    com /= pos.size();

    return com;
}

void RigidMesh::shapeMatch(const std::vector<glm::vec3> &predict_pos) {
    pos = predict_pos;
    glm::vec3 COM = computeCOM(pos);

    // Compute M
    glm::mat3 M(0);
    for (int i = 0; i < pos.size(); i++) {
        const glm::vec3 r = pos[i] - COM;
        const glm::vec3 r_ref = originalPos[i] - originalCOM;
        M += tensorProduct(r, r_ref);
    }

    const glm::mat3 R = polarDecomposition(M);
    const glm::mat3 Rinv = inverse(R);

    for (int i = 0; i < pos.size(); i++) {
        pos[i] = R * (originalPos[i] - originalCOM) + COM;
    }

    // Update mesh
    if (meshToPos.size() != 0) {
        for (int i = 0; i < meshToPos.size(); i++) {
            vertices[i] = pos[meshToPos[i]];
        }
        updateVertices();
    } else {
        setVertices(pos);
    }

    updateNormals();
}

std::shared_ptr<RigidMesh> RigidMesh::createCube(int resolution, float w) {
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

    std::vector<glm::vec3> pos = {
        {-w, -w, -w}, // 0
        {w, -w, -w},  // 1
        {-w, w, -w},  // 2
        {w, w, -w},   // 3
        {-w, -w, w},  // 4
        {w, -w, w},   // 5
        {-w, w, w},   // 6
        {w, w, w},    // 7
    };

    float step = 2.0 * w / resolution;
    for (int face = 0; face < 6; face++) {
        glm::vec3 normal(0.0f); // Normale de la face
        glm::vec3 right(0.0f);  // Axe "droit" sur la face
        glm::vec3 up(0.0f);     // Axe "haut" sur la face

        // Définir les orientations des axes pour chaque face
        switch (face) {
        case 0:
            normal = {1, 0, 0};
            right = {0, 1, 0};
            up = {0, 0, 1};
            break; // Face +X
        case 1:
            normal = {-1, 0, 0};
            right = {0, 1, 0};
            up = {0, 0, -1};
            break; // Face -X
        case 2:
            normal = {0, 1, 0};
            right = {1, 0, 0};
            up = {0, 0, 1};
            break; // Face +Y
        case 3:
            normal = {0, -1, 0};
            right = {1, 0, 0};
            up = {0, 0, -1};
            break; // Face -Y
        case 4:
            normal = {0, 0, 1};
            right = {1, 0, 0};
            up = {0, 1, 0};
            break; // Face +Z
        case 5:
            normal = {0, 0, -1};
            right = {1, 0, 0};
            up = {0, -1, 0};
            break; // Face -Z
        }

        // Parcourir la grille de la face
        for (int i = 0; i <= resolution; ++i) {
            for (int j = 0; j <= resolution; ++j) {
                // skip corners
                if ((i == 0 || i == resolution) && (j == 0 || j == resolution)) continue;
                // skip duplicate edges
                if (face >= 2 && (i == 0 || i == resolution)) continue;
                if (face >= 4 && (j == 0 || j == resolution)) continue;

                glm::vec3 offset =
                    normal * w +              // Position centrale de la face
                    right * (-w + step * i) + // Position horizontale
                    up * (-w + step * j);     // Position verticale

                pos.push_back(offset);
            }
        }
    }

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

    std::vector<uint> meshToPos = {
        0, 1, 3, 2,
        4, 5, 7, 6,
        0, 2, 6, 4,
        1, 3, 7, 5,
        0, 1, 5, 4,
        2, 3, 7, 6};

    return std::make_shared<RigidMesh>(pos, meshToPos, vertices, normals, indices);
}