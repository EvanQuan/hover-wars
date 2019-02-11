#pragma once

/* INCLUDES */
#include "stdafx.h"
#include <unordered_map>
#include "Shader.h"
#include "EntityComponentHeaders/LightingComponent.h"

// Class: Shader Manager
// Purpose: Manages all the Shaders used by every Assignment.  Manages Uniform variable
//			manipulation and properly initializes and destroys created Shaders.
// Current Issues: Uniform variables hard-coded.
// Written by: James Coté
class ShaderManager
{
public:
	// Enum for Shaders
	enum eShaderType
	{
		LIGHT_SHDR = 0,
		TOON_SHDR,
		BLINN_PHONG_SHDR,
		PLANE_SHDR,
		PARTICLE_SHDR,
		WORLD_SHDR,
		BILLBOARD_SHDR,
		BOID_SHDR,
		MAX_SHDRS
	};
	const static unordered_map<string, eShaderType> pShaderTypeMap;

	// Singleton implementation and Destructor
	static ShaderManager* getInstance();
	~ShaderManager();

	// Initialize Shaders
	bool initializeShaders();

	void setProjectionModelViewMatrix( const mat4* pProjMat, const mat4* pModelViewMat );
	void setLightsInUniformBuffer(const LightingComponent* pDirectionalLight, const vector< LightingComponent* >* pPointLights );

	// Get the specified program for using shaders for rendering
	GLuint getProgram(eShaderType eType) { return m_pShader[eType].getProgram(); }
	ShaderManager::eShaderType getShaderType(const string& sKey);

	// Setup Buffers and Arrays in GPU
	GLuint genVertexBuffer( GLuint iVertArray, const void* pData, GLsizeiptr pSize, GLenum usage );
	void setAttrib(GLuint iVertArray, GLuint iSpecifiedIndex,
					 GLint iChunkSize, GLsizei iStride, const void *pOffset);
	GLuint genIndicesBuffer( GLuint iVertArray, 
							 const void* pData, GLsizeiptr pSize, GLenum usage );

	// Shader Uniform Variable Manipulation 
	void setUnifromMatrix4x4( eShaderType eType, string sVarName, const mat4* pResultingMatrix );
	void setUniformVec3( eShaderType eType, string sVarName, const glm::vec3* pLightPos );
	void setUniformFloat(eShaderType eType, string sVarName, float fVal);
	void setUniformInt( eShaderType eType, string sVarName, int iVal );
	void setUniformBool( eShaderType eType, string sVarName, bool bVal );
	void toggleUniformBool( eShaderType eType, string sVarName );

private:
	// Singleton Implementation
	ShaderManager();
	ShaderManager( const ShaderManager* pCopy );
	static ShaderManager* m_pInstance;

	GLuint m_iMatricesBuffer, m_iLightsBuffer;

	// Should only be initialized once.
	bool m_bInitialized;

	// Shader Variables
	Shader m_pShader[MAX_SHDRS];
};

