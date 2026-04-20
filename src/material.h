#ifndef LEARN_OPENGL_MATERIAL_H
#define LEARN_OPENGL_MATERIAL_H

#include <glm/glm.hpp>
#include "shaderClass.h"
#include "texture.h"

//-------------------------------------------------------------------------------------
// BASE CLASS
class Material {
public:
    Shader* shader;
    Tex*    texture;

    Material(Shader& shaderReference, Tex& texReference) :
    shader(&shaderReference),  texture(&texReference) {}
    virtual ~Material() = default;

    virtual void apply() = 0;
    void getShader(Shader*& outShader) const {
        outShader = shader;
    }
};


//-------------------------------------------------------------------------------------
// DERIVED CLASSES

class EmissiveMaterial : public Material {
public:
    glm::vec3 lightColor;

    EmissiveMaterial(Shader& shader, Tex& tex) :
    Material(shader, tex), lightColor(glm::vec3(1.0f, 1.0f, 1.0f)) {}
    void apply() override {
        shader->Activate();
        constexpr float brightness  = 5.0f;
        shader->setUniform("lightColor", (lightColor * brightness));
    }
};

class DefaultMaterial : public Material{

public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float     shininess;

    DefaultMaterial(Shader& shader, Tex& tex) :
        Material(shader, tex),
        ambient( glm::vec3(0.25f, 0.25f, 0.25f)),
        diffuse( glm::vec3(0.4f, 0.4f, 0.4f)),
        specular(glm::vec3(0.77f, 0.77f, 0.77f)),
        shininess(76.8f){}

    void apply() override {
        struct Light {
            glm::vec3 position  = glm::vec3(3.3f, 0.5f, 0.7f);
            glm::vec3 direction = glm::vec3(-0.2f, -0.0f, -0.1f);
            glm::vec3 ambient   = glm::vec3(0.3f, 0.3f, 0.3f);
            glm::vec3 diffuse   = glm::vec3(0.5f, 0.5f, 0.5f);
            glm::vec3 specular  = glm::vec3(1.0f, 1.0f, 1.0f);
        };
        Light light;

        shader->Activate();
        shader->setUniform("light.position",  light.position);
        shader->setUniform("light.direction", light.direction);
        shader->setUniform("light.ambient",   light.ambient);
        shader->setUniform("light.diffuse",   light.diffuse);
        shader->setUniform("light.specular",  light.specular);

        shader->setUniform("light.constant", 1.0f);
        shader->setUniform("light.linear", 0.09f);
        shader->setUniform("light.quadratic", 0.032f);

        shader->setUniform("material.ambient", ambient);
        shader->setUniform("material.shininess", shininess);

        glActiveTexture(GL_TEXTURE0);
        texture->Bind();
        texture->setUniform(*shader, "material.diffuse", 0);

        // glActiveTexture(GL_TEXTURE1);
        // texture2.Bind();
        // texture2.setUniform(*shader, "material.specular", 1);

        // texture->Bind();
    }




};
#endif //LEARN_OPENGL_MATERIAL_H