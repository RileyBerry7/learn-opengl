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

class DirectionalLight : public Light {
public:
    glm::vec3 direction;

    DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) :
        Light(LightType::Point, amb, diff, spec),
        direction(dir) {}
};

//----------------------------------------------------------------------------------------------------------------------


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

class SpotLight : public Light {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float     cutOff;
    float     outerCutOff;

    SpotLight(glm::vec3 pos, glm::vec3 dir, float cutOff, float outCutOff, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) :
        Light(LightType::Spot, amb, diff, spec),
        position(pos),
        direction(dir),
        cutOff(cutOff),
        outerCutOff(outCutOff) {}
};

//----------------------------------------------------------------------------------------------------------------------

#endif //LEARN_OPENGL_LIGHT_H