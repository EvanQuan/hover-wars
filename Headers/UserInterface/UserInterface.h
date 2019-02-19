#pragma once
#include "stdafx.h"
#include "Shader.h"

#define DISPLAY_COUNT_MIN 0
#define DISPLAY_COUNT_MAX 4
/*
An interface to all user interface features.

- Display text, images
*/
class UserInterface
{
public:
    static UserInterface* getInstance(GLFWwindow* window);

    ~UserInterface();

    void update();

    void setDisplayCount(int count);
    void addScore(int joystickID, int score);
    void subtractScore(int joystickID, int score);
    void resetScore(int joystickID);

private:
    UserInterface(GLFWwindow* window);
    static UserInterface* m_pInstance;

    void setScore(int joystickID, int score);
    void renderText(Shader &shader, string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
    void initializeScores();
    void initializeUserInterface();
    void initializeCooldowns();

    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
        GLuint textureID;  // ID handle of the glyph texture
        ivec2 size;        // Size of glyph
        ivec2 bearing;     // Offset from baseline to left/top of glyph
        GLuint advance;    // Horizontal offset to advance to next glyph
    };

    map<GLchar, Character> characters;

    map<int, int> scores;

    int m_pDisplayCount;

    // Window reference
    GLFWwindow* m_pWindow;
};



