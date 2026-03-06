#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

// std
#include <iostream>
#include <string>

constexpr int  WIDTH  = 800;
constexpr int  HEIGHT = 600;
constexpr char WINDOW_NAME[] = "Hello Window";


int main() {
    std::cout << "Hello world!" << "!\n";

    // GLFW Initialization
    if (!glfwInit()) return -1;

    // GLFW Configuration
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core -> deprecated functions removed

    // create window
    GLFWwindow *window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        WINDOW_NAME,
        nullptr,
        nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    // let window be the current context
    glfwMakeContextCurrent(window);

    // loop until user closes window
    while (!glfwWindowShouldClose(window)) {

        /* render here */
        // glClear(GL_COLOR_BUFFER_BIT);
        // glBegin(GL_TRIANGLES);
        // glVertex2f(-0.5f, -0.5f);
        // glVertex2f(0.0f, 0.5f);
        // glVertex2f(0.5f, -0.5f);
        // glEnd();



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}