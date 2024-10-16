# SNHU_CS330: 3D Scene Rendering Project in OpenGL

## Project Overview

This project is a 3D scene rendering application developed in C++ using OpenGL. It demonstrates the rendering of various 3D shapes, texture mapping, transformations, and lighting to create an immersive visual scene. The focus is on applying textures to 3D objects using relative paths, ensuring portability across different systems.

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
│   
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


# Reflection

1. How do I approach designing software?

	•	New design skills: This project has sharpened my skills in planning and implementing texture mapping and scene management within 3D rendering environments. Learning to handle transformations and object hierarchies was also a valuable addition to my skillset.
	•	Design process: I followed an iterative design process, where I first outlined the key elements of the 3D scene, built the basic shapes, and progressively added textures and lighting.
	•	Future applications: My design approach can be applied in future projects involving complex environments, as breaking the project into smaller, manageable components ensures that I maintain control and focus on one aspect at a time.

2. How do I approach developing programs?

	•	New development strategies: While developing the 3D scene, I focused on modular code design, ensuring each feature (e.g., shapes, lighting, textures) was isolated into separate modules, making the code easier to debug and maintain.
	•	Iteration: Iteration played a significant role in my development process. For example, I frequently adjusted object transformations and lighting parameters to improve visual realism, gradually refining the scene’s appearance.
	•	Evolution of development approach: Throughout the milestones, my coding practices improved in terms of efficiency and clarity. By the project’s completion, I had adopted a more systematic approach to testing each component before integrating it into the larger scene.

3. How can computer science help me in reaching my goals?

	•	Educational pathway: Computational graphics and visualizations have expanded my understanding of 3D environments, which is vital for fields like game development and simulations. These skills will be essential as I continue my education and delve deeper into advanced graphics programming.
	•	Professional pathway: The ability to create visual representations through computational graphics adds a valuable tool to my skillset as a software engineer. These skills will be critical in industries that require visualization, simulation, or immersive interfaces, such as VR, AR, or even architectural visualization.

Dependencies

	•	OpenGL: Core rendering library.
	•	GLFW: Window and input handling.
	•	GLEW/GLAD: OpenGL extension loader.
	•	GLM: Mathematics library for transformations.
	•	stb_image: Image loading library (included in include/).

Troubleshooting

	•	Textures Not Loading:
	•	Ensure texture files are in the correct folder (textures/).
	•	Verify that the paths in SceneManager.cpp match the actual file locations.
	•	Check for typos in file names and paths.
	•	Application Crashes:
	•	Run the app from the command line to check for errors.
	•	Verify shader compilation and check OpenGL error messages.
	•	Ensure all dependencies are properly installed.
	•	Build Errors:
	•	Double-check include paths and library links in your build configuration.
	•	Ensure you’re using a compatible C++ compiler version.

Acknowledgments

	•	Professor Eugenio Rodriguez (e.rodriguez1@snhu.edu) – For guidance and support.
	•	OpenGL Community & Tutorials – For invaluable learning resources.
	•	stb_image – For simplifying texture loading.

Contact

	•	Name: Victor Udeh
	•	Email: victor.o.udeh@gmail.com
	•	GitHub: Vhicktour

