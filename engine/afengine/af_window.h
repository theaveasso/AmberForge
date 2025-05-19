#pragma once

#include "amber_forge.h"

#include <GLFW/glfw3.h>
#include <cglm/vec2.h>

struct GLFWwindow;

typedef enum
{
    AF_AXIS_HORIZONTAL,
    AF_AXIS_VERTICAL,
    AF_AXIS_MAX_COUNT
} AFaxis;

typedef enum
{
    AF_MOUSE_BUTTON_LEFT,
    AF_MOUSE_BUTTON_MIDDLE,
    AF_MOUSE_BUTTON_RIGHT,
    AF_MOUSE_BUTTON_MAX_COUNT
} AFmouse_button;

typedef enum
{
    AF_KEY_CODE_UNKNOWN       = -1,
    AF_KEY_CODE_SPACE         = 32,
    AF_KEY_CODE_APOSTROPHE    = 39,
    AF_KEY_CODE_COMMA         = 44,
    AF_KEY_CODE_MINUS         = 45,
    AF_KEY_CODE_PERIOD        = 46,
    AF_KEY_CODE_SLASH         = 47,
    AF_KEY_CODE_0             = 48,
    AF_KEY_CODE_1             = 49,
    AF_KEY_CODE_2             = 50,
    AF_KEY_CODE_3             = 51,
    AF_KEY_CODE_4             = 52,
    AF_KEY_CODE_5             = 53,
    AF_KEY_CODE_6             = 54,
    AF_KEY_CODE_7             = 55,
    AF_KEY_CODE_8             = 56,
    AF_KEY_CODE_9             = 57,
    AF_KEY_CODE_SEMICOLON     = 59,
    AF_KEY_CODE_EQUAL         = 61,
    AF_KEY_CODE_A             = 65,
    AF_KEY_CODE_B             = 66,
    AF_KEY_CODE_C             = 67,
    AF_KEY_CODE_D             = 68,
    AF_KEY_CODE_E             = 69,
    AF_KEY_CODE_F             = 70,
    AF_KEY_CODE_G             = 71,
    AF_KEY_CODE_H             = 72,
    AF_KEY_CODE_I             = 73,
    AF_KEY_CODE_J             = 74,
    AF_KEY_CODE_K             = 75,
    AF_KEY_CODE_L             = 76,
    AF_KEY_CODE_M             = 77,
    AF_KEY_CODE_N             = 78,
    AF_KEY_CODE_O             = 79,
    AF_KEY_CODE_P             = 80,
    AF_KEY_CODE_Q             = 81,
    AF_KEY_CODE_R             = 82,
    AF_KEY_CODE_S             = 83,
    AF_KEY_CODE_T             = 84,
    AF_KEY_CODE_U             = 85,
    AF_KEY_CODE_V             = 86,
    AF_KEY_CODE_W             = 87,
    AF_KEY_CODE_X             = 88,
    AF_KEY_CODE_Y             = 89,
    AF_KEY_CODE_Z             = 90,
    AF_KEY_CODE_LEFT_BRACKET  = 91,
    AF_KEY_CODE_BACKSLASH     = 92,
    AF_KEY_CODE_RIGHT_BRACKET = 93,
    AF_KEY_CODE_GRAVE_ACCENT  = 96,
    AF_KEY_CODE_WORLD_1       = 161,
    AF_KEY_CODE_WORLD_2       = 162,
    AF_KEY_CODE_ESCAPE        = 256,
    AF_KEY_CODE_ENTER         = 257,
    AF_KEY_CODE_TAB           = 258,
    AF_KEY_CODE_BACKSPACE     = 259,
    AF_KEY_CODE_INSERT        = 260,
    AF_KEY_CODE_DELETE        = 261,
    AF_KEY_CODE_RIGHT         = 262,
    AF_KEY_CODE_LEFT          = 263,
    AF_KEY_CODE_DOWN          = 264,
    AF_KEY_CODE_UP            = 265,
    AF_KEY_CODE_PAGE_UP       = 266,
    AF_KEY_CODE_PAGE_DOWN     = 267,
    AF_KEY_CODE_HOME          = 268,
    AF_KEY_CODE_END           = 269,
    AF_KEY_CODE_CAPS_LOCK     = 280,
    AF_KEY_CODE_SCROLL_LOCK   = 281,
    AF_KEY_CODE_NUM_LOCK      = 282,
    AF_KEY_CODE_PRINT_SCREEN  = 283,
    AF_KEY_CODE_PAUSE         = 284,
    AF_KEY_CODE_F1            = 290,
    AF_KEY_CODE_F2            = 291,
    AF_KEY_CODE_F3            = 292,
    AF_KEY_CODE_F4            = 293,
    AF_KEY_CODE_F5            = 294,
    AF_KEY_CODE_F6            = 295,
    AF_KEY_CODE_F7            = 296,
    AF_KEY_CODE_F8            = 297,
    AF_KEY_CODE_F9            = 298,
    AF_KEY_CODE_F10           = 299,
    AF_KEY_CODE_F11           = 300,
    AF_KEY_CODE_F12           = 301,
    AF_KEY_CODE_F13           = 302,
    AF_KEY_CODE_F14           = 303,
    AF_KEY_CODE_F15           = 304,
    AF_KEY_CODE_F16           = 305,
    AF_KEY_CODE_F17           = 306,
    AF_KEY_CODE_F18           = 307,
    AF_KEY_CODE_F19           = 308,
    AF_KEY_CODE_F20           = 309,
    AF_KEY_CODE_F21           = 310,
    AF_KEY_CODE_F22           = 311,
    AF_KEY_CODE_F23           = 312,
    AF_KEY_CODE_F24           = 313,
    AF_KEY_CODE_F25           = 314,
    AF_KEY_CODE_KP_0          = 320,
    AF_KEY_CODE_KP_1          = 321,
    AF_KEY_CODE_KP_2          = 322,
    AF_KEY_CODE_KP_3          = 323,
    AF_KEY_CODE_KP_4          = 324,
    AF_KEY_CODE_KP_5          = 325,
    AF_KEY_CODE_KP_6          = 326,
    AF_KEY_CODE_KP_7          = 327,
    AF_KEY_CODE_KP_8          = 328,
    AF_KEY_CODE_KP_9          = 329,
    AF_KEY_CODE_KP_DECIMAL    = 330,
    AF_KEY_CODE_KP_DIVIDE     = 331,
    AF_KEY_CODE_KP_MULTIPLY   = 332,
    AF_KEY_CODE_KP_SUBTRACT   = 333,
    AF_KEY_CODE_KP_ADD        = 334,
    AF_KEY_CODE_KP_ENTER      = 335,
    AF_KEY_CODE_KP_EQUAL      = 336,
    AF_KEY_CODE_LEFT_SHIFT    = 340,
    AF_KEY_CODE_LEFT_CONTROL  = 341,
    AF_KEY_CODE_LEFT_ALT      = 342,
    AF_KEY_CODE_LEFT_SUPER    = 343,
    AF_KEY_CODE_RIGHT_SHIFT   = 344,
    AF_KEY_CODE_RIGHT_CONTROL = 345,
    AF_KEY_CODE_RIGHT_ALT     = 346,
    AF_KEY_CODE_RIGHT_SUPER   = 347,
    AF_KEY_CODE_MENU          = 348,
    AF_KEY_CODE_MAX_COUNT
} AFkey_code;

typedef struct AFinput AFinput;
struct AFinput {
    vec2 position;
    vec2 delta;
    vec2 wheel;
};

extern AFinput *input;

AFAPI void afInputInit(GLFWwindow *window, struct AFarena *default_allocator);
AFAPI void afInputUpdate(GLFWwindow *window);
AFAPI void afInputFini(GLFWwindow *window, const struct AFgame_config *config);

AFAPI int afInputKeyPress(GLFWwindow *window, AFkey_code key);
AFAPI int afInputKeyUp(GLFWwindow *window, AFkey_code key);
AFAPI int afInputKeyDown(GLFWwindow *window, AFkey_code key);

AFAPI int afInputButtonPress(GLFWwindow *window, AFmouse_button button);
AFAPI int afInputButtonUp(GLFWwindow *window, AFmouse_button button);
AFAPI int afInputButtonDown(GLFWwindow *window, AFmouse_button button);

AFAPI float afInputAxis(AFaxis a);

GLFWwindow *afWindowInit(int width, int height, const char *title);
void afWindowFini(GLFWwindow *window);