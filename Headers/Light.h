#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "Mesh.h"

// Infinitely small point that emits a light source.
//	Managed by the Environment Manager.
class Light :
	 public Entity
{
public:
	Light(int iID, const vec3* vPosition);
	~Light();

	// Function to Initialize the Light.
	void initialize(const vec3* vColor, const string* sMeshName, const string* sTexName, bool bStatic);

	// Light Manipulation
	void move( vec3 pMoveVec ) { m_vPosition += pMoveVec; }

private:
	// Private Light Copy constructor and assignment operator
	Light( const Light* newLight );
	Light& operator=(const Light& pCopy);

	// Private Variables
	vec3 m_pColor;
	Mesh* m_pMesh;
	RenderComponent* m_pRenderComponent;

};

