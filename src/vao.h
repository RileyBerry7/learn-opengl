#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"

//
class VAO
{
public:
    // ATTRIBUTES: ---------------------------------------

    GLuint ID; // ID reference for the Vertex Array Object


    // METHODS: -------------------------------------------

    // Constructor: generates a VAO ID
    VAO();
    void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
    void Bind();
    void Unbind();
    void Delete();
};
#endif