#include "EntityHeaders/DirectionalLight.h"
#include "EntityManager.h"


const float LIGHT_SIZE = 30.f;
const int LIGHT_HEIGHT = 1;
const int LIGHT_WIDTH = LIGHT_HEIGHT;
const int LIGHT_DEPTH = LIGHT_HEIGHT;

// Constructor
DirectionalLight::DirectionalLight(int iID) 
    : Entity( iID, vec3( -1.0 ), DIRECTIONAL_LIGHT_ENTITY )
{
    
}

// Destructor
DirectionalLight::~DirectionalLight()
{
    // Nothing to Destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void DirectionalLight::update(float fTimeInMilliseconds)
{
    /* Not Implemented */
}

// This function is not necessary as Directional Lights do not have a valid position in space.
void DirectionalLight::getSpatialDimensions(vec3* pNegativeOffset, vec3* pPositiveOffset) const
{
    /* Not Implemented */
}

// Initializes the Light Entity with a Color, possible texture, Static boolean and possible Mesh
//    If "" is provided for the Mesh name, a generic cube will be generated.
void DirectionalLight::initialize(const vec3* vDirection, const vec3* vAmbientColor, const vec3* vDiffuseColor, const vec3* vSpecularColor)
{
    // Create and Initialize the Lighting Component.
    m_pLightingComponent = ENTITY_MANAGER->generateLightingComponent(m_iID);
    m_pLightingComponent->initializeAsDirectionalLight(vDirection, vAmbientColor, vDiffuseColor, vSpecularColor);
}
