#include "CmdHandler.h"
#include "Scene_Loader.h"

/**********\
* DEFINES *
\**********/
#define INPUT_SIZE 128

CmdHandler* CmdHandler::m_pInstance = nullptr;

CmdHandler::CmdHandler(GLFWwindow *rWindow) : InputHandler()
{
	// Initializing Base Class
	m_pGPXMngr = GameManager::getInstance(rWindow);
	m_pEntMngr = ENTITY_MANAGER;

	bWireFrame = false;
}

// Get a copy of CmdHandler that doesn't have any initialized
//   Input associated with it.
CmdHandler* CmdHandler::getInstance(GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new CmdHandler(rWindow);
	}

	return m_pInstance;
}

// Get a copy of CmdHandler with an associated Input.
CmdHandler* CmdHandler::getInstance(const char* c_Input, int iInputSize, GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new CmdHandler(c_Input, iInputSize, rWindow);
	}
	else
	{
		m_pInstance->load_Input(c_Input, iInputSize);
	}

	return m_pInstance;
}

// Destructor
CmdHandler::~CmdHandler()
{
	m_pGPXMngr = nullptr;
	m_pEntMngr = nullptr;
}

void CmdHandler::handleKeyBoardInput(int cKey, int iAction, int iMods)
{
	vec3 pMoveVec(0.f, 0.f, 0.f);
	switch (cKey)
	{
	case (GLFW_KEY_W):			// Move Light Forward (along Z)
		pMoveVec.z += LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_S):			// Move Light Backward (along Z)
		pMoveVec.z -= LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_A):			// Move Light Left (along X)
		pMoveVec.x -= LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_D):			// Move Light Right (along X)
		pMoveVec.x += LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_SPACE):		// Move Light Up (along Y)
		pMoveVec.y += LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_X):			// Move Light Down (along Y)
		pMoveVec.y -= LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_F):			// Toggle Wireframe
		if ( GLFW_RELEASE == iAction )
		{
			bWireFrame = !bWireFrame;
			if ( bWireFrame )
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		break;
	case(GLFW_KEY_ENTER):		// Input Handling (TODO)
		break;
	case(GLFW_KEY_UP):
		break;
	case(GLFW_KEY_C):
		if( iAction == GLFW_RELEASE )
			m_pGPXMngr->switchView();
		break;
	case(GLFW_KEY_P):
		if ( iAction == GLFW_RELEASE )
			m_pEntMngr->pause();
		break;
	}
}
