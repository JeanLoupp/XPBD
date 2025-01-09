#pragma once

#include <string>
#include <glm/glm.hpp>
#include "render/ShaderProgram.hpp"
#include "mesh/Mesh.hpp"
#include "render/Camera.hpp"

class ShadowMap {
private:
    ShaderProgram shaderProgram;
    glm::mat4 depthMVP;

    GLuint depthMapFBO;
    GLuint depthMapTexture;
    int width, height;

    GLint viewport[4];

    void allocate();

public:
    ShadowMap(std::string vertexShader, std::string fragShader, const glm::vec3 &lightDir, const Camera &cam, int width, int height);
    ~ShadowMap();

    void beginRender();
    void addObject(std::shared_ptr<Mesh> mesh, glm::mat4 modelMat = glm::mat4(1.0));
    void endRender();
    void sendShadowMap(ShaderProgram &shaderProgram);

    void saveDepthMap(const std::string &filepath);
};
