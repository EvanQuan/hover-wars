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
	m_pTextureCache.clear();
}

// loadTexture: Takes in a FileName and User Object.  If the Texture is already loaded, 
//				Add the user to the use list of the texture and return the reference to that texture.
//				Otherwise, create the texture, attach the user, store this information in the cache and return the
//				Reference to the texture.
// Parameters:	sFileName - The location of the file to load.
//				pUser - The 3D Object requesting a reference to the texture.
// Written by:  James Cote
Texture* TextureManager::loadTexture(const string& sFileName, long lID)
{
	// Attempt to grab it from the texture cache if it already exists
	Texture* pReturnTexture = nullptr;
	TextureContainer* pContainer;
	
	if (m_pTextureCache.end() != m_pTextureCache.find(sFileName))
	{
		// Grab the Texture Container from the Cache
		pContainer = &m_pTextureCache[sFileName];

		// Return the Texture
		pReturnTexture = pContainer->pTexture;

		// Add the User to Use List and Sort for faster searching.
		pContainer->lUserIDs.push_back(lID);						
		sort(pContainer->lUserIDs.begin(), pContainer->lUserIDs.end());	
	}
	else // Create the New Texture in the Texture Cache, attach the User to the Texture and return the newly created texture.
	{
		// Generate Texture Container
		pContainer = new TextureContainer();
		pContainer->pTexture = new Texture( sFileName );

		if ( !InitializeTexture( pContainer->pTexture, sFileName ) )
		{
			cout << "Failed to read texture: \"" << sFileName << "\"\n";
			if( sFileName != "" )
				cout << "Error, unable to load texture: " << sFileName << endl;
			delete pContainer;
		}
		else
		{
			pContainer->lUserIDs.push_back( lID );

			// Attach Texture Container to the Cache
			m_pTextureCache[ sFileName ] = *pContainer;

			// Return Newly Created Texture.
			pReturnTexture = pContainer->pTexture;
		}
	}

	return pReturnTexture;
}

// unloadTexture: This tells the Texture Manager that the Object is requesting to drop 
//				  its connection to the texture.  Using the Object ID, this will attempt to remove
//				  its reference to it if it exists then if there are no remaining IDs attached to 
//				  the texture is deleted from memory.
// Params:		sFileName - The Name of the File; a Handle to the Texture Container
//				pUser - The 3D Object that is wanting to drop its reference to the texture.
void TextureManager::unloadTexture(const string& sFileName, long lID)
{
	TextureContainer* pContainer;
	vector<long>::iterator pUserID;

	if (m_pTextureCache.end() != m_pTextureCache.find(sFileName))
	{
		pContainer = &m_pTextureCache[sFileName];
		pUserID = find(pContainer->lUserIDs.begin(), pContainer->lUserIDs.end(), lID);
		
		if( pUserID != pContainer->lUserIDs.end())
			pContainer->lUserIDs.erase(pUserID);

		if( pContainer->lUserIDs.empty() )
			m_pTextureCache.erase(sFileName);
	}	
}
