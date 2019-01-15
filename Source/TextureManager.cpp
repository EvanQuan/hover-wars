#include "TextureManager.h"
#include "Object3D.h"
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
//				The Destructor of each texture should be called to release the memory of those textures.
TextureManager::~TextureManager()
{
	unloadAllTextures();
}

// unloadAllTextures: This will clear all the unique_ptrs for Textures, effectively clearing the heap
//						and unloading all allocated Texture data.
void TextureManager::unloadAllTextures( )
{
	m_pTextureCache.clear();
}

// loadTexture: Takes in a FileName that is uses for a hashmap used to store the textures.
//				If the texture has already been loaded, return a pointer to that loaded texture.
//				Otherwise, load the texture and return the pointer to the newly created texture.
//				Stores all created textures in its hash map.
// Parameters:	sFileName - The location of the file to load.
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
	else // Create the New Texture in the Texture Cache, attach the User to the Texture and return the newly created texture.
	{
		// Generate Texture Smart Pointer
		unique_ptr<Texture> pNewTexture = make_unique<Texture>(sFileName, Texture::manager_cookie() );

		if ( !InitializeTexture( pNewTexture.get(), sFileName ) )
		{
			cout << "Failed to read texture: \"" << sFileName << "\"\n";
			if( sFileName != "" )
				cout << "Error, unable to load texture: " << sFileName << endl;
			pNewTexture.reset();
		}
		else
		{
			// Return the raw pointer to the caller
			pReturnTexture = pNewTexture.get();
			
			// Attach Texture to the Cache
			m_pTextureCache.insert(make_pair(sFileName, move(pNewTexture)));
		}
	}

	return pReturnTexture;
}


