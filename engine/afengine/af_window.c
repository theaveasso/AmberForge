#include "af_window.h"
#include "af_log.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>

GLFWwindow *afWindowInit(const int width, const int height, const char *title) {
    if (!glfwInit())
    {
        AFERROR("(afWindowInit) failed to init GLFW");
        abort();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);// Required on macOS
#endif

    const GLFWvidmode *display = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (display)
    {
        glfwWindowHint(GLFW_REFRESH_RATE, display->refreshRate);
        glfwWindowHint(GLFW_RED_BITS, display->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, display->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, display->blueBits);
    }
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        AFERROR("(afWindowInit) failed to create GLFW window");
        abort();
    }
    glfwMakeContextCurrent(window);

    return window;
}
void afWindowFini(GLFWwindow *window) {
    glfwDestroyWindow(window);
}
