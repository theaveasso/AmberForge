project(AFEngine LANGUAGES C VERSION 0.1.0)

find_package(OpenGL REQUIRED)

add_library(${PROJECT_NAME} SHARED
        afengine/af_log.c
        afengine/af_engine.c
        afengine/af_memory.c
        afengine/af_window.c
        afengine/af_glrenderer.c
        afengine/amber_forge.c)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DAF_EXPORT -DGL_SILENCE_DEPRECATION _DEBUG)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad OpenGL::GL cglm)

set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
set(DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources")
if (EXISTS ${RESOURCE_DIR})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESOURCE_DIR}" "${DEST_DIR}")
else ()
    message(WARNING "[WARNING] no resource directory")
endif ()
