# SNHU_CS330: 3D Scene Rendering Project in OpenGL

## Project Overview

This project is a 3D scene rendering application developed in C++ using OpenGL. It demonstrates the rendering of various 3D shapes, texture mapping, transformations, and basic lighting to create an immersive visual scene. The focus is on applying textures to 3D objects using relative paths, ensuring portability across different systems.

## Key Features

- 3D Shape Rendering:
    - Plane
    - Cylinder
    - Box
    - Sphere
    - Cone

- Texture Mapping:
    - Application of multiple textures to different objects.
    - Use of relative paths for loading texture files.

- Transformations:
    - Scaling, rotation, and translation of objects.

- Basic Lighting:
    - Simple lighting effects applied to enhance the visual scene.

---

## Project Structure

```plaintext
ProjectTitle/
├── bin/                  // Contains the executable
├── textures/             // Contains all texture files
│   ├── brick.jpg
│   ├── wood-for-wood.jpg
│   ├── lawn_circle.jpg
│   ├── aluminum-foil-triangle.jpg
│   ├── granite-cube.jpg
│   └── gold-cylinder.jpg
├── src/                  // Contains source code files
│   ├── SceneManager.cpp
│   ├── SceneManager.h
│   ├── ShaderManager.cpp
│   ├── ShaderManager.h
│   ├── ShapeMeshes.cpp
│   ├── ShapeMeshes.h
│   └── main.cpp
├── shaders/              // Contains shader files
│   ├── vertex_shader.glsl
│   └── fragment_shader.glsl
├── include/              // Contains header files and external libraries
└── README.md             // Project documentation
```

---

## Installation Instructions

### Prerequisites

- **C++ Compiler** (supporting C++11 or higher)
- **OpenGL**
- **GLFW**
- **GLEW** or **GLAD**
- **GLM** (OpenGL Mathematics Library)
- **stb_image** (included in the `include/` directory)

### Steps

1. **Clone the Repository**:
    ```bash
    git clone https://github.com/yourusername/your-repo-name.git
    ```

2. **Navigate to the Project Directory**:
    ```bash
    cd your-repo-name
    ```

3. **Set Up Dependencies**:
    - Ensure OpenGL, GLFW, and GLEW/GLAD are installed.
    - Update your build configuration to include paths to these libraries.

4. **Place the Texture Files**:
    - Verify that the following texture files are in the `textures/` folder:
        - `brick.jpg`
        - `wood-for-wood.jpg`
        - `lawn_circle.jpg`
        - `aluminum-foil-triangle.jpg`
        - `granite-cube.jpg`
        - `gold-cylinder.jpg`

5. **Build the Project**:
    - **Using a Makefile**:
        ```bash
        make
        ```
    - **Using CMake**:
        ```bash
        mkdir build
        cd build
        cmake ..
        make
        ```

    - **Using an IDE**:
        - Open the project in your preferred IDE (e.g., Visual Studio, CLion).
        - Configure the project settings to include the required libraries.
        - Build the project.

---

## Usage

1. **Run the Executable**:
    - From the command line:
        ```bash
        ./bin/YourExecutableName
        ```
    - Or run it directly from your IDE.

2. **Interact with the Scene**:
    - Once launched, the application will render the 3D scene with textures applied.
    - Use mouse and keyboard inputs (if implemented) to interact with the scene.

---

## Important Notes

- **Relative Paths**: 
    - The project uses relative paths for texture files. Ensure the `textures/` folder is in the correct location relative to the executable.
    - Paths in `SceneManager.cpp` are defined as `textures/your-texture-file.jpg`.

- **File Names**:
    - Avoid using spaces in file names to prevent loading issues. Replace spaces with underscores or hyphens if necessary.

```cpp
if (!CreateGLTexture("textures/wood-for-wood.jpg", "woodTexture")) {
    std::cout << "Failed to load wood texture!" << std::endl;
}
```

---

## Code Overview

- **SceneManager.cpp**:
    - **Texture Loading**:
        - Textures are loaded via the `CreateGLTexture` function using relative paths.
        
    - **Rendering**:
        - The `RenderScene` function applies transformations (scaling, rotation, translation) and renders the objects.

```cpp
// Example: Applying texture and rendering the plane
SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
SetShaderTexture("brickTexture");
m_basicMeshes->DrawPlaneMesh();
```

---

## Dependencies

- **OpenGL**: Core rendering library.
- **GLFW**: Window and input handling.
- **GLEW**/**GLAD**: OpenGL extension loader.
- **GLM**: Mathematics library for transformations.
- **stb_image**: Image loading library (included in `include/`).

---

## Troubleshooting

- **Textures Not Loading**:
    - Ensure texture files are in the correct folder (`textures/`).
    - Verify that the paths in `SceneManager.cpp` match the actual file locations.
    - Check for typos in file names and paths.

- **Application Crashes**:
    - Run the app from the command line to check for errors.
    - Verify shader compilation and check OpenGL error messages.
    - Ensure all dependencies are properly installed.

- **Build Errors**:
    - Double-check include paths and library links in your build configuration.
    - Ensure you're using a compatible C++ compiler version.

---

## Acknowledgments

- **Professor Eugenio Rodriguez** (e.rodriguez1@snhu.edu) – For guidance and support.
- **OpenGL Community & Tutorials** – For invaluable learning resources.
- **stb_image** – For simplifying texture loading.

---

## Contact

- **Name**: Victor Udeh
- **Email**: victor.o.udeh@gmail.com
- **GitHub**: [Vhicktour](https://github.com/Vhicktour)
