#ifndef LEARN_OPENGL_WINDOW_H
#define LEARN_OPENGL_WINDOW_H

#include <string>
#include <GLFW/glfw3.h>

class Window {
public:
    int width  = 800;
    int height = 600;
    std::string windowName = "Riley's Renderer";
    GLFWwindow* window;

    Window() {
        // Initialize GLFW
        if (!glfwInit()) {
            // return -1;
            std::cerr << "GLFW failed to initialize." << std::endl;
        }

        // Specify OpenGL version/profile for GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core: lacks deprecated functions


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