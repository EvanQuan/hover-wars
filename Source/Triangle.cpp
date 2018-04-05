#include "Triangle.h"

#define VERTEX_0 m_pVertices[0]
#define VERTEX_1 m_pVertices[1]
#define VERTEX_2 m_pVertices[2]
#define EPSILON	1e-6
#define NUM_VERTS 3

// Constructor.
Triangle::Triangle( const vec3* pPosition, const vector<vec3>* pVerts, long lID, const string* sTexName, const Anim_Track* pAnimTrack )
	: Object3D( pPosition, lID, sTexName, pAnimTrack )
{
	if ( NUM_VERTS != pVerts->size() )
	{
		m_pVertices.push_back( vec3( 1.f, 0.f, 0.f ) );
		m_pVertices.push_back( vec3( 0.f, 1.f, 0.f ) );
		m_pVertices.push_back( vec3( 0.f, 0.f, 1.f ) );
	}
	else
		m_pVertices.insert( m_pVertices.begin(), pVerts->begin(), pVerts->end() );

	mat4 pTranslation = translate( m_pPosition - vec3( 0.f ) );

	// Translate the triangle to the position.
	// Question:  Where's the Anchor?
	for ( vector<vec3>::iterator pIter = m_pVertices.begin();
		  pIter != m_pVertices.end();
		  ++pIter )
	{
		(*pIter) = vec3( vec4(*pIter, 1.0f) * pTranslation );
	}

	// Set up GPU Buffers
	glGenVertexArrays( 1, &m_iVertexArray );

	// Generate Buffer
	m_iVertexBuffer = ShaderManager::getInstance()->genVertexBuffer( m_iVertexArray,
																	 m_pVertices.data(),
																	 m_pVertices.size() * sizeof( vec3 ), GL_STATIC_DRAW );

	// Set up Attributes
	ShaderManager::getInstance()->setAttrib(m_iVertexArray, 0, 3, 0, nullptr);

}

// Destructor
Triangle::~Triangle()
{
	glDeleteBuffers( 1, &m_iVertexBuffer );
	glDeleteVertexArrays( 1, &m_iVertexArray );
}

// Simply draw a triangle using the plane shader (for Now);
void Triangle::draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause )
{
	glBindVertexArray( m_iVertexArray );

	glUseProgram( ShaderManager::getInstance()->getProgram( ShaderManager::eShaderType::PLANE_SHDR ) );

	if ( nullptr != m_pTexture )
		m_pTexture->bindTexture( ShaderManager::eShaderType::PLANE_SHDR, "gSampler" );

	glDrawArrays( GL_TRIANGLES, 0, 1 );

	if ( nullptr != m_pTexture )
		m_pTexture->unbindTexture();

	glUseProgram(0);
	glBindVertexArray( 0 );
}

// Inherited from Parent
void Triangle::calculateUVs()
{
	vec3 v0v1 = VERTEX_1 - VERTEX_0;
	vec3 v0v2 = VERTEX_2 - VERTEX_0;
	vec3 normal = cross( v0v2, v0v1 );
	normal = normalize( normal );

	//
	//  x
	//  |\
	//  | \
	//  y--z
}

// Overridden Debug Output
string Triangle::getDebugOutput()
{
	string sOutput = "";

	sOutput += getType();
	sOutput += "/ID:" + to_string( m_lID );
	sOutput += "/P1:" + glm::to_string( VERTEX_0 );
	sOutput += "/P2:" + glm::to_string( VERTEX_1 );
	sOutput += "/P3:" + glm::to_string( VERTEX_2 );
	sOutput += "/Position:" + to_string( m_pPosition );

	return sOutput;
}

