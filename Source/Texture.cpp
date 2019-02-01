#include "Texture.h"

// Default Constructor, init everything to 0
Texture::Texture( const string& sFileName, Texture::manager_cookie )
{
	m_uiHeight = m_uiWidth = m_TextureName = 0;
	m_sManagerKey = sFileName;
}

// Destructor, delete textures on the GPU
Texture::~Texture()
{
	if ( m_TextureName )
		glDeleteTextures( 1, &m_TextureName );
}

// Binds Texture Data to the GPU and stores the Width, Height and Handle to the Texture information
void Texture::genTexture( const void* pBits, GLuint uiWidth, GLuint uiHeight, GLenum eFormat, GLenum eType )
{
	m_uiHeight = uiHeight;
	m_uiWidth = uiWidth;

	glGenTextures( 1, &m_TextureName );
	glBindTexture(GL_TEXTURE_2D, m_TextureName );
	glTexImage2D( GL_TEXTURE_2D, 0, eFormat, m_uiWidth, m_uiHeight, 0, eFormat, eType, pBits );
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set Texture Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

// Bind Texture for Drawing
void Texture::bindTexture( ShaderManager::eShaderType eType, string sVarName )
{
	glActiveTexture( GL_TEXTURE0 + m_TextureName );
	glBindTexture( GL_TEXTURE_2D, m_TextureName );
	SHADER_MANAGER->setUniformInt( eType, sVarName, m_TextureName );
}

// Unbind Texture when finished.
void Texture::unbindTexture()
{
	glBindTexture( GL_TEXTURE_2D, 0 );
}