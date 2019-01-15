#include "BoidEngine.h"
#include <glm/gtc/random.hpp>

/***********\
 * DEFINES *
\***********/
#define PI						3.14159265f
#define PI2						6.283185307f
#define ID						999
#define CUBE_SIZE				m_fGathrRadius
#define BOUNDARY_WIDTH			(3.0f * m_fScale)
#define MIN_PARAMS				9
#define NUM_ITERATIONS			16
#define DEFAULT_NUM_BOIDS		1000
#define DEFAULT_AVOID_DIST		0.33f
#define DEFAULT_COHES_DIST		0.66f
#define DEFAULT_GATHR_DIST		1.f
#define DEFAULT_MIN_SPEED		1.f
#define DEFAULT_MAX_SPEED		5.f
#define DEFAULT_SCALE			1.f
#define DEFAULT_DELTA_T			0.01f
#define DEFAULT_BOUNDARY_RADIUS	3.0f

// Default Model and Texture
const string sModelFileName = "models/paper_airplane.obj";
const string sTexFileName =   "textures-256-256/fig-10c.png";

////////////////////
// BOID FUNCTIONS //
////////////////////
float cohesFunc(float l) { return (0.5f * (-cos(PI2*l) + 1.0f)); }
float avoidFunc(float l) { return (1 / (l*l)) - 1.0f; }
float gathrFunc(float l) { return (-cos(PI*l) + 1.0f); }
float boundFunc(float l) { return (1 / (2*l*l)); }
const vec3 GRAVITY = vec3(0.0, -0.981, 0.0);

// Default Constructor
BoidEngine::BoidEngine( )
{
	// Init Useful data from defaults
	m_iNumBoids			= DEFAULT_NUM_BOIDS;
	m_fAvoidRadius		= DEFAULT_AVOID_DIST;
	m_fCohesRadius		= DEFAULT_COHES_DIST;
	m_fGathrRadius		= DEFAULT_GATHR_DIST;
	m_fMinSpeed			= DEFAULT_MIN_SPEED;
	m_fMaxSpeed			= DEFAULT_MAX_SPEED;
	m_fBoundaryRadius	= DEFAULT_BOUNDARY_RADIUS;
	m_fDeltaT			= DEFAULT_DELTA_T;
	m_fScale			= DEFAULT_SCALE;
	
	// Finish Initialization
	initializeBoids();
}

// Overloaded Constructor for loading from file/vector of data:
/*	Indices:
		0 - Num_Boids
		1 - Avoid Radius
		2 - Cohesion Radius
		3 - Gather Radius		4 - Min Speed (length)
		5 - Max Speed (length)
		6 - Boundary Radius
		7 - Delta T
		8 - Scale
		9 - Texture Location (Optional)
		10 - Mesh Location (Optional)
*/
BoidEngine::BoidEngine(vector< string > &sData)
{
	unsigned int iSize = sData.size();

	if (iSize < MIN_PARAMS)
		BoidEngine();
	else
	{
		// Parse and store data
		m_iNumBoids			= stoi(sData[0]);
		m_fScale			= stof(sData[8]);
		m_fAvoidRadius		= stof(sData[1]) * m_fScale;
		m_fCohesRadius		= stof(sData[2]) * m_fScale;
		m_fGathrRadius		= stof(sData[3]) * m_fScale;
		m_fMinSpeed			= stof(sData[4]);
		m_fMaxSpeed			= stof(sData[5]);
		m_fBoundaryRadius	= stof(sData[6]) * m_fScale;
		m_fDeltaT			= stof(sData[7]);

		// Texture
		if (iSize > MIN_PARAMS)
			m_pTexture		= TEXTURE_MANAGER->loadTexture(sData[9]);

		// Mesh
		if (iSize > (MIN_PARAMS + 1))
			m_pMesh			= MESH_MANAGER->loadMeshFromFile(sData[10]); 

		// Complete Initialization
		initializeBoids();
	}
}

// Delete any buffers that we initialized
BoidEngine::~BoidEngine()
{
	// Cleanup
	if (nullptr != m_pMesh)
		m_pMesh = nullptr;

	// Clear Vectors incase of resizing
	m_vBoids.clear();
	m_vInstanceData.clear();
	m_pScreenSpace.clear();
}

// Generate Initial Boid Population
void BoidEngine::initializeBoids()
{
	// Compute some useful values
	m_fCohesWidth = (m_fCohesRadius - m_fAvoidRadius);
	m_fGathrWidth = (m_fGathrRadius - m_fCohesRadius);
	m_fBoundaryLimit = m_fBoundaryRadius - BOUNDARY_WIDTH;
	m_fBoundaryCenter = vec3(0.0f, m_fBoundaryRadius, 0.0f);
	m_pQuaternion = glm::angleAxis(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));	// Hack, get the Mesh oriented properly

	// Compute Data Space
	m_iSpaceDim = (unsigned int)ceil(2.f * m_fBoundaryRadius / CUBE_SIZE);
	m_iSpaceDimSqr = m_iSpaceDim * m_iSpaceDim;
	m_pScreenSpace.resize(m_iSpaceDim * m_iSpaceDimSqr,				// Initialize the ScreenSpace to cubic size x*x*x where x: number of cubes per dimension of space
		pair< unsigned int, vector< unsigned int > >{0, {}});		// Initialze each value with 0th timestep.
	m_iCurrTimeStep = 1;

	// Set a size of Instance Matrices = Num of Boids
	m_vInstanceData.resize(m_iNumBoids, mat4(1.0f));

	// Load Default Texture or Mesh if it failed to load.
	if (nullptr == m_pTexture)
		m_pTexture = TEXTURE_MANAGER->loadTexture(sTexFileName);
	if (nullptr == m_pMesh)
		m_pMesh = MESH_MANAGER->loadMeshFromFile(sModelFileName);

	m_pMesh->initMesh();

	// Generate List of Boids
	for (unsigned int i = 0; i < m_iNumBoids; ++i)
	{
		Boid pNewBoid;
		pNewBoid.vPos = ballRand( m_fBoundaryLimit ) + m_fBoundaryCenter;
		pNewBoid.vVelocity = ballRand(3.0f);
		pNewBoid.vGravityForce = vec3(0.0f);
		pNewBoid.m4FrenetFrame[2] = vec4(pNewBoid.vVelocity, 0.0f );
		pNewBoid.m4FrenetFrame[0] = vec4(normalize( cross( vec3( 0.0f, 1.0f, 0.0f ), pNewBoid.vVelocity) ), 0.0f );
		pNewBoid.m4FrenetFrame[1] = vec4(normalize(cross(vec3(pNewBoid.m4FrenetFrame[2]), vec3(pNewBoid.m4FrenetFrame[0]))), 0.0f);
		pNewBoid.m4FrenetFrame[3] = vec4(pNewBoid.vPos, 1.0f);
		m_vBoids.push_back(pNewBoid);
	}
}

// Update each boid's forces and integrate their new velocities and positions
void BoidEngine::update()
{
	for (int i = 0; i < NUM_ITERATIONS; ++i)
	{
		// Apply this for Every Boid
		for (unsigned int iBoidIndex = 0; iBoidIndex < m_vBoids.size(); ++iBoidIndex)
		{
			unsigned int iCurrPos = getSpaceIndex(m_vBoids[iBoidIndex].vPos);

			// Check xz-Plane
			/*
				4-5-x
				2-1-x
				3-x-x
			*/
			checkCell(iBoidIndex, iCurrPos);					// 1
			getSpaceContents(iCurrPos)->push_back(iBoidIndex);	// Add Boid Index to current cell for reference.
			iCurrPos -= 1;										// Move left 1 space
			checkCell(iBoidIndex, iCurrPos);					// 2
			checkCell(iBoidIndex, iCurrPos + m_iSpaceDim);		// 3
			iCurrPos -= m_iSpaceDim;							// Move up a row
			checkCell(iBoidIndex, iCurrPos);					// 4
			checkCell(iBoidIndex, iCurrPos + 1);				// 5


			// Check next Plane over
			iCurrPos -= m_iSpaceDimSqr;
			/*
				1-2-3
				4-5-6
				7-8-9
			*/
			for (int y = 0; y < 3; ++y)
			{
				for (int x = 0; x < 3; ++x)
					checkCell(iBoidIndex, iCurrPos + x);

				iCurrPos += m_iSpaceDim;
			}

			checkBounds(m_vBoids[iBoidIndex]);
		}

		// Once all Boids have been updated, integrate them.
		for (unsigned int i = 0; i < m_iNumBoids; ++i)
			integrateBoid(i);

		// Increment the Timestep after the update.
		m_iCurrTimeStep++;
	}
}

// Compares an Indexed Boid with all other Boids in an Indexed Cell. Then adds this Boid index to the cell.
void BoidEngine::checkCell(unsigned int iBoidIndex, unsigned int iCellIndex)
{
	// Local Variables
	vector< unsigned int >* pCellList;

	// Check Self
	pCellList = getSpaceContents(iCellIndex);

	if (nullptr != pCellList)
	{
		for (vector< unsigned int >::const_iterator i = pCellList->begin();
			i != pCellList->end();
			++i)
			compareBoids(m_vBoids[iBoidIndex], m_vBoids[*i]);
	}
}

// Function to apply Boid Logic between two boids
void BoidEngine::compareBoids(Boid& pSource, Boid& pComp)
{
	// Distance between the two boids
	float l = distance(pSource.vPos, pComp.vPos);

	// Apply Force Logic to Boids
	if (l < m_fAvoidRadius)
	{
		// Want to push the boid away if it gets too close
		l /= m_fAvoidRadius; // Between 0-1
		vec3 vPushBackForce = normalize(pSource.vPos - pComp.vPos) * avoidFunc(l);
		pSource.vForce += vPushBackForce;
		pComp.vForce -= vPushBackForce;
	}
	else if (l < m_fCohesRadius)
	{
		// Want to keep the boids grouped up within a certain distance
		l = (l - m_fAvoidRadius) / m_fCohesWidth; // Between 0-1
		vec3 vCohesForce = normalize(pComp.vVelocity - pSource.vVelocity) * cohesFunc(l);
		pSource.vForce += vCohesForce;
		pComp.vForce -= vCohesForce;
	}
	else if (l < m_fGathrRadius)
	{
		// If the come into a proximity with one another, tell them to move together (inverse of the avoid function)
		l = (l - m_fCohesRadius) / m_fGathrWidth; // Between 0-1
		vec3 vGatherForce = normalize(pComp.vPos - pSource.vPos) * gathrFunc(l);
		pSource.vForce += vGatherForce;
		pComp.vForce -= vGatherForce;
	}
}

// Function to get index for ScreenSpace structure given an (x,y,z) position.
unsigned int BoidEngine::getSpaceIndex(const vec3& pPos)
{
	unsigned int iX, iY, iZ;
	vec3 vZeroedPos = pPos - m_fBoundaryCenter; // Center at Origin for accurate index

	// Get Indices
	iX = (unsigned int)floor((vZeroedPos.x + m_fBoundaryRadius) / CUBE_SIZE);
	iY = (unsigned int)floor((vZeroedPos.y + m_fBoundaryRadius) / CUBE_SIZE);
	iZ = (unsigned int)floor((vZeroedPos.z + m_fBoundaryRadius) / CUBE_SIZE);

	// Push them to nearest space if they go out of bounds momentarily
	iX = (iX < m_iSpaceDim) ? iX : m_iSpaceDim - 1;
	iY = (iY < m_iSpaceDim) ? iY : m_iSpaceDim - 1;
	iZ = (iZ < m_iSpaceDim) ? iZ : m_iSpaceDim - 1;

	// Combine into 1D Index
	return iX + (iY * m_iSpaceDim) + (iZ * m_iSpaceDimSqr);
}

vector< unsigned int >* BoidEngine::getSpaceContents(unsigned int iIndex)
{
	// Local Return Variable
	vector< unsigned int >* pReturn = nullptr;

	// Ensure Valid Index
	if (iIndex >= 0 && iIndex < m_pScreenSpace.size())
	{
		// Invalid Timestep? Reset Associated List
		if (m_pScreenSpace[iIndex].first != m_iCurrTimeStep)	
		{
			m_pScreenSpace[iIndex].first = m_iCurrTimeStep;
			m_pScreenSpace[iIndex].second.clear();
		}

		// Return Associated List
		pReturn = &m_pScreenSpace[iIndex].second;
	}

	return pReturn;
}

// Calculates Acceleration and updates Velocity and Position
//  Resets Force afterwards.
void BoidEngine::integrateBoid(unsigned int i)
{
	// Integrate Boid


	m_vBoids[i].vVelocity += m_vBoids[i].vForce * m_fDeltaT;

	float fLength = length(m_vBoids[i].vVelocity);

	if (fLength < m_fMinSpeed)
		m_vBoids[i].vVelocity = m_fMinSpeed * normalize(m_vBoids[i].vVelocity);
	else if( fLength > m_fMaxSpeed)
		m_vBoids[i].vVelocity = m_fMaxSpeed * normalize(m_vBoids[i].vVelocity);

	m_vBoids[i].vPos += m_vBoids[i].vVelocity * m_fDeltaT;

	// Setup Frenet Frame
	m_vBoids[i].m4FrenetFrame[2] = vec4(normalize(m_vBoids[i].vVelocity), 0.0f);																		// Tangent
	m_vBoids[i].m4FrenetFrame[0] = vec4(normalize(cross(m_vBoids[i].vForce + vec3(0.0f, 100.0f, 0.0f)/*UP-VEC*/, vec3(m_vBoids[i].m4FrenetFrame[2]))), 0.0f);	// BiNormal
	m_vBoids[i].m4FrenetFrame[1] = vec4(normalize(cross(vec3(m_vBoids[i].m4FrenetFrame[2]), vec3(m_vBoids[i].m4FrenetFrame[0]))), 0.0f);						// Normal
	m_vBoids[i].m4FrenetFrame[3] = vec4(m_vBoids[i].vPos, 1.0f);

	// Apply Rotation to face Model Forward
	m_vBoids[i].m4FrenetFrame = m_vBoids[i].m4FrenetFrame * scale(vec3( m_fScale )) * toMat4(m_pQuaternion);

	// Update Instance Location
	m_vInstanceData[i] = m_vBoids[i].m4FrenetFrame;
	m_vBoids[i].vForce = vec3(0.0f);
}

// Check boid against bounds of the sphere to keep the boid within bounds
void BoidEngine::checkBounds(Boid& pBoid)
{
	vec3 vToCenter = m_fBoundaryCenter - pBoid.vPos;
	float fDist = length( vToCenter );
	vToCenter /= fDist;	// Normalize vector

	// If within Boundary zone, push back toward center
	if (dot(pBoid.vVelocity, vToCenter) < -FLT_EPSILON && fDist >= m_fBoundaryLimit)
	{
		float fTest = 1.0f - fmodf(fDist - m_fBoundaryLimit, BOUNDARY_WIDTH);
		pBoid.vForce += vToCenter * boundFunc(fDist - m_fBoundaryLimit); // between 0.0(outer) - 1.0(inner)
	}

	return;
}

// draws the BoidEngine by setting up the Shader
void BoidEngine::draw( bool m_bPause )
{
	ShaderManager* pShdrMngr = SHADER_MANAGER;

	if ( nullptr != m_pTexture )
		m_pTexture->bindTexture( ShaderManager::eShaderType::BOID_SHDR, "gSampler" );

	if( !m_bPause )
		update();

	m_pMesh->loadInstanceData(&m_vInstanceData);
	m_pMesh->drawMesh(pShdrMngr->getProgram( ShaderManager::eShaderType::BOID_SHDR ) );

	if ( nullptr != m_pTexture )
		m_pTexture->unbindTexture();

	glUseProgram(0);
	glBindVertexArray( 0 );
}
