#include "Object3D.h"
#include "EnvironmentManager.h"

// Default Constructor.
// Initializes to the center of the world and adds
//		itself to the EnvironmentManager
Object3D::Object3D( const vec3* pPosition, long lID, const string* sTexName, const Anim_Track* pAnimTrack )
	: Object( pPosition, lID, sTexName, pAnimTrack )
{
	EnvironmentManager::getInstance()->addObject( this );

	m_fScale = 1.f;
}

Object3D::Object3D( const Object3D* pCopy ) : Object( pCopy )
{
	m_pVertices		= pCopy->m_pVertices;
	m_iVertexArray	= pCopy->m_iVertexArray;
	m_iVertexBuffer = pCopy->m_iVertexBuffer;
}

Object3D::~Object3D()
{
	glDeleteBuffers( 1, &m_iVertexBuffer );
	glDeleteBuffers( 1, &m_iNormalBuffer );
	glDeleteBuffers( 1, &m_iTextureBuffer );
	glDeleteVertexArrays( 1, &m_iVertexArray );
}
