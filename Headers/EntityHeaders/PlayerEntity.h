#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include <queue>

#define MAX_CAMERAS_PER_PLAYER 2 

class PlayerEntity :
    public Entity
{
public:
    PlayerEntity(int iID, const vec3* vPosition);
    virtual ~PlayerEntity();

    // Implementation of inherited functionality
    void update(float fTimeInMilliseconds);

    void initializePlayer(const string& sFileName, 
                          const Material* pMaterial, 
                          const string& sShaderType,
                          float fScale);

    void useAbility(eAbility ability);

    // TEMPORARY: Returns the directional Angle for cobbled camera controls
    quat getRotation() { return m_pPhysicsComponent->getRotation(); }

    const CameraComponent* getActiveCameraComponent() { return m_pCmrComponents[activeCamera]; }

    // The player has a front and back camera, which can be toggled as the
    // active camera
    void toggleActiveCamera() { activeCamera = !activeCamera; }

    /*
    Return the position that the camera is to look at.

    This value should be close to the position of the player to ensure the camera
    looks at the player.
    */
    vec3 getCameraPosition() { return m_vCameraPosition; }

private:
    // Private Variables
    int activeCamera;
    Mesh* m_pMesh;
    RenderComponent* m_pRenderComponent;
    PhysicsComponent* m_pPhysicsComponent;
    CameraComponent* m_pCmrComponents[MAX_CAMERAS_PER_PLAYER];

    vec3 m_vCameraPosition;
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

