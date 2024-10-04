///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport - camera, projection
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "camera.h"

// GLFW library
#include "GLFW/glfw3.h" 

class ViewManager
{
public:
	// constructor
	ViewManager(
		ShaderManager* pShaderManager);
	// destructor
	~ViewManager();

	// mouse position callback for mouse interaction with the 3D scene
	static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);

private:
	// pointer to shader manager object
	ShaderManager* m_pShaderManager;
	// active OpenGL display window
	GLFWwindow* m_pWindow;

	// process keyboard events for interaction with the 3D scene
	void ProcessKeyboardEvents();

public:
	// create the initial OpenGL display window
	GLFWwindow* CreateDisplayWindow(const char* windowTitle);
	
	// prepare the conversion from 3D object display to 2D scene display
	void PrepareSceneView();
};