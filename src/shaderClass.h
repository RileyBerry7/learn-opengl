//
// Created by Riley on 3/9/2026.
//

#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* fileName);

class Shader {
public:
    Shader(const char* vertexFile, const char* fragFile);

    void setUniform(const char *uniform_name, const glm::mat4& value){
        GLint location = glGetUniformLocation(ID, uniform_name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));

    }
    void setUniform(const char *uniform_name, const glm::vec3& value){
        GLint location = glGetUniformLocation(ID, uniform_name);
        glUniform3fv(location, 1, glm::value_ptr(value));

    }
    void setUniform(const char *uniform_name, const float& value){
        GLint location = glGetUniformLocation(ID, uniform_name);
        glUniform1f(location, value);

    }
    void Activate();
    void Delete();
    GLuint ID;
};


#endif //SHADER_CLASS_H