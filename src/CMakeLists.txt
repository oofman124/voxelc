# Gather source files
file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.hpp")

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

target_sources(voxelc PRIVATE 
    "${GLAD_ROOT}/src/glad.c"
    "Core/stb_image_impl.cpp"
    "Core/blockDatabase.cpp"
    "Core/block.cpp"
)

if (WIN32)
    target_link_libraries(voxelc 
        PRIVATE
        "${GLFW_ROOT}/lib/glfw3.lib"  # Link GLFW
        opengl32                       # Link OpenGL
    )
else()
    message(FATAL_ERROR "Unsupported platform. Linux and MacOS support coming soon!")
endif()