#pragma once
#include "stdafx.h"
#include "EntityComponent.h"

/************************************************************
* Name: LightingComponent
* Written by: James Cote
* Description: Component used for storing Lighting information.
*    Namely a position for the light and a type of light.
*    Additional lighting information is added depending on the light
*    type specified by the enum eLightType
***************************************************************/

class Texture;  // Forward Declaration to avoid include loop

class LightingComponent 
    : public EntityComponent
{
public:

    enum eLightType
    {
        POINT_LIGHT = 0,
        DIRECTIONAL_LIGHT,
        SPOTLIGHT,
        MAX_TYPES
    };

    LightingComponent(int iEntityID, int iComponentID);
    virtual ~LightingComponent();

    // Inherited update frunction from EntityComponent
    void update(float fTimeDeltaInMilliseconds);

    // Initializes the proper buffers on the GPU for rendering.
    void initializeAsPointLight( const vec3* vPosition, const vec3* vColor, float fPower );
    void initializeAsDirectionalLight(const vec3* vDirection, const vec3* vAmbient, const vec3* vDiffuse, const vec3* vSpecular);
    void initializeAsSpotLight(const vec3* vPosition, const vec3* vColor, const vec3* vDirection, float fPhi, float fSoftPhi);

    // Shadow Map functionality
    void setupShadowFBO() const;
    void setupPMVMatrices();
    void setupShadowUniforms( unsigned int iSpotLightIndex = 0) const;

    // Returns light information for storing in Shader
    const vector< vec4 >* getLightInformation() const { return &m_pLightData; }

    // Updates the position of the light, necessary for moving the light with dynamic objects
    void updatePosition(const vec3* vPosition) { m_vPosition = *vPosition; }
    void updateColor(const vec3* vColor) { m_vDiffuseColor = *vColor; }

    // Getters for the Lights
    vec3 getPosition() { return m_vPosition; }
    vec3 getColor() { return m_vDiffuseColor; }
    eLightType getType() { return m_eType; }
    void getSpatialDimensions(vec3* pNegativeOffset, vec3* pPositiveOffset);

private: 
    // Private Copy Constructor and Assignment operator overload.
    LightingComponent(const LightingComponent* pCopy);
    LightingComponent& operator=(const LightingComponent& pRHS);

    // Private Variables
    vec3 m_vPosition, m_vDiffuseColor;
    mat4 m_m4LightSpaceMatrix;
    float m_fLightPower, m_fPhi, m_fSoftPhi;
    float m_fSpotLightRadius;

    // Directional Light Variables
    vec3 m_vDirection, m_vAmbientColor, m_vSpecularColor;

    // Data Storage for Shader
    vector< vec4 > m_pLightData;
    
    eLightType m_eType;

    // Shadow Map Variables
    Texture* m_pShadowMap;
    GLuint m_iFrameBuffer;
};
