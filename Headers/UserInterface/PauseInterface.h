#pragma once
#include "UserInterface/MenuInterface.h"


class PauseInterface final : public MenuInterface
{
public:

    static PauseInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    PauseInterface(int iWidth, int iHeight);
    PauseInterface(const PauseInterface* pCopy);                              // Default Copy Constructor
    PauseInterface& operator=(const PauseInterface* pCopy) {return (*this); } // Assignment Operator.

    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

private:
    static PauseInterface* m_pInstance;
};
