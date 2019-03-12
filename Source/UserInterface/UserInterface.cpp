#include "UserInterface/UserInterface.h"
#include "GameStats.h"
#include "EntityManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "SoundManager.h"


/***********\
 * Defines *
\***********/
#define FONT_HEIGHT             48                      // 48 Pixels High.
#define BITMAP_HEIGHT           512
#define BITMAP_WIDTH            512
#define MAX_ASCII_CHARS         255
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

#define COLOR_READY             COLOR_GREEN
#define COLOR_MID_READY         COLOR_YELLOW
#define COLOR_NOT_READY         COLOR_RED

#define COOLDOWN_READY          "Ready"
#define COOLDOWN_DECIMAL_PLACES 1
#define SCORE_SCALE             1.0f
#define SCORE_COLOR             COLOR_WHITE

// Game time
#define SECONDS_PER_MINUTE      60
/*
This determines the length of time of a single round.
The timer will begin at this time and count down.

Unit : seconds
*/
#define ROUND_TIME              5 * SECONDS_PER_MINUTE

/*
All user interface components have a location relative to the window dimensions
to ensure that the UI scales with window resizes.

Coordinate system:

(0, 1)                  (1, 1) 

(x=0, y=0)              (1, 0)
*/
#define TIME_SCALE              1.0f
#define TIME_COLOR              COLOR_WHITE

#define TRAIL_SCALE             1.0f

#define ROCKET_SCALE            1.0f

#define SPIKES_SCALE            1.0f

#define DASH_SCALE              1.0f

#define SCORE_CHANGE_DURATION   1.0f
#define SCORE_CHANGE_SCALE      1.0f
#define SCORE_CHANGE_ADD_COLOR  COLOR_GREEN
#define SCORE_CHANGE_SUB_COLOR  COLOR_RED

#define MESSAGE_DURATION        3.0f
#define MESSAGE_SCALE           1.0f
#define MESSAGE_COLOR           COLOR_WHITE

/*************\
 * Constants *
\*************/
const string ANDROID_FONT("fonts/Android.ttf");
const string ARIAL_FONT("fonts/arial.ttf");
const string VANADINE_FONT("fonts/Vanadine Bold.ttf");
const string ASTRON_BOY_REGULAR_FONT("fonts/astron-boy.regular.ttf");
const string FUTURE_OUTRUN_FONT("fonts/future.outrun-future.otf");

/*
The default font to use for the UI components
*/
const string  DEFAULT_FONT = ASTRON_BOY_REGULAR_FONT;

const float F_BITMAP_HEIGHT = static_cast<float>(BITMAP_HEIGHT);
const float F_BITMAP_WIDTH = static_cast<float>(BITMAP_WIDTH);

// Singleton instance
UserInterface* UserInterface::m_pInstance = nullptr;

UserInterface::UserInterface(int iWidth, int iHeight)
{
    // Get Singleton Handles
    m_pShdrMngr = SHADER_MANAGER;

    updateWidthAndHeight(iWidth, iHeight);

    setDisplayCount(1);

    initFreeType();
    initializeVBOs();

    m_fGameTime = ROUND_TIME;
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
    m_pShdrMngr = nullptr;

    // Clean up VBO and VAO
    glDeleteBuffers(1, &m_iVertexBuffer);
    glDeleteTextures(1, &m_iTextureBuffer);
    glDeleteVertexArrays(1, &m_iVertexArray);
}

// Initializes The FreeType Library and Fonts as well as any other Characters necessary for displaying UI
void UserInterface::initFreeType()
{
    // Local Variables
    bool bLoaded = true;                // Determines if the Font was properly loaded or not.
    FT_Library  ftLibrary;              // FreeType Data Structures
    FT_Face     ftFace;
    GLsizei fHeight(0), fWidth(0);      // Height and Width Trackers for generating the Bitmap
    vec2 vOffsets = vec2(0.0f);         // Compute Offsets for UVs every entry.
    char cData[BITMAP_WIDTH * BITMAP_HEIGHT] = { '\0' };   // character array buffer for storing bitmap.
    char* cPtr = cData;                 // Pointer for inserting Data into the buffer.

    // Initialize FreeType Library and Default Font: Each function returns a non-zero integer when an error occurs.
    bLoaded = (0 == FT_Init_FreeType(&ftLibrary));
    if (!bLoaded )
        cout << "ERROR: Freetype: could not initialize FreeType Library for UI.\n";

    // Initialize Default Face
    bLoaded &= (0 == FT_New_Face(ftLibrary, DEFAULT_FONT.c_str(), 0, &ftFace));
    if (!bLoaded )
        cout << "ERROR: Freetype: failed to load \"" << DEFAULT_FONT << "\" for UI.\n";

    // Successfully loaded -> Finish loading the rest of the Library.
    if (bLoaded)
    {
        FT_Set_Pixel_Sizes(ftFace, 0, FONT_HEIGHT);   // Set the Font Width to 0 to automatically adjust width based on height.

        for (GLubyte c = 0; c < MAX_ASCII_CHARS; ++c)
        {
            // Load Character Glyph
            if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
            {
                cout << "ERROR: FreeType: Failed to load Glyph (\'" << c << "\')\n";
                continue;
            }
            else if (ftFace->glyph->bitmap.buffer != nullptr)
            {
                // Concat the resulting bitmap to a specified width.
                if (fWidth + ftFace->glyph->bitmap.width > BITMAP_WIDTH)
                {
                    fWidth = 0;             // Overflow? reset Width to 0 for next line
                    vOffsets.x = 0.0f;      // Set Offset x back to 0
                    vOffsets.y += static_cast<float>(fHeight);  // Set Offset y to next row
                    cPtr = cData + (BITMAP_WIDTH * static_cast<unsigned int>(vOffsets.y)); // Reset pointer to next line in buffer.
                    fHeight = 0;            // Reset Height for new pass.
                }

                // Probe to see how far the bitmap will write into the buffer.
                char* cTest = (cPtr + ftFace->glyph->bitmap.width + (BITMAP_WIDTH * ftFace->glyph->bitmap.rows));

                // If it won't go out of bounds, save to buffer
                if (cTest - cData < (BITMAP_HEIGHT * BITMAP_WIDTH))
                    addBitmapToBuffer(&ftFace->glyph->bitmap, cPtr);
                else    // If it will go out of bounds, escape and input an error.
                {
                    cout << "ERROR: FreeType: Could not construct proper Bitmap, Buffer went out of bounds.\n";
                    break;
                }

                // Fast Forward to next Glyph position in buffer.
                cPtr += ftFace->glyph->bitmap.width;
            }

            // Store Character in CharacterMap
            addNewCharacter(c, ftFace->glyph, &vOffsets);

            // Update tracking information for bitmaps.
            vOffsets.x += ftFace->glyph->bitmap.width;
            fWidth += ftFace->glyph->bitmap.width;
            fHeight = (ftFace->glyph->bitmap.rows > static_cast<unsigned int>(fHeight) ? ftFace->glyph->bitmap.rows : fHeight);
        }

        // Generate Bitmap Texture
        glGenTextures(1, &m_iTextureBuffer);
        glBindTexture(GL_TEXTURE_2D, m_iTextureBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, BITMAP_WIDTH, BITMAP_HEIGHT,
            0, GL_RED, GL_UNSIGNED_BYTE, cData);
        
        // Set Texture Options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    // Clean up FreeType after finished processing the glyphs
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLibrary);
}

// Given a FreeType Bitmap, add the buffer to a character Buffer at the given pointer.
//  This function is private and should only be called after ensuring that the bitmap will not write out of
//  the bounds of the character buffer.
void UserInterface::addBitmapToBuffer(const FT_Bitmap* pBitmap, char* cPtr)
{
    // Add Glyph Bitmap to larger Bitmap
    for (unsigned int y = 0; y < pBitmap->rows; ++y) // Iterate through rows
    {
        for (unsigned int x = 0; x < pBitmap->width; ++x) // Iterate through pixels within row
        {
            // Add the pixel to the buffer at the proper index.
            *(cPtr++) = pBitmap->buffer[(y * pBitmap->width) + x];
        }
        // Jump to the next row in the data buffer, offset by the glyph width for a new row.
        cPtr += BITMAP_WIDTH - pBitmap->width;
    }
}

// Generates a New Character struct for a given character and associated glyph, then adds the entry into
//  the character map.
void UserInterface::addNewCharacter(char c, const FT_GlyphSlotRec_* pGlyph, const vec2* vOffsets)
{
    // Generate Texture - Deal with Textures locally for simplicy as they don't need to be generalized yet.
    Character cNewChar;

    // Set up rest of Character Structure
    cNewChar.uvOffset = vec2(vOffsets->x / F_BITMAP_WIDTH,                                      // UV Offset for Bitmap
                             vOffsets->y / F_BITMAP_HEIGHT);
    cNewChar.uvSize = vec2(static_cast<float>(pGlyph->bitmap.width) / F_BITMAP_WIDTH,    // Size of UV section
                           static_cast<float>(pGlyph->bitmap.rows) / F_BITMAP_HEIGHT);
    cNewChar.size = ivec2(pGlyph->bitmap.width, pGlyph->bitmap.rows);     // Pixel Size
    cNewChar.bearing = ivec2(pGlyph->bitmap_left, pGlyph->bitmap_top);    // Bearing Information
    cNewChar.advance = pGlyph->advance.x;                                        // Glyph Advance information

    // Store Character for Later use.
    m_pCharacters.insert(make_pair(c, cNewChar));
}

void UserInterface::initializeVBOs()
{
    // Generate Vertex Array
    glGenVertexArrays(1, &m_iVertexArray);
    // dynamic draw, keep this ready, because we will draw this alot
    m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, nullptr, sizeof(vec4), GL_DYNAMIC_DRAW);    // Generate the Vertex Buffer and store some space on the GPU for Text Rendering.

    // chunk size  4 - internally treats each section as a float, it goes through 4 floats in each cunk
    // 
    // offset is 0
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

    // Update and store all component coordinates
    for (int component = 0; component < COMPONENT_COUNT; component++)
    {
        m_vComponentCoordinates[component][X] = m_iWidth * m_vComponentScaling[component][X];
        m_vComponentCoordinates[component][Y] = m_iHeight * m_vComponentScaling[component][Y];
    }
}

/*
Display a message for a givne hovercraft's UI for a short duration.
Depending on the message type, a sound may be played, and an alternate message
may be played for other displayed UI's.

@param attacker     of kill
@param hit          of kill
@param message      to display
*/
void UserInterface::displayMessage(eHovercraft attacker, eHovercraft hit, eKillMessage message)
{
    switch (message)
    {
    case KILL_MESSAGE_DOMINATION:
        SOUND_MANAGER->play(SoundManager::SOUND_KILL_DOMINATION);
        displayMessage(attacker, "You are now dominating Player " + std::to_string(hit + 1));
        displayMessage(hit, "Player " + std::to_string(attacker + 1) + " is now dominating you");
        break;
    case KILL_MESSAGE_FIRST_BLOOD:
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_KILL_FIRST_BLOOD);
        displayMessage(attacker, "You got first blood against Player " + std::to_string(hit + 1));
        break;
    case KILL_MESSAGE_REVENGE:
        SOUND_MANAGER->play(SoundManager::SOUND_KILL_REVENGE);
        displayMessage(attacker, "You got revenge from Player " + std::to_string(hit + 1));
        displayMessage(hit, "Player " + std::to_string(attacker + 1) + " got revenge from you");
        break;
    case KILL_MESSAGE_KILLSTREAK:
        SOUND_MANAGER->play(SoundManager::SOUND_KILL_STREAK);
        displayMessage(attacker, "You have a killstreak of " + std::to_string(GAME_STATS->get(attacker, GameStats::eStat::KILLSTREAK_CURRENT)));
        break;
    case KILL_MESSAGE_KILL:
        m_fScoreChangeTimes[attacker] = SCORE_CHANGE_DURATION;
        m_fScoreChangeTimes[hit] = SCORE_CHANGE_DURATION;
        break;
    }
}

/*
Display a message for a given hovercraft's UI for a short duration.

@param hovercraft   to display message for. This message is only displayed if
                    this hovercraft's UI is displayed.
@param text         to display
*/
void UserInterface::displayMessage(eHovercraft hovercraft, std::string text)
{
    m_sMessages[hovercraft] = text;
    m_fMessageTimes[hovercraft] = MESSAGE_DURATION;
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
        // updateScores();
        // updateCooldowns();
    }
}

/*
Renders the most recently updated state to the screen.
This this be called every render update, after the environment has been
rendered to ensure the UI is on top.
*/
void UserInterface::render()
{
    // renderText("Hello World!", 250.0f, 250.0f, 1.0f, vec3(1.0f));
    renderGameTime();
    renderScores();
    renderCooldowns();
    renderMessages();
}

/*

*/
void UserInterface::updateGameTime(float fSecondsSinceLastUpdate)
{

    m_fGameTime -= fSecondsSinceLastUpdate;

    for (int player = 0; player < m_iDisplayCount; player++)
    {
        m_fMessageTimes[player] -= fSecondsSinceLastUpdate;
        m_fScoreChangeTimes[player] -= fSecondsSinceLastUpdate;
    }
    // TODO make sure time does not become negative, or if it does, it signifies
    // the end of the round. Not sure if its worth the cost to check.

}

/*
The time is formatted as

        minutes:seconds

For now, the game time is going up from 0. Later this should count down.
*/
void UserInterface::renderGameTime()
{
    renderText(timeToString(),
               m_vComponentCoordinates[COMPONENT_TIME][X],
               m_vComponentCoordinates[COMPONENT_TIME][Y],
               TIME_SCALE, TIME_COLOR);

}

/*
This is calculated in renderGameTime() since there is no reason to calculated
it more than every render update (ie. no reason to update it every game update)
*/
std::string UserInterface::timeToString()
{
    int total = (int) m_fGameTime;
    int seconds = total % SECONDS_PER_MINUTE;
    int minutes = (total / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    return std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
}

void UserInterface::renderMessages()
{
    for (int player = 0; player < m_iDisplayCount; player++)
    {
        if (m_fMessageTimes[player] > 0)
        {
            renderText(m_sMessages[player],
                m_vComponentCoordinates[COMPONENT_MESSAGE][X],
                m_vComponentCoordinates[COMPONENT_MESSAGE][Y],
                MESSAGE_SCALE, MESSAGE_COLOR);
        }
        if (m_fScoreChangeTimes[player] > 0)
        {

            int scoreChange = GAME_STATS->get(static_cast<eHovercraft>(player),
                                              GameStats::eStat::SCORE_CHANGE);
            bool scoreIncreased = scoreChange >= 0;
            renderText((scoreIncreased ? "+" : "") + std::to_string(scoreChange) ,
                        m_vComponentCoordinates[COMPONENT_SCORE_CHANGE][X],
                        m_vComponentCoordinates[COMPONENT_SCORE_CHANGE][Y],
                        SCORE_CHANGE_SCALE,
                        scoreIncreased ? SCORE_CHANGE_ADD_COLOR : SCORE_CHANGE_SUB_COLOR);
        }
    }
}

/*
Update the scores for all display count players
*/
void UserInterface::updateScores()
{
    for (int player = 0; player < m_iDisplayCount; player++)
    {
        updateScore(static_cast<eHovercraft>(player),
                    GAME_STATS->get(static_cast<eHovercraft>(player),
                    GameStats::SCORE_CURRENT));
    }
}

void UserInterface::updateScore(eHovercraft hovecraft, int score)
{
    
    // cout << "Player " << (player + 1) << " score: " << score << endl;
}

void UserInterface::renderScores()
{
    // TODO put this in the proper place, font, scale etc.
    // Ad hoc for single player
    std::string score = std::to_string(GAME_STATS->get(HOVERCRAFT_PLAYER_1,
                        GameStats::eStat::SCORE_CURRENT));
    renderText("Score: " + score,
               m_vComponentCoordinates[COMPONENT_SCORE][X],
               m_vComponentCoordinates[COMPONENT_SCORE][Y],
               SCORE_SCALE, SCORE_COLOR);
}

void UserInterface::updateCooldowns()
{
}

void UserInterface::renderCooldowns()
{
    // TODO put this in the proper place, font, scale etc.
    // This formatting is all temporary
    // 0 - 100
    // Ad hoc for single player
    HovercraftEntity* player = ENTITY_MANAGER->getPlayer(HOVERCRAFT_PLAYER_1);
    float* cooldowns = player->getCooldowns();
    float trailPercent = player->getTrailGaugePercent();
    std::string trailPercentString = std::to_string((int) (trailPercent * 100));
    vec3 trailColor = trailPercent == 1.0 ?
        COLOR_READY : trailPercent == 0.0 ?
        COLOR_NOT_READY : COLOR_MID_READY;
    renderText("Flame: " + trailPercentString + "%",
               m_vComponentCoordinates[COMPONENT_TRAIL][X],
               m_vComponentCoordinates[COMPONENT_TRAIL][Y],
               TRAIL_SCALE,
               trailColor);
    renderCooldown("Rocket",
                   eCooldown::COOLDOWN_ROCKET,
                   cooldowns,
                   m_vComponentCoordinates[COMPONENT_ROCKET][X],
                   m_vComponentCoordinates[COMPONENT_ROCKET][Y],
                   ROCKET_SCALE);
    renderCooldown("Spikes" + std::string(player->hasSpikesActivated() ? " enabled" : ""),
                   eCooldown::COOLDOWN_SPIKES,
                   cooldowns,
                   m_vComponentCoordinates[COMPONENT_SPIKES][X],
                   m_vComponentCoordinates[COMPONENT_SPIKES][Y],
                   SPIKES_SCALE);
    renderCooldown("Dash",
                   eCooldown::COOLDOWN_DASH,
                   cooldowns,
                   m_vComponentCoordinates[COMPONENT_DASH][X],
                   m_vComponentCoordinates[COMPONENT_DASH][Y],
                   DASH_SCALE);

    //  renderImage(IMAGE_TRAIL, 0, 0, 10);
}

void UserInterface::renderCooldown(std::string label,
                                   eCooldown cooldown,
                                   float* cooldowns,
                                   GLfloat x, GLfloat y, GLfloat scale)
{
    bool isReady = cooldowns[cooldown] == 0;
    std::string cooldownString = isReady ? COOLDOWN_READY : FuncUtils::to_string(cooldowns[cooldown], COOLDOWN_DECIMAL_PLACES) + "s";
    vec3 color = isReady ? COLOR_READY : COLOR_NOT_READY;
    renderText(label + ": " + cooldownString, x, y, scale, color);
}


/*

*/
void UserInterface::renderComponent(eUIComponent component, GLfloat scale, vec3 color)
{
    /* TODO */
}
void UserInterface::renderText(int text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
    renderText(std::to_string(text), x, y, scale, color);
}
/*
Render text to the screen.

Window coordinates in pixels
(0, height)     (width, height)


(0, 0)          (width, 0)

@param text     to render
@param x        x-coordinate of the bottom-left corner of text, in pixels
@param y        y-coordinate of the bottom-left corner of text, in pixels
@param scale    text, where 1.0 is the default size
@param color    rgb colors of the text
*/
void UserInterface::renderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
    // Vector for storing VBO data.
    vector<vec4> vTextOutput;

    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::UI_SHDR));
    m_pShdrMngr->setUniformVec3(ShaderManager::eShaderType::UI_SHDR, "textColor", &color);
    // m_pShdrMngr->setUniformBool()// shader, name, value

    // Bind Texture.
    glActiveTexture(GL_TEXTURE0 + m_iTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, m_iTextureBuffer);
    SHADER_MANAGER->setUniformInt(ShaderManager::eShaderType::UI_SHDR, "text", m_iTextureBuffer);

    // TODO
    // Need a different shader for images
    // Change the texture class to store height and width, loaded dynamically
    // Create a quad similar to text
    // Iterate through all Characters
    string::const_iterator c;
    for (c = text.begin(); c != text.end(); ++c)
    {
        // Grab Character Glyph information
        Character ch = m_pCharacters[*c];

        // Calculate Position offset by the bearings of the glyph
        // x and y in screen space
        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        // Calculate width and height of the glyph
        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        // Generate Information for 4 Corners for readability and less computation
        vec4 vCorners[4] = {
            vec4(xpos,      ypos,       ch.uvOffset.x,                  ch.uvOffset.y + ch.uvSize.y),
            vec4(xpos + w,  ypos,       ch.uvOffset.x + ch.uvSize.x,    ch.uvOffset.y + ch.uvSize.y),
            vec4(xpos,      ypos + h,   ch.uvOffset.x,                  ch.uvOffset.y),
            vec4(xpos + w,  ypos + h,   ch.uvOffset.x + ch.uvSize.x,    ch.uvOffset.y)
        };

        // TODO, use triangle strip instead to reduce vertices to 4
        // Update VBO for each character
        // Triangle 1:
        /*
            2
            |\
            | \ 
            |  \
            0---1
        */
        vTextOutput.push_back(vCorners[BOTTOM_LEFT]);
        vTextOutput.push_back(vCorners[BOTTOM_RIGHT]);
        vTextOutput.push_back(vCorners[TOP_LEFT]);

        // Triangle 2
        /*
            2---1
             \  |
              \ |
               \|
                0
        */
        vTextOutput.push_back(vCorners[BOTTOM_RIGHT]);
        vTextOutput.push_back(vCorners[TOP_RIGHT]);
        vTextOutput.push_back(vCorners[TOP_LEFT]);

        // Now advance the cursors for next glyph (note: advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // >> 6 == 1/64 (2^6 = 64)
    }

    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vTextOutput.size() * sizeof(vec4), vTextOutput.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render Quad
    glDrawArrays(GL_TRIANGLES, 0, vTextOutput.size());

    // Clean up OpenGL
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*
TODO Use hashmap for image instead of image filepath directly
Render image to the screen.

Window coordinates in pixels
(0, height)     (width, height)


(0, 0)          (width, 0)

@param filepath of image
@param x        x-coordinate of the bottom-left corner of text, in pixels
@param y        y-coordinate of the bottom-left corner of text, in pixels
@param scale    image, where 1.0 is the default size
*/
void UserInterface::renderImage(string filepath, GLfloat x, GLfloat y, GLfloat scale)
{
    // This implementation is close to renderText.

    // Vector for storing VBO data.
    vector<vec4> vTextOutput;

    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::UI_SHDR));
    // m_pShdrMngr->setUniformVec3(ShaderManager::eShaderType::UI_SHDR, "textColor", &color);
    // m_pShdrMngr->setUniformBool()// shader, name, value

    // Bind Texture.
    glActiveTexture(GL_TEXTURE0 + m_iTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, m_iTextureBuffer);
    SHADER_MANAGER->setUniformInt(ShaderManager::eShaderType::UI_SHDR, "text", m_iTextureBuffer);
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
