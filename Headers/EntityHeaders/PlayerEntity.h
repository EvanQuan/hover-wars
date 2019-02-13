#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include <queue>

#define CAMERA_PER_PLAYER_COUNT 2 

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

private:
    // Private Variables
    Mesh* m_pMesh;
    RenderComponent* m_pRenderComponent;
    PhysicsComponent* m_pPhysicsComponent;
    CameraComponent* m_pCmrComponents[CAMERA_PER_PLAYER_COUNT];
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

