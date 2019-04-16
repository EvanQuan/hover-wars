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


/*
    The root directory for all UI textures.
    Any UI textures to be loaded should be placed in here.
*/
#define IMAGE_DIRECTORY "textures/menu/"

// font website: https://fontmeme.com/tron-font/
// size 30
// effect 'Style-Neon'
// color 00FCFF and FF00E1
#define IMAGE_0_1 "0_1.png"
#define IMAGE_1_1 "1_1.png"
#define IMAGE_2_1 "2_1.png"
#define IMAGE_3_1 "3_1.png"
#define IMAGE_4_1 "4_1.png"
#define IMAGE_5_1 "5_1.png"
#define IMAGE_6_1 "6_1.png"
#define IMAGE_7_1 "7_1.png"
#define IMAGE_8_1 "8_1.png"
#define IMAGE_9_1 "9_1.png"
#define IMAGE_10_1 "10_1.png"
#define IMAGE_0_2 "0.png"
#define IMAGE_1_2 "1.png"
#define IMAGE_2_2 "2.png"
#define IMAGE_3_2 "3.png"
#define IMAGE_4_2 "4.png"
#define IMAGE_5_2 "5.png"
#define IMAGE_6_2 "6.png"
#define IMAGE_7_2 "7.png"
#define IMAGE_8_2 "8.png"
#define IMAGE_9_2 "9.png"
#define IMAGE_10_2 "10.png"
#define IMAGE_MAIN_MENU_BUTTON_1 "main_menu_button.png"
#define IMAGE_MAIN_MENU_BUTTON_2 "main_menu_button2.png"
#define IMAGE_BACKGROUND_START_MENU "start_menu.jpg"
#define IMAGE_BACKGROUND_MAIN_MENU "post_menu3.jpg"
#define IMAGE_NEW_GAME_1 "new_game.png"
#define IMAGE_NEW_GAME_2 "new_game2.png"
#define IMAGE_EXIT_1 "exit.png"
#define IMAGE_EXIT_2 "exit2.png"
#define IMAGE_BACKGROUND_PAUSE_MENU "pause_menu4.jpg"
#define IMAGE_BACKGROUND_POST_MENU "post_menu3.jpg" 
#define IMAGE_BACKGROUND_PRE_MENU "pre_menu2.jpg" 
#define IMAGE_RESUME_1 "resume.png"
#define IMAGE_RESUME_2 "resume2.png"
#define IMAGE_START_1 "start.png"
#define IMAGE_START_2 "start2.png"
#define IMAGE_TITLE "Title.png"
#define IMAGE_INSERT_COIN_1 "insert_coin.png"
#define IMAGE_INSERT_COIN_2 "insert_coin2.png"
#define IMAGE_NUMBER_OF_PLAYER_1 "players.png"
#define IMAGE_NUMBER_OF_PLAYER_2 "players2.png"
#define IMAGE_NUMBER_OF_BOT_1 "bots.png"
#define IMAGE_NUMBER_OF_BOT_2 "bots2.png"
#define IMAGE_END_GAME_1 "end_game.png"
#define IMAGE_END_GAME_2 "end_game2.png"
#define IMAGE_GAME_TIME_1 "game_time.png"
#define IMAGE_GAME_TIME_2 "game_time2.png"
#define IMAGE_MIN "min2.png"
#define IMAGE_CONTROLLER_1 "controls.png"
#define IMAGE_CONTROLLER_2 "controls2.png"
#define IMAGE_GAME_RULE_1 "rules.png"
#define IMAGE_GAME_RULE_2 "rules2.png"
#define IMAGE_CONTROLLER_MENU "controller_menu.png"
#define IMAGE_RULE_MENU "rule_menu.png"

#define IMAGE_GAMEMODE_1 "game_mode.png"
#define IMAGE_GAMEMODE_2 "game_mode2.png"
#define IMAGE_FREE_FOR_ALL "free_for_all2.png"
#define IMAGE_PLAYER_TEAMS "player_teams2.png"
#define IMAGE_BOT_TEAM "bot_team2.png"
#define IMAGE_PLAYER_TEAM "player_team2.png"
#define IMAGE_PLAYER_TEAM1 "team1.png"
#define IMAGE_PLAYER_TEAM2 "team2.png"

// #define IMAGE_BOT_TEAM IMAGE_10_MIN
#define IMAGE_BOT_VS_PLAYERS "bots_vs_players2.png"
#define IMAGE_SCORE_LOSS_1 "score_loss.png"
#define IMAGE_SCORE_LOSS_2 "score_loss2.png"
#define IMAGE_ENABLED "enabled2.png"
#define IMAGE_DISABLED "disabled2.png"
#define IMAGE_MAP_1 "map.png"
#define IMAGE_MAP_2 "map2.png"

#define IMAGE_AI_1 "ai.png"
#define IMAGE_AI_2 "ai_2.png"
#define IMAGE_CONFIG_1 "config_1.png"
#define IMAGE_CONFIG_2 "config_2.png"
#define IMAGE_PLACEMENT "placement.png"
#define IMAGE_AWARDS_1 "awards.png"
#define IMAGE_AWARDS_2 "awards_2.png"

#define IMAGE_PLAYER "player.png"

#define IMAGE_BOT_1 "bot_1.png"
#define IMAGE_BOT_2 "bot_2.png"
#define IMAGE_BOT_3 "bot_3.png"
#define IMAGE_BOT_4 "bot_4.png"

#define IMAGE_NUMBER_1 "number_1.png"
#define IMAGE_NUMBER_2 "number_2.png"
#define IMAGE_NUMBER_3 "number_3.png"
#define IMAGE_POINT "pts.png"

// #define IMAGE_ROCKET "textures/hud/rocket.png"
// #define IMAGE_TRAIL "textures/hud/trail.png"
// #define IMAGE_EXPLOSION "textures/hud/explosion.png"

#define IMAGE_BOT_DIFFICULTY_1 "bot_difficulty.png"
#define IMAGE_BOT_DIFFICULTY_2 "bot_difficulty2.png"

#define IMAGE_HARD "hard.png"
#define IMAGE_MEDIUM "medium.png"
#define IMAGE_EASY "easy.png"

#define IMAGE_MUSIC_1 "music.png"
#define IMAGE_MUSIC_2 "music2.png"
#define IMAGE_SETTINGS_1 "settings.png"
#define IMAGE_SETTINGS_2 "settings2.png"

#define IMAGE_GAME_OVER "game_over_large.png"

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

    unordered_map<string, Texture*>m_Textures;

    // Global message values
    string m_sGlobalMessage;
    float m_fGlobalMessageTime;
    float m_fGlobalMessageX;
    float m_fGlobalMessageY;

    /*
        Whenever a UI texture is to be loaded, the filepath should be added to this
        vector.
    */
    const vector<string> m_vTextureFiles
    {
        IMAGE_0_1,  
        IMAGE_0_2,
        IMAGE_10_1,
        IMAGE_10_2,
        IMAGE_1_1,  
        IMAGE_1_2,
        IMAGE_2_1,  
        IMAGE_2_2,
        IMAGE_3_1,
        IMAGE_3_2,
        IMAGE_4_1,
        IMAGE_4_2,
        IMAGE_5_1,
        IMAGE_5_2,
        IMAGE_6_1,
        IMAGE_6_2,
        IMAGE_7_1,
        IMAGE_7_2,
        IMAGE_8_1,
        IMAGE_8_2,
        IMAGE_9_1,
        IMAGE_9_2,
        IMAGE_AI_1,
        IMAGE_AI_2,
        IMAGE_AWARDS_1,
        IMAGE_AWARDS_2,
        IMAGE_BACKGROUND_MAIN_MENU,
        IMAGE_BACKGROUND_PAUSE_MENU,
        IMAGE_BACKGROUND_POST_MENU,
        IMAGE_BACKGROUND_PRE_MENU,
        IMAGE_BACKGROUND_START_MENU,
        IMAGE_BOT_1,
        IMAGE_BOT_2,
        IMAGE_BOT_3,
        IMAGE_BOT_4,
        IMAGE_BOT_DIFFICULTY_1,
        IMAGE_BOT_DIFFICULTY_2,
        IMAGE_BOT_TEAM,
        IMAGE_BOT_VS_PLAYERS,
        IMAGE_CONFIG_1,
        IMAGE_CONFIG_2,
        IMAGE_CONTROLLER_1,
        IMAGE_CONTROLLER_2,
        IMAGE_CONTROLLER_MENU,
        IMAGE_DISABLED,
        IMAGE_EASY,
        IMAGE_ENABLED,
        IMAGE_END_GAME_1,
        IMAGE_END_GAME_2,
        IMAGE_EXIT_1,
        IMAGE_EXIT_2,
        // IMAGE_EXPLOSION,
        IMAGE_FREE_FOR_ALL,
        IMAGE_GAMEMODE_1,
        IMAGE_GAMEMODE_2,
        IMAGE_GAME_OVER,
        IMAGE_GAME_RULE_1,
        IMAGE_GAME_RULE_2,
        IMAGE_GAME_TIME_1,
        IMAGE_GAME_TIME_2,
        IMAGE_HARD,
        IMAGE_INSERT_COIN_1,
        IMAGE_INSERT_COIN_2,
        IMAGE_MAIN_MENU_BUTTON_1,
        IMAGE_MAIN_MENU_BUTTON_2,
        IMAGE_MAP_1,
        IMAGE_MAP_2,
        IMAGE_MEDIUM,
        IMAGE_MIN,
        IMAGE_MUSIC_1,
        IMAGE_MUSIC_2,
        IMAGE_NEW_GAME_1,
        IMAGE_NEW_GAME_2,
        IMAGE_NUMBER_1,
        IMAGE_NUMBER_2,
        IMAGE_NUMBER_3,
        IMAGE_NUMBER_OF_BOT_1,
        IMAGE_NUMBER_OF_BOT_2,
        IMAGE_NUMBER_OF_PLAYER_1,
        IMAGE_NUMBER_OF_PLAYER_2,
        IMAGE_PLACEMENT,
        IMAGE_PLAYER,
        IMAGE_PLAYER_TEAM,
        IMAGE_PLAYER_TEAM1,
        IMAGE_PLAYER_TEAM2,
        IMAGE_PLAYER_TEAMS,
        IMAGE_POINT,
        IMAGE_RESUME_1,
        IMAGE_RESUME_2,
        // IMAGE_ROCKET,
        IMAGE_RULE_MENU,
        IMAGE_SCORE_LOSS_1,
        IMAGE_SCORE_LOSS_2,
        IMAGE_SETTINGS_1,
        IMAGE_SETTINGS_2,
        IMAGE_START_1,
        IMAGE_START_2,
        IMAGE_TITLE,
        // IMAGE_TRAIL,
    };
};
