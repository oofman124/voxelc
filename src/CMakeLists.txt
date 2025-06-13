# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.15)

# Project name and version
project(voxelc VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(CORE_SOURCES
    "main.cpp"
    "Core/stb_image_impl.cpp"
    "Core/Block/blockDatabase.cpp"
    "Core/Input/inputManager.cpp"
    "Core/Block/block.cpp"
    "Core/world/world.cpp"
    "Core/World/chunkMesh.cpp"
    "Core/Renderer/renderer.cpp"
    "Core/Renderer/renderer2D.cpp"
)

set(GLAD_SOURCE "${GLAD_ROOT}/src/glad.c")

set(SOURCES ${CORE_SOURCES} ${GLAD_SOURCE})

# Create executable
add_executable(voxelc ${SOURCES})

# Include directories
target_include_directories(voxelc SYSTEM PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${GLFW_ROOT}/include"
    "${GLAD_ROOT}/include"
    "${FREETYPE_ROOT}/include"
)

# Set C++ standard for the target
target_compile_features(voxelc PRIVATE cxx_std_20)

# Platform-specific libraries
target_link_libraries(voxelc PRIVATE
    freetype
    $<$<PLATFORM_ID:Windows>:${GLFW_ROOT}/lib/glfw3.lib;opengl32>
)

# Compiler warnings
if (MSVC)
    target_compile_options(voxelc PRIVATE /W4)
else()
    target_compile_options(voxelc PRIVATE -Wall -Wextra -pedantic)
endif()

# Debug and Release configurations
target_compile_options(voxelc PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
)

# Installation rules
install(TARGETS voxelc RUNTIME DESTINATION bin)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include" DESTINATION include)
