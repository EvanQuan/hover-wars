#include "MeshManager.h"

MeshManager* MeshManager::pInstance = nullptr;

MeshManager::MeshManager()
{
	// Nothing to Construct
}

MeshManager* MeshManager::getInstance()
{
	if ( nullptr == pInstance )
		pInstance = new MeshManager();

	return pInstance;
}


// Destructor: Clear the Texture Cache and release the memory
//				The Destructor of each texture should be called to release the memory of those textures.
MeshManager::~MeshManager()
{
	m_pMeshCache.clear();
}

// loadTexture: Takes in a FileName and User Object.  If the Texture is already loaded, 
//				Add the user to the use list of the texture and return the reference to that texture.
//				Otherwise, create the texture, attach the user, store this information in the cache and return the
//				Reference to the texture.
// Parameters:	sFileName - The location of the file to load.
//				pUser - The 3D Object requesting a reference to the texture.
// Written by:  James Cote
Mesh* MeshManager::loadMesh( const string& sFileName, long lID )
{
	// Attempt to grab it from the texture cache if it already exists
	Mesh* pReturnMesh = nullptr;
	MeshContainer* pContainer;

	if ( m_pMeshCache.end() != m_pMeshCache.find( sFileName ) )
	{
		// Grab the Texture Container from the Cache
		pContainer = &m_pMeshCache[ sFileName ];

		// Return the Texture
		pReturnMesh = pContainer->pMesh;

		// Add the User to Use List and Sort for faster searching.
		pContainer->lUserIDs.push_back( lID );
		sort( pContainer->lUserIDs.begin(), pContainer->lUserIDs.end() );
	}
	else // Create the New Texture in the Texture Cache, attach the User to the Texture and return the newly created texture.
	{
		// Generate Texture Container
		pContainer = new MeshContainer();
		pContainer->pMesh = new Mesh( sFileName );

		if ( !initializeMesh( pContainer->pMesh, sFileName ) )
		{
			if ( sFileName != "" )
				cout << "Error, unable to load texture: " << sFileName << endl;
			delete pContainer;
		}
		else
		{
			pContainer->lUserIDs.push_back( lID );

			// Attach Texture Container to the Cache
			m_pMeshCache[ sFileName ] = *pContainer;

			// Return Newly Created Texture.
			pReturnMesh = pContainer->pMesh;
		}
	}

	return pReturnMesh;
}

// Attempts to Initialize and return a new mesh object from a given object file.
// Returns: Mesh Object created or nullptr if mesh failed to create.
//			Bool: Returns true on Success, False on failure.
bool MeshManager::initializeMesh( Mesh* pReturnMesh, const string& sFileName )
{
	// Return Value
	bool bReturnValue = ( nullptr != pReturnMesh );

	if ( bReturnValue )
		bReturnValue = pReturnMesh->genMesh( sFileName );

	// Return result.
	return bReturnValue;
}

// unloadTexture: This tells the Texture Manager that the Object is requesting to drop 
//				  its connection to the texture.  Using the Object ID, this will attempt to remove
//				  its reference to it if it exists then if there are no remaining IDs attached to 
//				  the texture is deleted from memory.
// Params:		sFileName - The Name of the File; a Handle to the Texture Container
//				pUser - The 3D Object that is wanting to drop its reference to the texture.
// Note:		inherently avoids invalid IDs.
void MeshManager::unloadMesh( const string& sFileName, long lID )
{
	// Local Variables
	MeshContainer* pContainer;
	vector<long>::iterator pUserID;

	// Search for the specified texture.
	if ( m_pMeshCache.end() != m_pMeshCache.find( sFileName ) )
	{
		// Grab container and find specified user that's referencing the texture
		pContainer = &m_pMeshCache[ sFileName ];
		pUserID = find( pContainer->lUserIDs.begin(), pContainer->lUserIDs.end(), lID );

		// If the user is found in the texture userIDs, delete the user.
		if ( pUserID != pContainer->lUserIDs.end() )
			pContainer->lUserIDs.erase( pUserID );

		// Unload the texture if the last user was removed
		if ( pContainer->lUserIDs.empty() )
			m_pMeshCache.erase( sFileName );
	}
}