#include "UserInterface.h"


// Singleton instance
UserInterface* UserInterface::m_pInstance = nullptr;

UserInterface::UserInterface()
{
}

UserInterface* UserInterface::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new UserInterface();
	}
	return m_pInstance;
}

UserInterface::~UserInterface()
{
}

