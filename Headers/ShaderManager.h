#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "Shader.h"
#include "EntityComponentHeaders/LightingComponent.h"

// Class: Shader Manager
// Purpose: Manages all the Shaders used by every Assignment.  Manages Uniform variable
//          manipulation and properly initializes and destroys created Shaders.
// Current Issues: Uniform variables hard-coded.
// Written by: James Coté
class ShaderManager final
{
public:
    // Enum for Shaders
    enum eShaderType
    {
        LIGHT_SHDR = 0,
        TOON_SHDR,
        BLINN_PHONG_SHDR,
        PARTICLE_SHDR,
        WORLD_SHDR,
        BILLBOARD_SHDR,
        MAP_SHDR,
        DEBUG_SHDR,
        SHADOW_SHDR,
        UI_SHDR,
        SKYBOX_SHDR,
        SPLIT_SCREEN_SHDR,
        BLUR_SHDR,
        TRON_SHDR,
        MAX_SHDRS
    };
    const static unordered_map<string, eShaderType> pShaderTypeMap;

    // Singleton implementation and Destructor
    static ShaderManager* getInstance();
    ~ShaderManager();

    // Initialize Shaders
    bool initializeShaders();

    void setProjectionModelViewMatrix( const mat4* pProjMat, const mat4* pModelViewMat );
    void setDirectionalModelMatrix(const mat4* pDirModelMat);
    void setSpotLightModelMatrices(const mat4* pSpotLightMat, unsigned int iIndex);
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
    GLuint genInstanceBuffer(GLuint iVertArray, GLuint iStartIndex, const void* pData, GLsizeiptr pSize, GLenum usage);

    // Shader Uniform Variable Manipulation
    void setUnifromMatrix4x4( eShaderType eType, string sVarName, const mat4* pResultingMatrix );
    void setUniformVec3( eShaderType eType, string sVarName, const glm::vec3* pValue );
    void setUniformVec4( eShaderType eType, string sVarName, const vec4* pValue );
    void setUniformFloat(eShaderType eType, string sVarName, float fVal);
    void setUniformInt( eShaderType eType, string sVarName, int iVal, unsigned int iIndex = 0);
    void setUniformIntAll(string sVarName, int iVal, unsigned int iIndex = 0);
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
