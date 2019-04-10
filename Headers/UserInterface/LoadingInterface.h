#pragma once
#include "UserInterface/MenuInterface.h"

class LoadingInterface final : public MenuInterface
{
public:

    static LoadingInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    LoadingInterface();
    LoadingInterface(const LoadingInterface* pCopy);                              // Default Copy Constructor
    LoadingInterface& operator=(const LoadingInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:

    static LoadingInterface* m_pInstance;
};
