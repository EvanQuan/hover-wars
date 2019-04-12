#pragma once
#include "UserInterface/MenuInterface.h"

#define START_GAME_OPTION "Start Game"
#define BACK_OPTION "Back"
#define PLAY_COUNT_OPTION "Player count"
#define BOT_COUNT_OPTION "Bot count"
#define AI_OPTION "AI"
#define GAME_TIME_OPTION "Game time"
#define GAMEMODE_OPTION "Game mode"
#define SCORELOSS_OPTION "Score loss"
#define MAP_OPTION "Map"
#define BOT_DIFFICULTY_OPTION "Bot difficulty"


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
        BOT_DIFFICULTY,
        BOT_DIFFICULTY_VALUE,
        GAME_TIME,
        GAME_TIME_NUMBER,
        GAMEMODE_LABEL,
        GAMEMODE_TYPE,
        SCORELOSS_LABEL,
        SCORELOSS_TYPE,
        MAP_LABEL,
        MAP_NUMBER,
        MAIN_MENU,
        START_GAME,
    };
    static PregameInterface* m_pInstance;

    void renderNumberOfPlayer(int i_NumOfPlayer);
    void renderNumberOfBot(int i_NumOfBot);
    void renderGameTime(int i_GameTime);
    void renderGameMode(eGameMode m_eGameMode);
    void renderScoreLoss(bool scoreLossEnabled);
    void renderMap(unsigned int mapNumber);
    void renderBotDifficulty(eBotDifficulty m_eBotDifficulty);
    void renderOption();
};
