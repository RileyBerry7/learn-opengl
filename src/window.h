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

    // Timing
    unsigned int frameCounter;
    double lastTime;
    double currentTime;
    double timeDiff;
    double frameRate;
    double frameTime;

    // Input attributes
    bool f_toggle;

    // Constructor
    Window() :  f_toggle(false),
                frameCounter(0),
                lastTime(0.0),
                currentTime(0.0),
                timeDiff(0.0),
                frameRate(0.0),
                frameTime(0.0)
    {
        // Initialize GLFW
        if (!glfwInit()) {
            // return -1;
            std::cerr << "GLFW failed to initialize." << std::endl;
        }

        // Specify OpenGL version/profile for GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
        lastTime = glfwGetTime();
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

    void tick() {
        currentTime = glfwGetTime();
        timeDiff = currentTime - lastTime;
        frameCounter++;
        if (timeDiff >= 1.0 / 30.0) {
            std::string FPS   = std::to_string(int((1.0 / timeDiff) * frameCounter));
            std::string ms    = std::to_string((timeDiff / frameCounter) * 1000);
            std::string title = windowName + "\t-\tFPS: " + std::string(FPS) + " / " + ms + "ms";
            glfwSetWindowTitle(window, title.c_str());
            frameCounter = 0;
            lastTime = currentTime;
        }

    }
};


#endif //LEARN_OPENGL_WINDOW_H