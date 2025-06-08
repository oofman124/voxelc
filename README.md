# VoxelC Engine
A modern C++ voxel game engine built with OpenGL.

## Overview
VoxelC is a voxel-based game engine that provides:
- Efficient chunk-based world management
- Modern OpenGL rendering
- Dynamic terrain generation 
- Multithreaded chunk loading/unloading
- Input management system
- Asset management system
- 2D/3D rendering capabilities

## Note:
**You should press ESC on startup!**
**You should expect a white screen for a little bit before the game loads!**


## Structure
```
voxelc/
├── src/                    # Source files
│   ├── Core/              # Core engine systems
│   │   ├── Input/        # Input management
│   │   ├── Renderer/     # 2D/3D renderers
│   │   ├── Rendering/    # Rendering utilities
│   │   ├── World/        # World/chunk management
│   │   ├── Block/        # Block system
│   │   └── Math/         # Math utilities
│   └── main.cpp          # Entry point
├── include/               # External libraries
│   ├── glad/             # OpenGL loader
│   ├── glfw/             # Window management
│   ├── glm/              # Math library
│   └── stb_image.h       # Image loading
├── resources/            # Assets
│   ├── shaders/         # GLSL shaders
│   └── textures/        # Textures
└── CMakeLists.txt       # Build configuration
```

## Building
Prerequisites:
- CMake 3.8+
- C++20 compatible compiler
- OpenGL 4.3+

```bash
# Clone the repository
git clone https://github.com/oofman124/voxelc.git
cd voxelc

# Configure with CMake
cmake -B build -S .

# Build
cmake --build build
```

## Usage
```cpp
// Initialize engine systems
auto renderer = std::make_shared<Renderer>();
renderer->initialize();

// Create world
auto world = std::make_shared<World>();

// Generate terrain
world->generateTerrain(12, 12);

// Main loop
while (!renderer->shouldClose()) {
    // Update world
    world->tickUpdate();
    
    // Render
    renderer->beginFrame();
    // ... render code ...
    renderer->endFrame();
}
```


## Controls
W,A,S,D: Move
Right mouse button + Mouse move: Rotate camera
ESC: Toggle Mouse Lock (rotates camera without right mouse button)


## Contributing
Contributions are welcome! Please feel free to submit pull requests.

### Guidelines
1. Follow the existing code style (unless the new code style is good)
2. Add tests for new features
3. Update documentation
4. Make focused, single-purpose changes

### Areas for Contribution
- Terrain generation algorithms
- Block types and behaviors  
- Rendering optimizations
- Physics system
- Multiplayer support
- Documentation improvements
- UI RENDERING!!!

## License
This project is licensed under the GNU General Public License v2.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments
- [GLFW](https://www.glfw.org/) for window management
- [GLM](https://github.com/g-truc/glm) for mathematics
- [stb](https://github.com/nothings/stb) for image loading
- All contributors

## Contact
- Create an issue on GitHub
- Message me on Github

