#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO {

public:
    // ATTRIBUTES: ------------------------------------------------

    GLuint ID;  // Reference ID  the Vertex Buffer Object

    // METHODS: ----------------------------------------------------

    // Constructor -  generates a VBO and links it to vertices
    VBO(GLfloat* vertices, GLsizeiptr size);

    void Bind();
    void Unbind();
    void Delete();
};

#endif