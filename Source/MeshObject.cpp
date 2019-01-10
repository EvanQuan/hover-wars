#include "MeshObject.h"
#define PI					3.14159265f

MeshObject::MeshObject( const glm::vec3* pPosition, const string* sFileName, long lID, 
						const string* sTexName, const Anim_Track* pAnimTrack )
	: Object3D( pPosition, lID, sTexName, pAnimTrack )
{
	m_fScale = 0.01f;
	m_pPosition = vec3( 0.0, 0.0, 0.0 );
	m_pQuaternion = quat(0.7071f, 0.0f, -0.7071f, 0.0f);	// Hack, get the Mesh oriented properly

	ShaderManager* pShdrMngr = ShaderManager::getInstance();

	m_pMesh = MeshManager::getInstance()->loadMeshFromFile( *sFileName, m_lID );
	m_pMesh->initMesh( );
	
	//m_pEdgeBuffer = new EdgeBuffer( m_iVertexArray );
	//m_pEdgeBuffer->GenerateAdjListMesh( m_pMesh );
}

// Delete any buffers that we initialized
MeshObject::~MeshObject()
{
	glDeleteVertexArrays( 1, &m_iVertexArray );

	if ( nullptr != m_pEdgeBuffer )
		delete m_pEdgeBuffer;

	if ( nullptr != m_pAnimTrack )
		delete m_pAnimTrack;

	m_pMesh = nullptr;
}

// draws the MeshObject by setting up the Shader
void MeshObject::draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause )
{
	ShaderManager* pShdrMngr = ShaderManager::getInstance();
	mat4 pPositionTranslated = mat4( m_fScale );

	if ( nullptr != m_pTexture )
		m_pTexture->bindTexture( ShaderManager::eShaderType::MESH_SHDR, "mySampler" );

	pShdrMngr->setUniformFloat( ShaderManager::eShaderType::MESH_SHDR, "fScale", m_fScale );

	// Draw the Animation Track
	if ( nullptr != m_pAnimTrack )
	{
		if( !m_bPause )
			m_pAnimTrack->animate();
		pPositionTranslated = m_pAnimTrack->getFreNetFrames() * scale(vec3(m_fScale)) * toMat4(m_pQuaternion);
		pShdrMngr->setUnifromMatrix4x4( ShaderManager::eShaderType::MESH_SHDR, "translate", &pPositionTranslated );
		m_pAnimTrack->draw();
	}
	

	//m_pEdgeBuffer->CalculateEdgeBufferMesh( m_pMesh, &vCamLookAt );
	vector< mat4 > pTestVec = { 1, pPositionTranslated };
	m_pMesh->loadInstanceData(&pTestVec);
	m_pMesh->drawMesh( ShaderManager::getInstance()->getProgram( ShaderManager::eShaderType::MESH_SHDR ) );
	//m_pEdgeBuffer->drawEdgeBuffer( m_fScale, m_pPosition, fMinThreshold, fMaxThreshold );


	if ( nullptr != m_pTexture )
		m_pTexture->unbindTexture();

	glUseProgram(0);
	glBindVertexArray( 0 );
}

// Data Dump for Debugging
string MeshObject::getDebugOutput()
{
	string sReturnVal = "";

	sReturnVal += getType();
	sReturnVal += "/ID:" + to_string( m_lID );

	return sReturnVal;
}

void MeshObject::calculateUVs()
{
	// Nothing ot Implement
}
