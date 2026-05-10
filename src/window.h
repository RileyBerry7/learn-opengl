#ifndef LEARN_OPENGL_WINDOW_H
#define LEARN_OPENGL_WINDOW_H

#include <string>
#include <GLFW/glfw3.h>

class Window {
public:
    // Window attributes
    int width  = 800;
    int height = 600;
    std::string windowName = "Riley's Renderer";
    GLFWwindow* window;

    // Input attributes
    bool f_toggle;

    Window() : f_toggle(false) {
        // Initialize GLFW
        if (!glfwInit()) {
            // return -1;
            std::cerr << "GLFW failed to initialize." << std::endl;
        }

        // Specify OpenGL version/profile for GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core: lacks deprecated functions


        // Create a GLFW window object
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            // return -1;
        }

        // Introduce window to the current context
        glfwSetWindowUserPointer(getWindow(), this);
        glfwMakeContextCurrent(window);

        glfwSetKeyCallback(window, key_callback_static);
    }

    ~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void processInput () {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  {
            // std::cout << "User pressed ESC\n";
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

    }

    // static bridge function
    static void key_callback_static(GLFWwindow* win, int key, int scancode, int action, int mods) {
        // retrieve class instance
        Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(win));
        if (instance) {
            instance->handleKey(key, action); // call actual member function
        }
    }

    void handleKey(int key, int action) {
        if (key == int(GLFW_KEY_F) && action == GLFW_PRESS) {
            std::cout << "F down (" << (f_toggle ? "On)" : "Off)") << std::endl;
            f_toggle = !f_toggle;
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