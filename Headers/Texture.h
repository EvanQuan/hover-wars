#pragma once
#include "stdafx.h"
#include "ShaderManager.h"

class Texture
{
public:
	void genTexture( const void* pBits, GLuint uiWidth, GLuint uiHeight, GLenum eFormat, GLenum eType );
	void bindTexture( ShaderManager::eShaderType eType, string sVarName );
	void unbindTexture();

	const string& getFileName() { return m_sFileName; }

private:
	Texture( const string& sFileName );
	Texture( const Texture& pCopyTexture );
	Texture& operator=( const Texture& pRHS );
	~Texture();

	// OpenGL names for array buffer objects, vertex array object
	GLuint  m_TextureName;

	// dimensions of the image stored in this texture
	GLuint  m_uiWidth, m_uiHeight;

	// Reference to texture's File Name
	string m_sFileName;

	// Friend class is TextureManager so only this Manager can create Texture Objects
	friend class TextureManager;
};

