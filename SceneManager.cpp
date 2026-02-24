///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
//  @updated by: Allan Torres
//  @Version 1.0: 02/10/2026
/////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#include <iostream>

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
	m_loadedTextures = 0;
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
	glm::vec3 positionXYZ,
	glm::vec3 offset)
{
	// variables for this method
	glm::mat4 model;
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
	translation = glm::translate(positionXYZ + offset);

	model = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, model);
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
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadHollowCylinderMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadFlatSphereMesh();
	m_basicMeshes->LoadWedgeMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadHemisphereMesh();
	m_basicMeshes->LoadPentagonalPrismMesh();

	CreateGLTexture(
		"textures/bark_5-4K/bark_5-4K/4K-bark_5-diffuse.jpg",
		"bark");
	CreateGLTexture(
		"textures/blue_plaster_19-4K/blue_plaster_19-4K/4K-plaster_19.jpg-diffuse.jpg",
		"blue_plaster");
	CreateGLTexture(
		"textures/PaintedPlaster002_4K-JPG/PaintedPlaster002_4K_Color.jpg",
		"painted_plaster");
	CreateGLTexture(
		"textures/SurfaceImperfections016_4K-JPG/SurfaceImperfections016_4K_Color.jpg",
		"surface_imperfections");
	CreateGLTexture(
		"textures/keyboard.jpg",
		"keyboard");
	CreateGLTexture(
		"textures/black_leather_24-4K/black_leather_24-4K/4K-Leather_24_Base Color.jpg",
		"black_leather");
	CreateGLTexture(
		"textures/mouse.png",
		"mouse");
	CreateGLTexture(
		"textures/Jojo.jpg",
		"jojo");
	CreateGLTexture(
		"textures/background.jpeg",
		"background");
	CreateGLTexture(
		"textures/black_metal-4K/black_metal-4K/4K-metal_5-specular.jpg",
		"black_metal");
	CreateGLTexture(
		"textures/brick_wall_001_4K-JPG/brick_wall_001/brick_wall_001_diffuse_4k.jpg",
		"brick_wall");
	CreateGLTexture(
		"textures/Paper001_4K-JPG/Paper001_4K_Color.jpg",
		"roof");
	CreateGLTexture(
		"textures/afromosia-4K/afromosia-4K/4K_afromosia_basecolor.png",
		"afromosia_floor");

	// Use standard repeat for the keyboard texture (no mirroring).
	{
		int keyboardTextureID = FindTextureID("keyboard");
		if (keyboardTextureID >= 0)
		{
			glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(keyboardTextureID));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	// Clamp the mouse texture to avoid repeating artifacts on the body.
	{
		int mouseTextureID = FindTextureID("mouse");
		if (mouseTextureID >= 0)
		{
			glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(mouseTextureID));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	BindGLTextures();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseLightingName, true);
		m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
		m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.35f, 0.35f, 0.35f));
		m_pShaderManager->setFloatValue("material.shininess", 32.0f);

		// Soft directional light to lift the scene and reveal plane highlights.
		glm::vec3 previousAmbient = glm::vec3(0.28f, 0.28f, 0.28f);
		glm::vec3 currentAmbient = glm::vec3(0.28f, 0.28f, 0.28f);
		// Ambient values saved for quick restore after testing.
		glm::vec3 directionalAmbientDefault = currentAmbient;
		m_pShaderManager->setVec3Value("directionalLight.direction", glm::vec3(-0.2f, -1.0f, -0.1f));
		m_pShaderManager->setVec3Value("directionalLight.ambient", directionalAmbientDefault);
		m_pShaderManager->setVec3Value("directionalLight.diffuse", glm::vec3(0.18f, 0.18f, 0.18f));
		m_pShaderManager->setVec3Value("directionalLight.specular", glm::vec3(0.22f, 0.22f, 0.22f));
		m_pShaderManager->setIntValue("directionalLight.bActive", true);

		// Disable unused point lights.
		for (int i = 0; i < 5; ++i)
		{
			std::string lightBase = "pointLights[" + std::to_string(i) + "]";
			m_pShaderManager->setIntValue(lightBase + ".bActive", false);
		}

		// Disable the monitor point light glow (spotlight used instead).
		m_pShaderManager->setIntValue("pointLights[0].bActive", false);

		// Soft point light fill to satisfy the point light requirement.
		m_pShaderManager->setVec3Value("pointLights[1].position", glm::vec3(-12.5f, 18.0f, 0.0f));
		glm::vec3 pointLightAmbientDefault = glm::vec3(0.12f, 0.12f, 0.12f);
		m_pShaderManager->setVec3Value("pointLights[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f)); // restore: pointLightAmbientDefault
		m_pShaderManager->setVec3Value("pointLights[1].diffuse", glm::vec3(0.35f, 0.35f, 0.35f));
		m_pShaderManager->setVec3Value("pointLights[1].specular", glm::vec3(0.25f, 0.25f, 0.25f));
		m_pShaderManager->setIntValue("pointLights[1].bActive", true);

		// Monitor spotlight aimed forward so it only lights what's in front of the screen.
		glm::vec3 spotLightPosition(-7.3f, 4.2f, -2.15f);
		glm::vec3 screenTarget(-7.3f, 3.0f, 1.0f);
		glm::vec3 spotLightDirection = glm::normalize(screenTarget - spotLightPosition);
		m_pShaderManager->setVec3Value("spotLight.position", spotLightPosition);
		m_pShaderManager->setVec3Value("spotLight.direction", spotLightDirection);
		m_pShaderManager->setFloatValue("spotLight.cutOff", glm::cos(glm::radians(20.0f)));
		m_pShaderManager->setFloatValue("spotLight.outerCutOff", glm::cos(glm::radians(32.0f)));
		m_pShaderManager->setFloatValue("spotLight.constant", 1.0f);
		m_pShaderManager->setFloatValue("spotLight.linear", 0.30f);
		m_pShaderManager->setFloatValue("spotLight.quadratic", 0.28f);
		glm::vec3 spotLightAmbientDefault = glm::vec3(0.20f, 0.12f, 0.24f);
		m_pShaderManager->setVec3Value("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f)); // restore: spotLightAmbientDefault
		m_pShaderManager->setVec3Value("spotLight.diffuse", glm::vec3(8.50f, 5.75f, 10.50f));
		m_pShaderManager->setVec3Value("spotLight.specular", glm::vec3(5.50f, 4.00f, 6.50f));
		m_pShaderManager->setIntValue("spotLight.bActive", true);
	}

	// Marker cube at the point light position to help visualize the emitter.
	// {
	// 	glm::vec3 markerScale = glm::vec3(1.0f, 1.0f, 1.0f);
	// 	glm::vec3 markerPosition = glm::vec3(-12.5f, 18.0f, 0.0f);
	// 	SetTransformations(
	// 		markerScale,
	// 		0.0f,
	// 		0.0f,
	// 		0.0f,
	// 		markerPosition);
	// 	if (NULL != m_pShaderManager)
	// 	{
	// 		m_pShaderManager->setIntValue(g_UseLightingName, false);
	// 	}
	// 	SetShaderColor(1.0f, 0.95f, 0.2f, 1.0f);
	// 	m_basicMeshes->DrawBoxMesh();
	// 	if (NULL != m_pShaderManager)
	// 	{
	// 		m_pShaderManager->setIntValue(g_UseLightingName, true);
	// 	}
	// }

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	//set the XYZ scale for the mesh (table)
	// draw back wall
	scaleXYZ = glm::vec3(104.0f, 1.0f, 50.0f);
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-12.5f, 10.0f, -37.5f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseLightingName, false);
	}
	SetShaderTexture("background");
	SetTextureUVScale(1.0f, -1.0f);
	m_basicMeshes->DrawPlaneMesh();
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseLightingName, true);
	}

	// draw floor (1/5 back, 4/5 front)
	scaleXYZ = glm::vec3(104.0f, 1.0f, 75.0f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-12.5f, -15.0f, 0.0f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("afromosia_floor");
	SetTextureUVScale(6.0f, 6.0f);
	m_basicMeshes->DrawPlaneMesh();

	// draw left wall
	scaleXYZ = glm::vec3(75.0f, 1.0f, 50.0f);
	XrotationDegrees = 90.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-64.5f, 10.0f, 0.0f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("brick_wall");
	SetTextureUVScale(4.0f, 2.0f);
	m_basicMeshes->DrawPlaneMesh();

	// draw right wall
	scaleXYZ = glm::vec3(75.0f, 1.0f, 50.0f);
	XrotationDegrees = 90.0f;
	YrotationDegrees = -90.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(39.5f, 10.0f, 0.0f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("brick_wall");
	SetTextureUVScale(4.0f, 2.0f);
	m_basicMeshes->DrawPlaneMesh();

	// draw front wall
	scaleXYZ = glm::vec3(104.0f, 1.0f, 50.0f);
	XrotationDegrees = 90.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-12.5f, 10.0f, 37.5f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("brick_wall");
	SetTextureUVScale(6.0f, 2.0f);
	m_basicMeshes->DrawPlaneMesh();

	// draw roof (portrait texture rotated to landscape)
	scaleXYZ = glm::vec3(104.0f, 1.0f, 75.0f);
	XrotationDegrees = 180.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-12.5f, 35.0f, 0.0f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("roof");
	SetTextureUVScale(-1.0f, 1.0f);
	m_basicMeshes->DrawPlaneMesh();

	//set the XYZ scale for the mesh (table)
	scaleXYZ = glm::vec3(40.0f, 0.6f, 10.0f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(0.0f, -0.4f, 0.0f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
		m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(1.2f, 1.2f, 1.2f));
		m_pShaderManager->setFloatValue("material.shininess", 128.0f);
	}
	SetShaderTexture("bark");
	SetTextureUVScale(4.0f, 4.0f);
	m_basicMeshes->DrawBoxMesh();

	// draw table legs (square posts + smashed sphere feet)
	float tableHalfX = 20.0f;
	float tableHalfZ = 5.0f;
	float legInsetX = 1.5f;
	float legInsetZ = 1.0f;
	float tableBottomY = -0.7f;
	float floorY = -15.0f;
	float legHeight = tableBottomY - floorY;
	float legCenterY = floorY + (legHeight * 0.5f);
	float legSize = 1.2f;
	glm::vec3 footScale = glm::vec3(2.4f, 0.4f, 2.4f);
	float footCenterY = floorY;
	glm::vec3 legOffsets[4] = {
		glm::vec3(tableHalfX - legInsetX, 0.0f, tableHalfZ - legInsetZ),
		glm::vec3(-(tableHalfX - legInsetX), 0.0f, tableHalfZ - legInsetZ),
		glm::vec3(tableHalfX - legInsetX, 0.0f, -(tableHalfZ - legInsetZ)),
		glm::vec3(-(tableHalfX - legInsetX), 0.0f, -(tableHalfZ - legInsetZ))
	};

	SetShaderTexture("black_metal");
	SetTextureUVScale(1.0f, 1.0f);
	for (int i = 0; i < 4; ++i)
	{
		glm::vec3 legPos = glm::vec3(legOffsets[i].x, legCenterY, legOffsets[i].z);
		SetTransformations(
			glm::vec3(legSize, legHeight, legSize),
			0.0f,
			0.0f,
			0.0f,
			legPos);
		m_basicMeshes->DrawBoxMesh();

		glm::vec3 footPos = glm::vec3(legOffsets[i].x, footCenterY, legOffsets[i].z);
		SetTransformations(
			footScale,
			0.0f,
			0.0f,
			0.0f,
			footPos);
		m_basicMeshes->DrawFlatSphereMesh();
	}

	// draw mouse pad (17.5% of table surface area)
	scaleXYZ = glm::vec3(17.0f, 1.0f, 5.0f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-4.5f, -0.05f, 1.5f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("surface_imperfections");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawPlaneMesh();

	// draw keyboard (78-key footprint, tilted plane, match texture aspect)
	scaleXYZ = glm::vec3(8.4f, 1.0f, 2.74f);
	XrotationDegrees = -15.0f;
	YrotationDegrees = 190.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.3f, 0.317f, 1.13f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("keyboard");
	SetTextureUVScale(-1.0f, 1.0f);
	m_basicMeshes->DrawPlaneMesh();

	// draw keyboard wedge (fill gap to mousepad)
	scaleXYZ = glm::vec3(8.4f, 0.71f, 2.74f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 190.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.3f, -0.05f, 1.13f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("black_leather");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawWedgeMesh();

	// draw mouse (three-piece approximation)
	// dimensions based on Apple Magic Mouse 2, scaled up for visibility and to fit the mouse pad
	//Decided to use this one since I had so much trouble with the textures. Magic Mouse 2 does not have any visible textures. :)
	float mouseLength = 0.113f;
	float mouseWidth = 0.057f;
	float mouseHeight = 0.021f;
	float mouseScaleFactor = 15.0f;
	float cylinderLength = (mouseLength - mouseWidth) * mouseScaleFactor;
	float mouseHeightScaled = mouseHeight * mouseScaleFactor;
	float mouseWidthScaled = mouseWidth * mouseScaleFactor;
	float mouseYaw = -45.0f;
	glm::vec3 mousePosition = glm::vec3(-1.04f, -0.05f + (mouseHeightScaled * 0.5f), 1.13f);
	glm::vec3 cylinderScale = glm::vec3(cylinderLength, mouseHeightScaled, mouseWidthScaled);
	glm::vec3 capScale = glm::vec3(mouseWidthScaled, mouseHeightScaled, mouseWidthScaled);

	glm::mat4 mouseRotation = glm::rotate(glm::radians(mouseYaw), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 frontOffset = glm::vec3(cylinderLength * 0.5f, 0.0f, 0.0f);
	glm::vec3 backOffset = glm::vec3(-cylinderLength * 0.5f, 0.0f, 0.0f);
	glm::vec3 frontPosition = mousePosition + glm::vec3(mouseRotation * glm::vec4(frontOffset, 1.0f));
	glm::vec3 backPosition = mousePosition + glm::vec3(mouseRotation * glm::vec4(backOffset, 1.0f));

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.7f, 0.7f, 0.7f));
		m_pShaderManager->setFloatValue("material.shininess", 128.0f);
	}
	SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

	// middle cylinder
	SetTransformations(
		cylinderScale,
		0.0f,
		mouseYaw,
		0.0f,
		mousePosition);
	m_basicMeshes->DrawCylinderMesh();

	// front hemisphere
	SetTransformations(
		capScale,
		0.0f,
		mouseYaw,
		0.0f,
		frontPosition);
	m_basicMeshes->DrawHemisphereMesh();

	// back hemisphere (rotate 180 to face backward)
	SetTransformations(
		capScale,
		0.0f,
		mouseYaw + 180.0f,
		0.0f,
		backPosition);
	m_basicMeshes->DrawHemisphereMesh();

	// draw monitor base (flat sphere)
	scaleXYZ = glm::vec3(3.0f, 0.2f, 3.0f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.3f, 0.0f, -2.5f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.12f, 0.12f, 0.12f, 1.0f);
	m_basicMeshes->DrawFlatSphereMesh();

	// draw monitor neck (pentagonal prism)
	scaleXYZ = glm::vec3(0.7f, 1.5f, 0.5f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.3f, 1.0f, -2.5f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.18f, 0.18f, 0.18f, 1.0f);
	m_basicMeshes->DrawPentagonalPrismMesh();

	// draw monitor frame (thin box)
	float screenWidth = 8.4f;
	float screenHeight = screenWidth * (9.0f / 16.0f);
	float frameHeight = screenHeight + 0.5f;
	float screenWidthInner = screenWidth - 0.1f;
	float screenHeightInner = screenHeight - 0.1f;
	scaleXYZ = glm::vec3(screenWidth, 0.12f, frameHeight);
	XrotationDegrees = -90.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.3f, 4.2f, -2.30f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.08f, 0.08f, 0.08f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// draw monitor screen (1080p aspect, shifted up for bottom bezel)
	scaleXYZ = glm::vec3(screenWidthInner, 1.0f, screenHeightInner);
	XrotationDegrees = -90.0f;
	YrotationDegrees = 180.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.3f, 4.2f + 0.25f, -2.22f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("jojo");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawPlaneMesh();

	// draw hollow mug body (light blue)
	scaleXYZ = glm::vec3(2.0f, 4.0f, 2.0f); // radius = 2, height = 4
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(10.0f, 2.0f, -2.5f); // shifted X+10, Z=-2.5
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
		m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.3f, 0.3f, 0.3f));
		m_pShaderManager->setFloatValue("material.shininess", 16.0f);
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	SetShaderTexture("painted_plaster");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawHollowCylinderMesh();
	glCullFace(GL_FRONT);
	SetShaderTexture("blue_plaster");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawHollowCylinderMesh();
	glDisable(GL_CULL_FACE);

	//** draw thin inner cylinder (painted plaster lining)
	scaleXYZ = glm::vec3(1.99f, 4.0f, 1.99f); // slightly smaller radius to fit just inside the hollow mug body
	XrotationDegrees = 0.0f;                  // could not get the main cylinder to get a different plaster
	YrotationDegrees = 0.0f;                  // texture in the inside vs. the outside, so I added a second cylinder for the inside.
	ZrotationDegrees = 0.0f;                  // Hopefully I learn how to by next assignment!
	positionXYZ = glm::vec3(10.0f, 2.0f, -2.5f);
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	SetShaderTexture("painted_plaster");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawHollowCylinderMesh();
	glDisable(GL_CULL_FACE); 

	// draw mug base (flat sphere)
	scaleXYZ = glm::vec3(4.0f, 0.2f, 4.0f); // match mug outer diameter
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(10.0f, 0.1f, -2.5f); // shifted X+10, Z=-2.5
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("surface_imperfections");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawFlatSphereMesh();

	// draw mug handle (torus)
	scaleXYZ = glm::vec3(1.6f, 1.6f, 1.6f); // uniform scale to keep donut shape
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 90.0f; // rotate so the ring is vertical
	positionXYZ = glm::vec3(12.0f, 2.0f, -2.5f); // shifted X+10, Z=2.5
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderTexture("blue_plaster");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawTorusMesh();
	/****************************************************************/
}
