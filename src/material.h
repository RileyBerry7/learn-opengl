#ifndef LEARN_OPENGL_MATERIAL_H
#define LEARN_OPENGL_MATERIAL_H

#include <glm/glm.hpp>
#include "shaderClass.h"
#include "texture.h"

//-------------------------------------------------------------------------------------
// BASE CLASS
class Material {
public:
    Shader* shader; // Suggested shader

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
    Tex* specMap;
    Tex* diffuseMap;

    DefaultMaterial(Shader& shader, Tex* tex = nullptr, Tex* diffMap = nullptr) :
        Material(shader),
        specMap(tex),
        diffuseMap(diffMap),
        ambient( glm::vec3(0.25f, 0.25f, 0.25f)),
        diffuse( glm::vec3(0.4f, 0.4f, 0.4f)),
        specular(glm::vec3(0.77f, 0.77f, 0.77f)),
        shininess(76.8f){}

    void apply() override {

        // Inject Material Uniforms
        shader->setUniform("material.ambient", ambient);
        shader->setUniform("material.shininess", shininess);

        // Set diffuse map
        if (diffuseMap != nullptr) {
            glActiveTexture(GL_TEXTURE0);
            diffuseMap->Bind();
            diffuseMap->setUniform(*shader, "material.diffuse", 1);
        }

            // Set specular map
        if (specMap != nullptr) {
            glActiveTexture(GL_TEXTURE1);
            specMap->Bind();
            specMap->setUniform(*shader, "material.specular", 0);
        }
    }


};
#endif //LEARN_OPENGL_MATERIAL_H