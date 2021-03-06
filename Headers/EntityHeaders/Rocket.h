#pragma once
#include "stdafx.h"
#include "InteractableEntity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"


// Forward Declarations
class EmitterEngine;

// Name: InteractableEntity
// Written by: James Cote
// Description: General Entity for objects that players can interact with.
//        These types of entities can be objects such as pickups or killzones (i.e. flame paths).
//        Idea: these entities, when a player collides with them, will send a message to that player 
//            telling them what happens as a result of this collision. Different messages can be generated
//            by inheriting from this class and defining their own message.
class Rocket
    : public InteractableEntity
{
public:

    // Unit: meters / second
    static int LAUNCH_SPEED;

    /*
        Length of the rocket hit box
        This value is currently determined by the size of the model of the rocket,
        so probably should not be manipulated. The radius of the rocket is
        currently defined in PhysicsManager.
    */
    static float BOUNDING_BOX;

    Rocket(int iID, int iOwnerID, const vec3* vColor);
    virtual ~Rocket();

    // Implementation of inherited functionality
    void initialize(const string& sFileName,
                    const ObjectInfo* pObjectProperties,
                    const string& sShaderType,
                    float fScale);
    void update(float fTimeInSeconds);
    void handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    // Rocket Functionality
    // Spawns a rocket of the specified parameters
    void launchRocket(const mat4* m4InitialTransform, const vec3* vVelocity, float fBBLength, bool includeSound);

    // Reflect a rocket. In practice this deletes the rockets, spawns a rocket
    // facing the opposite direction and of the specified reflected owner.
    void reflect(unsigned int iVictimMsg, HovercraftEntity *pOther);

    void explode(unsigned int iVictimMsg);

private:
    EmitterEngine*                          m_pEmitterEngine;
    vector<string>                          m_pReferenceList;
    unsigned int                            m_iRocketID;
    unsigned int getNewRocketID()           { return ++m_iRocketID; }
    vec3                                    m_vExplosionColor;

    void removeFromScene(unsigned int iVictimMsg, bool shouldExplode);

    // @Override
    eAbility getAbility() const { return eAbility::ABILITY_ROCKET; }
    // @Override
    void handleHovercraftCollision(HovercraftEntity *owner, HovercraftEntity *hit);
};
