#include "ShadowMap.hpp"
#include "stb_image_write.h"
#include "utils/utils.hpp"

ShadowMap::ShadowMap(std::string vertexShader, std::string fragShader, const glm::vec3 &lightDir, const Camera &cam, int width, int height)
    : width(width), height(height), shaderProgram(vertexShader, fragShader) {
    allocate();

    glm::mat4 viewMat = glm::lookAt(-lightDir * 10.0f, glm::vec3(0), glm::vec3(0, 1, 0));
    float near_plane = 1.0f, far_plane = 20.0f;
    glm::mat4 projMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    depthMVP = projMat * viewMat;
}

ShadowMap::~ShadowMap() {
    glDeleteTextures(1, &depthMapTexture);
    glDeleteFramebuffers(1, &depthMapFBO);
}

void ShadowMap::allocate() {
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Clamp the texture to 1 no border
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::beginRender() {
    shaderProgram.use();
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);
}

void ShadowMap::addObject(std::shared_ptr<Mesh> mesh, glm::mat4 modelMat) {
    shaderProgram.set("lightMVP", depthMVP * modelMat);

    glBindVertexArray(mesh->getVAO());
    glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ShadowMap::endRender() {
    // Reset viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
}

void ShadowMap::sendShadowMap(ShaderProgram &otherShaderProgram) {
    otherShaderProgram.set("shadowMap.MVP", depthMVP);

    // Shadow map on texture0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    otherShaderProgram.set("shadowMap.depthMap", 0);
    otherShaderProgram.set("shadowMap.use", true);
}

void ShadowMap::saveDepthMap(const std::string &filepath) {
    std::vector<float> depthData(width * height);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depthData.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // Normalisation des données de profondeur pour les convertir en image
    std::vector<uint8_t> imageData(width * height);
    for (int i = 0; i < width * height; ++i) {
        imageData[i] = static_cast<uint8_t>(depthData[i] * 255.0f); // Convertir [0,1] en [0,255]
    }

    // Sauvegarde de l'image
    if (stbi_write_png(filepath.c_str(), width, height, 1, imageData.data(), width) == 0) {
        std::cerr << "Erreur lors de la sauvegarde de la depthMap dans " << filepath << std::endl;
    } else {
        std::cout << "DepthMap sauvegardée avec succès dans " << filepath << std::endl;
    }
}