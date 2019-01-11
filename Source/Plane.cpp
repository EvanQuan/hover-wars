#include "Plane.h"

#define NUM_CORNERS 4

// Constructor
Plane::Plane( const vec3* pPosition,
			  const vector<glm::vec3>* pCorners, 
			  long lID,
			  const string* sTexName,
			  bool bUseEB, const Anim_Track* pAnimTrack ) : Object3D( pPosition, lID, sTexName, pAnimTrack )
{
	if ( NUM_CORNERS != pCorners->size() )	// Set up a default Plane
	{
		m_pVertices.push_back( glm::vec3( -1.f, 0.f, -1.f ) );
		m_pVertices.push_back( glm::vec3( -1.f, 0.f, 1.f ) );
		m_pVertices.push_back( glm::vec3( 1.f, 0.f, -1.f ) );
		m_pVertices.push_back( glm::vec3( 1.f, 0.f, 1.f ) );
	}
	else
		m_pVertices.insert( m_pVertices.begin(), pCorners->begin(), pCorners->end() );

	// Set up Normal and Vertex Normals
	m_pNormal = normalize( cross( m_pVertices[ 1 ] - m_pVertices[ 0 ], m_pVertices[ 2 ] - m_pVertices[ 0 ] ) );
	m_pNormals.resize( m_pVertices.size(), m_pNormal );

	m_fScale = 1.f;

	glGenVertexArrays( 1, &m_iVertexArray );

	// Generate Buffers and Set Attributes in VAO
	m_iVertexBuffer = SHADER_MANAGER->genVertexBuffer( m_iVertexArray,  m_pVertices.data(),
																	 m_pVertices.size() * sizeof( glm::vec3 ), GL_STATIC_DRAW );
	SHADER_MANAGER->setAttrib(m_iVertexArray, 0, 3, 0, nullptr);
	m_iNormalBuffer = SHADER_MANAGER->genVertexBuffer( m_iVertexArray, m_pNormals.data(),
																	 m_pNormals.size() * sizeof( glm::vec3 ), GL_STATIC_DRAW );
	SHADER_MANAGER->setAttrib(m_iVertexArray, 1, 3, 0, nullptr);
	calculateUVs();

	if ( bUseEB )
	{
		m_pEdgeBuffer = new EdgeBuffer( m_iVertexArray );
		m_pEdgeBuffer->GenerateAdjListStrip( m_pVertices, m_pNormals, m_pVertices.size() );
	}
	else
		m_pEdgeBuffer = nullptr;

	if ( nullptr == m_pTexture )
		SHADER_MANAGER->setUniformBool( ShaderManager::eShaderType::PLANE_SHDR, "bTextureLoaded", false );
}

// Destructor
Plane::~Plane()
{
	if ( nullptr != m_pEdgeBuffer )
		delete m_pEdgeBuffer;
}

// Overridden Debug Output
string Plane::getDebugOutput()
{
	string sOutput = "";

	sOutput += getType();
	sOutput += "/ID:" + to_string( m_lID );
	sOutput += "/Normal:" + glm::to_string( m_pNormal );

	return sOutput;
}

// Setup OpenGl to draw the Plane using the Plane Shader.
void Plane::draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause )
{
	glBindVertexArray( m_iVertexArray );

	if ( nullptr != m_pTexture )
	{
		m_pTexture->bindTexture( ShaderManager::eShaderType::PLANE_SHDR, "gSampler" );
	}

	glUseProgram( SHADER_MANAGER->getProgram( ShaderManager::eShaderType::PLANE_SHDR ) );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	// Render EdgeBuffer
	if ( nullptr != m_pEdgeBuffer ) 
	{
		m_pEdgeBuffer->CalculateEdgeBufferStrip( m_pNormals, &vCamLookAt );
		m_pEdgeBuffer->drawEdgeBuffer( m_fScale, m_pPosition, fMinThreshold, fMaxThreshold );
	}

	if ( nullptr != m_pTexture )
		m_pTexture->unbindTexture();

	glUseProgram(0);
	glBindVertexArray( 0 );
}

void Plane::calculateUVs()
{
	m_pUVs.push_back( vec2( 0.0, 0.0 ) );
	m_pUVs.push_back( vec2( 0.0, 1.0 ) );
	m_pUVs.push_back( vec2( 1.0, 0.0 ) );
	m_pUVs.push_back( vec2( 1.0, 1.0 ) );

	// Generate Buffer and Set Up Attribute
	m_iTextureBuffer = SHADER_MANAGER->genVertexBuffer( m_iVertexArray, m_pUVs.data(),
																	  m_pUVs.size() * sizeof( vec2 ), GL_STATIC_DRAW );
	SHADER_MANAGER->setAttrib(m_iVertexArray, 2, 2, 0, nullptr);
}
