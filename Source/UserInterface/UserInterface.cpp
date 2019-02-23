#include "UserInterface/UserInterface.h"


// Singleton instance
UserInterface* UserInterface::m_pInstance = nullptr;

UserInterface::UserInterface(int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_iDisplayCount = 0;

    m_pGameStats = GameStats::getInstance();
    // m_pUIRenderer = UIRenderer::getInstance(window);

    initializeUserInterface();

}

UserInterface* UserInterface::getInstance(int iWidth, int iHeight)
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new UserInterface(iWidth, iHeight);
    }
    return m_pInstance;
}

UserInterface::~UserInterface()
{
    // Let go of Window
    m_pWindow = nullptr;
    m_pGameStats = nullptr;
    if (nullptr != m_pUIRenderer)
    {
        delete m_pUIRenderer;
    }

}

/*
This visually updates the UserInterface to all value changes since last update.

Under the scenes, this retrieves all needed the values from GameStats and
displays them.

@TODO
*/
void UserInterface::update()
{
    if (m_iDisplayCount > 0)
    {
        // system("CLS");
        updateScores();
        updateCooldowns();
    }
}

/*
Update the scores for all display count players
*/
void UserInterface::updateScores()
{
    for (int player = 0; player < m_iDisplayCount; player++)
    {
        updateScore((ePlayer) player, m_pGameStats->get((ePlayer) player, GameStats::CURRENT_SCORE));
    }
}

void UserInterface::updateScore(ePlayer player, int score)
{
    // cout << "Player " << (player + 1) << " score: " << score << endl;
}

void UserInterface::updateCooldowns()
{

}

/*
Set the number of UIs to display.
Values:
    0 - No UIs are displayed. This should be done in the main menu (not in game).
    1 - 1 player. This will display across the whole window.
    2 - 2 player. Each UI will display across 1/4 of the window.
    3 - 3 player. Each UI will display across 1/4 of the window.
    4 - 4 player. Each UI will display across 1/4 of the window.

@TODO set count for main menu or pause menu
*/
void UserInterface::setDisplayCount(int count)
{
    if (DISPLAY_COUNT_MIN > count)
    {
        count = DISPLAY_COUNT_MIN;
    }
    else if (DISPLAY_COUNT_MAX < count)
    {
        count = DISPLAY_COUNT_MAX;
    }
    m_iDisplayCount = count;

    initializeUserInterface();
}

/*
Initialize all aspects of the UI according to the current display count. This
should be done at the start of every game, or if the game resets.
*/
void UserInterface::initializeUserInterface()
{
    initializeScores();
    initializeCooldowns();
}

/*
Initialize the scores of all players to 0
*/
void UserInterface::initializeScores()
{
    for (int i = DISPLAY_COUNT_MIN; i < DISPLAY_COUNT_MAX; i++)
    {
    }
}

/*
Initialize the cool down counts for all abilities
*/
void UserInterface::initializeCooldowns()
{
    for (int i = DISPLAY_COUNT_MIN; i < DISPLAY_COUNT_MAX; i++)
    {
        // TODO
    }
}

/*
Set a specified player's score to the specified value.
*/
void UserInterface::setScore(int joystickID, int score)
{
}

/*
Initialize all UI components according to the current value of m_iDisplayCount
*/
void initializeDisplayCount()
{
    // TODO
}


// https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering
// void UserInterface::renderText(Shader &shader, string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
// {
    // Activate corresponding render state    
    // TODO shader stuff  here
    // shader.Use();
    // glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
    // glActiveTexture(GL_TEXTURE0);
    // glBindVertexArray(VAO);
    
    // Iterate through all characters
//    std::string::const_iterator c;
//    for (c = text.begin(); c != text.end(); c++) 
//    {
//        Character ch = characters[*c];
//        
//        GLfloat xpos = x + ch.bearing.x * scale;
//        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
//        
//        GLfloat w = ch.size.x * scale;
//        GLfloat h = ch.size.y * scale;
//        // Update VBO for each character
//        GLfloat vertices[6][4] = {
//            { xpos,     ypos + h,   0.0, 0.0 },            
//            { xpos,     ypos,       0.0, 1.0 },
//            { xpos + w, ypos,       1.0, 1.0 },
//            
//            { xpos,     ypos + h,   0.0, 0.0 },
//            { xpos + w, ypos,       1.0, 1.0 },
//            { xpos + w, ypos + h,   1.0, 0.0 }           
//        };
//        // Render glyph texture over quad
//        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//        // Update content of VBO memory
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
//        
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        // Render quad
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//        x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//    }
//    glBindVertexArray(0);
//    glBindTexture(GL_TEXTURE_2D, 0);
// }
