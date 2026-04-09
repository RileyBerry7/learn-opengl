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
        src/obj_parser.h
        src/obj_parser.c
        src/obj_parser.c
        src/OBJ_Loader.h
        src/camera.cpp
        src/camera.h
        src/json.h
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
# Shaders
add_custom_command(TARGET learn-opengl PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
        COMMENT "Copying shaders..."
)

# Resources (textures, models, etc.)
#add_custom_command(TARGET learn-opengl PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/resources
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
#        COMMENT "Copying resources..."
#)