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

#define IMAGE_1_1 "textures/menu/1.png"
#define IMAGE_1_2 "textures/menu/1_2.png"
#define IMAGE_2_1 "textures/menu/2.png"
#define IMAGE_2_2 "textures/menu/2_2.png"
#define IMAGE_3_1 "textures/menu/3.png"
#define IMAGE_3_2 "textures/menu/3_2.png"
#define IMAGE_4_1 "textures/menu/4.png"
#define IMAGE_4_2 "textures/menu/4_2.png"
#define IMAGE_MAIN_MENU_BUTTON_1 "textures/menu/main_menu_button.png"
#define IMAGE_MAIN_MENU_BUTTON_2 "textures/menu/main_menu_button2.png"
#define IMAGE_BACKGROUND "textures/menu/main_menu.png"
#define IMAGE_NEW_GAME_1 "textures/menu/new_game.png"
#define IMAGE_NEW_GAME_2 "textures/menu/new_game2.png"
#define IMAGE_EXIT_1 "textures/menu/exit.png"
#define IMAGE_EXIT_2 "textures/menu/exit2.png"
#define IMAGE_PAUSE_MENU "textures/menu/pause_menu.jpg"
#define IMAGE_POST_MENU "textures/menu/post_menu.jpg"
#define IMAGE_RESTART_1 "textures/menu/restart.png"
#define IMAGE_RESTART_2 "textures/menu/restart2.png"
#define IMAGE_RESUME_1 "textures/menu/resume.png"
#define IMAGE_RESUME_2 "textures/menu/resume2.png"
#define IMAGE_START_1 "textures/menu/start.png"
#define IMAGE_START_2 "textures/menu/start2.png"
#define IMAGE_TITLE "textures/menu/Title.png"
#define IMAGE_INSERT_COIN_1 "textures/menu/insert_coin.png"
#define IMAGE_INSERT_COIN_2 "textures/menu/insert_coin2.png"
#define IMAGE_NUMBER_OF_PLAYER "textures/menu/number_of_player.png"
#define IMAGE_NUMBER_OF_BOT "textures/menu/number_of_bot.png"

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
    virtual void update(float fSecondsSinceLastUpdate) = 0;

    virtual void reinitialize(float gameTime) = 0;

    virtual void render() = 0;

    virtual void updateWidthAndHeight(int iWidth, int iHeight) final;

    // Display debug message
    // Set message to "" to disable debug message
    void displayDebug(const char* message);

protected:

    // Default Constructor
    UserInterface(int iWidth, int iHeight,
        vector<pair<float, float>> componentScaling,
        vector<pair<float, float>> componentTranslating);
    UserInterface(const UserInterface* pCopy);                              // Default Copy Constructor
    UserInterface& operator=(const UserInterface* pCopy) {return (*this); } // Assignment Operator.

    void renderText(int text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
    void renderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
    void renderImage(string filepath, GLfloat x, GLfloat y, GLfloat scale);

    // width of the window in pixels
    int m_iWidth;
    // height of the window in pixels
    int m_iHeight;


    const char* debugMessage;
    float debugWidth;
    float debugHeight;

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

private:
    // Singleton Pointers
    // The UI needs to render its own components
    ShaderManager *m_pShdrMngr;


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

};
