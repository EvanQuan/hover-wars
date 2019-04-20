#pragma once
#include "stdafx.h"
#include "UserInterface/UserInterface.h"

class GameInterface;
class PromptInterface;


/***********\
 * Defines *
\***********/

/*
    The root directory for all UI textures.
    Any UI textures to be loaded should be placed in here.
*/
#define IMAGE_DIRECTORY "textures/menu/"
#define TRON_DIRECTORY "textures/menu/tron/"
#define OUTRUN_DIRECTORY "textures/menu/outrun/"

// font website: https://fontmeme.com/tron-font/
// size 30
// effect 'Style-Neon'
// color 00FCFF and FF00E1

// Forward Declaration
class ShaderManager;
class Texture;

/*
    Handles all UserInterface instances. Tracks the current interface.
    Loads and stores all the textures for all UIs.

    @author Evan Quan
*/
class UserInterfaceManager final
{
public:

    static UserInterfaceManager* getInstance();
    ~UserInterfaceManager();

    /*
        Themes determine the menu appearance.
    */
    enum eTheme
    {
        /*
            80s retro-future
        */
        THEME_OUTRUN,
        /*
            Tron-style future
        */
        THEME_TRON,
    };


    /*
        As UserInterface instances are generated, they are each added to
        the m_vInterfaceInstances list.
        At the end of the program, all interfaces are deleted.
    */
    void addInterface(UserInterface* prompt) { m_vInterfaceInstances.push_back(prompt); }
    void setCurrentInterface(PromptInterface* prompt) { m_pPrompInterface = prompt; }
    PromptInterface* getCurrentInterface() const { return m_pPrompInterface; }
    /*
        Set the current theme to be used by all UserInterfaces.

        @param[in] theme    to set
    */
    virtual void setTheme(eTheme theme) final;

    /*
        Get the current theme in use.

        @return the currently used theme
    */
    virtual eTheme getTheme() const final { return m_eCurrentTheme; }

    /*
        Get the texture o the specified image. The returned texture is
        determined by the current theme.

        @return the texture of the specified image
    */
    Texture* getTexture(const UserInterface::eImage image) const { return m_mTexturesCurrent.find(image)->second; };

    void updateWidthAndHeight(int iWidth, int iHeight);

    void render();

    void update(float fFrameDeltaTime);
private:

    // Default Constructor
    UserInterfaceManager();
    UserInterfaceManager(const UserInterfaceManager* pCopy);            // Default Copy Constructor
    UserInterfaceManager& operator=(const UserInterfaceManager* pCopy); // Assignment Operator.

    static UserInterfaceManager* m_pInstance;

    PromptInterface*            m_pPrompInterface;
    GameInterface*              m_GameInterface;
    vector<UserInterface*>    m_vInterfaceInstances;

    /*
        For tron aesthetics
    */
    unordered_map<UserInterface::eImage, Texture*> m_mTexturesTron;

    /*
        For outrun aesthetics
    */
    unordered_map<UserInterface::eImage, Texture*> m_mTexturesOutrun;

    /*
        Currently used textures based on the current theme.
    */
    unordered_map<UserInterface::eImage, Texture*> m_mTexturesCurrent;

    /*
        Current theme. Used by the SettingsInterface to display the theme.
    */
    eTheme m_eCurrentTheme;

    /*
        Whenever a UI texture is to be loaded, the filepath should be added to
        this vector.
    */
    const unordered_map<UserInterface::eImage, string> m_mImagePaths
    {
        {UserInterface::IMAGE_0_1, "0_1.png"},
        {UserInterface::IMAGE_0_2, "0.png"},
        {UserInterface::IMAGE_10_1, "10_1.png"},
        {UserInterface::IMAGE_10_2, "10.png"},
        {UserInterface::IMAGE_1_1, "1_1.png"},
        {UserInterface::IMAGE_1_2, "1.png"},
        {UserInterface::IMAGE_2_1, "2_1.png"},
        {UserInterface::IMAGE_2_2, "2.png"},
        {UserInterface::IMAGE_3_1, "3_1.png"},
        {UserInterface::IMAGE_3_2, "3.png"},
        {UserInterface::IMAGE_4_1, "4_1.png"},
        {UserInterface::IMAGE_4_2, "4.png"},
        {UserInterface::IMAGE_5_1, "5_1.png"},
        {UserInterface::IMAGE_5_2, "5.png"},
        {UserInterface::IMAGE_6_1, "6_1.png"},
        {UserInterface::IMAGE_6_2, "6.png"},
        {UserInterface::IMAGE_7_1, "7_1.png"},
        {UserInterface::IMAGE_7_2, "7.png"},
        {UserInterface::IMAGE_8_1, "8_1.png"},
        {UserInterface::IMAGE_8_2, "8.png"},
        {UserInterface::IMAGE_9_1, "9_1.png"},
        {UserInterface::IMAGE_9_2, "9.png"},
        {UserInterface::IMAGE_AI_1, "ai.png"},
        {UserInterface::IMAGE_AI_2, "ai_2.png"},
        {UserInterface::IMAGE_AWARDS_1, "awards.png"},
        {UserInterface::IMAGE_AWARDS_2, "awards_2.png"},
        {UserInterface::IMAGE_BACKGROUND_MAIN_MENU, "post_menu.jpg"},
        {UserInterface::IMAGE_BACKGROUND_PAUSE_MENU, "pause_menu.jpg"},
        {UserInterface::IMAGE_BACKGROUND_POST_MENU, "post_menu.jpg" },
        {UserInterface::IMAGE_BACKGROUND_PRE_MENU, "pre_menu.jpg" },
        {UserInterface::IMAGE_BACKGROUND_START_MENU, "start_menu.jpg"},
        {UserInterface::IMAGE_BOTS_VS_PLAYERS, "bots_vs_players2.png"},
        {UserInterface::IMAGE_BOT_1, "bot_1.png"},
        {UserInterface::IMAGE_BOT_2, "bot_2.png"},
        {UserInterface::IMAGE_BOT_3, "bot_3.png"},
        {UserInterface::IMAGE_BOT_4, "bot_4.png"},
        {UserInterface::IMAGE_BOT_DIFFICULTY_1, "bot_difficulty.png"},
        {UserInterface::IMAGE_BOT_DIFFICULTY_2, "bot_difficulty2.png"},
        {UserInterface::IMAGE_BOT_TEAM, "bot_team.png"},
        {UserInterface::IMAGE_BOT_TEAM_2, "bot_team2.png"},
        {UserInterface::IMAGE_CONFIG_1, "config_1.png"},
        {UserInterface::IMAGE_CONFIG_2, "config_2.png"},
        {UserInterface::IMAGE_CONTROLLER_1, "controls.png"},
        {UserInterface::IMAGE_CONTROLLER_2, "controls2.png"},
        {UserInterface::IMAGE_CONTROLLER_MENU, "controller_menu.png"},
        {UserInterface::IMAGE_DISABLED, "disabled2.png"},
        {UserInterface::IMAGE_EASY, "easy.png"},
        {UserInterface::IMAGE_ENABLED, "enabled2.png"},
        {UserInterface::IMAGE_END_GAME_1, "end_game.png"},
        {UserInterface::IMAGE_END_GAME_2, "end_game2.png"},
        {UserInterface::IMAGE_EXIT_1, "exit.png"},
        {UserInterface::IMAGE_EXIT_2, "exit2.png"},
        {UserInterface::IMAGE_FREE_FOR_ALL, "free_for_all2.png"},
        {UserInterface::IMAGE_GAMEMODE_1, "game_mode.png"},
        {UserInterface::IMAGE_GAMEMODE_2, "game_mode2.png"},
        {UserInterface::IMAGE_GAME_OVER, "game_over_large.png"},
        {UserInterface::IMAGE_GAME_RULE_1, "rules.png"},
        {UserInterface::IMAGE_GAME_RULE_2, "rules2.png"},
        {UserInterface::IMAGE_GAME_TIME_1, "game_time.png"},
        {UserInterface::IMAGE_GAME_TIME_2, "game_time2.png"},
        {UserInterface::IMAGE_HARD, "hard.png"},
        {UserInterface::IMAGE_INSERT_COIN_1, "insert_coin.png"},
        {UserInterface::IMAGE_INSERT_COIN_2, "insert_coin2.png"},
        {UserInterface::IMAGE_MAIN_MENU_BUTTON_1, "main_menu_button.png"},
        {UserInterface::IMAGE_MAIN_MENU_BUTTON_2, "main_menu_button2.png"},
        {UserInterface::IMAGE_MAP_1, "map.png"},
        {UserInterface::IMAGE_MAP_2, "map2.png"},
        {UserInterface::IMAGE_MEDIUM, "medium.png"},
        {UserInterface::IMAGE_MIN, "min2.png"},
        {UserInterface::IMAGE_MUSIC_1, "music.png"},
        {UserInterface::IMAGE_MUSIC_2, "music2.png"},
        {UserInterface::IMAGE_NEW_GAME_1, "new_game.png"},
        {UserInterface::IMAGE_NEW_GAME_2, "new_game2.png"},
        {UserInterface::IMAGE_NUMBER_1, "number_1.png"},
        {UserInterface::IMAGE_NUMBER_2, "number_2.png"},
        {UserInterface::IMAGE_NUMBER_3, "number_3.png"},
        {UserInterface::IMAGE_NUMBER_OF_BOT_1, "bots.png"},
        {UserInterface::IMAGE_NUMBER_OF_BOT_2, "bots2.png"},
        {UserInterface::IMAGE_NUMBER_OF_PLAYER_1, "players.png"},
        {UserInterface::IMAGE_NUMBER_OF_PLAYER_2, "players2.png"},
        {UserInterface::IMAGE_PLACEMENT, "placement.png"},
        {UserInterface::IMAGE_PLAYER, "player.png"},
        {UserInterface::IMAGE_PLAYER_TEAM, "player_team.png"},
        {UserInterface::IMAGE_PLAYER_TEAM1, "team1.png"},
        {UserInterface::IMAGE_PLAYER_TEAM2, "team2.png"},
        {UserInterface::IMAGE_PLAYER_TEAMS, "player_teams2.png"},
        {UserInterface::IMAGE_POINT, "pts.png"},
        {UserInterface::IMAGE_RESUME_1, "resume.png"},
        {UserInterface::IMAGE_RESUME_2, "resume2.png"},
        {UserInterface::IMAGE_RULE_MENU, "rule_menu.png"},
        {UserInterface::IMAGE_SCORE_LOSS_1, "score_loss.png"},
        {UserInterface::IMAGE_SCORE_LOSS_2, "score_loss2.png"},
        {UserInterface::IMAGE_SETTINGS_1, "settings.png"},
        {UserInterface::IMAGE_SETTINGS_2, "settings2.png"},
        {UserInterface::IMAGE_START_1, "start.png"},
        {UserInterface::IMAGE_START_2, "start2.png"},
        {UserInterface::IMAGE_TITLE, "title.png"},
    };
};
