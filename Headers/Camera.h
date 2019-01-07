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

private:
	CameraComponent* m_pCmraCmp;

	Camera& operator=(Camera* pCopy) {}		// Overloaded assignment operator; not to be used.
	Camera(Camera& pCopy);					// Overloaded Copy Constructor; not to be used.
};

