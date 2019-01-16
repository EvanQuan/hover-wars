#include "Light.h"
#include "EntityManager.h"
#include "MeshManager.h"

const float LIGHT_SIZE = 30.f;

// Constructor
Light::Light(int iID, const vec3* vPosition) 
	: Entity( iID, *vPosition )
{
	
}

// Destructor
Light::~Light()
{
	// Nothing to Destruct
}

void Light::initialize(const vec3* vColor, const string* sMeshName, const string* sTexName, bool bStatic)
{
	m_pColor = (*vColor);
	m_pMesh = MESH_MANAGER->loadMeshFromFile(*sMeshName);

}
