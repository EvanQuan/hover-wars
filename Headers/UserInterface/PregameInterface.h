#pragma once
#include "UserInterface/MenuInterface.h"

#define START_GAME_OPTION "Start Game"
#define BACK_OPTION "Back"
#define PLAY_COUNT_OPTION "Player count"
#define BOT_COUNT_OPTION "Bot count"
#define GAME_TIME_OPTION "Game time"


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
    enum eUIComponent
    {
        BACKGROUND = 0,
        PLAYER_COUNT,
        PLAYER_COUNT_NUMBER,
        BOT_COUNT,
        BOT_COUNT_NUMBER,
        GAME_TIME,
        GAME_TIME_NUMBER,
        MAIN_MENU,
        START_GAME,
    };
    static PregameInterface* m_pInstance;

    void renderNumberOfPlayer(int i_NumOfPlayer);
    void renderNumberOfBot(int i_NumOfBot);
    void renderGameTime(int i_GameTime);
    void renderOption();
};
