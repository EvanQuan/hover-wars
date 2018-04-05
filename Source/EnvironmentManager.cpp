#include "EnvironmentManager.h"
#include "Object_Factory.h"

#define INTERSECTION_EPSILON 1e-4	// Minimum intersect distance (so we don't intersect with ourselves)
#define MAX_REFLECTIONS	800

// Initialize Static Instance Variable
EnvironmentManager* EnvironmentManager::m_pInstance = nullptr;

EnvironmentManager::EnvironmentManager()
{
	// Initialize the Edge Threshold to 0 and 360 degrees.
	m_fMinEdgeThreshold = 0.0f;
	m_fMaxEdgeThreshold = 360.0f;
	m_bPause = false;
}

// Gets the instance of the environment manager.
EnvironmentManager* EnvironmentManager::getInstance()
{
	if ( nullptr == m_pInstance )
		m_pInstance = new EnvironmentManager();
	return m_pInstance;
}

// Destructor.
EnvironmentManager::~EnvironmentManager()
{
	purgeEnvironment();
}

// Clears Environment and loads a new environment from specified file.
void EnvironmentManager::initializeEnvironment(string sFileName)
{
	Object_Factory* pObjFctry = Object_Factory::getInstance();

	purgeEnvironment();
	pObjFctry->loadFromFile(sFileName);
}

// Adds object to back of List
void EnvironmentManager::addObject( Object3D* pNewObject )
{
	m_pObjects.push_back( pNewObject );
}

// Adds a Light to back of List
void EnvironmentManager::addLight( Light* pNewLight )
{
	m_pLights.push_back( pNewLight );
}

// Remove Object from List with given ID
void EnvironmentManager::killObject( long lID )
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
void EnvironmentManager::killLight( long lID )
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
void EnvironmentManager::listEnvironment()
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
void EnvironmentManager::purgeEnvironment()
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
mat4 EnvironmentManager::getFrenetFrame()
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

void EnvironmentManager::renderEnvironment( const vec3& vCamLookAt )
{
	// Local Variables
	ShaderManager* pShdrMngr = ShaderManager::getInstance();
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
	}

	m_pLights[0]->draw( vCamLookAt, m_fMinEdgeThreshold, m_fMaxEdgeThreshold, m_bPause );
}

/*********************************************************************************\
* Texture Manipulation                                                           *
\*********************************************************************************/
void EnvironmentManager::switchTexture( const string* sTexLocation, long lObjID )
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
void EnvironmentManager::moveLight(vec3 pMoveVec)
{
	m_pLights[0]->move(pMoveVec);
}

/*********************************************************************************\
* Object Management                                                              *
\*********************************************************************************/
// getObject
// Given a long integer ID of the Object, this will return the associated Light or 3D object
// associated with the ID.  Since this is a private function, only the EnvironmentManager
// can search in this manner and modify the object, preserving encapsulation.
Object* EnvironmentManager::getObject( long lID )
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