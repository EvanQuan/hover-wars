#include "BoidEngine.h"
#include <glm/gtc/random.hpp>

/***********\
 * DEFINES *
\***********/
#define PI					3.14159265f
#define ID					999
#define MIN_BOIDS			1000
#define HEIGHT				3.0f
#define HALF_HEIGHT			(HEIGHT * 0.5f)
#define WIDTH				3.0f
#define HALF_WIDTH			(WIDTH * 0.5f)
#define DEPTH				3.0f
#define HALF_DEPTH			(DEPTH * 0.5f)
#define AVOID_DIST			0.05f
#define COHES_DIST			0.1f
#define GATHR_DIST			0.15f

const vec3 SPACE_BOUND = vec3(HALF_WIDTH, HEIGHT, HALF_DEPTH);
const vec3 LOWER_SPACE_BOUND = vec3(-HALF_WIDTH, 0.0f, -HALF_DEPTH);

// Default Constructor
BoidEngine::BoidEngine( const string* sModelFileName, const string* sTexFileName )
{
	m_fScale = 3.0f;
	m_pQuaternion = quat( 0.7071f, 0.0f, -0.7071f, 0.0f );	// Hack, get the Mesh oriented properly

	// Load Texture
	if( !sTexFileName->empty() )
		m_pTexture = TextureManager::getInstance()->loadTexture(*sTexFileName, ID);
	else m_pTexture = nullptr;

	// Load Mesh
	if (!sModelFileName->empty())
	{
		m_pMesh = MeshManager::getInstance()->loadMesh(*sModelFileName, ID); // Hack - Giving magic ID as reference to BoidEngine
		m_pMesh->initMesh();
	}
	else m_pMesh = nullptr;

	initializeBoids();
}

// Delete any buffers that we initialized
BoidEngine::~BoidEngine()
{
	if( nullptr != m_pMesh )
		MeshManager::getInstance()->unloadMesh( m_pMesh->getFileName(), ID );

	if( nullptr != m_pTexture)
		TextureManager::getInstance()->unloadTexture(m_pTexture->getFileName(), ID);
}

// Generate Initial Boid Population
void BoidEngine::initializeBoids()
{
	for (unsigned int i = 0; i < MIN_BOIDS; ++i)
	{
		Boid pNewBoid;
		pNewBoid.vPos = linearRand(LOWER_SPACE_BOUND, SPACE_BOUND);
		pNewBoid.vVelocity = ballRand(1.0f);
		pNewBoid.fAvoidDistance = AVOID_DIST;
		pNewBoid.fCohesionDistance = COHES_DIST;
		pNewBoid.fGatherDistance = GATHR_DIST;
		pNewBoid.m4FrenetFrame[2] = vec4(pNewBoid.vVelocity, 0.0f );
		pNewBoid.m4FrenetFrame[0] = vec4(normalize( cross( vec3( 0.0f, 1.0f, 0.0f ), pNewBoid.vVelocity) ), 0.0f );
		pNewBoid.m4FrenetFrame[1] = vec4(normalize(cross(vec3(pNewBoid.m4FrenetFrame[2]), vec3(pNewBoid.m4FrenetFrame[0]))), 0.0f);
		pNewBoid.m4FrenetFrame[3] = vec4(pNewBoid.vPos, 1.0f);
		m_vBoids.push_back(pNewBoid);
	}
}

void BoidEngine::update()
{
	/* Not Implemented */
}

// draws the BoidEngine by setting up the Shader
void BoidEngine::draw( )
{
	ShaderManager* pShdrMngr = ShaderManager::getInstance();

	if ( nullptr != m_pTexture )
		m_pTexture->bindTexture( ShaderManager::eShaderType::BOID_SHDR, "mySampler" );

	pShdrMngr->setUniformFloat( ShaderManager::eShaderType::BOID_SHDR, "fScale", m_fScale );

	genInstanceData();
	m_pMesh->loadInstanceData(&m_vInstanceData);
	m_pMesh->drawMesh( ShaderManager::getInstance()->getProgram( ShaderManager::eShaderType::BOID_SHDR ) );

	if ( nullptr != m_pTexture )
		m_pTexture->unbindTexture();

	glUseProgram(0);
	glBindVertexArray( 0 );
}

// Gathers Boid FrenetFrames into vector for rendering
void BoidEngine::genInstanceData()
{
	m_vInstanceData.clear();
	for (vector< Boid >::const_iterator iter = m_vBoids.begin();
		iter != m_vBoids.end();
		++iter)
	{
		m_vInstanceData.push_back(iter->m4FrenetFrame);
	}
}
