#include "Sphere.h"

#define PI 3.14159265f
#define SLICE_SIZE 15.f
#define MAX_THETA_DEGS 360.f
#define MAX_PHI_DEGS 180.f
#define MAX_THETA_CUTS (int)(MAX_THETA_DEGS / SLICE_SIZE)
#define MAX_PHI_CUTS (int)(MAX_PHI_DEGS / SLICE_SIZE)

// Constructor for Spheres
Sphere::Sphere( const vec3* pPos,
				float fRadius,
				long lID, const string* sTexName, const Anim_Track* pAnimTrack )
	: Object3D( pPos, lID, sTexName, pAnimTrack )
{
	ShaderManager* pShdrMngr = ShaderManager::getInstance();

	m_fRadius = fRadius;
	generateMesh();
	calculateUVs();
	m_pAnimTrack->animate();


	// Setup Mesh information in GPU
	glGenVertexArrays( 1, &m_iVertexArray );

	// Generate Buffer and Set Attributes
	m_iVertexBuffer = pShdrMngr->genVertexBuffer( m_iVertexArray, m_pVertices.data(), m_pVertices.size() * sizeof( vec3 ), GL_STATIC_DRAW );
	ShaderManager::getInstance()->setAttrib(m_iVertexArray, 0, 3, 0, nullptr);
	m_iNormalBuffer = pShdrMngr->genVertexBuffer( m_iVertexArray, m_pNormals.data(), m_pNormals.size() * sizeof( vec3 ), GL_STATIC_DRAW );
	ShaderManager::getInstance()->setAttrib(m_iVertexArray, 1, 3, 0, nullptr);
	m_iTextureBuffer = pShdrMngr->genVertexBuffer( m_iVertexArray, m_pUVs.data(), m_pUVs.size() * sizeof( vec2 ), GL_STATIC_DRAW );
	ShaderManager::getInstance()->setAttrib(m_iVertexArray, 2, 2, 0, nullptr);

	m_pEdgeBuffer = new EdgeBuffer( m_iVertexArray );
	m_pEdgeBuffer->GenerateAdjListStrip( m_pVertices, m_pNormals, m_pVertices.size() );

}

// Destructor
Sphere::~Sphere()
{
	// Nothing local to destruct
}

// Draw the Sphere
void Sphere::draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause )
{
	ShaderManager* pShdrMngr = ShaderManager::getInstance();


	glBindVertexArray( m_iVertexArray );

	if ( nullptr != m_pTexture )
		m_pTexture->bindTexture( ShaderManager::eShaderType::MESH_SHDR, "mySampler" );

	if ( nullptr != m_pAnimTrack )
	{
		m_pAnimTrack->animate();
		m_pPosition = m_pAnimTrack->getPosition();
		m_pAnimTrack->draw();
	}

	mat4 pPositionTranslated = m_pAnimTrack->getFreNetFrames();
	glUseProgram( pShdrMngr->getProgram( ShaderManager::eShaderType::MESH_SHDR ) );
	pShdrMngr->setUniformFloat( ShaderManager::eShaderType::MESH_SHDR, "fScale", m_fScale );
	pShdrMngr->setUnifromMatrix4x4( ShaderManager::eShaderType::MESH_SHDR, "translate", &pPositionTranslated );


	//m_pEdgeBuffer->CalculateEdgeBufferStrip( m_pNormals, &vCamLookAt );	
	glDrawArrays( GL_TRIANGLE_STRIP, 0, m_pVertices.size() );

	//m_pEdgeBuffer->drawEdgeBuffer( m_fScale, m_pPosition, fMinThreshold, fMaxThreshold );

	if ( nullptr != m_pTexture )
		m_pTexture->unbindTexture();

	glUseProgram( 0 );
	glBindVertexArray( 0 );
}

// Overridden Debug Output
string Sphere::getDebugOutput()
{
	string sOutput = "";

	sOutput += getType();
	sOutput += "/ID:" + to_string( m_lID );
	sOutput += "/Position:" + to_string( m_pPosition );
	sOutput += "/Radius:" + to_string( m_fRadius );

	return sOutput;
}

// calculates a mesh of vertices and UV coords for the sphere
void Sphere::generateMesh()
{
	float fCurrParallel, fNextParallel, fCurrMeridian;

	for ( unsigned int p = 0; p < MAX_PHI_CUTS; ++p )
	{
		fCurrParallel = PI * p / MAX_PHI_CUTS;
		fNextParallel = PI * (p + 1) / MAX_PHI_CUTS;

		for ( unsigned int m = 0; m < MAX_THETA_CUTS; ++m )
		{
			fCurrMeridian = (2.f * PI * m) / MAX_THETA_CUTS;
			addCarteseanPoint( fCurrParallel, fCurrMeridian );
			addCarteseanPoint( fNextParallel, fCurrMeridian );
		}
	}
}

// Generates a Caresean point and normal for that point based on a given spherical coord
void Sphere::addCarteseanPoint( float fPhi, float fTheta )
{
	float fPhi_Rads = fPhi;// *PI / 180.f;
	float fTheta_Rads = fTheta;// *PI / 180.f;
	vec3 pPoint;

	pPoint.x = m_fRadius * sin( fPhi_Rads );
	pPoint.y = pPoint.x * sin( fTheta_Rads );
	pPoint.x *= cos( fTheta_Rads );
	pPoint.z = m_fRadius * cos( fPhi_Rads );

	mat4 mLookAtTranslation = translate( mat4( 1.f ), (normalize( pPoint ) * m_fRadius) );
	vec4 mTranslatedPos = mLookAtTranslation * vec4( pPoint, 1.f );
	m_pVertices.push_back( vec3( mTranslatedPos ) );
	m_pNormals.push_back( normalize( vec3( mTranslatedPos ) - m_pPosition ) );
}

void Sphere::calculateUVs()
{
	vec2 pUV;
	vec3 pToOrigin;

	for ( vector<vec3>::iterator pIter = m_pVertices.begin();
		 pIter != m_pVertices.end();
		 ++pIter )
	{
		pToOrigin = normalize( m_pPosition - (*pIter) );

		pUV.x = 0.5f + (atan2( pToOrigin.z, pToOrigin.x ) / 2.f * PI);
		pUV.y = 0.5f - asin( pToOrigin.y ) / PI;

		m_pUVs.push_back( pUV );
	}
}
