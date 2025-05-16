project(AFEngine LANGUAGES C VERSION 0.1.0)

add_library(${PROJECT_NAME} SHARED
        afengine/af_log.c
        afengine/af_engine.c
        afengine/amber_forge.c)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DAF_EXPORT _DEBUG)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
