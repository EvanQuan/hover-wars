#pragma once
#include "GraphicsManager.h"

class Mouse_Handler
{
public:
	static Mouse_Handler* getInstance(GLFWwindow *rWindow);
	~Mouse_Handler();

	void updateMouse(float fX, float fY);

	// Flag the Mouse for Rotations or Translations (Cannot do both at the same time).
	void mouseTStart() { m_bTranslateFlag = !m_bRotateFlag; }
	void mouseTEnd() { m_bTranslateFlag = false; }
	void mouseRStart() { m_bRotateFlag = !m_bTranslateFlag; };
	void mouseREnd() { m_bRotateFlag = false; }

	void mouseZoom(float fZoomVal);

private:
	// Singleton Implementation
	Mouse_Handler(GLFWwindow *rWindow);
	static Mouse_Handler* m_pInstance;

	bool m_bTranslateFlag;
	bool m_bRotateFlag;
	glm::vec2 m_pInitialPos;
	GraphicsManager* m_pGpxMngr;
};

