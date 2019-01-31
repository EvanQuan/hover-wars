#include "PointLight.h"
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

// Initializes the Light Entity with a Color, possible texture, Static boolean and possible Mesh
//	If "" is provided for the Mesh name, a generic cube will be generated.
void PointLight::initialize(float fPower, const vec3* vColor, bool bStatic, const Material* pMaterial, const string& sMeshName )
{
	// Set the color of the Light
	m_pColor = (*vColor);

	// Load Mesh
	if ("" == sMeshName)
	{
		m_pMesh = MESH_MANAGER->generateCubeMesh(bStatic, LIGHT_HEIGHT, LIGHT_WIDTH, LIGHT_DEPTH, m_vPosition);
	}
	else
	{
		m_pMesh = MESH_MANAGER->loadMeshFromFile(sMeshName, m_vPosition, bStatic);
	}

	// Create a Render Component
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, bStatic, ShaderManager::eShaderType::LIGHT_SHDR, GL_TRIANGLES);

	// Initialize Render Component
	assert(m_pRenderComponent != nullptr);
	m_pRenderComponent->initializeComponent(m_pMesh, pMaterial);
	vec4 pPoweredColor = vec4( m_pColor, 1.0 ) * fPower;
	m_pRenderComponent->generateDiffuseTexture(&pPoweredColor); // Generates a Texture for the light based on the light color.

	// Create and Initialize the Lighting Component.
	m_pLightingComponent = ENTITY_MANAGER->generateLightingComponent(m_iID);
	m_pLightingComponent->initializeAsPointLight(&m_vPosition, &m_pColor, fPower);
}
