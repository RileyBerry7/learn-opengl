#include <GLFW/glfw3.h>

// std
#include <iostream>

int main() {
    std::cout << "Hello world!" << "!\n";

    // initialize the library
    if (!glfwInit()) return -1;

    // create window
    GLFWwindow *window = glfwCreateWindow(640, 480, "Test", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {

    }




    return 0;
}