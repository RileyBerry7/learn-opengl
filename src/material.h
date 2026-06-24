#ifndef LEARN_OPENGL_MATERIAL_H
#define LEARN_OPENGL_MATERIAL_H

#include <glm/glm.hpp>
#include "shaderClass.h"
#include "betterTexture.h"

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
    glm::vec3 ambient;  // Ambient scalar
    glm::vec3 diffuse;  // Diffuse scalar
    glm::vec3 specular; // Specular scalar
    float     shininess;// Glossiness level

    // Optional
    BetterTexture* specMap;
    BetterTexture* diffuseMap;
    BetterTexture* normalMap;

    bool      hasNormalMap;

    DefaultMaterial(Shader& shader, BetterTexture* diffMap = nullptr, BetterTexture* specMap = nullptr, BetterTexture* normMap = nullptr) :
        Material(shader),
        diffuseMap(diffMap),
        specMap(specMap),
        normalMap(normMap),
        hasNormalMap(false),

        // Default material // Pretty sure this is redundant but default
        ambient( glm::vec3(0.25f, 0.25f, 0.25f)),
        diffuse( glm::vec3(0.4f, 0.4f, 0.4f)),
        specular(glm::vec3(0.77f, 0.77f, 0.77f)),
        shininess(76.8f){}

    void apply() override {

        // Inject Material Uniforms
        shader->setUniform("material.ambient",   ambient);
        shader->setUniform("material.diffuse",   diffuse);
        shader->setUniform("material.specular",  specular);
        shader->setUniform("material.shininess", shininess);

        // Set diffuse map
        if (diffuseMap != nullptr) {
            diffuseMap->bindUnit(0);
            shader->setUniform("material.diffuseMap", 0);
        }
        // Set specular map
        if (specMap != nullptr) {
            specMap->bindUnit(1);
            shader->setUniform("material.specularMap", 1);
        }
        if (normalMap != nullptr) {
            normalMap->bindUnit(2);
            shader->setUniform("material.normalMap", 2);
        }

        shader->setUniform("material.hasNormalMap", hasNormalMap);
    }


};
#endif //LEARN_OPENGL_MATERIAL_H