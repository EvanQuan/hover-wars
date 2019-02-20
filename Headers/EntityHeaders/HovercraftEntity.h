#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "InteractableEntity.h"
#include "SpatialDataMap.h"
#include "GameStats.h"
#include <queue>

/***********\
 * Defines *
\***********/
#define MAX_CAMERAS_PER_PLAYER  2 

// Fire Defines
#define FIRE_HEIGHT             2.0
#define FIRE_WIDTH              2.0

// Camera Defines
#define FRONT_CAMERA            0
#define BACK_CAMERA             1
#define PAST_CAMERA_POSITIONS   3000 // 200
#define AVERAGE_POSITION_MULTIPLIER (1.0f / static_cast<float>(PAST_CAMERA_POSITIONS))
#define PAST_CAMERA_ROTATIONS   100
#define AVERAGE_ROTATION_MULTIPLIER (1.0f / static_cast<float>(PAST_CAMERA_ROTATIONS))

#define FRONT_CAMERA_LONGITUDE  -90.0f  // theta
#define FRONT_CAMERA_LATITUDE   60.0f   // phi
#define FRONT_RADIUS            10.0f   // r

#define BACK_CAMERA_LONGITUDE   -90.0f  // theta
#define BACK_CAMERA_LATITUDE    45.0f   // phi
#define BACK_RADIUS             15.0f   //  r

/*
Determines the rate at the which the camera moves to catch up to player
movement. The greater it is, the faster the camera moves (the less it lags
behind).
*/
#define CAMERA_MOVEMENT_MULTIPLIER 0.004f
/*
Determines the rate at which the camera rotates to catch up be behind the
player. The greater it is, the faster the camera rotates (the less it lags
behind).
*/
#define CAMERA_ROTATION_MULTIPLIER 0.001f

const vec3 FRONT_CAMERA_START_VIEW = vec3(FRONT_CAMERA_LONGITUDE, FRONT_CAMERA_LATITUDE, FRONT_RADIUS); // (Theta, Phi, Radius)
const vec3 BACK_CAMERA_START_VIEW = vec3(BACK_CAMERA_LONGITUDE, BACK_CAMERA_LATITUDE, BACK_RADIUS); // (Theta, Phi, Radius)
/*
The position of the camera relative to the position of the player. Both vectors
will be added together to form the final camera position.
*/
const vec3 FRONT_CAMERA_POSITION_OFFSET = vec3(-5, 0, 0);
const vec3 BACK_CAMERA_POSITION_OFFSET = vec3(-10, 0, 0);

class HovercraftEntity :
    public Entity
{
public:
    HovercraftEntity(int iID, const vec3* vPosition, eEntityTypes entityType);
    virtual ~HovercraftEntity();

    // Implementation of inherited functionality
    void update(float fTimeInMilliseconds);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    void initialize(const string& sFileName,
                    const ObjectInfo* pObjectProperties,
                    const string& sShaderType,
                    float fScale);

    void useAbility(eAbility ability);
    void move(float x, float y);
    void turn(float x);

    // TEMPORARY: Returns the directional Angle for cobbled camera controls
    vec3 getCameraPosition() { return m_vCurrentCameraPosition; }
    quat getCameraRotation() { return m_qCurrentCameraRotation; }

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
    quat m_qRotationTotal;
    queue<vec3> m_vPastPositions;
    queue<quat> m_qPastRotations;

    /*
    These should lag behind

    m_vPostion : the desired position

    m_pPhysicsComponent->getRotation() : the desired rotation
    */
    vec3 m_vCurrentCameraPosition;
    quat m_qCurrentCameraRotation;

    // Private Functions
    void initializeCameraLookAts();
    // m_vPositionTotal += position;
    void updateCameraLookAts(float fTimeInMilliseconds);
    void updateCameraPosition(float fTimeInMilliseconds);
    void updateCameraRotation(float fTimeInMilliseconds);

    // Abilities
    void shootRocket();
    void activateSpikes();
    void activateTrail();
    void dash(eAbility direction);
};

