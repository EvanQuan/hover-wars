#include "Light.h"
#include "EntityManager.h"


const float LIGHT_SIZE = 30.f;
const int LIGHT_HEIGHT = 1;
const int LIGHT_WIDTH = LIGHT_HEIGHT;
const int LIGHT_DEPTH = LIGHT_HEIGHT;

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

// Initializes the Light Entity with a Color, possible texture, Static boolean and possible Mesh
//	If "" is provided for the Mesh name, a generic cube will be generated.
void Light::initialize(const vec3* vColor, const string* sTexName, bool bStatic, const string& sMeshName )
{
	// Set the color of the Light
	m_pColor = (*vColor);

	// Load Mesh
	if ("" == sMeshName)
		m_pMesh = MESH_MANAGER->generateCubeMesh(bStatic, LIGHT_HEIGHT, LIGHT_WIDTH, LIGHT_DEPTH, m_vPosition);
	else
		m_pMesh = MESH_MANAGER->loadMeshFromFile(sMeshName, m_vPosition, bStatic);

	// Create a Render Component
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, bStatic, ShaderManager::eShaderType::LIGHT_SHDR, GL_TRIANGLES);

	// Initialize Render Component
	assert(m_pRenderComponent != nullptr);
	m_pRenderComponent->initializeComponent(m_pMesh);

	// Set the Light Color for this light in the Light Shader
	//	TODO: This will need to be set another way if multiple lights with different colors are created.
	SHADER_MANAGER->setUniformVec3(ShaderManager::eShaderType::LIGHT_SHDR, "vLightColor", &m_pColor);

	// Create and Initialize the Lighting Component.
	m_pLightingComponent = ENTITY_MANAGER->generateLightingComponent(m_iID);
	m_pLightingComponent->initializeAsPointLight(&m_vPosition, &m_pColor);
}
