///////////////////////////////////////////////////////////////////////////////
// SceneManager.cpp
// ================
// Manage the preparation and rendering of 3D scenes - textures, materials, lighting
//
// AUTHOR: Victor Udeh
// Created for personal project involving 3D graphics using OpenGL
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>
#include <iostream>
#include <functional>

// Declaration of global variables
namespace
{
    const char* g_ModelName = "model";
    const char* g_ColorValueName = "objectColor";
    const char* g_TextureValueName = "objectTexture";
    const char* g_UseTextureName = "bUseTexture";
    const char* g_UseLightingName = "bUseLighting";
}

SceneManager::SceneManager(ShaderManager* pShaderManager)
{
    m_pShaderManager = pShaderManager;
    m_basicMeshes = new ShapeMeshes();
    m_loadedTextures = 0;
}

SceneManager::~SceneManager()
{
    m_pShaderManager = NULL;
    delete m_basicMeshes;
    m_basicMeshes = NULL;
}

bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
    int width = 0;
    int height = 0;
    int colorChannels = 0;
    GLuint textureID = 0;

    // Indicate to always flip images vertically when loaded
    stbi_set_flip_vertically_on_load(true);

    // Try to parse the image data from the specified image file
    unsigned char* image = stbi_load(
        filename,
        &width,
        &height,
        &colorChannels,
        0);

    // If the image was successfully read from the image file
    if (image)
    {
        std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // If the loaded image is in RGB format
        if (colorChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        // If the loaded image is in RGBA format - it supports transparency
        else if (colorChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
            stbi_image_free(image);
            return false;
        }

        // Generate the texture mipmaps for mapping textures to lower resolutions
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free the image data from local memory
        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        // Register the loaded texture and associate it with the special tag string
        m_textureIDs[m_loadedTextures].ID = textureID;
        m_textureIDs[m_loadedTextures].tag = tag;
        m_loadedTextures++;

        return true;
    }

    std::cout << "Could not load image:" << filename << std::endl;

    // Error loading the image
    return false;
}

void SceneManager::BindGLTextures()
{
    for (int i = 0; i < m_loadedTextures; i++)
    {
        // Bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
    }
}

void SceneManager::DestroyGLTextures()
{
    for (int i = 0; i < m_loadedTextures; i++)
    {
        glDeleteTextures(1, &m_textureIDs[i].ID);
    }
}

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

    return (textureID);
}

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

    return (textureSlot);
}

void SceneManager::SetTransformations(
    glm::vec3 scaleXYZ,
    float XrotationDegrees,
    float YrotationDegrees,
    float ZrotationDegrees,
    glm::vec3 positionXYZ)
{
    // Variables for this method
    glm::mat4 modelView;
    glm::mat4 scale;
    glm::mat4 rotationX;
    glm::mat4 rotationY;
    glm::mat4 rotationZ;
    glm::mat4 translation;

    // Set the scale value in the transform buffer
    scale = glm::scale(scaleXYZ);
    // Set the rotation values in the transform buffer
    rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
    // Set the translation value in the transform buffer
    translation = glm::translate(positionXYZ);

    modelView = translation * rotationZ * rotationY * rotationX * scale;

    if (NULL != m_pShaderManager)
    {
        m_pShaderManager->setMat4Value(g_ModelName, modelView);
    }
}

void SceneManager::SetShaderColor(
    float redColorValue,
    float greenColorValue,
    float blueColorValue,
    float alphaValue)
{
    glm::vec4 currentColor(redColorValue, greenColorValue, blueColorValue, alphaValue);

    if (NULL != m_pShaderManager)
    {
        m_pShaderManager->setIntValue(g_UseTextureName, false);
        m_pShaderManager->setIntValue(g_UseLightingName, true); // Enable lighting
        m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
        SetShaderMaterial(); // Set material properties
    }
}

void SceneManager::SetShaderTexture(
    std::string textureTag)
{
    if (NULL != m_pShaderManager)
    {
        m_pShaderManager->setIntValue(g_UseTextureName, true);
        m_pShaderManager->setIntValue(g_UseLightingName, true); // Enable lighting

        int textureID = -1;
        textureID = FindTextureSlot(textureTag);
        m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
        SetShaderMaterial(); // Set material properties
    }
}

void SceneManager::SetTextureUVScale(float u, float v)
{
    if (NULL != m_pShaderManager)
    {
        m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
    }
}

void SceneManager::SetShaderMaterial()
{
    // Default material properties
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.5f, 0.5f, 0.5f));
    m_pShaderManager->setFloatValue("material.shininess", 32.0f);
}

void SceneManager::PrepareScene()
{
    // Load all the meshes that will be used in the scene
    m_basicMeshes->LoadBoxMesh();
    m_basicMeshes->LoadCylinderMesh();
    m_basicMeshes->LoadConeMesh();
    m_basicMeshes->LoadSphereMesh();
    m_basicMeshes->LoadTorusMesh();
    m_basicMeshes->LoadPlaneMesh();

    // Load textures for the scene
    CreateGLTexture("textures/glass_lamp.jpg", "glass_lamp");
    CreateGLTexture("textures/wooden_table.jpg", "wooden_table");
    CreateGLTexture("textures/green_book.jpg", "green_book");
    CreateGLTexture("textures/lightbulb_filament.jpg", "lightbulb_filament");

    // Bind the loaded textures
    BindGLTextures();
}

void SceneManager::RenderScene()
{
    // Set up lights
    SetUpDirectionalLight();  // External light from window
    SetUpPointLight();        // Light bulb

    // Enable depth testing to ensure correct rendering order
    glEnable(GL_DEPTH_TEST);

    // Declare the variables for the transformations
    glm::vec3 scaleXYZ;
    float XrotationDegrees = 0.0f;
    float YrotationDegrees = 0.0f;
    float ZrotationDegrees = 0.0f;
    glm::vec3 positionXYZ;

    // Set line width for wireframe edges (optional)
    glLineWidth(1.5f);

    /*** Render each shape with filled and wireframe modes ***/
    /*********************************************************/

    // 1. Render plane (table surface) with wood texture and lighting
    scaleXYZ = glm::vec3(15.0f, 1.2f, 15.0f);
    positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderTexture("wooden_table");
        SetTextureUVScale(1.0f, 1.0f);
        m_basicMeshes->DrawPlaneMesh();
        }, glm::vec4(1.0f));

    // 2. Render cup (cylinder) with thickness

    // Outer wall
    scaleXYZ = glm::vec3(0.7f, 1.4f, 0.7f);
    positionXYZ = glm::vec3(0.0f, 0.1f, 3.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f); // White color
        m_basicMeshes->DrawCylinderMesh(false, true, true);
        }, glm::vec4(1.0f));

    // Inner wall (slightly smaller scale for thickness)
    scaleXYZ = glm::vec3(0.65f, 1.4f, 0.65f); // Inner cylinder for thickness
    positionXYZ = glm::vec3(0.0f, 0.1f, 3.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f); // White color
        m_basicMeshes->DrawCylinderMesh(false, true, true);
        }, glm::vec4(1.0f));

    // 3. Render cup handle (half torus)
    scaleXYZ = glm::vec3(0.5f);
    positionXYZ = glm::vec3(-0.7f, 0.7f, 3.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
        SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f); // White color
        m_basicMeshes->DrawHalfTorusMesh();
        }, glm::vec4(1.0f));

    // 4. Render pencil body (cylinder)
    scaleXYZ = glm::vec3(0.07f, 3.0f, 0.07f);
    positionXYZ = glm::vec3(-2.0f, 0.1f, 6.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
        SetShaderColor(1.0f, 0.843f, 0.0f, 1.0f); // Yellow color
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(1.0f));

    // Render pencil ferrule (metallic band)
    scaleXYZ = glm::vec3(0.075f, 0.15f, 0.075f);  // Slightly larger radius, shorter height
    positionXYZ = glm::vec3(-2.0f, 0.1f, 6.0f);  // Position at the end of the pencil body
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
        SetShaderColor(0.75f, 0.75f, 0.75f, 1.0f); // Silver/gray color
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(1.0f));

    // Render pencil eraser (cylinder)
    scaleXYZ = glm::vec3(0.075f, 0.2f, 0.075f);  // Same radius as ferrule, slightly taller
    positionXYZ = glm::vec3(-1.8f, 0.1f, 6.0f);  // Positioned on top of the ferrule
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
        SetShaderColor(0.96f, 0.8f, 0.8f, 1.0f); // Pink color
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(1.0f));

    // Render pencil wooden tip (cone)
    scaleXYZ = glm::vec3(0.065f, 0.2f, 0.065f);
    positionXYZ = glm::vec3(-5.0f, 0.1f, 6.0f);  // Adjust this to align with the pencil body
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 75.0f, 90.0f, positionXYZ);
        SetShaderColor(0.824f, 0.706f, 0.549f, 1.0f); // Light brown color
        m_basicMeshes->DrawConeMesh(true);
        }, glm::vec4(1.0f));

    // 5. Render pencil graphite tip (small cone)
    scaleXYZ = glm::vec3(0.01f, 0.08f, 0.02f);
    positionXYZ = glm::vec3(-5.16f, 0.1f, 6.0f);  // Adjust this to be slightly ahead of the wooden tip
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 75.0f, 90.0f, positionXYZ);
        SetShaderColor(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray color
        m_basicMeshes->DrawConeMesh(true);
        }, glm::vec4(1.0f));

    // 6. Render book cover (box) with texture, lighting disabled
    scaleXYZ = glm::vec3(2.5f, 0.2f, 4.2f);
    positionXYZ = glm::vec3(2.0f, 0.3f, 6.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 15.0f, 0.0f, positionXYZ);
        SetShaderTexture("green_book");
        m_pShaderManager->setIntValue(g_UseLightingName, false); // Disable lighting for complex object
        m_basicMeshes->DrawBoxMesh();
        }, glm::vec4(1.0f));

    // Render book pages (box inside the cover)
    glm::vec3 pagesScale = glm::vec3(2.4f, 0.18f, 4.2f);  // Slightly smaller than the cover
    glm::vec3 pagesPosition = glm::vec3(2.1f, 0.3f, 6.0f); // Same position as cover

    RenderShapeWithEdges([this, pagesScale, pagesPosition]() {
        SetTransformations(pagesScale, 0.0f, 15.0f, 0.0f, pagesPosition);
        SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);  // White color for pages
        m_pShaderManager->setIntValue(g_UseLightingName, false); // Disable lighting
        m_basicMeshes->DrawBoxMesh();
        }, glm::vec4(1.0f));

    
    // 7. Render instrument base (cylinder)
    scaleXYZ = glm::vec3(1.9f, 0.5f, 1.9f);
    positionXYZ = glm::vec3(4.0f, 0.1f, 1.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(0.3f, 0.3f, 0.3f, 1.0f); // Dark gray color with lighting
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    // 8. Render instrument knob (cylinder)
    scaleXYZ = glm::vec3(0.2f, 0.8f, 0.2f);
    positionXYZ = glm::vec3(2.7f, 0.1f, 1.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(0.3f, 0.3f, 0.3f, 1.0f);
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    // 9. Render instrument arm (cylinder) base
    scaleXYZ = glm::vec3(0.1f, 1.0f, 0.1f);
    positionXYZ = glm::vec3(6.5f, 1.1f, 1.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
        SetShaderColor(0.3f, 0.3f, 0.3f, 1.0f);
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    // 10. Render instrument hook to arm (cylinder)
    scaleXYZ = glm::vec3(0.5f, 1.0f, 0.5f);
    positionXYZ = glm::vec3(5.5f, 0.1f, 1.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(0.3f, 0.3f, 0.3f, 1.0f);
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    scaleXYZ = glm::vec3(0.1f, 1.2f, 0.1f);
    positionXYZ = glm::vec3(5.5f, 0.1f, 1.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(0.3f, 0.3f, 0.3f, 1.0f);
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    // 11. Render instrument stand (cylinder)
    scaleXYZ = glm::vec3(0.1f, 8.2f, 0.1f);
    positionXYZ = glm::vec3(6.5f, 0.7f, 1.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(0.3f, 0.3f, 0.3f, 1.0f);
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    // 12. Render instrument arm (cylinder) at top
    scaleXYZ = glm::vec3(0.1f, 3.0f, 0.1f);
    positionXYZ = glm::vec3(6.5f, 8.8f, 1.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
        SetShaderColor(0.3f, 0.3f, 0.3f, 1.0f);
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    // 13. Render instrument bulb hook to instrument (cylinder)
    scaleXYZ = glm::vec3(0.8f, 1.2f, 0.8f);
    positionXYZ = glm::vec3(4.0f, 7.7f, 1.0f);
    RenderShapeWithEdges([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(0.3f, 0.3f, 0.3f, 1.0f);
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));


    // 14. Render instrument head/glass (cylinder) with thickness

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writing for transparent objects
    glDepthMask(GL_FALSE);

    // Outer wall
    scaleXYZ = glm::vec3(1.7f, 2.7f, 1.7f);
    positionXYZ = glm::vec3(4.0f, 5.1f, 1.0f);
    RenderTransparentShape([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderTexture("glass_lamp");
        m_basicMeshes->DrawCylinderMesh(true, false, true);
        }, glm::vec4(0.9f, 0.9f, 0.9f, 0.3f));

    // Inner wall (slightly smaller for thickness)
    scaleXYZ = glm::vec3(1.6f, 2.7f, 1.6f);  // Inner cylinder for thickness, slightly smaller
    positionXYZ = glm::vec3(4.0f, 5.1f, 1.0f);
    RenderTransparentShape([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderTexture("glass_lamp");
        m_basicMeshes->DrawCylinderMesh(true, false, true);
        }, glm::vec4(0.9f, 0.9f, 0.9f, 0.3f));

    // Render top cap as a thin cylinder
    scaleXYZ = glm::vec3(1.7f, 0.00f, 1.7f);
    positionXYZ = glm::vec3(4.0f, 5.1f + 2.7f + 0.005f, 1.0f);
    RenderTransparentShape([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderTexture("glass_lamp");
        m_basicMeshes->DrawCylinderMesh(true, true, true);
        }, glm::vec4(0.9f, 0.9f, 0.9f, 0.3f));

    // Re-enable depth writing for subsequent opaque objects
    glDepthMask(GL_TRUE);

    // Disable blending
    glDisable(GL_BLEND);

    // 15. Render light bulb (sphere) as glowing light
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    scaleXYZ = glm::vec3(0.8f, 0.8f, 0.8f);  // Adjust size as needed
    positionXYZ = glm::vec3(4.0f, 7.1f, 1.0f);  // Position of the light bulb
    RenderTransparentShape([this, scaleXYZ, positionXYZ]() {
        SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
        SetShaderColor(1.0f, 1.0f, 0.6f, 0.8f); // Light yellow color with transparency
        m_basicMeshes->DrawSphereMesh();
        }, glm::vec4(1.0f, 1.0f, 0.6f, 0.8f));

    // Re-enable depth writing for subsequent opaque objects
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Disable depth testing if not needed elsewhere
    glDisable(GL_DEPTH_TEST);
}

void SceneManager::SetUpDirectionalLight()
{
    m_pShaderManager->use();

    // Set directional light properties
    m_pShaderManager->setVec3Value("directionalLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    m_pShaderManager->setVec3Value("directionalLight.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
    m_pShaderManager->setVec3Value("directionalLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
    m_pShaderManager->setVec3Value("directionalLight.specular", glm::vec3(0.9f, 0.9f, 0.9f));
    m_pShaderManager->setIntValue("directionalLight.bActive", true);
}

void SceneManager::SetUpPointLight()
{
    m_pShaderManager->use();

    // Set point light properties
    m_pShaderManager->setVec3Value("pointLights[0].position", glm::vec3(4.0f, 7.1f, 1.0f)); // Position of the light bulb
    m_pShaderManager->setVec3Value("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f)); // Low ambient light
    m_pShaderManager->setVec3Value("pointLights[0].diffuse", glm::vec3(1.0f, 1.0f, 0.8f));    // Warm light color
    m_pShaderManager->setVec3Value("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 0.8f));   // Specular highlights
    m_pShaderManager->setFloatValue("pointLights[0].constant", 1.0f);
    m_pShaderManager->setFloatValue("pointLights[0].linear", 0.09f);
    m_pShaderManager->setFloatValue("pointLights[0].quadratic", 0.032f);
    m_pShaderManager->setIntValue("pointLights[0].bActive", true);
}

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
