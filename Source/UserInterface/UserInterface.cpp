#include "UserInterface/UserInterface.h"
#include "ShaderManager.h"
#include "TextureManager.h"


/***********\
 * Defines *
\***********/
#define FONT_HEIGHT             48                      // 48 Pixels High.
#define VEC4_SIZE               (sizeof(GLfloat) << 2)  // Size of a single vertex
#define NUM_VERTS               6                       // Number of vertices to store per glyph

/*************\
 * Constants *
\*************/
const string ANDROID_FONT("fonts/Android.ttf");
const string ARIAL_FONT("fonts/arial.ttf");

// Singleton instance
UserInterface* UserInterface::m_pInstance = nullptr;

UserInterface::UserInterface(int iWidth, int iHeight)
{
    // Get Singleton Handles
    m_pGameStats = GameStats::getInstance();
    m_pShdrMngr = SHADER_MANAGER;

    m_iDisplayCount = 0;
    updateWidthAndHeight(iWidth, iHeight);

    initializeUserInterface();
    initFreeType();
    initializeVBOs();
}

UserInterface* UserInterface::getInstance(int iWidth, int iHeight)
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new UserInterface(iWidth, iHeight);
    }
    return m_pInstance;
}

/*
Assumes already instantiated beforehand.
*/
UserInterface* UserInterface::getInstance()
{
    assert(nullptr != m_pInstance);
    return m_pInstance;
}

UserInterface::~UserInterface()
{
    m_pGameStats = nullptr;
    m_pShdrMngr = nullptr;

    for (map<GLchar, Character>::const_iterator iter = m_pCharacters.begin();
        iter != m_pCharacters.end();
        ++iter)
        glDeleteTextures(1, &iter->second.textureID);
    m_pCharacters.clear();

    // Clean up VBO and VAO
    glDeleteBuffers(1, &m_iVertexBuffer);
    glDeleteVertexArrays(1, &m_iVertexArray);
}

// Initializes The FreeType Library and Fonts as well as any other Characters necessary for displaying UI
void UserInterface::initFreeType()
{
    // Local Variables
    bool bLoaded = true; // Determines if the Font was properly loaded or not.
    // FreeType Data Structures
    FT_Library  ftLibrary;
    FT_Face     ftFace;

    // Initialize FreeType Library and Default Font: Each function returns a non-zero integer when an error occurs.
    bLoaded = (0 == FT_Init_FreeType(&ftLibrary));
    if (!bLoaded )
        cout << "ERROR: Freetype: could not initialize FreeType Library for UI.\n";

    // Initialize Default Face
    bLoaded &= (0 == FT_New_Face(ftLibrary, ARIAL_FONT.c_str(), 0, &ftFace));
    if (!bLoaded )
        cout << "ERROR: Freetype: failed to load \"" << ARIAL_FONT << "\" for UI.\n";

    // Successfully loaded -> Finish loading the rest of the Library.
    if (bLoaded)
    {
        m_pFontBitmap = TEXTURE_MANAGER->loadTexture("textures/hud/trail.png");
        FT_Set_Pixel_Sizes(ftFace, 0, FONT_HEIGHT);   // Set the Font Width to 0 to automatically adjust width based on height.

        // Generate Textures for first 128 Characters
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // Disable byte-alignment restriction

        for (GLubyte c = 0; c < 128; ++c)
        {
            // Load Character Glyph
            if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
            {
                cout << "ERROR: FreeType: Failed to load Glyph (\'" << c << "\')\n";
                continue;
            }

            // Generate Texture - Deal with Textures locally for simplicy as they don't need to be generalized yet.
            Character cNewChar;
            glGenTextures(1, &cNewChar.textureID);
            glBindTexture(GL_TEXTURE_2D, cNewChar.textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows,
                         0, GL_RED, GL_UNSIGNED_BYTE, ftFace->glyph->bitmap.buffer);


            // Set Texture Options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Set up rest of Character Structure
            cNewChar.size       = ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows);
            cNewChar.bearing    = ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top);
            cNewChar.advance    = ftFace->glyph->advance.x;

            // Store Character for Later use.
            m_pCharacters.insert(make_pair(c, cNewChar));
        }
    }

    // Clean up FreeType after finished processing the glyphs
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLibrary);
}

void UserInterface::initializeVBOs()
{
    // Generate Vertex Array
    glGenVertexArrays(1, &m_iVertexArray);
    m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, nullptr, sizeof(vec4) * NUM_VERTS, GL_DYNAMIC_DRAW);    // Generate the Vertex Buffer and store some space on the GPU for Text Rendering.
    m_pShdrMngr->setAttrib(m_iVertexArray, 0, 4, sizeof(vec4), 0); // Set Attributes for the Buffer to let OpenGL know how to index the data.
}

/*
Update the window width and height due to window resizing.
*/
void UserInterface::updateWidthAndHeight(int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    // Update UI Projection Matrix Uniform for UI Shader
    mat4 m4UIProjection = ortho(0.0f, static_cast<GLfloat>(m_iWidth), 0.0f, static_cast<GLfloat>(m_iHeight), 0.0f, 1.0f);
    m_pShdrMngr->setUnifromMatrix4x4(ShaderManager::eShaderType::UI_SHDR, "UIProjection", &m4UIProjection);
}

/*
This visually updates the UserInterface to all value changes since last update.

Under the scenes, this retrieves all needed the values from GameStats and
displays them. This is why it does not need a time value in order to determine
time-sensitive information such as cooldown and game time.
@TODO this may change later on.

This should be called once per frame update.

*/
void UserInterface::update(float fSecondsSinceLastUpdate)
{
    if (m_iDisplayCount > 0)
    {
        // system("CLS");
        updateGameTime(fSecondsSinceLastUpdate);
        updateScores();
        updateCooldowns();
    }
}

/*

*/
void UserInterface::updateGameTime(float fSecondsSinceLastUpdate)
{

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
    // renderImage(IMAGE_TRAIL, 0, 0, 10);
}

void UserInterface::renderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::UI_SHDR));
    m_pShdrMngr->setUniformVec3(ShaderManager::eShaderType::UI_SHDR, "textColor", &color);
    glActiveTexture(GL_TEXTURE0);

    // Iterate through all Characters
    string::const_iterator c;
    for (c = text.begin(); c != text.end(); ++c)
    {
        Character ch = m_pCharacters[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        // Update VBO for each character
        GLfloat vertices[NUM_VERTS][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };

        // Render glyph texture over quad
        //m_pFontBitmap->bindTexture(ShaderManager::eShaderType::UI_SHDR, "text");

        glActiveTexture(GL_TEXTURE0 + ch.textureID);
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        SHADER_MANAGER->setUniformInt(ShaderManager::eShaderType::UI_SHDR, "text", ch.textureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render Quad
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);

        // Now advance the cursors for next glyph (note: advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // >> 6 == 1/64 (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void UserInterface::renderImage(string filepath, GLfloat x, GLfloat y, GLfloat scale)
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
