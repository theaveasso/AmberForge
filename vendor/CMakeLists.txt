project(AGVendor)

include(FetchContent)

set(GLFW_STANDALONE OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

macro(fetch_dep dep repo tag)
    if (NOT TARGET ${dep})
        FetchContent_Declare(
                ${dep}
                GIT_REPOSITORY ${repo}
                GIT_TAG ${tag}
        )
        FetchContent_MakeAvailable(${dep})
    endif ()
endmacro()

fetch_dep(glfw https://github.com/glfw/glfw.git 3.4)
fetch_dep(glad https://github.com/theaveasso/glad.git main)
fetch_dep(cglm https://github.com/recp/cglm.git v0.9.6)
fetch_dep(flecs https://github.com/SanderMertens/flecs.git v4.0.5)