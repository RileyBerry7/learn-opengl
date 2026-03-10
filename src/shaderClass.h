//
// Created by Riley on 3/9/2026.
//

#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* fileName);

class Shader {
public:
    Shader(const char* vertexFile, const char* fragFile);

    void Activate();
    void Delete();
    GLuint ID;
};


#endif //SHADER_CLASS_H