//
// Created by Riley on 4/12/2026.
//

#ifndef LEARN_OPENGL_OBJECT_H
#define LEARN_OPENGL_OBJECT_H

#include "glm/glm.hpp"

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

    Object(Mesh& model_mesh, Tex& model_texture) {
        // Default transformations
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        scale    = glm::vec3(1.0f, 1.0f, 1.0f);

        // Assign references
        mesh    = &model_mesh;
        texture = &model_texture;
    }

    glm::mat4 getModelMatrix() {
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }
};

#endif //LEARN_OPENGL_OBJECT_H