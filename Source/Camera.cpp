#include "Camera.h"
#include "EntityManager.h"

// Constructor
Camera::Camera( int iID )
	: Entity( iID, vec3(0.0f) )
{
	EntityManager* pEntMngr = EntityManager::getInstance();
	m_pCmraCmp = pEntMngr->generateCameraComponent( iID );
}

// Destructor
Camera::~Camera()
{
	m_pCmraCmp = NULL;
}





