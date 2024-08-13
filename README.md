
# Octree Example

Octree Example is an OpenGL-based application demonstrating the use of an octree structure for dynamically managing level of detail in a 3D scene. The octree is used to represent a cubic terrain block, which subdivides based on the camera's proximity to the nodes, allowing for efficient rendering of large, complex scenes.

## Features

- Real-time dynamic level of detail using an octree structure
- Interactive camera controls for exploring the scene
- Efficient mesh generation based on octree subdivisions

## Dependencies

This project relies on the following libraries, managed using [vcpkg](https://github.com/microsoft/vcpkg):

- **OpenGL**: Core rendering API
- **GLEW**: OpenGL Extension Wrangler Library
- **GLFW**: Library for handling window and input events
- **GLM**: OpenGL Mathematics library

## Getting Started

### Prerequisites

- **CMake** 3.10 or higher
- **vcpkg**: Package manager for installing dependencies
- **Visual Studio 2019/2022** or a similar C++20 compliant compiler

### Installing Dependencies

1. **Clone the vcpkg repository**:

   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   ```

2. **Bootstrap vcpkg**:

   - **On Windows**:

     ```bash
     .\bootstrap-vcpkg.bat
     ```

   - **On Unix**:

     ```bash
     ./bootstrap-vcpkg.sh
     ```

3. **Install required packages**:

   ```bash
   .\vcpkg install glew:x64-windows glfw3:x64-windows glm:x64-windows
   ```

### Building the Project

1. **Clone the Octree Example repository**:

   ```bash
   git clone <repository-url>
   cd OctreeExample/OctreeExample
   ```

2. **Create a build directory**:

   ```bash
   mkdir build
   cd build
   ```

3. **Configure the project with CMake**:

   ```bash
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

   Replace `C:/path/to/vcpkg` with the actual path to your vcpkg installation.

4. **Build the project**:

   ```bash
   cmake --build .
   ```

### Running the Application

After building, navigate to the `bin` directory inside your build folder and run the `OctreeExample.exe`:

```bash
cd bin
OctreeExample.exe
```

## Instructions

- **Camera Controls**: Use WASD to move the camera around the scene.
- **Mouse Control**: The mouse is used to look around.
- **Octree Interaction**: The octree dynamically subdivides as the camera approaches nodes, increasing the level of detail.

## Project Structure

- `Main.cpp`: Entry point for the application, handles the octree generation and rendering.
- `OctreeNode.h/cpp`: Defines the structure and behavior of the octree nodes.
- `Input.cpp`: Handles user input for camera movement.
- `Shader.cpp`: Manages shader programs used for rendering.
- `CMakeLists.txt`: Build configuration file.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request for any features or bug fixes you would like to add.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

This project uses open-source libraries provided by the community. Special thanks to the contributors of GLEW, GLFW, GLM, and the broader OpenGL community.
