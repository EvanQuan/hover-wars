#include "EntityHeaders/PointLight.h"
#include "EntityManager.h"


const float LIGHT_SIZE = 30.f;
const int LIGHT_HEIGHT = 1;
const int LIGHT_WIDTH = LIGHT_HEIGHT;
const int LIGHT_DEPTH = LIGHT_HEIGHT;

// Constructor
PointLight::PointLight(int iID, const vec3* vPosition) 
	: Entity( iID, *vPosition )
{
	
}

// Destructor
PointLight::~PointLight()
{
	// Nothing to Destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions								*
\****************************************************************/

void PointLight::update(float fTimeInMilliseconds)
{
	/* Not Implemented */
}

// Initializes the Light Entity with a Color, possible texture, Static boolean and possible Mesh
//	If "" is provided for the Mesh name, a generic cube will be generated.
void PointLight::initialize(float fPower, const vec3* vColor, bool bStatic, const Material* pMaterial, const string& sMeshName, float m_fMeshScale)
{
	// Set the color of the Light
	m_pColor = (*vColor);

	// Load Mesh
	if ("" == sMeshName)
	{
		m_pMesh = MESH_MANAGER->generateCubeMesh(bStatic, LIGHT_HEIGHT, LIGHT_WIDTH, LIGHT_DEPTH, pMaterial, m_vPosition);
	}
	else
	{
		m_pMesh = MESH_MANAGER->loadMeshFromFile(sMeshName, pMaterial, m_fMeshScale, m_vPosition, bStatic);
	}

	// Create a Render Component
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, bStatic, ShaderManager::eShaderType::LIGHT_SHDR, GL_TRIANGLES);

	// Create and Initialize the Lighting Component.
	m_pLightingComponent = ENTITY_MANAGER->generateLightingComponent(m_iID);
	m_pLightingComponent->initializeAsPointLight(&m_vPosition, &m_pColor, fPower);
}
