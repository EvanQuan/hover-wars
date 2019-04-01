#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "SpatialDataMap.h"
#include "GameStats.h"

/************************\
 * Forward Declarations *
\************************/
class FlameTrail;
class Rocket;
class Spikes;

/***********\
 * Defines *
\***********/
// Camera Defines
#define MAX_CAMERAS_PER_PLAYER  2 
#define FRONT_CAMERA            0
#define BACK_CAMERA             1


/*
The duration a powerup lasts for.
Used by GameInterface.
@TODO maybe move this to the powerup entity?

Unit : seconds
*/
#define POWERUP_TIME 15.0f


class HovercraftEntity :
    public Entity
{
public:

    HovercraftEntity(int iID, const vec3* vPosition);
    virtual ~HovercraftEntity();

    // Implementation of inherited functionality
    void update(float fTimeInSeconds);

    // Signifies to this HoverCraft that they were hit by a damaging attack.
    void getHitBy(eHovercraft attacker, eAbility ability);
    void handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg);

    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    void initialize(const string& sFileName,
        const ObjectInfo* pObjectProperties,
        const string& sShaderType,
        float fScale);

    void reinitialize();
    void setPosition(vec2 pos);
    bool useAbility(eAbility ability);
    void move(float x, float y);
    void moveGlobal(float x, float y) { m_pPhysicsComponent->moveGlobal(x, y); }
    void turn(float x);

    // TEMPORARY: Returns the directional Angle for cobbled camera controls
    vec3 getCameraPosition() const { return m_vCurrentCameraPosition; }
    quat getCameraRotation() const { return m_qCurrentCameraRotation; }

    const CameraComponent* getActiveCameraComponent() const { return m_pCmrComponents[activeCameraIndex]; }

    // The player has a front and back camera, which can be toggled as the
    // active camera
    void setActiveCameraToBack() { activeCameraIndex = BACK_CAMERA; }
    void setActiveCameraToFront() { activeCameraIndex = FRONT_CAMERA; }
    void toggleActiveCamera() { activeCameraIndex = !activeCameraIndex; }

    // Get ability statuses for UI
    // Get the status of the flame in percent.
    // @return 1.0f if full, 0.0f if empty, or intermediate value if in between
    float getTrailGaugePercent() const;

    // Get all the cooldowns to be used by the UI.
    // NOTE: why not send m_fCooldowns directly (make public)?
    // @return an array of all ability cooldowns.
    float* getCooldowns() { return m_fCooldowns; };

    // Set lose control until seconds runs out or manually reactivated with
    // setGainControl(), whichever happens first
    void setLoseControl(float seconds);
    // Gain control of hovercraft
    void setGainControl() { inControl = true; };

    bool isInControl() const { return inControl; }

    bool hasSpikesActivated() const { return m_bSpikesActivated; };

    // Signifies the hovercraft is vulernable to attack.
    // If true, ability collisions will count.
    // Otherwise, ignore ability collisions.
    bool isInvincible() const { return m_bInvincible; };

    void setInvincible();

    void enablePowerup(ePowerup powerup);
    void queuePowerup(ePowerup powerup);
    bool hasPowerup(ePowerup powerup) const { return m_vPowerupsTime[powerup] > 0; }

    // Units: m/s
    float getSpeed()                            { return glm::length(m_pPhysicsComponent->getLinearVelocity()); }
    vec3 getLinearVelocity()                    { return m_pPhysicsComponent->getLinearVelocity(); }
    PxTransform getGlobalTransform()            { return m_pPhysicsComponent->getGlobalPose(); }
    quat getRotation()                          { return m_pPhysicsComponent->getRotation(); }
    void getDirectionVector(vec3* vDirVector)   { m_pPhysicsComponent->getDirectionVector(vDirVector); }

    /*
        Reduces all cooldowns by a fixed factor, never dropping below the
        minimum cooldown values.
    */
    void reduceMaxCooldowns();
    /*
        Reset the maximum cooldowns to the base cooldowns.
    */
    void resetMaxCooldowns();

    bool canDash() const { return m_iDashCharges > 0; }

    int getDashCharges() const { return m_iDashCharges; }

    int getDashMaxCharges() const { return m_iDashMaxCharges; }

    float getDashMaxRecharge() const { return m_fDashMaxRecharge; }

    float getDashRecharge() const { return m_fDashRecharge; }

    bool hasMaxDashCharges() const { return m_iDashCharges == m_iDashMaxCharges; }

    // Should be initialized AFTER the hovercraft has been created when
    // GameStats corresponds entities to hovercrafts.
    // This prevents other classes from constantly needing to ask GameStats
    // for hovercraft information if the HovercraftEntity is given.
    void correspondToEHovercraft(eHovercraft hovercraft);

    bool isPlayer() const { return m_bIsPlayer; }
    bool isBot() const { return !m_bIsPlayer; }
    eHovercraft getEHovercraft() const { return m_eHovercraft; }

private:
    /*
        Queued actions are actions that cannot be applied immediately due to
        multithreading issues. Instead, they are queued and are acted upon
        during the next update() call. This is relevant for getHit, as that
        runs on a separate PhysX thread than the force applying thread that
        PhysX uses. So any movement changing effects that are in response to
        collisions needs to be queued.
    */
    enum eQueuedActions
    {
        QUEUED_SPEED_BOOST = 0,
        QUEUED_PUSH,
        QUEUED_COUNT
    };

    bool queuedActions[QUEUED_COUNT];
    float queuedX;
    float queuedY;

    void updateQueuedActions();

    // Private Variables
    int activeCameraIndex;
    Mesh* m_pMesh;
    SpatialDataMap* m_pSpatialMap;
    RenderComponent* m_pRenderComponent;
    CameraComponent* m_pActiveCameraComponent;
    CameraComponent* m_pCmrComponents[MAX_CAMERAS_PER_PLAYER];
    FlameTrail* m_pFireTrail;
    Rocket* m_pRocket;
    Spikes* m_pSpikes;
    PhysicsComponent* m_pPhysicsComponent;
    GameStats* m_pGameStats;

    /*
    These should lag behind

    m_vPostion : the desired position

    m_pPhysicsComponent->getRotation() : the desired rotation
    */
    vec3 m_vCurrentCameraPosition;
    quat m_qCurrentCameraRotation;

    // Private Functions
    void updateCameraLookAts(float fTimeInSeconds);
    void updateCameraPosition(float fTimeInSeconds);
    void updateCameraRotation(float fTimeInSeconds);
    void updateCooldowns(float fTimeInSeconds);
    void updateSpatialMap(vec3 &vNewPosition);
    void updatePhysicsComponent();

    // Abilities
    void shootRocket();
    void activateSpikes();
    void updateSpikes(float fTimeInSeconds);

    void activateTrail();
    void deactivateTrail();
    void updateTrail(float fTimeInSeconds);
    void createTrailInstance();
    float m_fTrailRechargeMultipler;

    void dash(eAbility direction);
    void updateDash(float fTimeInSeconds);

    // Like dashing, but weaker, no sound, and doese not count as an ability
    void push(float x, float y);
    void queuePush(float x, float y);

    // Cooldowns
    void initializeCooldowns();
    bool isOnCooldown(eAbility ability);
    // Current cooldown value
    float m_fCooldowns[COOLDOWN_COUNT];
    // Maximum cooldown value
    float m_fMaxCooldowns[COOLDOWN_COUNT];

    // Current dash charges
    int m_iDashCharges;
    // Max dash charges
    int m_iDashMaxCharges;
    // Maximum time it takes to gain a dash charge
    float m_fDashMaxRecharge;
    // Current dash recharge time
    float m_fDashRecharge;

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

    At maximum, this is TRAIL_GAUGE_FULL, at minimum, it is TRAIL_GAUGE_EMPTY.
    */
    float m_fTrailGauge;
    /*
    To ensure that the flames are generated at constant intervals, we need
    to track the time since the last flame was generated.
    */
    float m_fSecondsSinceLastFlame;
    /*
    To keep the distance between flame trail flames consistent at different
    velocities we must track the position of the last flame so we know when to
    place the next one.
    */
    vec3 m_vPositionOfLastFlame;
    // float m_fMinimumDistanceBetweenFlames;

    /*
    If true, car is able to receive and act upon movement input.
    Else, not movement input is processed.
    */
    void updateInControl(float fTimeInSeconds);
    bool inControl;
    float outOfControlTime;
    bool lowEnoughToMove;

    void updateVulnerability(float fTimeInSeconds);
    bool m_bInvincible;
    float m_fSecondsLeftUntilVulnerable;


    // Bool Spikes Information
    bool m_bSpikesActivated;
    float m_fSecondsSinceSpikesActivated;

    // Tracks enabled powerups for this hovercraft
    void initializePowerups();
    void updatePowerups(float fTimeInSeconds);
    void disablePowerup(ePowerup powerup);
    float m_vPowerupsTime[POWERUP_COUNT];
    bool m_vPowerupsEnabled[POWERUP_COUNT];

    // Used to determine if the hovercraft is a bot or player
    eHovercraft m_eHovercraft;
    bool m_bIsPlayer;
};

