//
// Created by Riley on 3/13/2026.
//

#include "texture.h"

Tex::Tex(const std::string imagePath, GLenum texType, GLenum slot, GLenum pixelType){

    int widthImg;
    int heightImg;
    int numColorCh;

    glGenTextures(1, &ID);
    glActiveTexture(slot);

    // Bind
    this->Bind();

    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    // glParameterfv(texType, GL_TEXTURE_BORDER_COLOR, flatColor);

    unsigned char* image = this->getImage(imagePath.c_str(), widthImg, heightImg, numColorCh);

    // Get pixel format
    GLenum format = (numColorCh == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(texType, 0, format, widthImg, heightImg, 0, format, pixelType, image);

    glGenerateMipmap(texType);
    stbi_image_free(image);
    glBindTexture(texType, 0);

}

unsigned char* Tex::getImage(const char* fileName, int& widthImg, int& heightImg, int& numColorCh) {
    // Get Image
    stbi_set_flip_vertically_on_load(true);

    unsigned char* image = stbi_load(fileName, &widthImg, &heightImg, &numColorCh, 0);
    if (!image) {
        std::cerr << "Failed to load texture: " << fileName << "\n";
    }
    return image;
}

void Tex::setUniform(Shader& shader, const char* uniform_name, GLuint unit){
    // Gets texture Uniform
    GLuint texUni = glGetUniformLocation(shader.ID, uniform_name);
    // Ensure shader is active and listening
    shader.Activate();
    // Links texture at the 'unit' slot to the texUni uniform
    glUniform1i(texUni, unit);
}

void Tex::Bind(){
        glBindTexture(GL_TEXTURE_2D, ID);
}

void Tex::Unbind(){
        glBindTexture(GL_TEXTURE_2D, 0);
}

void Tex::Delete(){
    glDeleteTextures(1, &ID);
}

