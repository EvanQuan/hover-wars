#include "EmitterEngine.h"

#define DEFAULT_NUM_PARTICLES 100
#define DEFAULT_DURATION_IN_SECONDS 5.f

// Initialization of Static Singleton instance
EmitterEngine* EmitterEngine::m_pInstance = nullptr;

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

// Default Destructor for EmitterEngine, just clears all emitters for now.
EmitterEngine::~EmitterEngine()
{
	clearAllEmitters();
}

// Clears all Emitters currently being managed.
//	Since these are made with unique_ptrs, a simple vector.clear will clean up all memory.
void EmitterEngine::clearAllEmitters()
{
	m_pEmitters.clear();
}

// Function to update all Emitters in the Engine.
void EmitterEngine::update(duration<float> dMilliSecondsSinceLastUpdate)
{
	// Do this cast once here instead of every emitter.
	float fDelta = static_cast<float>(dMilliSecondsSinceLastUpdate.count());

	// Update all Emitters
	for (vector<unique_ptr<Emitter>>::iterator iter = m_pEmitters.begin();
		iter != m_pEmitters.end();
		++iter )
		(*iter)->update(fDelta);

	// Clean up any Emitters that are subject for deletion.
	m_pEmitters.erase(
		remove_if(
			m_pEmitters.begin(),
			m_pEmitters.end(),
			[](unique_ptr<Emitter> const & e) { return e->readyToDelete(); }
		),
		m_pEmitters.end());
}

// Draws all Emitters.
void EmitterEngine::renderEmitters()
{
	for (vector<unique_ptr<Emitter>>::iterator iter = m_pEmitters.begin();
		iter != m_pEmitters.end();
		++iter)
		(*iter)->draw();
}

// Spawns an emitter at a given location with a given normal and degree from that normal. That degree creates a spherical cone of space from which
//	a particle can spawn; surrounding the normal.
void EmitterEngine::generateEmitter(vec3 vPos, 
									vec3 vNormal, 
									float fAngleFromNormal,
									float fParticleDuration,
									unsigned int iNumParticles,
									bool bExplosion,
									float fRadius )
{
	unique_ptr<Emitter> pNewEmitter = make_unique<Emitter>(&vPos);
	pNewEmitter->initializeEmitter(iNumParticles, vNormal, fAngleFromNormal, fParticleDuration, fRadius, bExplosion);
	m_pEmitters.push_back(move(pNewEmitter));
}
