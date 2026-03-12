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
# COPY SHADERS DIR TO BUILD DIR
# =========================================================
# Copy all shaders reliably on every build
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
        COMMENT "Copying all shaders..."
)
add_dependencies(learn-opengl copy_shaders)



# =========================================================
# COPY TEXTURES DIR TO BUILD DIR
# =========================================================
set(TEXTURE_DIR "${CMAKE_SOURCE_DIR}/resources/textures")

add_custom_command(TARGET learn-opengl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${TEXTURE_DIR}"
    $<TARGET_FILE_DIR:learn-opengl>/resources/textures
    COMMENT "Copying textures..."
)