#include "Mouse_Handler.h"

#define UPDATE_THRESHOLD .01f

// Static Singleton instance
Mouse_Handler* Mouse_Handler::m_pInstance = nullptr;

// Constructor
Mouse_Handler::Mouse_Handler(GLFWwindow *rWindow)
{
	m_pGpxMngr = GraphicsManager::getInstance( rWindow );
	m_bRotateFlag = m_bTranslateFlag = false;
}

// Fetch the Singleton instance
Mouse_Handler* Mouse_Handler::getInstance(GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
		m_pInstance = new Mouse_Handler(rWindow);

	return m_pInstance;
}

// Destructor
Mouse_Handler::~Mouse_Handler()
{
	m_pGpxMngr = nullptr;
}

// Updates the mouse posisions internally and applies a transform if necessary
void Mouse_Handler::updateMouse(float fX, float fY)
{
	if (m_bRotateFlag)
	{
		m_pGpxMngr->rotateCamera(m_pInitialPos - vec2(fX, fY));
	}

	// Set new current position
	m_pInitialPos.x = fX;
	m_pInitialPos.y = fY;
}

void Mouse_Handler::mouseZoom(float fZoomVal)
{
	m_pGpxMngr->zoomCamera(fZoomVal);
}
