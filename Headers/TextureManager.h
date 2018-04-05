#pragma once
#include "stdafx.h"
#include <unordered_map>
#include "Texture.h"

// Forward Declaration
class Object;

// Texture Manager Class
//		Manages all texture objects used by the system.
//	Methods:
//		- LoadTexture:
//			loads a texture into the system and returns a reference to that texture.
//			if the texture already exists, it will add a new userID to it and return
//			the already loaded texture. This avoids duplicate loads of a texture.
//		- unloadTexture:
//			Removes a user reference to the texture.  If there are no users left 
//			that reference the texture, it will unload the texture from memory.
//	Functionality:
//		Stores textures in a hash map of TextureContainer structures.  Each TextureContainer
//		contains the texture as well as a list of userIDs of users referencing that texture.
//		The file name of the texture is used as the unique hashmap handle.
//		This class is also a singleton so only 1 instance of TextureManager can be loaded at a time.
// Written By: James Coté
class TextureManager
{
public:
	// Singleton Implementation: returns instance of TextureManager
	static TextureManager* getInstance();
	~TextureManager();	// destructor.

	// Methods:
	Texture* loadTexture(const string &sFileName, long lID);
	void unloadTexture(const string &sFileName, long lID);

private:
	// Singleton Implementation
	static TextureManager* pInstance;
	TextureManager();
	TextureManager( const TextureManager& pCopy ) {}
	TextureManager& operator=( const TextureManager& pRHS ) {}

	// TextureContainer structure
	struct TextureContainer
	{
		Texture* pTexture;		// Texture Object Reference
		vector<long> lUserIDs;	// List of Users
	};

	// Hash Map
	unordered_map<string, TextureContainer> m_pTextureCache;
};

