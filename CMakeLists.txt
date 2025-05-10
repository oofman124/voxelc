cmake_minimum_required(VERSION 3.8)
project("voxelc")

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set directories
set(GLFW_ROOT "${CMAKE_SOURCE_DIR}/glfw")
set(GLAD_ROOT "${CMAKE_SOURCE_DIR}/glad")
set(IMGUI_ROOT "${CMAKE_SOURCE_DIR}/imgui")

# Add ImGui as a library
add_library(imgui STATIC
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    ${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp
    ${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_ROOT}
    ${IMGUI_ROOT}/backends
    ${GLFW_ROOT}/include
)

# Add subdirectory for the main project
add_subdirectory(src)