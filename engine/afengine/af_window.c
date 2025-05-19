#include "af_window.h"
#include "af_log.h"
#include "af_mathf.h"
#include "af_memory.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>

#define AF_AXIS_SPEED 8

typedef struct AFinput_state AFinput_state;
typedef struct AFinput_data AFinput_data;
typedef struct AFinput_axis AFinput_axis;

struct AFinput_state {
    int current;
    int prev;
};

struct AFinput_axis {
    float value;
    float velocity;
};

struct AFinput_data {
    AFinput_state key_state[AF_KEY_CODE_MAX_COUNT];
    AFinput_state button_state[AF_KEY_CODE_MAX_COUNT];
    AFinput_axis axis[AF_KEY_CODE_MAX_COUNT];
};

GlobalVariable AFinput_data *global_input = NULL;
AFinput *input;

Internal AF_INLINE void af_update_axis(const int ax, const int low, const int high) {
    const float to     = 0.0f + (low ? -1.0f : 0.0f) + (high ? 1.0f : 0.0f);
    AFinput_axis *axis = &(global_input->axis[ax]);
    axis->value        = afMoveTowards(axis->value, to, AF_AXIS_SPEED * 0.16);
}
Internal AF_INLINE void af_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    (void) window;
    input->wheel[0] = (float) xoffset;
    input->wheel[1] = (float) yoffset;
}

void afInputInit(GLFWwindow *window, AFarena *default_allocator) {
    global_input = afArenaAlloc(default_allocator, sizeof(AFinput_state));
    input        = afArenaAlloc(default_allocator, sizeof(AFinput));

    glfwSetScrollCallback(window, af_scroll_callback);
}
void afInputUpdate(GLFWwindow *window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    input->delta[0] = (float) xpos - input->position[0];
    input->delta[1] = (float) ypos - input->position[1];

    input->position[0] = (float) xpos;
    input->position[1] = (float) ypos;

    input->wheel[0] = afMoveTowards(input->wheel[0], 0.0f, 0.2f);
    input->wheel[1] = afMoveTowards(input->wheel[1], 0.0f, 0.2f);

    for (int i = 0; i < AF_KEY_CODE_MAX_COUNT; ++i)
    {
        AFinput_state *state = &(global_input->key_state[i]);
        state->prev          = state->current;
    }
    for (int i = 0; i < AF_MOUSE_BUTTON_MAX_COUNT; ++i)
    {
        AFinput_state *state = &(global_input->button_state[i]);
        state->prev          = state->current;
    }

    af_update_axis(AF_AXIS_HORIZONTAL,
                   afInputKeyPress(window, AF_KEY_CODE_LEFT) || afInputKeyPress(window, AF_KEY_CODE_A),
                   afInputKeyPress(window, AF_KEY_CODE_RIGHT) || afInputKeyPress(window, AF_KEY_CODE_D));

    af_update_axis(AF_AXIS_VERTICAL,
                   afInputKeyPress(window, AF_KEY_CODE_DOWN) || afInputKeyPress(window, AF_KEY_CODE_S),
                   afInputKeyPress(window, AF_KEY_CODE_UP) || afInputKeyPress(window, AF_KEY_CODE_W));
}
void afInputFini(GLFWwindow *window, const AFgame_config *config) {
    const float pad    = 10;
    char changed       = 0;
    const float width  = (float) config->width;
    const float height = (float) config->height;

    if (input->delta[0] < 0 && input->position[0] < pad)
    {
        input->position[0] = width;
        if (input->delta[0] < -(width - pad)) { input->delta[0] += width - pad; }
        changed = 1;
    }
    if (input->delta[0] > 0 && input->position[0] > width - pad)
    {
        input->position[0] = 0;
        if (input->delta[0] > width - pad) { input->delta[0] -= width - pad; }
        changed = 1;
    }
    if (input->delta[1] < 0 && input->position[1] < pad)
    {
        input->position[1] = height;
        if (input->delta[1] < -(height - pad)) { input->delta[1] += (height - pad); }
        changed = 1;
    }
    if (input->delta[1] > 0 && input->position[1] > height - pad)
    {
        input->position[1] = 0;
        if (input->delta[1] > (height - pad)) { input->delta[1] -= (height - pad); }
    }
    if (changed) { glfwSetCursorPos(window, input->position[0], input->position[1]); }
}

int afInputKeyPress(GLFWwindow *window, const AFkey_code key) {
    AFinput_state *state = &(global_input->key_state[key]);
    state->current       = glfwGetKey(window, key);
    return state->current;
}

int afInputKeyUp(GLFWwindow *window, const AFkey_code key) {
    AFinput_state *state = &(global_input->key_state[key]);
    state->current       = glfwGetKey(window, key);
    return (!(state->current) && state->prev);
}

int afInputKeyDown(GLFWwindow *window, const AFkey_code key) {
    AFinput_state *state = &(global_input->key_state[key]);
    state->current       = glfwGetKey(window, key);
    return state->current && !state->prev;
}

int afInputButtonPress(GLFWwindow *window, const AFmouse_button button) {
    AFinput_state *state = &(global_input->button_state[button]);
    state->current       = glfwGetMouseButton(window, button);
    return state->current;
}

int afInputButtonUp(GLFWwindow *window, const AFmouse_button button) {
    AFinput_state *state = &(global_input->button_state[button]);
    state->current       = glfwGetMouseButton(window, button);
    return (!(state->current) && state->prev);
}

int afInputButtonDown(GLFWwindow *window, const AFmouse_button button) {
    AFinput_state *state = &(global_input->button_state[button]);
    state->current       = glfwGetMouseButton(window, button);
    return state->current && !state->prev;
}

float afInputAxis(const AFaxis a) {
    const AFinput_axis *state = &(global_input->axis[a]);
    return state->value;
}

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
