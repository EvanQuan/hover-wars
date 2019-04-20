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
class UserInterfaceManager;

/*
    An interface to all user interface features.

    Retrieves its values from GameStats

    - Display text, images

    @author Evan Quan
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
    UserInterfaceManager *m_pUserInterfaceManager;

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

    // Global message values
    string m_sGlobalMessage;
    float m_fGlobalMessageTime;
    float m_fGlobalMessageX;
    float m_fGlobalMessageY;

};
