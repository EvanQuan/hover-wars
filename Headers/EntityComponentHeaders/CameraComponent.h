#pragma once
#include "stdafx.h"
#include "EntityComponent.h"

/************************************************************
* Name: CameraComponent
* Written by: James Cote
* Description: Provides functionality for manipulating and constructing
*	a camera that can be used by an entity for various purposes.
***************************************************************/
class CameraComponent :
	public EntityComponent
{
public:
	CameraComponent(int iEntityID, int iComponentID);
	virtual ~CameraComponent();

	// Updating Functions
	void updateHxW(int iHeight, int iWidth);
	void update() { m_bUpdated = true; }

	// Set up Camera Matrices
	mat4 getToCameraMat();
	mat4 getPerspectiveMat();
	vec3 getCameraWorldPos();
	const vec3 getLookAt();
	void setLookAt(vec3 vLookAt) { m_vWorldLookAt = vLookAt; }
	void positionCamera(mat4 frame) { m_m4Frame = frame; }
	void setSteady(bool bSteady) { m_bSteady = bSteady; }

	// Camera Manipulation Functions
	void orbit(vec2 pDelta);
	void zoom(float fDelta);
	void pan(vec3 pDirection);

private:
	CameraComponent(const CameraComponent* pCopy);
	CameraComponent& operator=(const CameraComponent* pCopy);
	vec3 m_vPos, m_vWorldLookAt;
	mat4 m_m4Frame;
	bool m_bUpdated;
	bool m_bSteady;
	int m_iHeight, m_iWidth;
	float m_fAspectRatio;

	vec3 getCartesianPos();
};
