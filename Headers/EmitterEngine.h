#pragma once

#include "stdafx.h"
#include "Emitter.h"

/*******************************************************
 * Name: Emitter Engine
 * Description: Provides utility for spawning particle emitters.
 *              The Engine will manage the lifespan of the particle emitters
 *              and will unload them once they finish.
 * Written By: James Coté
\**********************************************************/
class EmitterEngine final
{
public:
    // Singleton Implementation
    static EmitterEngine* getInstance();
    virtual ~EmitterEngine();

    // Clean up
    void clearAllEmitters();

    // Update and Render Functionality
    void update(float fDelta);
    void renderEmitters();

    // Generate a new Emitter with the given parameters
    void generateEmitter(const vec3& vPos,
                         const vec3& vNormal,
                         const vec3* vColor,
                         float fAngleFromNormal,
                         float fParticleDuration,
                         unsigned int iNumParticles,
                         bool bExplosion,
                         float fRadius);

private:
    // Private Constructors
    EmitterEngine();
    EmitterEngine(const EmitterEngine* pCopy);
    EmitterEngine* operator=(const EmitterEngine* pCopy);
    static EmitterEngine* m_pInstance;    // Singleton Implementation

    // Emitters bank
    vector<unique_ptr<Emitter>> m_pEmitters;
};

