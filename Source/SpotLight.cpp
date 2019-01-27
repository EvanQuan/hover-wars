#include "SpotLight.h"
#include "EntityManager.h"


const float LIGHT_SIZE = 30.f;
const int LIGHT_HEIGHT = 1;
const int LIGHT_WIDTH = LIGHT_HEIGHT;
const int LIGHT_DEPTH = LIGHT_HEIGHT;

// Constructor
SpotLight::SpotLight(int iID, const vec3* vPosition) 
	: Entity( iID, *vPosition )
{
	
}

// Destructor
SpotLight::~SpotLight()
{
	// Nothing to Destruct
}

// Initializes the Light Entity with a Color, possible texture, Static boolean and possible Mesh
//	If "" is provided for the Mesh name, a generic cube will be generated.
void SpotLight::initialize(float fPhi, float fSoftPhi, bool bStatic, const vec3* vColor, const vec3* vDirection, const string& sMeshLocation, const Material* sMaterial)
{
	// Set the color of the Light
	m_pColor = (*vColor);

	// Load Mesh
	if ("" == sMeshLocation)
		m_pMesh = MESH_MANAGER->generateSphereMesh(bStatic, 0.25f, m_vPosition);
	else
		m_pMesh = MESH_MANAGER->loadMeshFromFile(sMeshLocation, m_vPosition, bStatic);

	// Create a Render Component
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, bStatic, ShaderManager::eShaderType::LIGHT_SHDR, GL_TRIANGLE_STRIP);

	// Initialize Render Component
	assert(m_pRenderComponent != nullptr);
	m_pRenderComponent->initializeComponent(m_pMesh, sMaterial);
	vec4 pPoweredColor = vec4( m_pColor, 1.0);
	m_pRenderComponent->generateDiffuseTexture(&pPoweredColor); // Generates a Texture for the light based on the light color.

	// Create and Initialize the Lighting Component.
	m_pLightingComponent = ENTITY_MANAGER->generateLightingComponent(m_iID);
	m_pLightingComponent->initializeAsSpotLight(&m_vPosition, &m_pColor, vDirection, fPhi, fSoftPhi);
}
