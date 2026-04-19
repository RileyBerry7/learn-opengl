cmake_minimum_required(VERSION 3.20)

project(learn-opengl LANGUAGES C CXX)

# ---- C++ Standard ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# =========================================================
# GLFW
# =========================================================
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)


# =========================================================
# OpenGL
# =========================================================
find_package(OpenGL REQUIRED)


# =========================================================
# Executable
# =========================================================
add_executable(learn-opengl
        src/main.cpp
        src/glad.c
        src/shaderClass.h
        src/shaderClass.cpp
        src/vbo.cpp
        src/vbo.h
        src/ebo.cpp
        src/ebo.h
        src/vao.cpp
        src/vao.h
        src/stb.cpp
        src/texture.cpp
        src/texture.h
        src/camera.cpp
        src/camera.h
        src/Mesh.h
        src/object.h
        src/Renderer.cpp
        src/Renderer.h
        src/material.h
        src/tiny_obj_loader.cpp
        src/window.h
)


# =========================================================
# INCLUDE DIRECTORIES
# =========================================================
target_include_directories(learn-opengl PRIVATE include)


# =========================================================
# Link Libraries
# =========================================================
target_link_libraries(learn-opengl PRIVATE
        glfw
        OpenGL::GL
)

# =========================================================
# COPY SHADERS AND RESOURCES BEFORE BUILD
# =========================================================
# Always sync shaders
add_custom_target(sync_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
        COMMENT "Syncing shaders..."
)

add_dependencies(learn-opengl sync_shaders)

# Always sync resources
add_custom_target(sync_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources
        COMMENT "Syncing resources..."
)

add_dependencies(learn-opengl sync_resources)