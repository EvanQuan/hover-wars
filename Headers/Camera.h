#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "EntityComponentHeaders/CameraComponent.h"

#define ZOOM_MAX			1000.f

// Camera Class
// Positions a Camera in the world at some spherical coordinates 
class Camera : 
	public Entity
{
public:
	Camera( int iID );			// Default Constructor
	virtual ~Camera();

	// Camera Manipulation Functions
	void orbit(vec2 pDelta);
	void zoom(float fDelta);
	void setLookAt(vec3 fLookAt) { m_vWorldLookAt = fLookAt; updateCameraComponent(); }
	void positionCamera(mat4 m4FreNetFrame) { m_pCmraCmp->positionCamera(m4FreNetFrame); }
	void setSteady(bool bSteady) { m_bSteadyCam = bSteady; }

private:
	CameraComponent* m_pCmraCmp;
	void updateCameraComponent();
	bool m_bSteadyCam; // Boolean to steady the camera so it cannot be adjusted by user input.

	vec3 m_vWorldLookAt;

	Camera& operator=(Camera* pCopy) {}		// Overloaded assignment operator; not to be used.
	Camera(Camera& pCopy);					// Overloaded Copy Constructor; not to be used.
};

