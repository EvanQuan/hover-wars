#pragma once
#include "UserInterface/MenuInterface.h"


class PostgameInterface final : public MenuInterface
{
public:

    static PostgameInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    PostgameInterface(int iWidth, int iHeight);
    PostgameInterface(const PostgameInterface* pCopy);                              // Default Copy Constructor
    PostgameInterface& operator=(const PostgameInterface* pCopy) {return (*this); } // Assignment Operator.

    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

private:
    static PostgameInterface* m_pInstance;
};
