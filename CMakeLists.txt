cmake_minimum_required (VERSION 3.8)
project ("voxelc")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set GLFW and GLAD root directories
set(GLFW_ROOT "${CMAKE_SOURCE_DIR}/glfw")
set(GLAD_ROOT "${CMAKE_SOURCE_DIR}/glad")

# Gather source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

# Create executable
add_executable(voxelc ${SOURCES})

# Include directories
target_include_directories(voxelc PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${GLFW_ROOT}/include" 
    "${GLAD_ROOT}/include"
)


# Add glad.c as a source file
set(GLAD_SRC "${GLAD_ROOT}/src/glad.c")

# Link libraries (GLFW and OpenGL)
if (WIN32)
    target_link_libraries(voxelc 
        "${GLFW_ROOT}/lib/glfw3.lib"  # Link GLFW
        opengl32                     # Link OpenGL
    )

    # Add glad.c as a source file
    target_sources(voxelc PRIVATE ${GLAD_SRC})
else()
    message(FATAL_ERROR "Unsupported platform. Linux and MacOS support coming soon!")
endif()
