//
// Created by Riley on 4/12/2026.
//

#ifndef LEARN_OPENGL_OBJECT_H
#define LEARN_OPENGL_OBJECT_H

#include "glm/glm.hpp"
#include "shaderClass.h"
#include "material.h"
#include "Mesh.h"


class Object {
public:
    // Model Attributes
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3  scale;

    // Mesh Attributes
    Mesh* mesh;

    // Texture Attributes
    Tex* texture;

    // Shader Attributes
    Shader*   shader;
    Material* material;

    Object(Shader& shader_program, Mesh& model_mesh, Tex& model_texture, Material& model_material) {
        // Default transformations
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        scale    = glm::vec3(1.0f, 1.0f, 1.0f);

        // Assign references
        shader   = &shader_program;
        mesh     = &model_mesh;
        texture  = &model_texture;
        material = &model_material;
    }

    glm::mat4 getModelMatrix() {
        auto model = glm::mat4(1.0f); // Identity matrix
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }
};

#endif //LEARN_OPENGL_OBJECT_H