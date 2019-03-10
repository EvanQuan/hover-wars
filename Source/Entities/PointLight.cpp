#include "EntityHeaders/PointLight.h"
#include "EntityManager.h"


const float LIGHT_SIZE = 30.f;
const int LIGHT_HEIGHT = 1;
const int LIGHT_WIDTH = LIGHT_HEIGHT;
const int LIGHT_DEPTH = LIGHT_HEIGHT;

// Constructor
PointLight::PointLight(int iID, const vec3* vPosition) 
    : Entity( iID, *vPosition, ENTITY_POINT_LIGHT )
{
    
}

// Destructor
PointLight::~PointLight()
{
    // Nothing to Destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void PointLight::update(float fTimeInMilliseconds)
{
    /* Not Implemented */
}

// Fetch the Spatial dimensions from the Lighting component for the Spatial Data Map.
//  AKA: the data cells that this point light covers.
void PointLight::getSpatialDimensions(vec3* pNegativeOffset, vec3* pPositiveOffset) const
{
    m_pLightingComponent->getSpatialDimensions(pNegativeOffset, pPositiveOffset);
}


// Initializes the Light Entity with a Color, possible texture, Static boolean and possible Mesh
//    If "" is provided for the Mesh name, a generic cube will be generated.
void PointLight::initialize(float fPower, const vec3* vColor, bool bStatic, const ObjectInfo* pObjectProperties, const string& sMeshName, float m_fMeshScale)
{
    // Set the color of the Light
    m_pColor = (*vColor);

    // Load Mesh
    if ("" == sMeshName)
    {
        m_pMesh = MESH_MANAGER->generateCubeMesh(&m_iTransformationIndex, bStatic, LIGHT_HEIGHT, LIGHT_WIDTH, LIGHT_DEPTH, pObjectProperties);
    }
    else
    {
        m_pMesh = MESH_MANAGER->loadMeshFromFile(&m_iTransformationIndex, sMeshName, pObjectProperties, m_fMeshScale, bStatic);
    }

    // Create a Render Component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, bStatic, ShaderManager::eShaderType::LIGHT_SHDR, GL_TRIANGLES);

    // Create and Initialize the Lighting Component.
    m_pLightingComponent = ENTITY_MANAGER->generateLightingComponent(m_iID);
    m_pLightingComponent->initializeAsPointLight(&m_vPosition, &m_pColor, fPower);
}
