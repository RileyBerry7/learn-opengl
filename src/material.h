#ifndef LEARN_OPENGL_MATERIAL_H
#define LEARN_OPENGL_MATERIAL_H

#include <glm/glm.hpp>
#include "shaderClass.h"
#include "texture.h"

class Material {
// private:
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float     shininess;

    Shader* shader;
    Tex*    texture;

// public:
    //-------------------------------------------------------------------------------------
    // CONSTRUCTOR

    Material(Shader& shaderReference, Tex& texReference) :
        ambient( glm::vec3(0.25f, 0.25f, 0.25f)),
        diffuse( glm::vec3(0.4f, 0.4f, 0.4f)),
        specular(glm::vec3(0.77f, 0.77f, 0.77f)),
        shininess(76.8f)
    {
        shader  = &shaderReference;
        texture = &texReference;
    }

    //-------------------------------------------------------------------------------------
    // METHODS

    void apply() const {
        shader->Activate();
        texture->Bind();
    }

    void getShader(Shader*& outShader) const {
        outShader = shader;
    }


};
#endif //LEARN_OPENGL_MATERIAL_H