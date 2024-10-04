///////////////////////////////////////////////////////////////////////////////
// ViewManager.cpp
// ===============
// Manages the camera and projection for viewing 3D objects within the viewport
//
// AUTHOR: Victor Udeh
// Created for personal project involving 3D graphics using OpenGL
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"

// GLM Math Header inclusions for matrix and vector transformations
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>    

// Declaration of the global variables and defines
namespace
{
    // Window dimensions
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 800;

    // Shader variable names for view and projection matrices
    const char* g_ViewName = "view";
    const char* g_ProjectionName = "projection";

    // Mouse movement control variables
    float gLastX = WINDOW_WIDTH / 2.0f;  // Last recorded X position of the mouse
    float gLastY = WINDOW_HEIGHT / 2.0f; // Last recorded Y position of the mouse
    bool gFirstMouse = true;  // Check if this is the first time receiving mouse movement

    // Timing variables to control frame time
    float gDeltaTime = 0.0f;  // Time difference between the current and previous frame
    float gLastFrame = 0.0f;  // Time of the last frame
}

// Static member initialization
ViewManager* ViewManager::s_Instance = nullptr;  // Singleton pattern instance

/***********************************************************
 *  ViewManager()
 *
 *  Constructor for the ViewManager class.
 *  It initializes the camera and sets default camera parameters.
 ***********************************************************/
ViewManager::ViewManager(ShaderManager* pShaderManager)
{
    // Set the static instance pointer for singleton access
    s_Instance = this;
    m_pShaderManager = pShaderManager;  // Assign shader manager to class member
    m_pWindow = nullptr;  // Initialize window pointer to nullptr

    // Create and initialize camera object with default parameters
    m_pCamera = new Camera();
    m_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);  // Set camera position
    m_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);   // Camera is looking slightly downward
    m_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);        // Define the up direction
    m_pCamera->Zoom = 80.0f;                            // Initial zoom level
    m_pCamera->MovementSpeed = 2.5f;                    // Default camera movement speed
    m_cameraSpeed = 2.5f;                               // Initialize the camera speed for movement
    m_bOrthographicProjection = false;                  // Default to perspective projection
}

/***********************************************************
 *  ~ViewManager()
 *
 *  Destructor for the ViewManager class.
 *  It releases the allocated memory and resets static instance.
 ***********************************************************/
ViewManager::~ViewManager()
{
    // Reset the static instance pointer to null
    s_Instance = nullptr;
    // Release the shader manager and window pointers
    m_pShaderManager = nullptr;
    m_pWindow = nullptr;
    // Delete the camera object and free the memory
    delete m_pCamera;
    m_pCamera = nullptr;
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method creates the main display window using GLFW.
 *  It also sets input modes for capturing mouse and keyboard events.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
    // Create the GLFW window with given width, height, and title
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, nullptr, nullptr);
    if (window == nullptr)
    {
        // If window creation fails, print error message and terminate GLFW
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // Set the current OpenGL context to the newly created window
    glfwMakeContextCurrent(window);

    // Capture all mouse events by disabling the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set callbacks for mouse position, scroll, and keyboard input events
    glfwSetCursorPosCallback(window, Mouse_Position_Callback);
    glfwSetScrollCallback(window, Mouse_Scroll_Callback);
    glfwSetKeyCallback(window, Key_Callback);

    // Enable blending for transparency in rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Store the window pointer in the class member for later use
    m_pWindow = window;
    return window;
}

/***********************************************************
 *  PrepareSceneView()
 *
 *  This method prepares the 3D scene by updating the camera
 *  view and projection matrices, and sending them to the shader.
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
    // Calculate the time difference between frames to ensure smooth motion
    float currentFrame = glfwGetTime();
    gDeltaTime = currentFrame - gLastFrame;
    gLastFrame = currentFrame;

    // Process user input (keyboard events)
    ProcessKeyboardEvents();

    // Get the camera view matrix for rendering the scene from the camera's perspective
    glm::mat4 view = m_pCamera->GetViewMatrix();
    // Send the view matrix to the shader for transforming the scene
    m_pShaderManager->setMat4Value(g_ViewName, view);

    // Update and send the projection matrix to the shader
    UpdateProjection();

    // Send the camera position to the shader (useful for lighting calculations)
    m_pShaderManager->setVec3Value("viewPosition", m_pCamera->Position);
}

/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method processes keyboard inputs to move the camera
 *  in the 3D scene based on user input (W, A, S, D, Q, E).
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
    // Close the window if the ESC key is pressed
    if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_pWindow, true);

    // Adjust the camera's position based on user input (W, A, S, D, Q, E)
    float velocity = m_cameraSpeed * gDeltaTime;  // Movement speed depends on time between frames
    if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(FORWARD, velocity);   // Move forward
    if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(BACKWARD, velocity);  // Move backward
    if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(LEFT, velocity);      // Move left
    if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(RIGHT, velocity);     // Move right
    if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(UP, velocity);        // Move upward
    if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(DOWN, velocity);      // Move downward
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This callback is invoked whenever the mouse is moved.
 *  It calculates the mouse movement offset and updates the camera view.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xpos, double ypos)
{
    if (s_Instance == nullptr) return;  // Check if the instance is valid

    // If this is the first mouse event, initialize the last recorded positions
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    // Calculate the offset of the mouse movement
    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos;  // Reversed since y-coordinates go from bottom to top

    // Update the last recorded mouse positions
    gLastX = xpos;
    gLastY = ypos;

    // Pass the mouse movement offsets to the camera for updating the view
    s_Instance->m_pCamera->ProcessMouseMovement(xoffset, yoffset);
}

/***********************************************************
 *  Mouse_Scroll_Callback()
 *
 *  This callback is triggered when the mouse wheel is scrolled.
 *  It adjusts the camera's movement speed based on scroll input.
 ***********************************************************/
void ViewManager::Mouse_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (s_Instance == nullptr) return;  // Check if the instance is valid

    // Adjust the camera's movement speed based on the scroll input (zoom in/out effect)
    s_Instance->m_pCamera->MovementSpeed += static_cast<float>(yoffset) * 0.1f;
    // Clamp the movement speed to a minimum of 0.1 and a maximum of 10.0
    s_Instance->m_pCamera->MovementSpeed = std::max(0.1f, std::min(s_Instance->m_pCamera->MovementSpeed, 10.0f));
}

/***********************************************************
 *  Key_Callback()
 *
 *  This callback is invoked whenever a key is pressed.
 *  It handles toggling between orthographic and perspective projections.
 ***********************************************************/
void ViewManager::Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (s_Instance == nullptr) return;  // Check if the instance is valid

    // Switch to perspective projection when the 'P' key is pressed
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        s_Instance->m_bOrthographicProjection = false;
        s_Instance->UpdateProjection();  // Update the projection matrix
    }
    // Switch to orthographic projection when the 'O' key is pressed
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        s_Instance->m_bOrthographicProjection = true;
        s_Instance->UpdateProjection();  // Update the projection matrix
    }
}

/***********************************************************
 *  UpdateProjection()
 *
 *  This method updates the projection matrix based on whether
 *  the projection is orthographic or perspective.
 ***********************************************************/
void ViewManager::UpdateProjection()
{
    glm::mat4 projection;  // Projection matrix

    // Set the projection matrix to orthographic or perspective depending on the current mode
    if (m_bOrthographicProjection)
    {
        // Calculate the aspect ratio and set up the orthographic projection matrix
        float aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
        projection = glm::ortho(-10.0f * aspect, 10.0f * aspect, -10.0f, 10.0f, 0.1f, 100.0f);
    }
    else
    {
        // Set up the perspective projection matrix
        projection = glm::perspective(glm::radians(m_pCamera->Zoom), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);
    }

    // Send the projection matrix to the shader for use in rendering
    m_pShaderManager->setMat4Value(g_ProjectionName, projection);
}
