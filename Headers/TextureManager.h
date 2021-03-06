#pragma once
#include "stdafx.h"
#include "Texture.h"
#include "UserInterface/UserInterface.h"

// Texture Manager Class
//        Manages all texture objects used by the system.
//    Methods:
//        - LoadTexture:
//            loads a texture into the system and returns a reference to that texture.
//            if the texture already exists, it will add a new userID to it and return
//            the already loaded texture. This avoids duplicate loads of a texture.
//        - unloadTexture:
//            Removes a user reference to the texture.  If there are no users left
//            that reference the texture, it will unload the texture from memory.
//    Functionality:
//        Stores textures in a hash map of TextureContainer structures.  Each TextureContainer
//        contains the texture as well as a list of userIDs of users referencing that texture.
//        The file name of the texture is used as the unique hashmap handle.
//        This class is also a singleton so only 1 instance of TextureManager can be loaded at a time.
// Written By: James Coté
class TextureManager final
{
public:
    // Singleton Implementation: returns instance of TextureManager
    static TextureManager* getInstance();
    ~TextureManager();    // destructor.

    // Methods:
    Texture* loadTexture(const string &sFileName);

    /*
        Load a vector of textures from a given directory.

        @param[in] vFiles       paths of all the files to load from the specified
                                directory as the root
        @param[in] sDirectory   from which the files are located.

        @return a map of textures, of which the given file paths are their
                corresponding keys.
    */
    unordered_map<UserInterface::eImage, Texture*> loadTextures(const unordered_map<UserInterface::eImage, string> &mFiles, const string &sDirectory);
    Texture* loadCubeMap(const vector<string>* sFileNames);
    Texture* genTexture(const vec4* vColor);
    Texture* genDepthBuffer(unsigned int iWidth, unsigned int iHeight);
    Texture* genFrameBufferTexture(unsigned int iWidth, unsigned int iHeight, unsigned int iPlayer, unsigned int iBufferNum);
    void unloadTexture(Texture** pTexture);
    void unloadAllTextures();

private:
    // Singleton Implementation
    static TextureManager* pInstance;
    TextureManager();
    TextureManager(const TextureManager& pCopy);
    TextureManager& operator=(const TextureManager& pRHS);

    // Hash Map
    unordered_map<string, unique_ptr<Texture>> m_pTextureCache;
};
