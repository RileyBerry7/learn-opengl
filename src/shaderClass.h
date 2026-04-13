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

    void setUniform(const char *uniform_name, glm::mat4& value){
            glUniformMatrix4fv(glGetUniformLocation(ID, uniform_name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Activate();
    void Delete();
    GLuint ID;
};


#endif //SHADER_CLASS_H