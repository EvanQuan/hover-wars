#pragma once

/* INCLUDES */
#include "stdafx.h"
#include <unordered_map>
#include "Shader.h"

// Class: Shader Manager
// Purpose: Manages all the Shaders used by every Assignment.  Manages Uniform variable
//			manipulation and properly initializes and destroys created Shaders.
// Current Issues: Uniform variables hard-coded.
// TODO: (Low Priority) Scan through all shader code for uniform variables and set up
//						management of those variables dynamically. 
// Written by: James Coté
class ShaderManager
{
public:
	// Enum for Shaders
	enum eShaderType
	{
		LIGHT_SHDR = 0,
		MESH_SHDR,
		PLANE_SHDR,
		WORLD_SHDR,
		SILH_SHDR,
		INSIDE_EDGE_SHDR,
		OUTSIDE_EDGE_SHDR,
		INSIDE_BOUNDRY_SHDR,
		OUTSIDE_BOUNDRY_SHDR,
		RC_TRACK_SHDR,
		BOID_SHDR,
		MAX_SHDRS
	};
	const static unordered_map<string, eShaderType> pShaderTypeMap;

	const static eShaderType eDefaultEdgeShader = SILH_SHDR;
	const static eShaderType eLastEdgeShader = RC_TRACK_SHDR;

	// Singleton implementation and Destructor
	static ShaderManager* getInstance();
	~ShaderManager();

	// Initialize Shaders
	bool initializeShaders();

	void setProjectionModelViewMatrix( const mat4* pProjMat, const mat4* pModelViewMat );

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

	// Should only be initialized once.
	bool m_bInitialized;

	// Shader Variables
	Shader m_pShader[MAX_SHDRS];
};

