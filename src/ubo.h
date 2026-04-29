//
// Created by Riley on 4/29/2026.
//

#ifndef LEARN_OPENGL_UBO_H
#define LEARN_OPENGL_UBO_H

#include<glad/glad.h>

class UBO {
public:
    // ATTRIBUTES: ---------------------------------------
    GLuint ID; // ID reference for the Vertex Array Object

    // METHODS: -------------------------------------------

    // Constructor: generates a VAO ID
    UBO(int size_bytes) {
        glGenBuffers(1, &ID);
        Bind();
        glBufferData(GL_UNIFORM_BUFFER, size_bytes, nullptr, GL_STATIC_DRAW);
    }
    void BindToSLot(const GLuint slot) {
        Bind();
        glBindBufferBase(GL_UNIFORM_BUFFER, slot, ID);
    }
    void Bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
    }
    void Unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void Delete() {
        glDeleteBuffers(1, &ID);
    }
};

#endif //LEARN_OPENGL_UBO_H