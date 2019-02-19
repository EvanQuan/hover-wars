#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"

#define ZOOM_MAX            1000.f

// Camera Class
// Positions a Camera in the world at some spherical coordinates
class Camera :
    public Entity
{
public:
    Camera( int iID );              // Default Constructor
    Camera(const Camera& pCopy);    // Copy Constructor
    virtual ~Camera();

    // Implementation of inherited functionality
    void update(float fTimeInMilliseconds);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    // Camera Manipulation Functions
    void orbit(vec2 pDelta);
    void zoom(float fDelta);
    void setLookAt(vec3 fLookAt) { m_vWorldLookAt = fLookAt; updateCameraComponent(); }
    void positionCamera(mat4 m4FreNetFrame) { m_pCmraCmp->positionCamera(m4FreNetFrame); }
    void setSphericalPos(vec3 vPosition) { m_pCmraCmp->setSphericalPos(vPosition); }
    void setSteady(bool bSteady) { m_bSteadyCam = bSteady; }
    void setRotationQuat(quat pRotation) { m_pCmraCmp->setRotationQuat(pRotation); }

    // Get the Camera Component
    const CameraComponent* getCameraComponent() { return m_pCmraCmp; }

private:
    CameraComponent* m_pCmraCmp;
    PhysicsComponent* m_pPhysicsCmp;
    void updateCameraComponent();
    bool m_bSteadyCam; // Boolean to steady the camera so it cannot be adjusted by user input.

    vec3 m_vWorldLookAt;
};
