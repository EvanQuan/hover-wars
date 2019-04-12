#pragma once
#include "UserInterface/MenuInterface.h"

#define START_GAME_OPTION "Start Game"
#define BACK_OPTION "Back"
#define PLAY_COUNT_OPTION "Player count"
#define BOT_COUNT_OPTION "Bot count"
#define AI_OPTION "AI"
#define GAME_TIME_OPTION "Game time"


class PregameInterface final : public MenuInterface
{
public:

    static PregameInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    PregameInterface();
    PregameInterface(const PregameInterface* pCopy);                              // Default Copy Constructor
    PregameInterface& operator=(const PregameInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

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
        AI,
        AI_TYPE,
        MAIN_MENU,
        START_GAME,
    };
    static PregameInterface* m_pInstance;

    void renderNumberOfPlayer(int i_NumOfPlayer);
    void renderNumberOfBot(int i_NumOfBot);
    void renderGameTime(int i_GameTime);
    void renderAI(eGameMode m_eAIType);
    void renderOption();
};
