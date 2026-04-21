#ifndef LEARN_OPENGL_LIGHT_H
#define LEARN_OPENGL_LIGHT_H


#include <glm/glm.hpp>
#include "shaderClass.h"
#include "texture.h"

enum class LightType {Directional, Point, Spot};

//-------------------------------------------------------------------------------------
// BASE CLASS
class Light {
public:
    // Universal
    LightType type;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // Directional
    glm::vec3 direction;

    // Constructor
    Light(LightType lightType, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
        type(lightType),
        ambient(ambient),
        diffuse(diffuse),
        specular(specular) {};

    // Default Destructor
    virtual ~Light() = default;
};


//-------------------------------------------------------------------------------------
// DERIVED CLASSES

class PointLight : public Light {
public:
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;

    PointLight(glm::vec3 pos, float con, float lin, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) :
        Light(LightType::Point, amb, diff, spec),
        position(pos),
        constant(con),
        linear(lin),
        quadratic(quad) {}
};

//----------------------------------------------------------------------------------------------------------------------

        // struct Light {
        //     glm::vec3 position  = glm::vec3(3.3f, 0.5f, 0.7f);
        //     glm::vec3 direction = glm::vec3(-0.2f, -0.0f, -0.1f);
        //     glm::vec3 ambient   = glm::vec3(0.3f, 0.3f, 0.3f);
        //     glm::vec3 diffuse   = glm::vec3(0.5f, 0.5f, 0.5f);
        //     glm::vec3 specular  = glm::vec3(1.0f, 1.0f, 1.0f);
        // };
        // Light light;
        //
        // shader->setUniform("light.position",  light.position);
        // shader->setUniform("light.direction", light.direction);
        // shader->setUniform("light.ambient",   light.ambient);
        // shader->setUniform("light.diffuse",   light.diffuse);
        // shader->setUniform("light.specular",  light.specular);
        //
        // shader->setUniform("light.constant", 1.0f);
        // shader->setUniform("light.linear", 0.09f);
        // shader->setUniform("light.quadratic", 0.032f);


#endif //LEARN_OPENGL_LIGHT_H