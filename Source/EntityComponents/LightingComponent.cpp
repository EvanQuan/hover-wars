#include "EntityComponentHeaders/LightingComponent.h"

// Default Constructor:
//      Requires an EntityID for the Entity that the component is a part of
//      and a ComponentID issued by the EntityManager.
LightingComponent::LightingComponent(int iEntityID, int iComponentID)
    : EntityComponent( iEntityID, iComponentID )
{
    
}

// Destructor
LightingComponent::~LightingComponent()
{

}

// Overloaded Update Function
void LightingComponent::update(float fTimeDeltaInMilliseconds)
{

}

// Initializes this Lighting Component as a Simple Point Light with a position and a color
void LightingComponent::initializeAsPointLight(const vec3* vPosition, const vec3* vColor, float fPower)
{
    m_eType         = POINT_LIGHT;
    m_fLightPower   = fPower;
    m_vPosition     = *vPosition;
    m_vDiffuseColor = *vColor;

    // Store Light Data for passing to Shaders :> Assumes the Light is static and won't be moving.
    m_pLightData = { vec4(m_fLightPower), vec4(m_vPosition, 0.0), vec4(m_vDiffuseColor, 0.0) };
}

// Initializes this Lighting Component as a Directional Light with a Direction, Ambient Color, Diffuse Color and Specular Color
void LightingComponent::initializeAsDirectionalLight(const vec3* vDirection, const vec3* vAmbient, const vec3* vDiffuse, const vec3* vSpecular)
{
    m_eType                 = DIRECTIONAL_LIGHT;
    m_vDirection            = *vDirection;
    m_vDiffuseColor         = *vDiffuse;
    m_vAmbientColor         = *vAmbient;
    m_vSpecularColor        = *vSpecular;

    // Store Light Data for passing to Shaders :> Assumes the Light is static and won't be moving.
    m_pLightData = { vec4(m_vDirection, 0.0), vec4(m_vAmbientColor, 0.0), vec4(m_vDiffuseColor, 0.0), vec4(m_vSpecularColor, 0.0) };
}

// Initializes this Lighting Component as a SpotLight with a Position, Color, Direction and cutoff angle.
void LightingComponent::initializeAsSpotLight(const vec3* vPosition, const vec3* vColor, const vec3* vDirection, float fPhi, float fSoftPhi)
{
    float fPhiInRadians     = radians(fPhi);

    m_eType                 = SPOTLIGHT;
    m_vPosition             = *vPosition;
    m_vDiffuseColor         = *vColor;
    m_vDirection            = *vDirection;
    m_fPhi                  = cos(fPhiInRadians);    // Do one conversion of the cut off angle now instead of every fragment.
    m_fSoftPhi              = cos(radians(fPhi - fSoftPhi));
    m_fSpotLightRadius      = (sin(fPhiInRadians) / m_fPhi) * m_vPosition.y; // Calculates the Radius of the Spotlight on the xz-plane (y = 0)

    // Store Light Data for passing to Shaders :> Assumes the Light is static and won't be moving.
    m_pLightData = { vec4(m_fPhi, m_fSoftPhi, 0.0, 0.0), vec4(m_vPosition, 0.0), vec4(m_vDirection, 0.0), vec4(m_vDiffuseColor, 0.0) };
}

// Returns the Spatial Coverage of the light, only really applies to Spot Lights and Point Lights.
//  If pNegativeOffset = pPositiveOffset = vec3(0.0f), then the Dimensions could not be calculated.
void LightingComponent::getSpatialDimensions(vec3* pNegativeOffset, vec3* pPositiveOffset)
{
    // Ensure the return pointers are valid
    assert(nullptr != pNegativeOffset && nullptr != pPositiveOffset);

    // Local Variables
    float fHalfPower;

    // Compute the Spaces different based on Light type
    switch (m_eType)
    {
    case SPOTLIGHT:     // Refer to the radius that the spot light covers on the xz-plane (y = 0)
        *pNegativeOffset = vec3(-m_fSpotLightRadius, 0.0f, -m_fSpotLightRadius);
        *pPositiveOffset = vec3(m_fSpotLightRadius, 0.0f, m_fSpotLightRadius);
        break;
    case POINT_LIGHT:   // Refer to the Power of the light and create a cutoff based on a sphere of power for the light.
        fHalfPower = m_fLightPower * 0.5f;
        *pNegativeOffset = vec3(-fHalfPower);
        *pPositiveOffset = vec3(fHalfPower);
        break;
    default:    // Default to 0.0 to signify unusable dimensions.
        *pNegativeOffset = *pPositiveOffset = vec3(0.0f);
        break;
    }
}
