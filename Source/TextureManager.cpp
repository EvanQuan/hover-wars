#include "TextureManager.h"
#include "ImageReader.h"

TextureManager* TextureManager::pInstance = nullptr;

TextureManager::TextureManager()
{
    // Nothing to Construct
}

TextureManager* TextureManager::getInstance()
{
    if ( nullptr == pInstance )
        pInstance = new TextureManager();

    return pInstance;
}


// Destructor: Clear the Texture Cache and release the memory
//             The Destructor of each texture should be called to release the memory of those textures.
TextureManager::~TextureManager()
{
    unloadAllTextures();
}

// unloadAllTextures: This will clear all the unique_ptrs for Textures, effectively clearing the heap
//                    and unloading all allocated Texture data.
void TextureManager::unloadAllTextures( )
{
    m_pTextureCache.clear();
}

// unloadTexture: This will unload a specified texture and set the texture pointer to null.
void TextureManager::unloadTexture(Texture** pTexture)
{
    // Ensure the Texture Pointer isn't null
    assert(nullptr != *pTexture);

    // Make sure the Texture is found.
    if (m_pTextureCache.end() != m_pTextureCache.find((*pTexture)->m_sManagerKey))
    {
        m_pTextureCache.erase((*pTexture)->m_sManagerKey);  // Erase the Texture
        *pTexture = nullptr;                                // Set the Texture to Null
    }
}

// loadTexture: Takes in a FileName that is uses for a hashmap used to store the textures.
//              If the texture has already been loaded, return a pointer to that loaded texture.
//              Otherwise, load the texture and return the pointer to the newly created texture.
//              Stores all created textures in its hash map.
// Parameters:  sFileName - The location of the file to load.
// Written by:  James Cote
Texture* TextureManager::loadTexture(const string& sFileName )
{
    // Attempt to grab it from the texture cache if it already exists
    Texture* pReturnTexture = nullptr;

    if (m_pTextureCache.end() != m_pTextureCache.find(sFileName))
    {
        // Grab the Texture Container from the Cache
        pReturnTexture = m_pTextureCache[sFileName].get();
    }
    else // Create the New Texture in the Texture Cache
    {
        // Generate Texture Smart Pointer
        unique_ptr<Texture> pNewTexture = make_unique<Texture>(sFileName, Texture::manager_cookie() );

        if ( InitializeTexture( pNewTexture.get(), sFileName, true ) )
        {
            // Return the raw pointer to the caller
            pReturnTexture = pNewTexture.get();

            // Attach Texture to the Cache
            m_pTextureCache.insert(make_pair(sFileName, move(pNewTexture)));
        }
    }

    return pReturnTexture;
}

unordered_map<UserInterface::eImage, Texture*> TextureManager::loadTextures(
    const unordered_map<UserInterface::eImage, string>& mFiles,
    const string sDirectory)
{
    unordered_map<UserInterface::eImage, Texture*> textureMap;
    for each (pair<UserInterface::eImage, string> pair in mFiles)
    {
        textureMap[pair.first] = loadTexture(sDirectory + pair.second);
    }
    return textureMap;

}

// Loads a Cube Map Texture; Returns Null if it failed to load for some reason.
Texture* TextureManager::loadCubeMap(const vector<string>* sFileNames)
{
    // Return Variables
    Texture* pReturnTexture = nullptr;
    string sHashKey;

    // Only proceed if there's at least 6 image files to load for the cubemap.
    if (6 == sFileNames->size())
    {
        // Generate HashKey via concatonation of all file names
        for each (string sFileName in *sFileNames)
            sHashKey += sFileName;

        // If HashKey already exists, return the found texture.
        if (m_pTextureCache.end() != m_pTextureCache.find(sHashKey))
            pReturnTexture = m_pTextureCache[sHashKey].get();
        else    // Otherwise, generate new Cubemap Texture
        {
            // Generate Texture Smart Pointer
            unique_ptr<Texture> pNewTexture = make_unique<Texture>(sHashKey, Texture::manager_cookie());
            pNewTexture->genCubeMap(sFileNames);

            // Set Return Variables
            pReturnTexture = pNewTexture.get();

            // Store New Texture in the Texture Cache
            m_pTextureCache.insert(make_pair(sHashKey, move(pNewTexture)));
        }
    }

    // Return Texture
    return pReturnTexture;
}

// This will generate a small 1x1 texture with the given color to be used as a simple texture.
Texture* TextureManager::genTexture(const vec4* vColor)
{
    Texture* pReturnTexture = nullptr;
    string sHashName = glm::to_string(*vColor);

    if (m_pTextureCache.end() != m_pTextureCache.find(sHashName))
    {
        // Found a texture for that color that already exists, return it.
        pReturnTexture = m_pTextureCache[sHashName].get();
    }
    else
    {
        // generate new texture unique_ptr
        unique_ptr<Texture> pNewTexture = make_unique<Texture>(sHashName, Texture::manager_cookie());

        pNewTexture->genTexture(vColor, 1, 1, GL_RGB16F, GL_RGBA, GL_FLOAT);

        pReturnTexture = pNewTexture.get();

        m_pTextureCache.insert(make_pair(sHashName, move(pNewTexture)));
    }

    return pReturnTexture;
}

Texture* TextureManager::genDepthBuffer(unsigned int iWidth, unsigned int iHeight)
{
    // Attempt to grab it from the texture cache if it already exists
    Texture* pReturnTexture = nullptr;
    string sHashValue = "DepthBuffer" + to_string(iWidth) + to_string(iHeight);

    if (m_pTextureCache.end() != m_pTextureCache.find(sHashValue))
    {
        // Grab the Texture Container from the Cache
        pReturnTexture = m_pTextureCache[sHashValue].get();
    }
    else // Create the New Texture in the Texture Cache
    {
        // Generate Texture Smart Pointer
        unique_ptr<Texture> pNewTexture = make_unique<Texture>(sHashValue, Texture::manager_cookie());

        // Generate Texture information in GPU
        pNewTexture->genTexture(nullptr, iWidth, iHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

        // Set Texture Parameters
        pNewTexture->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        pNewTexture->setTexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        pNewTexture->setTexParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        pNewTexture->setTexParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        pNewTexture->setTexParameterfv(GL_TEXTURE_BORDER_COLOR, borderColor);

        // Return the raw pointer to the caller
        pReturnTexture = pNewTexture.get();

        // Attach Texture to the Cache
        m_pTextureCache.insert(make_pair(sHashValue, move(pNewTexture)));
    }

    return pReturnTexture;
}

// Generates a Frame Buffer Object as a Texture
Texture* TextureManager::genFrameBufferTexture(unsigned int iWidth, unsigned int iHeight, unsigned int iPlayer, unsigned int iBufferNum)
{
    // Attempt to grab it from the texture cache if it already exists
    Texture* pReturnTexture = nullptr;
    string sHashValue = "FrameBuffer" + to_string(iWidth) + to_string(iHeight) + to_string(iPlayer) + to_string(iBufferNum);

    if (m_pTextureCache.end() != m_pTextureCache.find(sHashValue))
    {
        // Grab the Texture Container from the Cache
        pReturnTexture = m_pTextureCache[sHashValue].get();
    }
    else // Create the New Texture in the Texture Cache
    {
        // Generate Texture Smart Pointer
        unique_ptr<Texture> pNewTexture = make_unique<Texture>(sHashValue, Texture::manager_cookie());

        // Generate Texture information in GPU
        // Generate as 16bit Floating Point internal Format for HDR rendering
        pNewTexture->genTexture(nullptr, iWidth, iHeight, GL_RGB16F, GL_RGB, GL_FLOAT);

        // Set Texture Parameters
        pNewTexture->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        pNewTexture->setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        pNewTexture->setTexParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        pNewTexture->setTexParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Return the raw pointer to the caller
        pReturnTexture = pNewTexture.get();

        // Attach Texture to the Cache
        m_pTextureCache.insert(make_pair(sHashValue, move(pNewTexture)));
    }

    return pReturnTexture;
}
