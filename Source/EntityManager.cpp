#include "EntityManager.h"
#include "Scene_Loader.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "StaticEntity.h"
#include "PlayerEntity.h"

#define INTERSECTION_EPSILON 1e-4	// Minimum intersect distance (so we don't intersect with ourselves)
#define MAX_REFLECTIONS	800

// Initialize Static Instance Variable
EntityManager* EntityManager::m_pInstance = nullptr;

EntityManager::EntityManager()
{
	// Initialize ID Pools
	m_iComponentIDPool = m_iEntityIDPool = 0;

	// Initialize the Edge Threshold to 0 and 360 degrees.
	m_fMinEdgeThreshold = 0.0f;
	m_fMaxEdgeThreshold = 360.0f;
	m_iHeight = START_HEIGHT;
	m_iWidth = START_WIDTH;
	m_bPause = false;
	m_pMshMngr = MESH_MANAGER;
	m_pTxtMngr = TEXTURE_MANAGER;

	//TODO: redesign Boid Engine: m_pBoidEngine = new BoidEngine();
}

// Gets the instance of the environment manager.
EntityManager* EntityManager::getInstance()
{
	if ( nullptr == m_pInstance )
		m_pInstance = new EntityManager();
	return m_pInstance;
}

// Destructor.
EntityManager::~EntityManager()
{
	purgeEnvironment();

	// Delete Boid Engine
	if (nullptr != m_pBoidEngine)
		delete m_pBoidEngine;

	// Delete Mesh Manager
	if (nullptr != m_pMshMngr)
		delete m_pMshMngr;
	
	// Delete Texture Manager
	if (nullptr != m_pTxtMngr)
		delete m_pTxtMngr;
}

// Clears Environment and loads a new environment from specified file.
void EntityManager::initializeEnvironment(string sFileName)
{
	Scene_Loader* pObjFctry = Scene_Loader::getInstance();

	purgeEnvironment();
	pObjFctry->loadFromFile(sFileName);
}

// Remove Object from List with given ID
//void EntityManager::killObject( long lID )
//{
//	unsigned int i = 0;
//
//	// Iterate to find Object
//	while ( i < m_pObjects.size() && nullptr != m_pObjects[i] && lID != m_pObjects[i]->ID() )
//		++i;
//
//	// Delete Object and remove it from list.
//	if ( i < m_pObjects.size() )
//	{
//		swap( m_pObjects[i], m_pObjects.back() );
//		delete m_pObjects.back();
//		m_pObjects.pop_back();
//	}
//}

// Outputs all the objects in the environment for debugging.
//void EntityManager::listEnvironment()
//{
//	cout << "Environment:" << endl;
//	for ( vector<Object3D*>::iterator pIter = m_pObjects.begin();
//		  pIter != m_pObjects.end();
//		  ++pIter )
//		cout << "\t" << (*pIter)->getDebugOutput() << endl;
//
//	for ( vector<Light*>::iterator pIter = m_pLights.begin();
//		  pIter != m_pLights.end();
//		  ++pIter )
//		cout << "\t" << (*pIter)->getDebugOutput() << endl;
//
//	cout << endl;
//}

// Clears out the entire environment
void EntityManager::purgeEnvironment()
{
	// Clear unique_ptrs of Components and Entities
	m_pMasterComponentList.clear();
	m_pMasterEntityList.clear();
}

// Fetch the Frenet Frame of the first MeshObject found (Hack for assignment)
//mat4 EntityManager::getFrenetFrame()
//{ 
//	mat4 pReturnVal = mat4( 1.0 );	// Default: return Identity Matrix
//
//	for ( vector<Object3D*>::iterator pObjIter = m_pObjects.begin();
//		 pObjIter != m_pObjects.end();
//		 ++pObjIter )
//	{
//		if ( !(*pObjIter)->getType().compare( "MeshObject" ) )
//		{
//			pReturnVal = (*pObjIter)->getFreNetFrames();
//			break;
//		}
//	}
//
//	// Return
//	return pReturnVal;
//}

void EntityManager::renderEnvironment( const vec3& vCamLookAt )
{
	// Local Variables
	ShaderManager* pShdrMngr = SHADER_MANAGER;
	vec3 pLightPosition, pLightColor;

	// Calculate information for each Light in the scene (Current max = 1)
	for (vector<LightingComponent*>::iterator pLightIter = m_pLights.begin();
		pLightIter != m_pLights.end();
		++pLightIter)
	{
		pLightPosition = (*pLightIter)->getPosition();
		pLightColor = (*pLightIter)->getColor();

		// Store Information to all Shaders
		pShdrMngr->setUniformVec3( ShaderManager::eShaderType::MESH_SHDR, "lightPosition",  &pLightPosition );
		pShdrMngr->setUniformVec3( ShaderManager::eShaderType::PLANE_SHDR, "lightPosition", &pLightPosition );
		pShdrMngr->setUniformVec3(ShaderManager::eShaderType::BOID_SHDR, "lightPosition_worldSpace", &pLightPosition );
		pShdrMngr->setUniformVec3(ShaderManager::eShaderType::PLANE_SHDR, "vLightColor", &pLightColor);

		//(*pLightIter)->draw( vCamLookAt );
		for (vector<RenderComponent*>::iterator pIter = m_pRenderingComponents.begin();
			pIter != m_pRenderingComponents.end();
			++pIter)
			(*pIter)->render();

		// Draw Boid Engine
		if( nullptr != m_pBoidEngine )
			m_pBoidEngine->draw( m_bPause );
	}
}

/*******************************************************************************\
* Boid Manipulation														   *
\*******************************************************************************/
void EntityManager::initializeBoidEngine(vector< string >& sData)
{
	if (nullptr != m_pBoidEngine)
		delete m_pBoidEngine;

	m_pBoidEngine = new BoidEngine(sData);
}

/*********************************************************************************\
* Entity Management                                                              *
\*********************************************************************************/

// Fetches the current position of Entity with ID: iEntityID
vec3 EntityManager::getEntityPosition(int iEntityID)
{
	// Return Value: Origin as a default if entity not found.
	vec3 vReturn = vec3(0.0f);

	// Iterate through all Entities.
	for (vector<unique_ptr<Entity>>::iterator iter = m_pMasterEntityList.begin();
		iter != m_pMasterEntityList.end();
		++iter)
	{
		// If Entity found, get the position and break from the loop
		if (iEntityID == (*iter)->getID())
		{
			vReturn = (*iter)->getPosition();
			break;
		}
	}

	// Return result.
	return vReturn;
}

// Generates a Camera Entity and stores it in the Master Entity List.
Camera* EntityManager::generateCameraEntity()
{
	unique_ptr<Camera> pNewCamera = make_unique<Camera>(getNewEntityID());
	Camera* pReturnCamera = pNewCamera.get();
	m_pMasterEntityList.push_back(move(pNewCamera));

	return pReturnCamera;
}

// Generates a Static Plane Entity into the world.
void EntityManager::generateStaticPlane(int iHeight, int iWidth, vec3 vPosition, vec3 vNormal, const string& sTextureLocation, const string& sShaderType)
{
	unique_ptr<StaticEntity> pNewPlane = make_unique<StaticEntity>(getNewEntityID(), vPosition);
	pNewPlane->loadAsPlane(vNormal, iHeight, iWidth, sTextureLocation, sShaderType);
	m_pMasterEntityList.push_back(move(pNewPlane));
}

// Generates a Static Plane Entity into the world.
void EntityManager::generateStaticSphere(float fRadius, vec3 vPosition, const string& sTextureLocation, const string& sShaderType)
{
	unique_ptr<StaticEntity> pNewSphere = make_unique<StaticEntity>(getNewEntityID(), vPosition);
	pNewSphere->loadAsSphere(fRadius, sTextureLocation, sShaderType);
	m_pMasterEntityList.push_back(move(pNewSphere));
}

// Generates a Static Mesh at a given location
void EntityManager::generateStaticMesh(const string& sMeshLocation, vec3 vPosition, const string& sTextureLocation, const string& sShaderType )
{
	unique_ptr<StaticEntity> pNewMesh = make_unique<StaticEntity>(getNewEntityID(), vPosition);
	pNewMesh->loadFromFile(sMeshLocation, sTextureLocation, sShaderType);
	m_pMasterEntityList.push_back(move(pNewMesh));
}

void EntityManager::generatePlayerEntity(vec3 vPosition, const string& sMeshLocation, const string& sTextureLocation, const string& sShaderType)
{
	unique_ptr<PlayerEntity> pNewPlayer = make_unique<PlayerEntity>(getNewEntityID(), vPosition);
	pNewPlayer->initializePlayer(sMeshLocation, sTextureLocation, sShaderType);
	m_pMasterEntityList.push_back(move(pNewPlayer));
}

// Generates a Static light at a given position. Position and Color are required, but default meshes and textures are available.
void EntityManager::generateStaticLight( vec3 vPosition, vec3 vColor, const string& sMeshLocation, const string& sTextureLocation)
{
	unique_ptr<Light> pNewLight = make_unique<Light>(getNewEntityID(), &vPosition);
	pNewLight->initialize(&vColor, &sTextureLocation, true, sMeshLocation);

	if (nullptr == m_pTestingLight)
		m_pTestingLight = pNewLight.get();

	m_pMasterEntityList.push_back(move(pNewLight));
}

// Goes through all Existing Camera Components and updates their aspect ratio.
void EntityManager::updateHxW(int iHeight, int iWidth)
{
	for (vector<CameraComponent*>::iterator iter = m_pCameraComponents.begin();
		iter != m_pCameraComponents.end();
		++iter)
	{
		(*iter)->updateHxW(iHeight, iWidth);
	}

	// Store new Height and Width in case another Camera Component is created.
	m_iHeight = iHeight;
	m_iWidth = iWidth;
}

// Main Update Function
// This function checks the timer and updates necessary components
//	in the game world. No rendering is done here.
void EntityManager::updateEnvironment(const Time& pTimer)
{
	// Get Total Frame Time and Benchmark for 60 fps
	duration<double> pFrameTime = pTimer.getFrameTime();
	constexpr auto pMaxDeltaTime = sixtieths_of_a_sec{ 1 };

	// Loop updates to maintain 60 fps
	while (pFrameTime > milliseconds(0))
	{
		// Get the Delta of this time step <= 1/60th of a second (60 fps)
		// Interpolate on steps < 1/60th of a second
		auto pDeltaTime = 
			std::min<common_type<decltype(pFrameTime),decltype(pMaxDeltaTime)>::type>(pFrameTime, pMaxDeltaTime);

		pFrameTime -= pDeltaTime;
		
		// UPDATES GO HERE
	}
}

/*********************************************************************************\
* Entity Component Management                                                    *
\*********************************************************************************/

// Generates a new Camera Component. Stores it in the Camera Component and Master component lists.
CameraComponent* EntityManager::generateCameraComponent( int iEntityID )
{
	// Generate new Camera Component
	unique_ptr<CameraComponent> pNewCameraPtr = make_unique<CameraComponent>(iEntityID, getNewComponentID(), m_iHeight, m_iWidth);

	// Store new Camera Component
	m_pCameraComponents.push_back(pNewCameraPtr.get());
	m_pMasterComponentList.push_back(move(pNewCameraPtr));

	// Set the active Camera if no camera is currently active.
	if (NULL == m_pActiveCamera)
		m_pActiveCamera = m_pCameraComponents.back();

	return m_pCameraComponents.back();
}

// Generates a new Render Component, stores it in the Rendering Components list and Master Components list.
//	Manages component with a unique pointer stored internally in the Master Components list.
RenderComponent* EntityManager::generateRenderComponent(int iEntityID, bool bStaticDraw, ShaderManager::eShaderType eType, GLenum eMode)
{
	// Generate new Render Component
	unique_ptr<RenderComponent> pNewRenderComponent = make_unique<RenderComponent>(iEntityID, getNewComponentID(), bStaticDraw, eType, eMode);
	RenderComponent* pReturnComponent = pNewRenderComponent.get();	// Return pointer
	m_pRenderingComponents.push_back(pReturnComponent);				// store in Rendering components list
	m_pMasterComponentList.push_back(move(pNewRenderComponent));	// move to Master Components list.

	// Return newly created component.
	return pReturnComponent;
}

// Generates a Lighting Component, stores the light in the Lights list as well as manages it in the Master Components list.
//	Creates a Barebone light component, up to the caller to initialize the light how they desire.
LightingComponent* EntityManager::generateLightingComponent(int iEntityID)
{
	// Generate new Lighting Component
	unique_ptr<LightingComponent> pNewComponent = make_unique<LightingComponent>(iEntityID, getNewComponentID());
	LightingComponent* pReturnComponent = pNewComponent.get();
	m_pLights.push_back(pReturnComponent);
	m_pMasterComponentList.push_back(move(pNewComponent));

	// Return newly created component
	return pReturnComponent;
}