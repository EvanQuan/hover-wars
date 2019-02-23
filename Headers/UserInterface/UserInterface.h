#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "GameStats.h"
#include "UserInterface/UIRenderer.h"

#define DISPLAY_COUNT_MIN 0
#define DISPLAY_COUNT_MAX 4
/*
An interface to all user interface features.

Retrieves its values from GameStats

- Display text, images
*/
class UserInterface
{
public:
    static UserInterface* getInstance(int iWidth, int iHeight);

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

    void setDisplayCount(int count);

    void updateWidthAndHeight(int iWidth, int iHeight);

private:
    UserInterface(int iWidth, int iHeight);
    static UserInterface* m_pInstance;

    void setScore(int joystickID, int score);
    void renderText(Shader &shader, string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
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
        GLuint textureID;  // ID handle of the glyph texture
        ivec2 size;        // Size of glyph
        ivec2 bearing;     // Offset from baseline to left/top of glyph
        GLuint advance;    // Horizontal offset to advance to next glyph
    };

    map<GLchar, Character> characters;

    map<int, int> scores;

    int m_iDisplayCount;

    // Window reference
    GLFWwindow *m_pWindow;
    int m_iWidth;
    int m_iHeight;

    GameStats *m_pGameStats;
    UIRenderer *m_pUIRenderer;
};
