#pragma once
#include "UserInterface/MenuInterface.h"


class PregameInterface final : public MenuInterface
{
public:

    static PregameInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    PregameInterface(int iWidth, int iHeight);
    PregameInterface(const PregameInterface* pCopy);                              // Default Copy Constructor
    PregameInterface& operator=(const PregameInterface* pCopy) {return (*this); } // Assignment Operator.

    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

private:
    static PregameInterface* m_pInstance;
};
