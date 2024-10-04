///////////////////////////////////////////////////////////////////////////////
// ViewManager.h
// =============
// Declaration of the ViewManager class, which handles the creation of an
// OpenGL window, camera movement, keyboard/mouse input, and toggling between
// orthographic and perspective projections.
//
// AUTHOR: Victor Udeh
// Created for personal project involving 3D graphics using OpenGL
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "camera.h"
#include "GLFW/glfw3.h"

class ViewManager
{
public:
    /***********************************************************
     *  ViewManager(ShaderManager* pShaderManager)
     *
     *  Constructor for the ViewManager class.
     *  It initializes the shader manager and sets up default
     *  camera parameters.
     ***********************************************************/
    ViewManager(ShaderManager* pShaderManager);

    /***********************************************************
     *  ~ViewManager()
     *
     *  Destructor for the ViewManager class.
     *  It cleans up the camera and resets the static instance.
     ***********************************************************/
    ~ViewManager();

    /***********************************************************
     *  CreateDisplayWindow(const char* windowTitle)
     *
     *  Creates the main OpenGL display window using GLFW.
     *  It sets the necessary input modes and returns a pointer
     *  to the created window.
     ***********************************************************/
    GLFWwindow* CreateDisplayWindow(const char* windowTitle);

    /***********************************************************
     *  PrepareSceneView()
     *
     *  Prepares the view for rendering the 3D scene by updating
     *  the camera view and projection matrices, sending them
     *  to the shader for use in rendering.
     ***********************************************************/
    void PrepareSceneView();

    /***********************************************************
     *  Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
     *
     *  Static callback function for handling mouse movement events.
     *  It updates the camera's orientation based on the mouse position.
     ***********************************************************/
    static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);

    /***********************************************************
     *  Mouse_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset)
     *
     *  Static callback function for handling mouse scroll events.
     *  It adjusts the camera's movement speed based on scroll input.
     ***********************************************************/
    static void Mouse_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset);

    /***********************************************************
     *  Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
     *
     *  Static callback function for handling keyboard events.
     *  It processes input for toggling projection modes and interacting
     *  with the scene.
     ***********************************************************/
    static void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /***********************************************************
     *  GetInstance()
     *
     *  Static method to retrieve the singleton instance of the
     *  ViewManager class.
     ***********************************************************/
    static ViewManager* GetInstance() { return s_Instance; }

private:
    /***********************************************************
     *  ProcessKeyboardEvents()
     *
     *  Handles keyboard events for controlling the camera movement
     *  and interaction with the 3D scene.
     ***********************************************************/
    void ProcessKeyboardEvents();

    /***********************************************************
     *  ToggleProjection()
     *
     *  Switches between perspective and orthographic projection
     *  modes.
     ***********************************************************/
    void ToggleProjection();

    /***********************************************************
     *  UpdateProjection()
     *
     *  Updates the projection matrix for either orthographic
     *  or perspective projection, and sends it to the shader.
     ***********************************************************/
    void UpdateProjection();

    // Pointer to the ShaderManager object, used for sending matrices to shaders
    ShaderManager* m_pShaderManager;

    // Pointer to the active OpenGL display window created by GLFW
    GLFWwindow* m_pWindow;

    // Pointer to the Camera object, which handles view transformation and movement
    Camera* m_pCamera;

    // Boolean flag to track whether the projection is orthographic or perspective
    bool m_bOrthographicProjection;

    // Float value to store the speed at which the camera moves in the 3D scene
    float m_cameraSpeed;

    // Static pointer to the singleton instance of the ViewManager class
    static ViewManager* s_Instance;
};
