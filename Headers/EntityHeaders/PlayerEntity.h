#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "InteractableEntity.h"
#include <queue>

#define MAX_CAMERAS_PER_PLAYER 2 
#define FRONT_CAMERA 0
#define BACK_CAMERA 1

class PlayerEntity :
    public Entity
{
public:
    PlayerEntity(int iID, const vec3* vPosition);
    virtual ~PlayerEntity();

    // Implementation of inherited functionality
    void update(float fTimeInMilliseconds);

    void initializePlayer(const string& sFileName,
                          const ObjectInfo* pObjectProperties,
                          const string& sShaderType,
                          float fScale);

    void useAbility(eAbility ability);
    void move(float x, float y);
    void turn(float x);

    // TEMPORARY: Returns the directional Angle for cobbled camera controls
    quat getRotation() { return m_pPhysicsComponent->getRotation(); }

    const CameraComponent* getActiveCameraComponent() { return m_pCmrComponents[activeCameraIndex]; }

    // The player has a front and back camera, which can be toggled as the
    // active camera
    void setActiveCameraToBack() { activeCameraIndex = BACK_CAMERA; }
    void setActiveCameraToFront() { activeCameraIndex = FRONT_CAMERA; }
    void toggleActiveCamera() { activeCameraIndex = !activeCameraIndex; }

private:
    // Private Variables
    int activeCameraIndex;
    Mesh* m_pMesh;
    RenderComponent* m_pRenderComponent;
    PhysicsComponent* m_pPhysicsComponent;
    CameraComponent* m_pActiveCameraComponent;
    CameraComponent* m_pCmrComponents[MAX_CAMERAS_PER_PLAYER];
    InteractableEntity* m_pFireTrail;
    vec3 m_vPositionTotal;
    queue<vec3> m_vPastPositions;

    // Private Functions
    void updateCameraLookAts();

    // Abilities
    void shootRocket();
    void activateSpikes();
    void activateTrail();
    void dash(eAbility direction);
};

