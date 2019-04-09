#include "EmitterEngine.h"

// Initialization of Static Singleton instance
EmitterEngine* EmitterEngine::m_pInstance = nullptr;

// Singleton Implementation
EmitterEngine* EmitterEngine::getInstance()
{
    if (nullptr == m_pInstance)
        m_pInstance = new EmitterEngine();

    return m_pInstance;
}

// Default Constructor
EmitterEngine::EmitterEngine()
{

}

// Destructor
EmitterEngine::~EmitterEngine()
{
    clearAllEmitters();
}

// Clear all the Emitters for memory management
void EmitterEngine::clearAllEmitters()
{
    m_pEmitters.clear();
}

// Function to update all Emitters in the Engine.
void EmitterEngine::update(float fDelta)
{
    // Local Variables
    bool bCleanUp = false;

    // Update all Emitter
    for (vector<unique_ptr<Emitter>>::iterator iter = m_pEmitters.begin();
        iter != m_pEmitters.end();
        ++iter )
        bCleanUp |= (*iter)->update(fDelta);

    // Clean up any Emitters that are subject for deletion.
    if (bCleanUp)
    {
        m_pEmitters.erase(
            remove_if(
                m_pEmitters.begin(),
                m_pEmitters.end(),
                [](unique_ptr<Emitter> const & e) { return e->readyToDelete(); }
            ),
            m_pEmitters.end());
    }
}

// Draws all Emitters.
void EmitterEngine::renderEmitters()
{
    for (vector<unique_ptr<Emitter>>::iterator iter = m_pEmitters.begin();
        iter != m_pEmitters.end();
        ++iter)
        (*iter)->draw();
}

// Generates a new Emitter with the given parameters
void EmitterEngine::generateEmitter(const vec3& vPos,               // Position of Emitter
                                    const vec3& vNormal,            // Direction of Emission
                                    const vec3* vColor,             // Color of Emission
                                    float fAngleFromNormal,         // Spread of Emission
                                    float fParticleDuration,        // Duration of Particles
                                    unsigned int iNumParticles,     // Number of Particles to spawn
                                    bool bExplosion,                // Set as Explosion type or Fountain
                                    float fRadius)                  // Radius of Emission
{
    unique_ptr<Emitter> pNewEmitter = make_unique<Emitter>(&vPos);  // Generate new Emiter
    pNewEmitter->initializeEmitter(iNumParticles, &vNormal, vColor, fAngleFromNormal, fParticleDuration, fRadius, bExplosion);   // Initialize it
    m_pEmitters.push_back(move(pNewEmitter));                       // Store Emitter
}
