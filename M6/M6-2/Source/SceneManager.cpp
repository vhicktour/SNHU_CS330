///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//  Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#include <glm/gtx/transform.hpp>
#include <string>
#include <sstream>

#define TOTAL_POINT_LIGHTS 5

// declare the global variables
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
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
    m_pShaderManager = pShaderManager;
    m_basicMeshes = new ShapeMeshes();
    m_loadedTextures = 0; // Initialize member variable
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
    // free up the allocated memory
    m_pShaderManager = NULL;
    if (NULL != m_basicMeshes)
    {
        delete m_basicMeshes;
        m_basicMeshes = NULL;
    }
    // clear the collection of defined materials
    m_objectMaterials.clear();
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
        return false;
    }

    size_t index = 0;
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

    return bFound;
}

/***********************************************************
 *  SetTransformation()
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

    // matrix math is used to calculate the final model matrix
    modelView = translation * rotationZ * rotationY * rotationX * scale;
    if (NULL != m_pShaderManager)
    {
        // pass the model matrix into the shader
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
        // pass the color values into the shader
        m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
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

        // find the defined material that matches the tag
        bReturn = FindMaterial(materialTag, material);
        if (bReturn == true)
        {
            // pass the material properties into the shader
            m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
            m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
            m_pShaderManager->setFloatValue("material.shininess", material.shininess);
        }
    }
}

/***********************************************************
 *  DefineObjectMaterials()
 *
 *  This method is used for configuring the various material
 *  settings for all of the objects within the 3D scene.
 ***********************************************************/
void SceneManager::DefineObjectMaterials()
{
    /*** Define object materials ***/

    // Material for the plane (base)
    OBJECT_MATERIAL planeMaterial;
    planeMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f); // White diffuse color
    planeMaterial.specularColor = glm::vec3(0.5f, 0.5f, 0.5f); // Grey specular color
    planeMaterial.shininess = 32.0f; // Shininess factor
    planeMaterial.tag = "planeMaterial"; // Material tag
    m_objectMaterials.push_back(planeMaterial); // Add to materials list

    // Material for other objects
    OBJECT_MATERIAL objectMaterial;
    objectMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f); // White diffuse color
    objectMaterial.specularColor = glm::vec3(0.5f, 0.5f, 0.5f); // Grey specular color
    objectMaterial.shininess = 32.0f; // Shininess factor
    objectMaterial.tag = "objectMaterial"; // Material tag
    m_objectMaterials.push_back(objectMaterial); // Add to materials list
}

/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
    // Enable custom lighting in the shader
    m_pShaderManager->setBoolValue(g_UseLightingName, true);

    /*** Define the first point light (Orange Light) ***/
    m_pShaderManager->setBoolValue("pointLights[0].bActive", true);
    m_pShaderManager->setVec3Value("pointLights[0].position", glm::vec3(5.0f, 5.0f, 5.0f));
    m_pShaderManager->setVec3Value("pointLights[0].ambient", glm::vec3(0.2f, 0.1f, 0.0f));   // Orange ambient
    m_pShaderManager->setVec3Value("pointLights[0].diffuse", glm::vec3(1.0f, 0.5f, 0.0f));   // Orange diffuse
    m_pShaderManager->setVec3Value("pointLights[0].specular", glm::vec3(0.5f, 0.25f, 0.0f)); // Orange specular

    /*** Define the second point light (Teal Light) ***/
    m_pShaderManager->setBoolValue("pointLights[1].bActive", true);
    m_pShaderManager->setVec3Value("pointLights[1].position", glm::vec3(-5.0f, 5.0f, -5.0f));
    m_pShaderManager->setVec3Value("pointLights[1].ambient", glm::vec3(0.0f, 0.1f, 0.1f));    // Teal ambient
    m_pShaderManager->setVec3Value("pointLights[1].diffuse", glm::vec3(0.0f, 1.0f, 1.0f));    // Teal diffuse
    m_pShaderManager->setVec3Value("pointLights[1].specular", glm::vec3(0.0f, 0.5f, 0.5f));   // Teal specular

    // Deactivate other point lights if not used
    for (int i = 2; i < TOTAL_POINT_LIGHTS; ++i)
    {
        std::stringstream ss;
        ss << "pointLights[" << i << "].bActive";
        std::string lightName = ss.str();
        m_pShaderManager->setBoolValue(lightName.c_str(), false);
    }

    // Deactivate directional and spotlights if not used
    m_pShaderManager->setBoolValue("directionalLight.bActive", false);
    m_pShaderManager->setBoolValue("spotLight.bActive", false);
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes and textures into memory.
 ***********************************************************/
void SceneManager::PrepareScene()
{
    // define the materials for objects in the scene
    DefineObjectMaterials();
    // add and define the light sources for the scene
    SetupSceneLights();

    // Load the basic 3D meshes
    m_basicMeshes->LoadBoxMesh();
    m_basicMeshes->LoadPlaneMesh();
    m_basicMeshes->LoadCylinderMesh();
    m_basicMeshes->LoadConeMesh();
    m_basicMeshes->LoadSphereMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
    // Declare the variables for the transformations
    glm::vec3 scaleXYZ;
    float XrotationDegrees = 0.0f;
    float YrotationDegrees = 0.0f;
    float ZrotationDegrees = 0.0f;
    glm::vec3 positionXYZ;

    /*** Render the Plane (Base) ***/
    // Set the XYZ scale for the mesh
    scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

    // Set the XYZ rotation for the mesh
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;

    // Set the XYZ position for the mesh
    positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

    // Set the transformations into memory to be used on the drawn meshes
    SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);

    // Set the material for the plane
    SetShaderMaterial("planeMaterial");

    // Set the active color values in the shader (RGBA)
    SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Draw the mesh with transformation values - this plane is used for the base
    m_basicMeshes->DrawPlaneMesh();
    /****************************************************************/

    /*** Render the Cylinder ***/
    // Set the XYZ scale for the mesh
    scaleXYZ = glm::vec3(0.9f, 2.8f, 0.9f);

    // Set the XYZ rotation for the mesh
    XrotationDegrees = 90.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = -15.0f;

    // Set the XYZ position for the mesh
    positionXYZ = glm::vec3(0.0f, 0.9f, 0.4f);

    // Set the transformations
    SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);

    // Set the material for the cylinder
    SetShaderMaterial("objectMaterial");

    // Set the active color values in the shader (RGBA)
    SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Draw a filled cylinder shape
    m_basicMeshes->DrawCylinderMesh();
    /****************************************************************/

    /*** Render the First Box ***/
    // Set the XYZ scale for the mesh
    scaleXYZ = glm::vec3(1.0f, 9.0f, 1.3f);

    // Set the XYZ rotation for the mesh
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 95.0f;

    // Set the XYZ position for the mesh
    positionXYZ = glm::vec3(0.2f, 2.27f, 2.0f);

    // Set the transformations
    SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);

    // Set the material for the box
    SetShaderMaterial("objectMaterial");

    // Set the active color values in the shader (RGBA)
    SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Draw a filled box shape
    m_basicMeshes->DrawBoxMesh();
    /****************************************************************/

    /*** Render the Second Box ***/
    // Set the XYZ scale for the mesh
    scaleXYZ = glm::vec3(1.7f, 1.5f, 1.5f);

    // Set the XYZ rotation for the mesh
    XrotationDegrees = 0.0f;
    YrotationDegrees = 40.0f;
    ZrotationDegrees = 8.0f;

    // Set the XYZ position for the mesh
    positionXYZ = glm::vec3(3.3f, 3.83f, 2.19f);

    // Set the transformations
    SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);

    // Set the material for the box
    SetShaderMaterial("objectMaterial");

    // Set the active color values in the shader (RGBA)
    SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Draw a filled box shape
    m_basicMeshes->DrawBoxMesh();
    /****************************************************************/

    /*** Render the Sphere ***/
    // Set the XYZ scale for the mesh
    scaleXYZ = glm::vec3(1.0f, 1.0f, 1.0f);

    // Set the XYZ rotation for the mesh
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 0.0f;

    // Set the XYZ position for the mesh
    positionXYZ = glm::vec3(3.1f, 5.6f, 2.5f);

    // Set the transformations
    SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);

    // Set the material for the sphere
    SetShaderMaterial("objectMaterial");

    // Set the active color values in the shader (RGBA)
    SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Draw a filled sphere shape
    m_basicMeshes->DrawSphereMesh();
    /****************************************************************/

    /*** Render the Cone ***/
    // Set the XYZ scale for the mesh
    scaleXYZ = glm::vec3(1.2f, 4.0f, 1.2f);

    // Set the XYZ rotation for the mesh
    XrotationDegrees = 0.0f;
    YrotationDegrees = 0.0f;
    ZrotationDegrees = 5.0f;

    // Set the XYZ position for the mesh
    positionXYZ = glm::vec3(-3.3f, 2.48f, 2.0f);

    // Set the transformations
    SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);

    // Set the material for the cone
    SetShaderMaterial("objectMaterial");

    // Set the active color values in the shader (RGBA)
    SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Draw a filled cone shape
    m_basicMeshes->DrawConeMesh();
    /****************************************************************/
}
