#pragma once
#include "stdafx.h"
#include "GameStats.h"

#define IMAGE_ROCKET "textures/hud/rocket.png"
#define IMAGE_TRAIL "textures/hud/trail.png"
#define IMAGE_EXPLOSION "textures/hud/explosion.png"
#define DISPLAY_COUNT_MIN 0
#define DISPLAY_COUNT_MAX 4

// Forward Declaration
class ShaderManager;
class Texture;

/*
An interface to all user interface features.

Retrieves its values from GameStats

- Display text, images
*/
class UserInterface
{
public:
    static UserInterface* getInstance(int iWidth, int iHeight);
    static UserInterface* getInstance();

    ~UserInterface();

    /*
    This visually updates the UserInterface to all value changes since last update.
    
    Under the scenes, this retrieves all needed the values from GameStats and
    displays them. This is why it does not need a time value in order to determine
    time-sensitive information such as cooldown and game time.
    @TODO this may change later on.
    
    This should be called once per frame update.
    
    */
    void update(float fSecondsSinceLastUpdate);
    void renderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);

    void setDisplayCount(int count);

    void updateWidthAndHeight(int iWidth, int iHeight);

private:
    UserInterface(int iWidth, int iHeight);                                 // Default Constructor
    UserInterface(const UserInterface* pCopy);                              // Default Copy Constructor
    UserInterface& operator=(const UserInterface* pCopy) {return (*this); } // Assignment Operator.
    static UserInterface* m_pInstance;

    // Initializes FreeType and the Font Library
    void initFreeType();
    void initializeVBOs();

    void setScore(int joystickID, int score);
    void renderImage(string filepath, GLfloat x, GLfloat y, GLfloat scale);
    void initializeUserInterface();

    // Game Time
    void updateGameTime(float fSecondsSinceLastUpdate);

    // Score
    void initializeScores();
    void updateScores();
    void updateScore(ePlayer player, int score);

    // Cooldowns
    void initializeCooldowns();
    void updateCooldowns();

    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
        vec2    uvOffset;   // Offset to index the UV in the bitmap
        vec2    uvSize;     // Size of UVs for bitmap
        ivec2   size;       // Size of glyph
        ivec2   bearing;    // Offset from baseline to left/top of glyph
        GLuint  advance;    // Horizontal offset to advance to next glyph
    };
    map<GLchar, Character> m_pCharacters;
    Texture* m_pFontBitmap;

    // VBO and VAO for rendering
    GLuint m_iVertexArray, m_iVertexBuffer, m_iTextureBuffer;

    map<int, int> scores;

    int m_iDisplayCount;

    // Window reference
    int m_iWidth;
    int m_iHeight;

    // Singleton Pointers
    GameStats *m_pGameStats;
    ShaderManager *m_pShdrMngr;
};
