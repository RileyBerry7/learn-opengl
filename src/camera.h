//
// Created by Riley on 3/16/2026.
//

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H


#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shaderClass.h"

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    // Camera Size
    int width;
    int height;

    // Current Speed
    float speed = 0.0f;
    float sensitivity = 60.0f;

    float slowSpeed = 1.6f;
    float fastSpeed = 0.7f;

    // Prevents the camera from jumping around when first clicking left click
    bool firstClick = true;

    Camera(int width, int height, glm::vec3 position);
    void Matrix(Shader& shader, const char* uniform);
    void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void Inputs(GLFWwindow* window, float delta_time);

};


#endif //CAMERA_CLASS_H