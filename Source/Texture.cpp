#include "Texture.h"
#include "stb_image.h"

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

// Generates a CubeMap Texture 
void Texture::genCubeMap(const vector<string>* sFileNames)
{
    // Local Variables
    unsigned char* data = nullptr;
    int iWidth, iHeight, nrChannels;

    // Generate Texture
    glGenTextures(1, &m_TextureName);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureName);

    // Iterate through the FileNames, load the data and set the proper channels of the Cube Map:
    //      0 - Positive X (Right)
    //      1 - Negative X (Left)
    //      2 - Positive Y (Top)
    //      3 - Negative Y (Bottom)
    //      4 - Positive Z (Back)
    //      5 - Negative Z (Front)
    for (GLuint i = 0; i < sFileNames->size(); ++i)
    {
        data = stbi_load((*sFileNames)[i].c_str(), &iWidth, &iHeight, &nrChannels, 0);

        if (nullptr != data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else
            cout << "ERROR: Cubemap texture failed to load file: \"" << sFileNames->at(i) << "\"\n";

        // Free Data that was loaded.
        stbi_image_free(data);
    }

    // Set Texture Parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

// Set Texture Parameters for more Generic Texture control.
void Texture::setTexParameter(GLenum eTexEnum, GLint iParam)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureName);
    glTexParameteri(GL_TEXTURE_2D, eTexEnum, iParam);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setTexParameterfv(GLenum eTexEnum, const GLfloat* pParams)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureName);
    glTexParameterfv(GL_TEXTURE_2D, eTexEnum, pParams);
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

void Texture::bindTextureAsCubeMap(ShaderManager::eShaderType eType, string sVarName)
{
    glActiveTexture(GL_TEXTURE0 + m_TextureName);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureName);
    SHADER_MANAGER->setUniformInt( eType, sVarName, m_TextureName );
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
