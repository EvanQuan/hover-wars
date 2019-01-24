#include "EntityComponentHeaders/LightingComponent.h"

// Default Constructor:
//		Requires an EntityID for the Entity that the component is a part of
//			and a ComponentID issued by the EntityManager.
LightingComponent::LightingComponent(int iEntityID, int iComponentID)
	: EntityComponent( iEntityID, iComponentID )
{
	
}

// Destructor
LightingComponent::~LightingComponent()
{

}

// Overloaded Update Function
void LightingComponent::update(double dTimeDelta)
{

}

vector< vec4 > LightingComponent::getLightInformation() const 
{
	// Local Variables
	vector< vec4 > m_pReturnVector;

	// Populate the Return Vector with relavent data based on Light Type.
	switch (m_eType)
	{
	case POINT_LIGHT:
		m_pReturnVector.push_back(vec4( m_vPosition, 0.0 ));
		m_pReturnVector.push_back(vec4( m_vDiffuseColor, 0.0 ));
		break;
	case DIRECTIONAL_LIGHT:
		m_pReturnVector.push_back(vec4( m_vDirection, 0.0 ));
		m_pReturnVector.push_back(vec4( m_vAmbientColor, 0.0));
		m_pReturnVector.push_back(vec4( m_vDiffuseColor, 0.0));
		m_pReturnVector.push_back(vec4( m_vSpecularColor, 0.0 ));
		break;
	default:
		break;
	}

	// Return Result
	return m_pReturnVector;
}

// Initializes this Lighting Component as a Simple Point Light with a position and a color
void LightingComponent::initializeAsPointLight(const vec3* vPosition, const vec3* vColor)
{
	m_eType				= POINT_LIGHT;
	m_vPosition			= *vPosition;
	m_vDiffuseColor		= *vColor;
}

// Initializes this Lighting Component as a Directional Light with a Direction, Ambient Color, Diffuse Color and Specular Color
void LightingComponent::initializeAsDirectionalLight(const vec3* vDirection, const vec3* vAmbient, const vec3* vDiffuse, const vec3* vSpecular)
{
	m_eType				= DIRECTIONAL_LIGHT;
	m_vDirection		= *vDirection;
	m_vDiffuseColor		= *vDiffuse;
	m_vAmbientColor		= *vAmbient;
	m_vSpecularColor	= *vSpecular;
}