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

void Light::initialize(const vec3* vColor, const string* sTexName, bool bStatic, const string& sMeshName )
{
	m_pColor = (*vColor);

	if ("" == sMeshName)
		m_pMesh = MESH_MANAGER->generateCubeMesh(bStatic, LIGHT_HEIGHT, LIGHT_WIDTH, LIGHT_DEPTH, m_vPosition);
	else
		m_pMesh = MESH_MANAGER->loadMeshFromFile(sMeshName, m_vPosition, bStatic);

	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, bStatic, ShaderManager::eShaderType::LIGHT_SHDR, GL_TRIANGLES);

	assert(m_pRenderComponent != nullptr);
	m_pRenderComponent->initializeComponent(m_pMesh);

	SHADER_MANAGER->setUniformVec3(ShaderManager::eShaderType::LIGHT_SHDR, "vLightColor", &m_pColor);

	m_pLightingComponent = ENTITY_MANAGER->generateLightingComponent(m_iID);
	m_pLightingComponent->initializeAsPointLight(&m_vPosition, &m_pColor);
}
