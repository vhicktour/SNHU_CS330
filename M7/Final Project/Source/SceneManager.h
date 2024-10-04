///////////////////////////////////////////////////////////////////////////////
// SceneManager.h
// ==============
// Manage the preparation and rendering of 3D scenes - textures, materials, lighting
//
// AUTHOR: Victor Udeh
// Created for personal project involving 3D graphics using OpenGL
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "ShapeMeshes.h"

#include <string>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

/***********************************************************
 *  SceneManager
 *
 *  This class contains the code for preparing and rendering
 *  3D scenes, including the shader settings.
 ***********************************************************/
class SceneManager
{
public:
    // Constructor
    SceneManager(ShaderManager* pShaderManager);
    // Destructor
    ~SceneManager();

    // The following methods are for the students to
    // customize for their own 3D scene
    void PrepareScene();
    void RenderScene();

    // Utility methods for rendering shapes with edges and transparency
    void RenderShapeWithEdges(std::function<void()> drawFunction, const glm::vec4& fillColor);
    void RenderTransparentShape(std::function<void()> drawFunction, const glm::vec4& color);

private:
    // Pointer to shader manager object
    ShaderManager* m_pShaderManager;
    // Pointer to basic shapes object
    ShapeMeshes* m_basicMeshes;
    // Total number of loaded textures
    int m_loadedTextures;
    // Structure to hold texture information
    struct TEXTURE_INFO
    {
        std::string tag;
        GLuint ID = 0;
    };
    // Array to store texture IDs
    TEXTURE_INFO m_textureIDs[16];

    // Load texture images and convert to OpenGL texture data
    bool CreateGLTexture(const char* filename, std::string tag);
    // Bind loaded OpenGL textures to slots in memory
    void BindGLTextures();
    // Free the loaded OpenGL textures
    void DestroyGLTextures();
    // Find a loaded texture by tag
    int FindTextureID(std::string tag);
    int FindTextureSlot(std::string tag);

    // Set the transformation values into the transform buffer
    void SetTransformations(
        glm::vec3 scaleXYZ,
        float XrotationDegrees,
        float YrotationDegrees,
        float ZrotationDegrees,
        glm::vec3 positionXYZ);

    // Set the color values into the shader
    void SetShaderColor(
        float redColorValue,
        float greenColorValue,
        float blueColorValue,
        float alphaValue);

    // Set the texture data into the shader
    void SetShaderTexture(std::string textureTag);

    // Set the UV scale for the texture mapping
    void SetTextureUVScale(float u, float v);

    // Set the object material into the shader
    void SetShaderMaterial();

    // Methods to set up lights
    void SetUpDirectionalLight();
    void SetUpPointLight();
};
