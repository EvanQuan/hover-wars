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
    glBindTexture( GL_TEXTURE_2D, 0 );
}

// Set Texture Parameters for more Generic Texture control.
void Texture::setTexParameter(GLenum eTexEnum, GLint iParam)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureName);
    glTexParameteri(GL_TEXTURE_2D, eTexEnum, iParam);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Activate MipMaps for the texture. These are generated internally
void Texture::genMipMaps()
{
    glBindTexture(GL_TEXTURE_2D, m_TextureName);
    glGenerateMipmap(GL_TEXTURE_2D);        // Generate Mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);               // set Mag Filter to Linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // set Min Filter to Mipmap Linear
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Bind Texture for Drawing to a specified shader.
void Texture::bindTexture( ShaderManager::eShaderType eType, string sVarName, unsigned int iIndex)
{
    glActiveTexture( GL_TEXTURE0 + m_TextureName );
    glBindTexture( GL_TEXTURE_2D, m_TextureName );
    SHADER_MANAGER->setUniformInt( eType, sVarName, m_TextureName, iIndex );
}

// Binds Texture to all Shaders
void Texture::bindTextureAllShaders(string sVarName, unsigned int iIndex)
{
    glActiveTexture(GL_TEXTURE0 + m_TextureName);
    glBindTexture(GL_TEXTURE_2D, m_TextureName);
    SHADER_MANAGER->setUniformIntAll(sVarName, m_TextureName, iIndex);
}

void Texture::bindToFrameBuffer(GLuint iFrameBuffer, GLenum eAttachment, GLenum eTexTarget, GLint iLevel)
{
    glBindFramebuffer(GL_FRAMEBUFFER, iFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, eAttachment, eTexTarget, m_TextureName, iLevel);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Unbind Texture when finished.
void Texture::unbindTexture()
{
    glBindTexture( GL_TEXTURE_2D, 0 );
}
