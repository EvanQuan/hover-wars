#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "InteractableEntity.h"
#include "SpatialDataMap.h"
#include <queue>

/***********\
 * Defines *
\***********/
#define MAX_CAMERAS_PER_PLAYER 2 
#define FRONT_CAMERA 0
#define BACK_CAMERA 1

// Fire Defines
#define FIRE_HEIGHT 2.0
#define FIRE_WIDTH 2.0

// Camera Defines
#define FRONT_CAMERA 0
#define BACK_CAMERA 1
#define PAST_CAMERA_POSITIONS 150
#define AVERAGE_MULTIPLIER (1.0f / static_cast<float>(PAST_CAMERA_POSITIONS))
/*

*/
#define FRONT_CAMERA_THETA -90.0f
#define FRONT_CAMERA_PHI 60.0f
#define FRONT_RADIUS 10.0f

// #define BACK_CAMERA_THETA 90.0f
// #define BACK_CAMERA_PHI 60.0f
#define BACK_CAMERA_THETA -90.0f
#define BACK_CAMERA_PHI 45.0f
#define BACK_RADIUS 15.0f

const vec3 FRONT_CAMERA_START_VIEW = vec3(FRONT_CAMERA_THETA, FRONT_CAMERA_PHI, FRONT_RADIUS); // (Theta, Phi, Radius)
const vec3 BACK_CAMERA_START_VIEW = vec3(BACK_CAMERA_THETA, BACK_CAMERA_PHI, BACK_RADIUS); // (Theta, Phi, Radius)
/*
The position of the camera relative to the position of the player. Both vectors
will be added together to form the final camera position.
*/
const vec3 FRONT_CAMERA_POSITION_OFFSET = vec3(-5, 0, 0);
const vec3 BACK_CAMERA_POSITION_OFFSET = vec3(-10, 0, 0);

class PlayerEntity :
    public Entity
{
public:
    PlayerEntity(int iID, const vec3* vPosition);
    virtual ~PlayerEntity();

    // Implementation of inherited functionality
    void update(float fTimeInMilliseconds);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    void initializePlayer(const string& sFileName,
                          const ObjectInfo* pObjectProperties,
                          const string& sShaderType,
                          float fScale,
                          ePlayer ePlayerNumber);

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
    SpatialDataMap* m_pSpatialMap;
    RenderComponent* m_pRenderComponent;
    PhysicsComponent* m_pPhysicsComponent;
    CameraComponent* m_pActiveCameraComponent;
    CameraComponent* m_pCmrComponents[MAX_CAMERAS_PER_PLAYER];
    InteractableEntity* m_pFireTrail;
    vec3 m_vPositionTotal;
    queue<vec3> m_vPastPositions;
    ePlayer m_ePlayerNumber;

    // Private Functions
    void updateCameraLookAts();

    // Abilities
    void shootRocket();
    void activateSpikes();
    void activateTrail();
    void dash(eAbility direction);
};

