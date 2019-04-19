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
An interface to all user interface features.

Retrieves its values from GameStats

- Display text, images
*/
class UserInterface abstract
{
public:

    /*
        Represents an image. Each image has an image path based on a specified
        root directory. Depending on the currently selected theme, the root
        directory may change.
    */
    enum eImage
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
        IMAGE_BOTS_VS_PLAYERS,
        IMAGE_BOT_1,
        IMAGE_BOT_2,
        IMAGE_BOT_3,
        IMAGE_BOT_4,
        IMAGE_BOT_DIFFICULTY_1,
        IMAGE_BOT_DIFFICULTY_2,
        IMAGE_BOT_TEAM,
        IMAGE_BOT_TEAM_2,
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
        IMAGE_RULE_MENU,
        IMAGE_SCORE_LOSS_1,
        IMAGE_SCORE_LOSS_2,
        IMAGE_SETTINGS_1,
        IMAGE_SETTINGS_2,
        IMAGE_START_1,
        IMAGE_START_2,
        IMAGE_TITLE,
    };


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

    void displayGlobalMessage(const string message);

protected:

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

    virtual void setTheme (eTheme theme) final;

    /*
        Get the current theme in use.
    */
    virtual eTheme getTheme() const final { return m_eCurrentTheme; }

    // Default Constructor
    UserInterface(vector<pair<float, float>> componentScaling,
                  vector<pair<float, float>> componentTranslating);
    UserInterface(const UserInterface* pCopy);            // Default Copy Constructor
    UserInterface& operator=(const UserInterface* pCopy); // Assignment Operator.

    virtual void updateOverride(float fFrameDeltaTime) = 0;

    eImage digitToImage(unsigned int digit) const;

    void renderText(int text, GLfloat x, GLfloat y, GLfloat scale, const vec3 color);
    void renderText(const string text, GLfloat x, GLfloat y, GLfloat scale, const vec3 color);
    void renderText(const string text, int component, vec3 color);
    void renderText(const string text, int component, float x, float y, const vec3 color);
    void renderImage(const eImage image, GLfloat x, GLfloat y, GLfloat scale);
    void renderImage(const eImage image, int component);
    void renderImage(const eImage image, int component, float x, float y);

    /*
        Background images stretch to fill the window dimensions.
    */
    void renderBackgroundImage(const eImage image);

    // width of the window in pixels
    int m_iWidth;
    // height of the window in pixels
    int m_iHeight;

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
        m_vComponentScaling and m_vComponentTranslating should have the same
        number of elements. They size is tracked here.
    */
    int m_iComponentCount;

    /*
        For tron aesthetics
    */
    unordered_map<eImage, Texture*> m_TexturesTron;

    /*
        For outrun aesthetics
    */
    unordered_map<eImage, Texture*> m_TexturesOutrun;

    // Global message values
    string m_sGlobalMessage;
    float m_fGlobalMessageTime;
    float m_fGlobalMessageX;
    float m_fGlobalMessageY;

    eTheme m_eCurrentTheme;

    /*
        Whenever a UI texture is to be loaded, the filepath should be added to
        this vector.
    */
    const unordered_map<eImage, string> m_mImagePaths
    {
        {IMAGE_0_1, "0_1.png"},
        {IMAGE_0_2, "0.png"},
        {IMAGE_10_1, "10_1.png"},
        {IMAGE_10_2, "10.png"},
        {IMAGE_1_1, "1_1.png"},
        {IMAGE_1_2, "1.png"},
        {IMAGE_2_1, "2_1.png"},
        {IMAGE_2_2, "2.png"},
        {IMAGE_3_1, "3_1.png"},
        {IMAGE_3_2, "3.png"},
        {IMAGE_4_1, "4_1.png"},
        {IMAGE_4_2, "4.png"},
        {IMAGE_5_1, "5_1.png"},
        {IMAGE_5_2, "5.png"},
        {IMAGE_6_1, "6_1.png"},
        {IMAGE_6_2, "6.png"},
        {IMAGE_7_1, "7_1.png"},
        {IMAGE_7_2, "7.png"},
        {IMAGE_8_1, "8_1.png"},
        {IMAGE_8_2, "8.png"},
        {IMAGE_9_1, "9_1.png"},
        {IMAGE_9_2, "9.png"},
        {IMAGE_AI_1, "ai.png"},
        {IMAGE_AI_2, "ai_2.png"},
        {IMAGE_AWARDS_1, "awards.png"},
        {IMAGE_AWARDS_2, "awards_2.png"},
        {IMAGE_BACKGROUND_MAIN_MENU, "post_menu.jpg"},
        {IMAGE_BACKGROUND_PAUSE_MENU, "pause_menu.jpg"},
        {IMAGE_BACKGROUND_POST_MENU, "post_menu.jpg" },
        {IMAGE_BACKGROUND_PRE_MENU, "pre_menu.jpg" },
        {IMAGE_BACKGROUND_START_MENU, "start_menu.jpg"},
        {IMAGE_BOTS_VS_PLAYERS, "bots_vs_players2.png"},
        {IMAGE_BOT_1, "bot_1.png"},
        {IMAGE_BOT_2, "bot_2.png"},
        {IMAGE_BOT_3, "bot_3.png"},
        {IMAGE_BOT_4, "bot_4.png"},
        {IMAGE_BOT_DIFFICULTY_1, "bot_difficulty.png"},
        {IMAGE_BOT_DIFFICULTY_2, "bot_difficulty2.png"},
        {IMAGE_BOT_TEAM, "bot_team.png"},
        {IMAGE_BOT_TEAM_2, "bot_team2.png"},
        {IMAGE_CONFIG_1, "config_1.png"},
        {IMAGE_CONFIG_2, "config_2.png"},
        {IMAGE_CONTROLLER_1, "controls.png"},
        {IMAGE_CONTROLLER_2, "controls2.png"},
        {IMAGE_CONTROLLER_MENU, "controller_menu.png"},
        {IMAGE_DISABLED, "disabled2.png"},
        {IMAGE_EASY, "easy.png"},
        {IMAGE_ENABLED, "enabled2.png"},
        {IMAGE_END_GAME_1, "end_game.png"},
        {IMAGE_END_GAME_2, "end_game2.png"},
        {IMAGE_EXIT_1, "exit.png"},
        {IMAGE_EXIT_2, "exit2.png"},
        {IMAGE_FREE_FOR_ALL, "free_for_all2.png"},
        {IMAGE_GAMEMODE_1, "game_mode.png"},
        {IMAGE_GAMEMODE_2, "game_mode2.png"},
        {IMAGE_GAME_OVER, "game_over_large.png"},
        {IMAGE_GAME_RULE_1, "rules.png"},
        {IMAGE_GAME_RULE_2, "rules2.png"},
        {IMAGE_GAME_TIME_1, "game_time.png"},
        {IMAGE_GAME_TIME_2, "game_time2.png"},
        {IMAGE_HARD, "hard.png"},
        {IMAGE_INSERT_COIN_1, "insert_coin.png"},
        {IMAGE_INSERT_COIN_2, "insert_coin2.png"},
        {IMAGE_MAIN_MENU_BUTTON_1, "main_menu_button.png"},
        {IMAGE_MAIN_MENU_BUTTON_2, "main_menu_button2.png"},
        {IMAGE_MAP_1, "map.png"},
        {IMAGE_MAP_2, "map2.png"},
        {IMAGE_MEDIUM, "medium.png"},
        {IMAGE_MIN, "min2.png"},
        {IMAGE_MUSIC_1, "music.png"},
        {IMAGE_MUSIC_2, "music2.png"},
        {IMAGE_NEW_GAME_1, "new_game.png"},
        {IMAGE_NEW_GAME_2, "new_game2.png"},
        {IMAGE_NUMBER_1, "number_1.png"},
        {IMAGE_NUMBER_2, "number_2.png"},
        {IMAGE_NUMBER_3, "number_3.png"},
        {IMAGE_NUMBER_OF_BOT_1, "bots.png"},
        {IMAGE_NUMBER_OF_BOT_2, "bots2.png"},
        {IMAGE_NUMBER_OF_PLAYER_1, "players.png"},
        {IMAGE_NUMBER_OF_PLAYER_2, "players2.png"},
        {IMAGE_PLACEMENT, "placement.png"},
        {IMAGE_PLAYER, "player.png"},
        {IMAGE_PLAYER_TEAM, "player_team.png"},
        {IMAGE_PLAYER_TEAM1, "team1.png"},
        {IMAGE_PLAYER_TEAM2, "team2.png"},
        {IMAGE_PLAYER_TEAMS, "player_teams2.png"},
        {IMAGE_POINT, "pts.png"},
        {IMAGE_RESUME_1, "resume.png"},
        {IMAGE_RESUME_2, "resume2.png"},
        {IMAGE_RULE_MENU, "rule_menu.png"},
        {IMAGE_SCORE_LOSS_1, "score_loss.png"},
        {IMAGE_SCORE_LOSS_2, "score_loss2.png"},
        {IMAGE_SETTINGS_1, "settings.png"},
        {IMAGE_SETTINGS_2, "settings2.png"},
        {IMAGE_START_1, "start.png"},
        {IMAGE_START_2, "start2.png"},
        {IMAGE_TITLE, "title.png"},
    };
};
