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
    void LinkVBO(VBO& VBO, GLuint layout);
    void Bind();
    void Unbind();
    void Delete();
};
#endif