cmake_minimum_required(VERSION 3.8)
project("voxelc")

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set directories
set(GLFW_ROOT "${CMAKE_SOURCE_DIR}/glfw")
set(GLAD_ROOT "${CMAKE_SOURCE_DIR}/glad")


# Add subdirectory for the main project
add_subdirectory(src)