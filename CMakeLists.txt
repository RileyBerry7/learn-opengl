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
# Vulkan
# =========================================================
#find_package(Vulkan REQUIRED)

# =========================================================
# Executable
# =========================================================
add_executable(learn-opengl
        main.cpp
)

# =========================================================
# Link Libraries
# =========================================================
target_link_libraries(learn-opengl PRIVATE
        glfw
)
