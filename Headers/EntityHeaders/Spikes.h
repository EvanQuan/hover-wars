#pragma once
#include "stdafx.h"
#include "InteractableEntity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"

/***********\
 * DEFINES *
\***********/
#define NUM_SPIKES 4

/************************\
 * Forward Declarations *
\************************/
class SoundManager;

// Name: Spikes
// Written by: James Coté
// Description: An inherited type of InteractableEntity that manages positioning, animating and logic of
//      the spikes on a hovercraft.
class Spikes
    : public InteractableEntity
{
public:
    Spikes(int iID, int iOwnerID);
    virtual ~Spikes();

    // Public Spike functionality
    void initialize(const string& sFileName, const ObjectInfo* pObjectProperties, const string& sShaderType, float fScale);

    // Implementation of inherited functionality
    void update(float fTimeInSeconds);
    void updateWorldTransform(const mat4* pWorldTransform) { m_m4WorldTransform = *pWorldTransform; }
    void handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    // Spikes Functionality
    void animateSpikes();

private:
    SoundManager* m_pSoundMngr;
    mat4 m_m4WorldTransform;
    AnimationComponent* m_pSpikeAnimations[NUM_SPIKES];

    // @Override
    eAbility getAbility() const { return eAbility::ABILITY_ROCKET; }
};
