#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/LightingComponent.h"
#include "EntityComponentHeaders/RenderComponent.h"

// Infinitely small point that emits a light source.
//	Managed by the Environment Manager.
class Light :
	 public Entity
{
public:
	Light(int iID, const vec3* vPosition);
	virtual ~Light();

	// Function to Initialize the Light.
	void initialize(const vec3* vColor, const string* sTexName, bool bStatic, const string& sMeshName = "" );

	// Light Manipulation
	void move(vec3 pMoveVec) { m_vPosition += pMoveVec; m_pLightingComponent->updatePosition(&m_vPosition); }

private:
	// Private Light Copy constructor and assignment operator
	Light( const Light* newLight );
	Light& operator=(const Light& pCopy);

	// Private Variables
	vec3 m_pColor;
	Mesh* m_pMesh;
	RenderComponent* m_pRenderComponent;
	LightingComponent* m_pLightingComponent;

};

