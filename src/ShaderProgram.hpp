#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

class ShaderProgram {
public:
    ShaderProgram() {};
    ShaderProgram(GLuint programID) : programID(programID) {};
    ShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);
    ~ShaderProgram();

    GLuint getProgram() const { return programID; }

    void use() { glUseProgram(programID); };

    void set(const GLchar *name, int i) { glUniform1i(glGetUniformLocation(programID, name), i); };
    void set(const GLchar *name, float val) { glUniform1f(glGetUniformLocation(programID, name), val); };
    void set(const GLchar *name, const glm::vec3 &vec) { glUniform3fv(glGetUniformLocation(programID, name), 1, glm::value_ptr(vec)); };
    void set(const GLchar *name, const glm::mat4 &mat) { glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, glm::value_ptr(mat)); };

    void setArray(const std::string &array, unsigned int index, const std::string &name, int i);
    void setArray(const std::string &array, unsigned int index, const std::string &name, float i);
    void setArray(const std::string &array, unsigned int index, const std::string &name, const glm::vec3 &vec);
    void setArray(const std::string &array, unsigned int index, const std::string &name, const glm::mat4 &mat);

    static std::string loadShaderSource(const std::string &filePath);
    static unsigned int compileShader(const std::string &source, GLenum type);

protected:
    GLuint programID;
};

#endif // SHADER_PROGRAM_HPP