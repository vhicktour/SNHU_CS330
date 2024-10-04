///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene
	// Load all the meshes that will be used in the scene
	
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadSphereMesh();  // In case you need it later
	m_basicMeshes->LoadTorusMesh();   // In case you need it later

	m_basicMeshes->LoadPlaneMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes with
 *  wireframe edges for better visual detail.
 ***********************************************************/
void SceneManager::RenderScene()
{
	// Declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// Set line width for wireframe edges (optional)
	glLineWidth(1.5f);

	// Enable depth testing to ensure correct rendering order
	glEnable(GL_DEPTH_TEST);

	/*** Render each shape with filled and wireframe modes ***/
	/*********************************************************/

	// 1. Render plane (table surface)
	scaleXYZ = glm::vec3(15.0f, 1.2f, 15.0f);
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawPlaneMesh();
		}, glm::vec4(0.545f, 0.271f, 0.075f, 1.0f));

	// 2. Render cup (cylinder)
	scaleXYZ = glm::vec3(0.7f, 1.4f, 0.7f);
	positionXYZ = glm::vec3(0.0f, 0.0f, 3.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(false, true, true);
		}, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// 3. Render cup handle (half torus)
	scaleXYZ = glm::vec3(0.5f);
	positionXYZ = glm::vec3(-0.7f, 0.7f, 3.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
		m_basicMeshes->DrawHalfTorusMesh();
		}, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// 4. Render pen (cylinder)
	scaleXYZ = glm::vec3(0.07f, 3.0f, 0.07f);
	positionXYZ = glm::vec3(-2.0f, 0.1f, 6.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(1.0f, 0.843f, 0.0f, 1.0f));

	// 5. Render pen tip (cone)
	scaleXYZ = glm::vec3(0.06f, 0.3f, 0.061f);
	positionXYZ = glm::vec3(-5.0f, 0.1f, 6.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 75.0f, 90.0f, positionXYZ);
		m_basicMeshes->DrawConeMesh(true);
		}, glm::vec4(0.294f, 0.294f, 0.294f, 1.0f));

	// 6. Render book (box)
	scaleXYZ = glm::vec3(2.5f, 0.2f, 4.2f);
	positionXYZ = glm::vec3(2.0f, 0.3f, 6.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 15.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawBoxMesh();
		}, glm::vec4(0.596f, 1.0f, 0.596f, 1.0f));

	// 7. Render instrument base (cylinder)
	scaleXYZ = glm::vec3(1.9f, 0.5f, 1.9f);
	positionXYZ = glm::vec3(4.0f, 0.1f, 1.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	// 8. Render instrument knob (cylinder)
	scaleXYZ = glm::vec3(0.2f, 0.8f, 0.2f);
	positionXYZ = glm::vec3(2.7f, 0.1f, 1.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	// 9. Render instrument arm (cylinder) base
	scaleXYZ = glm::vec3(0.1f, 1.0f, 0.1f);
	positionXYZ = glm::vec3(6.5f, 1.1f, 1.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	// 10. Render instrument hook to arm (cylinder)
	scaleXYZ = glm::vec3(0.5f, 1.0f, 0.5f);
	positionXYZ = glm::vec3(5.5f, 0.1f, 1.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	scaleXYZ = glm::vec3(0.1f, 1.2f, 0.1f);
	positionXYZ = glm::vec3(5.5f, 0.1f, 1.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	// 11. Render instrument stand (cylinder)
	scaleXYZ = glm::vec3(0.1f, 8.2f, 0.1f);
	positionXYZ = glm::vec3(6.5f, 0.7f, 1.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	// 12. Render instrument arm (cylinder) at top
	scaleXYZ = glm::vec3(0.1f, 3.0f, 0.1f);
	positionXYZ = glm::vec3(6.5f, 8.8f, 1.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	// 13. Render instrument hook to instrument (cylinder)
	scaleXYZ = glm::vec3(0.8f, 1.2f, 0.8f);
	positionXYZ = glm::vec3(4.0f, 7.7f, 1.0f);
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, true, true);
		}, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	// 14. Render instrument head/glass (cylinder)
	// Enable blending for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable depth writing for transparent objects
	glDepthMask(GL_FALSE);

	scaleXYZ = glm::vec3(1.7f, 2.7f, 1.7f);
	positionXYZ = glm::vec3(4.0f, 5.1f, 1.0f);
	RenderTransparentShape([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawCylinderMesh(true, false, true);
		}, glm::vec4(0.9f, 0.9f, 0.9f, 0.3f));  // Light gray with low alpha for transparency

	// Re-enable depth writing for subsequent opaque objects
	glDepthMask(GL_TRUE);

	// Disable blending
	glDisable(GL_BLEND);




	// 15. Render light bulb inside the instrument head (sphere)
	scaleXYZ = glm::vec3(0.8f, 0.8f, 0.8f);  // Adjust size as needed
	positionXYZ = glm::vec3(4.0f, 7.1f, 1.0f);  // Same position as the glass, but can be adjusted
	RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
		SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
		m_basicMeshes->DrawSphereMesh();
		}, glm::vec4(1.0f, 1.0f, 0.8f, 1.0f));  // Light yellow color for the bulb

	// Disable depth testing if not needed elsewhere
	glDisable(GL_DEPTH_TEST);
}

/***********************************************************
 *  RenderShapeWithEdges()
 *
 *  Helper function to render a shape with both filled and
 *  wireframe edges for better visual detail.
 ***********************************************************/
void SceneManager::RenderShapeWithEdges(std::function<void()> drawFunction, const glm::vec4& fillColor)
{
	// First pass: Render filled shape
	SetShaderColor(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawFunction();

	// Second pass: Render wireframe edges
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0f, -1.0f); // Offset to prevent z-fighting
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f); // Edge color (black)
	drawFunction();
	glDisable(GL_POLYGON_OFFSET_LINE);

	// Reset to fill mode for subsequent shapes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// transparency 
void SceneManager::RenderTransparentShape(std::function<void()> drawFunction, const glm::vec4& color)
{
	// Render filled shape
	SetShaderColor(color.r, color.g, color.b, color.a);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawFunction();

	// Render wireframe edges (optional, for additional detail)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	SetShaderColor(1.0f, 1.0f, 1.0f, color.a * 0.7f);  // Slightly more opaque edges
	drawFunction();

	// Reset to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}