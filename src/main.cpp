#include <GLFW/glfw3.h>
// #include <glad/glad.h>
#include <GL/gl.h>

// std
#include <iostream>

int main() {
    std::cout << "Hello world!" << "!\n";

    // initialize the glfw
    if (!glfwInit()) return -1;

    // create window
    GLFWwindow *window = glfwCreateWindow(640, 480, "Test", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // let window be the current context
    glfwMakeContextCurrent(window);

    // loop until user closes window
    while (!glfwWindowShouldClose(window)) {

        /* render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}