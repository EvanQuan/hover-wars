#pragma once
#include "stdafx.h"


/***********\
 * Defines *
\***********/
#define TOP_LEFT                2
#define BOTTOM_LEFT             0
#define BOTTOM_RIGHT            1
#define TOP_RIGHT               3

// UI Component locations
// These should all be relative to the window dimensions
// not hardcoded pixel values, as they are now.
#define COLOR_WHITE             vec3(1.0)
#define COLOR_RED               vec3(1.0, 0.0, 0.0)
#define COLOR_GREEN             vec3(0.0, 1.0, 0.0)
#define COLOR_YELLOW            vec3(1.0, 1.0, 0.0)

// font website: https://fontmeme.com/tron-font/
// size 30
// effect 'Style-Neon'
// color 00FCFF and FF00E1
#define IMAGE_MENU_0 "textures/menu/0.png"
#define IMAGE_MENU_1 "textures/menu/1.png"
#define IMAGE_MENU_2 "textures/menu/2.png"
#define IMAGE_MENU_3 "textures/menu/3.png"
#define IMAGE_MENU_4 "textures/menu/4.png"
#define IMAGE_0_1 "textures/menu/0_1.png"
#define IMAGE_1_1 "textures/menu/1_1.png"
#define IMAGE_2_1 "textures/menu/2_1.png"
#define IMAGE_3_1 "textures/menu/3_1.png"
#define IMAGE_4_1 "textures/menu/4_1.png"
#define IMAGE_5_1 "textures/menu/5_1.png"
#define IMAGE_6_1 "textures/menu/6_1.png"
#define IMAGE_7_1 "textures/menu/7_1.png"
#define IMAGE_8_1 "textures/menu/8_1.png"
#define IMAGE_9_1 "textures/menu/9_1.png"
#define IMAGE_10_1 "textures/menu/10_1.png"
#define IMAGE_0_2 "textures/menu/0.png"
#define IMAGE_1_2 "textures/menu/1.png"
#define IMAGE_2_2 "textures/menu/2.png"
#define IMAGE_3_2 "textures/menu/3.png"
#define IMAGE_4_2 "textures/menu/4.png"
#define IMAGE_5_2 "textures/menu/5.png"
#define IMAGE_6_2 "textures/menu/6.png"
#define IMAGE_7_2 "textures/menu/7.png"
#define IMAGE_8_2 "textures/menu/8.png"
#define IMAGE_9_2 "textures/menu/9.png"
#define IMAGE_10_2 "textures/menu/10.png"
#define IMAGE_MAIN_MENU_BUTTON_1 "textures/menu/main_menu_button.png"
#define IMAGE_MAIN_MENU_BUTTON_2 "textures/menu/main_menu_button2.png"
#define IMAGE_BACKGROUND_START_MENU "textures/menu/start_menu.jpg"
#define IMAGE_BACKGROUND_MAIN_MENU "textures/menu/post_menu3.jpg"
#define IMAGE_NEW_GAME_1 "textures/menu/new_game.png"
#define IMAGE_NEW_GAME_2 "textures/menu/new_game2.png"
#define IMAGE_EXIT_1 "textures/menu/exit.png"
#define IMAGE_EXIT_2 "textures/menu/exit2.png"
#define IMAGE_BACKGROUND_PAUSE_MENU "textures/menu/pause_menu4.jpg"
#define IMAGE_BACKGROUND_POST_MENU "textures/menu/post_menu3.jpg" 
#define IMAGE_BACKGROUND_PRE_MENU "textures/menu/pre_menu2.jpg" 
#define IMAGE_RESUME_1 "textures/menu/resume.png"
#define IMAGE_RESUME_2 "textures/menu/resume2.png"
#define IMAGE_START_1 "textures/menu/start.png"
#define IMAGE_START_2 "textures/menu/start2.png"
#define IMAGE_TITLE "textures/menu/Title.png"
#define IMAGE_INSERT_COIN_1 "textures/menu/insert_coin.png"
#define IMAGE_INSERT_COIN_2 "textures/menu/insert_coin2.png"
#define IMAGE_NUMBER_OF_PLAYER_1 "textures/menu/players.png"
#define IMAGE_NUMBER_OF_PLAYER_2 "textures/menu/players2.png"
#define IMAGE_NUMBER_OF_BOT_1 "textures/menu/bots.png"
#define IMAGE_NUMBER_OF_BOT_2 "textures/menu/bots2.png"
#define IMAGE_END_GAME_1 "textures/menu/end_game.png"
#define IMAGE_END_GAME_2 "textures/menu/end_game2.png"
#define IMAGE_GAME_TIME_1 "textures/menu/game_time.png"
#define IMAGE_GAME_TIME_2 "textures/menu/game_time2.png"
#define IMAGE_1_MIN "textures/menu/1_min.png"
#define IMAGE_2_MIN "textures/menu/2_min.png"
#define IMAGE_3_MIN "textures/menu/3_min.png"
#define IMAGE_4_MIN "textures/menu/4_min.png"
#define IMAGE_5_MIN "textures/menu/5_min.png"
#define IMAGE_6_MIN "textures/menu/6_min.png"
#define IMAGE_7_MIN "textures/menu/7_min.png"
#define IMAGE_8_MIN "textures/menu/8_min.png"
#define IMAGE_9_MIN "textures/menu/9_min.png"
#define IMAGE_10_MIN "textures/menu/10_min.png"
#define IMAGE_MIN "textures/menu/min2.png"
#define IMAGE_CONTROLLER_1 "textures/menu/controls.png"
#define IMAGE_CONTROLLER_2 "textures/menu/controls2.png"
#define IMAGE_GAME_RULE_1 "textures/menu/rules.png"
#define IMAGE_GAME_RULE_2 "textures/menu/rules2.png"
#define IMAGE_CONTROLLER_MENU "textures/menu/controller_menu.png"
#define IMAGE_RULE_MENU "textures/menu/rule_menu.png"

#define IMAGE_GAMEMODE_1 "textures/menu/game_mode.png"
#define IMAGE_GAMEMODE_2 "textures/menu/game_mode2.png"
#define IMAGE_FREE_FOR_ALL "textures/menu/free_for_all2.png"
#define IMAGE_PLAYER_TEAMS "textures/menu/player_teams2.png"
#define IMAGE_BOT_TEAM "textures/menu/bot_team2.png"
#define IMAGE_PLAYER_TEAM "textures/menu/player_team2.png"
#define IMAGE_PLAYER_TEAM1 "textures/menu/team1.png"
#define IMAGE_PLAYER_TEAM2 "textures/menu/team2.png"

// #define IMAGE_BOT_TEAM IMAGE_10_MIN
#define IMAGE_BOT_VS_PLAYERS "textures/menu/bots_vs_players2.png"
#define IMAGE_SCORE_LOSS_1 "textures/menu/score_loss.png"
#define IMAGE_SCORE_LOSS_2 "textures/menu/score_loss2.png"
#define IMAGE_ENABLED "textures/menu/enabled2.png"
#define IMAGE_DISABLED "textures/menu/disabled2.png"
#define IMAGE_MAP_1 "textures/menu/map.png"
#define IMAGE_MAP_2 "textures/menu/map2.png"

#define IMAGE_AI_1 "textures/menu/ai.png"
#define IMAGE_AI_2 "textures/menu/ai_2.png"
#define IMAGE_CONFIG_1 "textures/menu/config_1.png"
#define IMAGE_CONFIG_2 "textures/menu/config_2.png"
#define IMAGE_PLACEMENT "textures/menu/placement.png"
#define IMAGE_AWARDS_1 "textures/menu/awards.png"
#define IMAGE_AWARDS_2 "textures/menu/awards_2.png"
#define IMAGE_PLAYER_1 "textures/menu/player1.png"

#define IMAGE_PLAYER "textures/menu/player.png"

// @Deprecated
#define IMAGE_PLAYER_1 "textures/menu/player1.png"
#define IMAGE_PLAYER_2 "textures/menu/player2.png"
#define IMAGE_PLAYER_3 "textures/menu/player3.png"
#define IMAGE_PLAYER_4 "textures/menu/player4.png"

#define IMAGE_BOT_1 "textures/menu/bot_1.png"
#define IMAGE_BOT_2 "textures/menu/bot_2.png"
#define IMAGE_BOT_3 "textures/menu/bot_3.png"
#define IMAGE_BOT_4 "textures/menu/bot_4.png"

#define IMAGE_NUMBER_1 "textures/menu/number_1.png"
#define IMAGE_NUMBER_2 "textures/menu/number_2.png"
#define IMAGE_NUMBER_3 "textures/menu/number_3.png"
#define IMAGE_POINT "textures/menu/pts.png"

#define IMAGE_ROCKET "textures/hud/rocket.png"
#define IMAGE_TRAIL "textures/hud/trail.png"
#define IMAGE_EXPLOSION "textures/hud/explosion.png"

#define IMAGE_BOT_DIFFICULTY_1 "textures/menu/bot_difficulty.png"
#define IMAGE_BOT_DIFFICULTY_2 "textures/menu/bot_difficulty2.png"

#define IMAGE_HARD "textures/menu/hard.png"
#define IMAGE_MEDIUM "textures/menu/medium.png"
#define IMAGE_EASY "textures/menu/easy.png"

#define IMAGE_MUSIC_1 "textures/menu/music.png"
#define IMAGE_MUSIC_2 "textures/menu/music2.png"
#define IMAGE_SETTINGS_1 "textures/menu/settings.png"
#define IMAGE_SETTINGS_2 "textures/menu/settings2.png"

#define IMAGE_GAME_OVER "textures/menu/game_over_large.png"

// Forward Declaration
class ShaderManager;
class Texture;

/*
An interface to all user interface features.

Retrieves its values from GameStats

- Display text, images
*/
class UserInterface abstract
{
public:

    ~UserInterface();

    /*
        This visually updates the UserInterface to all value changes since last update.
        
        Under the scenes, this retrieves all needed the values from GameStats and
        displays them. This is why it does not need a time value in order to determine
        time-sensitive information such as cooldown and game time.
        @TODO this may change later on.
        
        This should be called once per frame update.
    */
    virtual void update(float fDeltaTime) final {
        m_fGlobalMessageTime -= fDeltaTime;
        updateOverride(fDeltaTime);
        renderGlobalMessage();
    }

    virtual void reinitialize(float gameTime) = 0;

    virtual void render() final;
    virtual void renderOverride() {};

    virtual void updateWidthAndHeight(int iWidth, int iHeight) final;

    // Shows the Sprite Map for the text map for debugging.
    void debugFont();

    void displayGlobalMessage(string message);

protected:

    // Default Constructor
    UserInterface(vector<pair<float, float>> componentScaling,
                  vector<pair<float, float>> componentTranslating);
    UserInterface(const UserInterface* pCopy);            // Default Copy Constructor
    UserInterface& operator=(const UserInterface* pCopy); // Assignment Operator.

    virtual void updateOverride(float fFrameDeltaTime) = 0;

    void renderText(int text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
    void renderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
    void renderImage(string filepath, GLfloat x, GLfloat y, GLfloat scale);
    void renderBackgroundImage(string filepath);

    // width of the window in pixels
    int m_iWidth;
    // height of the window in pixels
    int m_iHeight;

    /*
        Determines how each UI component is scaled based on window dimensions.
        This can use useful for determining the general position of components
        as window dimensions change.
    */
    vector<pair<float, float>> m_vComponentScaling;
    /*
        Determines how each UI component is translated after scaling. This can
        be useful when certain components are placed a fixed distance relative
        to other components.
    */
    vector<pair<float, float>> m_vComponentTranslating;
    /*
        The absolute value of the UI componnets are stored once they are
        calculated so they do not need to be calculated every frame. Wheneever
        there is a window resize event, these values are all recalculated.
    */
    vector<pair<float, float>> m_vComponentCoordinates;
    /*
        Every hovercraft has a corresponding name. If the UI ever needs to user
        a hovercraft's name, it should use this.
    */
    const unordered_map<eHovercraft, std::string> m_eHovercraftToString =
    {
        {HOVERCRAFT_BOT_1, "Alfa Bot"},
        {HOVERCRAFT_BOT_2, "Bravo Bot"},
        {HOVERCRAFT_BOT_3, "Charlie Bot"},
        {HOVERCRAFT_BOT_4, "Delta Bot"},
        {HOVERCRAFT_PLAYER_1, "Player 1"},
        {HOVERCRAFT_PLAYER_2, "Player 2"},
        {HOVERCRAFT_PLAYER_3, "Player 3"},
        {HOVERCRAFT_PLAYER_4, "Player 4"},
    };

private:
    // Singleton Pointers
    // The UI needs to render its own components
    ShaderManager *m_pShdrMngr;

    void renderGlobalMessage();


    // Initializes FreeType and the Font Library
    void initFreeType();
    void initializeVBOs();
    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
        vec2    uvOffset;   // Offset to index the UV in the bitmap
        vec2    uvSize;     // Size of UVs for bitmap
        ivec2   size;       // Size of glyph
        ivec2   bearing;    // Offset from baseline to left/top of glyph
        GLuint  advance;    // Horizontal offset to advance to next glyph
    };
    map<GLchar, Character> m_pCharacters;
    void addNewCharacter(char c, const FT_GlyphSlotRec_* pGlyph, const vec2* vOffsets);
    void addBitmapToBuffer(const FT_Bitmap* pBitmap, char* cPtr);

    // VBO and VAO for rendering
    GLuint m_iVertexArray, m_iVertexBuffer, m_iTextureBuffer;

    void initializeComponentCoordinates();

    /*
        m_vComponentScaling and m_vComponentTranslating should have the same
        number of elements. They size is tracked here.
    */
    int m_iComponentCount;

    map<string, Texture*>m_Textures;

    // Global message values
    string m_sGlobalMessage;
    float m_fGlobalMessageTime;
    float m_fGlobalMessageX;
    float m_fGlobalMessageY;


};
