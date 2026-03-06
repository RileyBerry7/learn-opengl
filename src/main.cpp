#include <GLFW/glfw3.h>

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




        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}