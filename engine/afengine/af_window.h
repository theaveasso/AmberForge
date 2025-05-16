#pragma once
#include "GLFW/glfw3.h"

struct GLFWwindow;

GLFWwindow* afWindowInit(int width, int height, const char* title);
void afWindowFini(GLFWwindow* window);