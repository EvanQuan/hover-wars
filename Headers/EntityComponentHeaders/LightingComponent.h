#pragma once
#include "stdafx.h"
#include "EntityComponent.h"

/************************************************************
* Name: LightingComponent
* Written by: James Cote
* Description: Component used for storing Lighting information.
*	Namely a position for the light and a type of light.
***************************************************************/

class LightingComponent 
	: public EntityComponent
{
public:
	LightingComponent(int iEntityID, int iComponentID);
	virtual ~LightingComponent();

	// Inherited update frunction from EntityComponent
	void update(double dTimeStep);

	// Initializes the proper buffers on the GPU for rendering.
	void initializeAsPointLight( const vec3* vPosition, const vec3* vColor );
	void initializeAsDirectionalLight(const vec3* vDirection, const vec3* vAmbient, const vec3* vDiffuse, const vec3* vSpecular);

	vector< vec4 > getLightInformation() const;

	// Updates the position of the light, necessary for moving the light with dynamic objects
	void updatePosition(const vec3* vPosition) { m_vPosition = *vPosition; }
	void updateColor(const vec3* vColor) { m_vDiffuseColor = *vColor; }

	// Getters for the Lights
	vec3 getPosition() { return m_vPosition; }
	vec3 getColor() { return m_vDiffuseColor; }

private: 
	// Private Copy Constructor and Assignment operator overload.
	LightingComponent(const LightingComponent* pCopy);
	LightingComponent& operator=(const LightingComponent& pRHS);

	// Private Variables
	vec3 m_vPosition, m_vDiffuseColor;

	// Directional Light Variables
	vec3 m_vDirection, m_vAmbientColor, m_vSpecularColor;

	enum eLightType
	{
		POINT_LIGHT = 0,
		DIRECTIONAL_LIGHT,
		SPOTLIGHT,
		MAX_TYPES
	};
	eLightType m_eType;
};
