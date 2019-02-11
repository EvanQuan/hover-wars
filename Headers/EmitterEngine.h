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
class EmitterEngine
{
public:
    static EmitterEngine* getInstance();
    virtual ~EmitterEngine();
    void clearAllEmitters();

    void update(float fDelta);
    void renderEmitters();

    void generateEmitter(vec3 vPos,
                         vec3 vNormal,
                         float fAngleFromNormal,
                         float fParticleDuration,
                         unsigned int iNumParticles,
                         bool bExplosion,
                         float fRadius);

private:
    EmitterEngine();
    EmitterEngine(const EmitterEngine* pCopy);
    EmitterEngine* operator=(const EmitterEngine* pCopy);
    static EmitterEngine* m_pInstance;    // Singleton Implementation

    vector<unique_ptr<Emitter>> m_pEmitters;
};

