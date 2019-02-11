#pragma once
#include "stdafx.h"
#include "EntityComponent.h"

/************************************************************
* Name: CameraComponent
* Written by: James Cote
* Description: Provides functionality for manipulating and constructing
*    a camera that can be used by an entity for various purposes.
***************************************************************/
class CameraComponent :
    public EntityComponent
{
public:
    CameraComponent(int iEntityID, int iComponentID, int iHeight, int iWidth);
    virtual ~CameraComponent();

    // Updating Functions
    void updateHxW(int iHeight, int iWidth);
    void update(duration<float> fTimeDelta); // Overloading virtual update function from Entity Component

    // Set up Camera Matrices
    mat4 getToCameraMat() const;
    mat4 getPerspectiveMat() const;
    vec3 getCameraWorldPos() const;
    vec3 getLookAt() const;

    // Getters
    vec3 getRay(float fX, float fY) const;

    // Setters for modifying Camera
    void setLookAt(vec3 vLookAt) { m_vWorldLookAt = vLookAt; }
    void positionCamera(mat4 frame) { m_m4Frame = frame; }
    void setSphericalPos(vec3 vSPos) { m_vPos = vSPos; }
    void setCartesianPos(vec3 vCPos) { m_vPos = vCPos; }
    void setFOV_Y(float fFOVY) { m_fFOV_Y = fFOVY; }
    void setZRange(float fClose, float fFar) 
    { 
        m_fZClose = fClose;
        m_fZFar = fFar;
    }

private:
    CameraComponent(const CameraComponent* pCopy);
    CameraComponent& operator=(const CameraComponent* pCopy);
    vec3 m_vPos, m_vWorldLookAt;
    mat4 m_m4Frame;

    int m_iHeight, m_iWidth;
    float m_fAspectRatio;
    float m_fFOV_Y;
    float m_fZClose, m_fZFar;

    vec3 getCartesianPos() const;
};
