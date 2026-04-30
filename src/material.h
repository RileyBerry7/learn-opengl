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

    Material(Shader& shaderReference) :
    shader(&shaderReference) {}
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

    EmissiveMaterial(Shader& shader) :
    Material(shader), lightColor(glm::vec3(1.0f, 1.0f, 1.0f)) {}

    void apply() override {
        shader->Activate();
        constexpr float brightness  = 5.0f;
        shader->setUniform("lightColor", (lightColor * brightness));
    }
};

//----------------------------------------------------------------------------------------------------------------------

class DefaultMaterial : public Material{

public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float     shininess;

    // Optional
    Tex* texture;
    Tex* diffuseMap;

    DefaultMaterial(Shader& shader, Tex* tex = nullptr, Tex* diffMap = nullptr) :
        Material(shader),
        texture(tex),
        diffuseMap(diffMap),
        ambient( glm::vec3(0.25f, 0.25f, 0.25f)),
        diffuse( glm::vec3(0.4f, 0.4f, 0.4f)),
        specular(glm::vec3(0.77f, 0.77f, 0.77f)),
        shininess(106.8f){}

    void apply() override {
        // struct Light {
        //     glm::vec3 position  = glm::vec3(3.3f, 0.5f, 0.7f);
        //     glm::vec3 direction = glm::vec3(-0.2f, -0.0f, -0.1f);
        //     glm::vec3 ambient   = glm::vec3(0.3f, 0.3f, 0.3f);
        //     glm::vec3 diffuse   = glm::vec3(0.5f, 0.5f, 0.5f);
        //     glm::vec3 specular  = glm::vec3(1.0f, 1.0f, 1.0f);
        // };
        // Light light;
        //

        shader->setUniform("material.ambient", ambient);
        shader->setUniform("material.shininess", shininess);

        if (texture != nullptr) {
            glActiveTexture(GL_TEXTURE0);
            texture->Bind();
            texture->setUniform(*shader, "material.diffuse", 0);
        }

        if (diffuseMap != nullptr) {
            glActiveTexture(GL_TEXTURE1);
            diffuseMap->Bind();
            diffuseMap->setUniform(*shader, "material.specular", 1);
        }
    }




};
#endif //LEARN_OPENGL_MATERIAL_H