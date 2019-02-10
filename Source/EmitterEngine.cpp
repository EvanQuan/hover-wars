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


EmitterEngine::~EmitterEngine()
{
	clearAllEmitters();
}

void EmitterEngine::clearAllEmitters()
{
	m_pEmitters.clear();
}

// Function to update all Emitters in the Engine.
void EmitterEngine::update(duration<float> dMilliSecondsSinceLastUpdate)
{
	// Update all Emitters
	for (vector<unique_ptr<Emitter>>::iterator iter = m_pEmitters.begin();
		iter != m_pEmitters.end();
		++iter )
		(*iter)->update(dMilliSecondsSinceLastUpdate);

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

void EmitterEngine::generateEmitter(vec3 vPos, 
									vec3 vNormal, 
									float fAngleFromNormal)
{
	unique_ptr<Emitter> pNewEmitter = make_unique<Emitter>(&vPos);
	pNewEmitter->initializeEmitter(DEFAULT_NUM_PARTICLES, vNormal, fAngleFromNormal, DEFAULT_DURATION_IN_SECONDS, 2.0f, true);
	m_pEmitters.push_back(move(pNewEmitter));
}
