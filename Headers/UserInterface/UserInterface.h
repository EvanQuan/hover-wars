#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "GameStats.h"
#include "UserInterface/FontManager.h"

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
    static UserInterface* getInstance(GLFWwindow* window);

    ~UserInterface();

    void update();

    void setDisplayCount(int count);

private:
    UserInterface(GLFWwindow* window);
    static UserInterface* m_pInstance;

    void setScore(int joystickID, int score);
    void renderText(Shader &shader, string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
    void initializeUserInterface();

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

    GameStats *m_pGameStats;
    UIRenderer *m_pUIRenderer;
};
