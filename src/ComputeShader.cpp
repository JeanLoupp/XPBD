#include "ComputeShader.hpp"

ComputeShader::ComputeShader(const std::string &computeShaderPath) {
    std::string computeShaderSource = loadShaderSource(computeShaderPath);
    GLuint computeShader = compileShader(computeShaderSource, GL_COMPUTE_SHADER);

    // Créer le programme de shader et lier le shader de calcul
    programID = glCreateProgram();
    glAttachShader(programID, computeShader);
    glLinkProgram(programID);

    // Vérifier les erreurs de linkage
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        return;
    }

    glDeleteShader(computeShader);
}