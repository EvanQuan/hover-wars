#pragma once

#include "stdafx.h"

/**************************************************************************************************
 * Name: Shader
 * Written By: James Coté
 * Description: Contains functionality for initializing and compiling shaders. ShaderManager will set
 *  locations for necessary shader types. Vertex and Fragment Shader will need to be set at minimum for
 *  the Shader to initialize properly. Once initialized, the Shader class will maintain the program handle
 *  for the Shader and will unload the shader once the class is destructed.
\****************************************************************************************************/
class Shader final
{
public:
    // Enum to signify type of Shader
    enum eShader
    {
        VERTEX = 0,
        FRAGMENT,
        GEOMETRY,
        TESS_CNTRL,
        TESS_EVAL,
        MAX_PARTS
    };

    // Constructor/Destructor
    Shader();
    ~Shader();

    // Initialize Shaders, will fail if Vertex or Fragment Shader
    // don't have locations set yet.
    bool initializeShader( );

    // Set Shader Location
    void storeShadrLoc( eShader shaderType, const string& sShaderLoc );

    // Getters, Setters not required
    GLuint getProgram() { return m_uiProgram; }
    GLint fetchVarLocation(const GLchar* sVarName) { return glGetUniformLocation(m_uiProgram, sVarName); }

private:
    GLuint m_uiShader[ MAX_PARTS ];
    string m_uiShaderLocations[ MAX_PARTS ];

    // Shader Variables
    GLuint  m_uiProgram;
    bool    m_bInitialized;

    // Protected Functions for Setting up Shaders
    string LoadSource( int iShaderType, bool* bError);
    GLuint CompileShader(GLenum shaderType, const string &source, bool* bError);
    GLuint LinkProgram( bool* bError);
};
