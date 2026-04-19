#ifndef LEARN_OPENGL_WINDOW_H
#define LEARN_OPENGL_WINDOW_H

#include <string>
#include <GLFW/glfw3.h>

class Window {
    int width  = 800;
    int height = 600;
    std::string windowName = "Riley's Renderer";

    GLFWwindow* window;

public:
    Window() {
        // Create a GLFW window object
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            // return -1;
        }

        // Introduce window to the current context
        glfwMakeContextCurrent(window);
    }

    ~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void processInput () {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  {
            // std::cout << "User pressed ESC\n";
            glfwSetWindowShouldClose(window, true);
        }
    }
    bool shouldClose() const {
        return glfwWindowShouldClose(window);
    }
    void swapBuffers() const {

        // Swap Front/Back Buffers
        glfwSwapBuffers(window);

        // Detect and handle events
        glfwPollEvents();
    }
    GLFWwindow* getWindow() const {
        return window;
    }
};


#endif //LEARN_OPENGL_WINDOW_H