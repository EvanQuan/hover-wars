#include "Object.h"
#include "TextureManager.h"

// Default Construtor to initialize position and ID
Object::Object( const vec3* pPos, long lID, const string* sTexName, const Anim_Track* pAnimTrack )
{
	m_pPosition = *pPos;
	m_lID = lID;

	if( !sTexName->empty() )
		m_pTexture = TEXTURE_MANAGER->loadTexture( *sTexName );
	else m_pTexture = nullptr;

	if ( nullptr != pAnimTrack )
		m_pAnimTrack = new Anim_Track( *pAnimTrack );
	else m_pAnimTrack = nullptr;
}

// Copy Constructor
Object::Object( const Object* pCopy )
{
	m_pPosition = pCopy->m_pPosition;
	m_lID = pCopy->m_lID;

	m_pTexture = nullptr;

	m_pAnimTrack = pCopy->m_pAnimTrack;
}

// Destructor: unload Texture.
Object::~Object()
{
	if ( nullptr != m_pAnimTrack )
		delete m_pAnimTrack;
}

// Changes the Texture of the Object.  Given a Texture Location, it will attempt to
// Load the new texture first to see if it can be loaded.  If it can, it will unload its
// current texture and store the new texture.
void Object::switchTexture( const string* sTexLoc )
{
	TextureManager* pTxMngr = TEXTURE_MANAGER;
	Texture* pNewTexture = pTxMngr->loadTexture( *sTexLoc );

	if ( nullptr != pNewTexture )
	{
		m_pTexture = pNewTexture;
	}
}

// If there's an Animation Track, return the Frenet frames, otherwise
//	Return Identity matrix.
mat4 Object::getFreNetFrames()
{
	mat4 pReturnObj = mat4( 1.0 ); // Identity Matrix to return as default
	if ( nullptr != m_pAnimTrack )
		pReturnObj = m_pAnimTrack->getFreNetFrames();

	return pReturnObj;
}
