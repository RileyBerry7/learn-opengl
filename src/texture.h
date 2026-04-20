//
// Created by Riley on 3/13/2026.
//

#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
# include <stb/stb_image.h>
#include <string>

// std
#include <cerrno>
#include <iostream>

#include "shaderClass.h"

class Tex {
public:
    // ATTRIBUTES ------------------------
    GLuint ID;
    GLenum type;

    // METHODS -----------------------------
	Tex(const std::string imagePath, GLenum texType, GLenum slot, GLenum pixelType);
    ~Tex() {
        this->Delete();
    }

    unsigned char* getImage(const char* fileName, int& widthImg, int& heightImg, int& numColorCh);
    void setUniform(Shader& shader, const char* uniform, GLuint unit);
    void Bind();
    void Unbind();
    void Delete();
};


#endif // TEXTURE_CLASS_H