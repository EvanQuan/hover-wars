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

	// Updates the position of the light, necessary for moving the light with dynamic objects
	void updatePosition(const vec3* vPosition) { m_vPosition = *vPosition; }
	void updateColor(const vec3* vColor) { m_vColor = *vColor; }

	// Getters for the Lights
	vec3 getPosition() { return m_vPosition; }
	vec3 getColor() { return m_vColor; }

private: 
	// Private Copy Constructor and Assignment operator overload.
	LightingComponent(const LightingComponent* pCopy);
	LightingComponent& operator=(const LightingComponent& pRHS);

	// Private Variables
	vec3 m_vPosition, m_vColor;
};
