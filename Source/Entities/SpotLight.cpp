#include "EntityHeaders/SpotLight.h"
#include "EntityManager.h"


const float LIGHT_SIZE = 30.f;
const int LIGHT_HEIGHT = 1;
const int LIGHT_WIDTH = LIGHT_HEIGHT;
const int LIGHT_DEPTH = LIGHT_HEIGHT;

// Constructor
SpotLight::SpotLight(int iID, const vec3* vPosition) 
    : Entity( iID, *vPosition, ENTITY_SPOT_LIGHT )
{
    
}

// Destructor
SpotLight::~SpotLight()
{
    // Nothing to Destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void SpotLight::update(float fTimeInSeconds)
{
    /* Not Implemented */
}

// Fetch the Spatial dimensions from the Lighting component for the Spatial Data Map.
//  AKA: the data cells that this spot light covers.
void SpotLight::getSpatialDimensions(vec3* pNegativeOffset, vec3* pPositiveOffset) const
{
    m_pLightingComponent->getSpatialDimensions(pNegativeOffset, pPositiveOffset);
}

// Initializes the Light Entity with a Color, possible texture, Static boolean and possible Mesh
//    If "" is provided for the Mesh name, a generic cube will be generated.
void SpotLight::initialize(float fPhi, float fSoftPhi, bool bStatic, const vec3* vColor, const vec3* vDirection, const string& sMeshLocation, const ObjectInfo* pObjectProperties, float m_fMeshScale)
{
    // Set the color of the Light
    m_pColor = (*vColor);

    // Load Mesh
    if ("" == sMeshLocation)
        m_pMesh = MESH_MANAGER->generateSphereMesh(&m_iTransformationIndex, bStatic, 0.25f, pObjectProperties);
    else
        m_pMesh = MESH_MANAGER->loadMeshFromFile(&m_iTransformationIndex, sMeshLocation, pObjectProperties, m_fMeshScale, bStatic);

    // Create a Render Component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, false, ShaderManager::eShaderType::LIGHT_SHDR, GL_TRIANGLE_STRIP);

    // Create and Initialize the Lighting Component.
    m_pLightingComponent = ENTITY_MANAGER->generateLightingComponent(m_iID);
    m_pLightingComponent->initializeAsSpotLight(&m_vPosition, &m_pColor, vDirection, fPhi, fSoftPhi);
}
