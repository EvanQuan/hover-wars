#include "Light.h"
#include "EntityManager.h"

const float LIGHT_SIZE = 30.f;

// Constructor
Light::Light(const glm::vec3* pPos,
			 const glm::vec3* pColor,
			 long lID, const string* sTexName, const Anim_Track* pAnimTrack ) : Object( pPos, lID, sTexName, pAnimTrack )
{
	m_pColor		= (*pColor);
	EntityManager::getInstance()->addLight( this );
	glGenVertexArrays(1, &m_iVertexArray);

	// Generate Buffer and Set up Attributes
	m_iVertexBuffer = ShaderManager::getInstance()->genVertexBuffer(m_iVertexArray, &m_pPosition,
																	sizeof(m_pPosition), GL_STATIC_DRAW );
	ShaderManager::getInstance()->setAttrib(m_iVertexArray, 0, 3, 0, nullptr);
}

// Destructor
Light::~Light()
{
	glDeleteBuffers(1, &m_iVertexBuffer);
	glDeleteVertexArrays(1, &m_iVertexArray);
}

void Light::draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause )
{
	ShaderManager* pShdrMngr = ShaderManager::getInstance();

	if ( nullptr != m_pTexture )
	{
		m_pTexture->bindTexture( ShaderManager::eShaderType::LIGHT_SHDR, "gSampler" );
	}

	glBindVertexArray(m_iVertexArray);
	glUseProgram(pShdrMngr->getProgram( ShaderManager::eShaderType::LIGHT_SHDR));

	glPointSize(20.f);
	glDrawArrays(GL_POINTS, 0, 1);
	glPointSize( 1.f );

	if ( nullptr != m_pTexture )
	{
		m_pTexture->unbindTexture();
	}

	glUseProgram(0);
	glBindVertexArray(0);
}
