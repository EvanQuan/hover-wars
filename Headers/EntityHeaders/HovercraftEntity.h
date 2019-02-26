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
// Camera Defines
#define MAX_CAMERAS_PER_PLAYER  2 
#define FRONT_CAMERA            0
#define BACK_CAMERA             1

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

    bool useAbility(eAbility ability);
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

    /*
    These should lag behind

    m_vPostion : the desired position

    m_pPhysicsComponent->getRotation() : the desired rotation
    */
    vec3 m_vCurrentCameraPosition;
    quat m_qCurrentCameraRotation;

    // Private Functions
    void updateCameraLookAts(float fSecondsSinceLastUpdate);
    void updateCameraPosition(float fSecondsSinceLastUpdate);
    void updateCameraRotation(float fSecondsSinceLastUpdate);
    void updateCooldowns(float fSecondsSinceLastUpdate);

    // Abilities
    void shootRocket();
    void activateSpikes();

    void activateTrail();
    void deactivateTrail();
    void updateTrail(float fSecondsSinceLastUpdate);
    void createTrailInstance();

    void dash(eAbility direction);

    // Cooldowns
    void initializeCooldowns();
    bool isOnCooldown(eAbility ability);
    float m_fCooldowns[COOLDOWN_COUNT];

    /*
    Tracks the state of the flame trail

    true: trail is activated and fuel gauge is draining.
    false: trail is deactivated and fuel gauge is recharging.

    Even if there is no fuel available, the trail can still be activated.
    The trail will simply not produce any flames.
    This activation state is based on player input.
    */
    bool m_bTrailActivated;
    float m_fSecondsSinceTrailDeactivated;
    bool m_bTrailReadyToRecharge;
    /*
    Stores the flame trail fuel gauge values.
    Since the gauge drain and recharge rates are in terms of time, so
    is the gauge itself.
    */
    float m_fTrailGauge;
    /*
    To ensure that the flames are generated at constant intervals, we need
    to track the time since the last flame was generated.
    */
    float m_fSecondsSinceLastFlame;
};

