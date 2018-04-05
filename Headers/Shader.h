#pragma once

#include "stdafx.h"

class Shader
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
	bool	m_bInitialized;

	// Protected Functions for Setting up Shaders
	string LoadSource( int iShaderType, bool* bError);
	GLuint CompileShader(GLenum shaderType, const string &source, bool* bError);
	GLuint LinkProgram( bool* bError);
};

