#include "UserInterface.h"


// Singleton instance
UserInterface* UserInterface::m_pInstance = nullptr;

UserInterface::UserInterface(GLFWwindow* window)
{
	m_pWindow = window;
}

UserInterface* UserInterface::getInstance(GLFWwindow* window)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new UserInterface(window);
	}
	return m_pInstance;
}

UserInterface::~UserInterface()
{
	// Let go of Window
	m_pWindow = nullptr;
}

