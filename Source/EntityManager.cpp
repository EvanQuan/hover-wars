#include "EntityManager.h"
#include "Object_Factory.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "StaticEntity.h"

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

	m_pMasterComponentList.clear();
	m_pMasterEntityList.clear();
}

// Clears Environment and loads a new environment from specified file.
void EntityManager::initializeEnvironment(string sFileName)
{
	Object_Factory* pObjFctry = Object_Factory::getInstance();

	purgeEnvironment();
	pObjFctry->loadFromFile(sFileName);
}

// Adds object to back of List
void EntityManager::addObject( Object3D* pNewObject )
{
	m_pObjects.push_back( pNewObject );
}

// Adds a Light to back of List
void EntityManager::addLight( Light* pNewLight )
{
	m_pLights.push_back( pNewLight );
}

// Remove Object from List with given ID
void EntityManager::killObject( long lID )
{
	unsigned int i = 0;

	// Iterate to find Object
	while ( i < m_pObjects.size() && nullptr != m_pObjects[i] && lID != m_pObjects[i]->ID() )
		++i;

	// Delete Object and remove it from list.
	if ( i < m_pObjects.size() )
	{
		swap( m_pObjects[i], m_pObjects.back() );
		delete m_pObjects.back();
		m_pObjects.pop_back();
	}
}

// Remove Object from List with given ID
void EntityManager::killLight( long lID )
{
	unsigned int i = 0;

	// Iterate to find Object
	while ( i < m_pLights.size() && nullptr != m_pLights[i] && lID != m_pLights[i]->ID() )
		++i;

	// Delete Object and remove it from list.
	if ( i < m_pLights.size() )
	{
		swap( m_pLights[i], m_pLights.back() );
		delete m_pLights.back();
		m_pLights.pop_back();
	}
}

// Outputs all the objects in the environment for debugging.
void EntityManager::listEnvironment()
{
	cout << "Environment:" << endl;
	for ( vector<Object3D*>::iterator pIter = m_pObjects.begin();
		  pIter != m_pObjects.end();
		  ++pIter )
		cout << "\t" << (*pIter)->getDebugOutput() << endl;

	for ( vector<Light*>::iterator pIter = m_pLights.begin();
		  pIter != m_pLights.end();
		  ++pIter )
		cout << "\t" << (*pIter)->getDebugOutput() << endl;

	cout << endl;
}

// Clears out the entire environment
void EntityManager::purgeEnvironment()
{
	// Clean Up objects
	for ( vector<Object3D*>::iterator pIter = m_pObjects.begin();
		  pIter != m_pObjects.end();
		  ++pIter )
	{
		if ( nullptr != (*pIter) )
			delete (*pIter);
	}

	// Clean up Lights
	for ( vector<Light*>::iterator pIter = m_pLights.begin();
		  pIter != m_pLights.end();
		  ++pIter )
	{
		if ( nullptr != (*pIter) )
			delete (*pIter);
	}

	// Clear the array of Dangling pointers
	m_pObjects.clear();
	m_pLights.clear();
}

// Fetch the Frenet Frame of the first MeshObject found (Hack for assignment)
mat4 EntityManager::getFrenetFrame()
{ 
	mat4 pReturnVal = mat4( 1.0 );	// Default: return Identity Matrix

	for ( vector<Object3D*>::iterator pObjIter = m_pObjects.begin();
		 pObjIter != m_pObjects.end();
		 ++pObjIter )
	{
		if ( !(*pObjIter)->getType().compare( "MeshObject" ) )
		{
			pReturnVal = (*pObjIter)->getFreNetFrames();
			break;
		}
	}

	// Return
	return pReturnVal;
}

void EntityManager::renderEnvironment( const vec3& vCamLookAt )
{
	// Local Variables
	ShaderManager* pShdrMngr = SHADER_MANAGER;
	vec3 pLightPosition;

	// Calculate information for each Light in the scene (Current max = 1)
	for (vector<Light*>::iterator pLightIter = m_pLights.begin();
		pLightIter != m_pLights.end();
		++pLightIter)
	{
		pLightPosition = (*pLightIter)->getPosition();

		// Store Information to all Shaders
		pShdrMngr->setUniformVec3( ShaderManager::eShaderType::LIGHT_SHDR, "lightPosition", &pLightPosition );
		pShdrMngr->setUniformVec3( ShaderManager::eShaderType::MESH_SHDR, "lightPosition",  &pLightPosition );
		pShdrMngr->setUniformVec3( ShaderManager::eShaderType::PLANE_SHDR, "lightPosition", &pLightPosition );
		pShdrMngr->setUniformVec3(ShaderManager::eShaderType::BOID_SHDR, "lightPosition_worldSpace", &pLightPosition );

		//(*pLightIter)->draw( vCamLookAt );
		for ( vector<Object3D*>::iterator pIter = m_pObjects.begin();
			pIter != m_pObjects.end();
			++pIter )
		{
			if ( nullptr != (*pIter) )
				(*pIter)->draw( vCamLookAt, m_fMinEdgeThreshold, m_fMaxEdgeThreshold, m_bPause );
		}

		// Draw Boid Engine
		if( nullptr != m_pBoidEngine )
			m_pBoidEngine->draw( m_bPause );
	}

	m_pLights[0]->draw( vCamLookAt, m_fMinEdgeThreshold, m_fMaxEdgeThreshold, m_bPause );
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
* Texture Manipulation                                                           *
\*********************************************************************************/
void EntityManager::switchTexture( const string* sTexLocation, long lObjID )
{
	Object* pObj = getObject( lObjID );

	if ( nullptr != pObj )
		pObj->switchTexture( sTexLocation );
	else
		cout << "Unable to find Object with ID " << lObjID << " to switch texture to \""
			 << sTexLocation << "\".\n";
}

/*********************************************************************************\
 * Light Manipulation                                                            *
\*********************************************************************************/
void EntityManager::moveLight(vec3 pMoveVec)
{
	m_pLights[0]->move(pMoveVec);
}

/*********************************************************************************\
* Entity Management                                                              *
\*********************************************************************************/
// getObject
// Given a long integer ID of the Object, this will return the associated Light or 3D object
// associated with the ID.  Since this is a private function, only the EnvironmentManager
// can search in this manner and modify the object, preserving encapsulation.
Object* EntityManager::getObject( long lID )
{
	Object* pReturnObj = nullptr;
	unsigned int i = 0;

	// Iterate to find Object
	while ( i < m_pLights.size() && nullptr != m_pLights[ i ] && lID != m_pLights[ i ]->ID() )
		++i;

	// Return Light
	if ( i < m_pLights.size() )
		pReturnObj = m_pLights[ i ];
	else // Not a Light, see if it's a 3D object
	{
		i = 0;
		// Iterate to find Object
		while ( i < m_pObjects.size() && nullptr != m_pObjects[ i ] && lID != m_pObjects[ i ]->ID() )
			++i;

		// Return Object
		if ( i < m_pObjects.size() )
			pReturnObj = m_pObjects[ i ];
	}

	return pReturnObj;
}

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
	m_pMasterEntityList.push_back(move(pNewCamera));

	return pNewCamera.get();
}

// Generates a Static Plane Entity into the world.
void EntityManager::generateStaticPlane(int iHeight, int iWidth, vec3 vPosition, vec3 vNormal)
{
	unique_ptr<StaticEntity> pNewPlane = make_unique<StaticEntity>(getNewEntityID(), vPosition);
	pNewPlane.get()->loadAsPlane(vNormal, iHeight, iWidth);
	m_pMasterEntityList.push_back(move(pNewPlane));
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