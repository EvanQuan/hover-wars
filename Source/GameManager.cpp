#include "EntityManager.h"
#include "GameManager.h"
#include "CommandHandler.h"
#include "ArtificialIntelligence/AIManager.h"
#include "SceneLoader.h"
#include "ShaderManager.h"
#include "UserInterface/StartInterface.h"
#include "Menus/PostgameMenu.h"
#include "GameStats.h"
#include "UserInterface/GameInterface.h"
#include "UserInterface/LoadingInterface.h"
#include "UserInterface/PostgameInterface.h"
#include "Menus/GameMenu.h"
#include "Menus/StartMenu.h"
#include "Menus/LoadingMenu.h"
#include "TextureManager.h"

// Unit: seconds
#define GAME_OVER_TIME 0.0f

// Time before the game resumes
#define GAME_RESUME_TIME 3.0f

// Corner Indices
#define BOTTOM_LEFT 0
#define BOTTOM_RIGHT 1
#define TOP_LEFT 2
#define TOP_RIGHT 3

// Blur
#define BLUR_AMOUNT 10

/*************\
 * Constants *
\*************/
const unsigned int FOUR_VEC4 = (sizeof(vec4) << 2);

const vec3 COLORS[MAX_HOVERCRAFT_COUNT]{
    // Player colours should be very distinct from each other and stand out
    vec3(1.0f, 0.5490196078431373f, 0.0f),                  // Orange
    vec3(1.0f, 0.4117647058823529f, 0.7058823529411765f),   // Pink
    vec3(0.1176470588235294f, 0.5647058823529412f, 1.0f),   // Dodger Blue (because pure blue is too dark, and makes flame difficult to see)
    vec3(1.0f, 0.0f, 0.0f),                                 // Red (because red fire is cool, and should be in every game)
    vec3(0.4862745098039216f,	0.9882352941176471f, 0.0f), // Lawn Green
    vec3(0.00f, 1.00f, 0.59f),                              // Spring Green
    vec3(1.0f, 1.0f, 0.0f),                                 // Yellow
    vec3(1.0f)                                              // White
};

const vec4 BLUR_QUAD[4]{
    vec4(-1.0f, -1.0f, 0.0f, 0.0f), /*Bottom Left*/
    vec4(1.0f,  -1.0f, 1.0f, 0.0f), /*Bottom Right*/
    vec4(-1.0f, 1.0f,  0.0f, 1.0f), /*Top Left*/
    vec4(1.0f,  1.0f,  1.0f, 1.0f), /*Top Right*/
};

// Singleton Variable initialization
GameManager* GameManager::m_pInstance = nullptr;

// Constructor - Private, only accessable within the Graphics Manager
GameManager::GameManager(GLFWwindow* rWindow)
{
    // Initialize and Get Shader and Environment Managers
    m_pShaderManager    = SHADER_MANAGER;
    m_pEntityManager    = ENTITY_MANAGER;

    m_pAIManager = AIManager::getInstance();

    // Fetch and update Window HxW
    m_pWindow = rWindow;
    glfwGetWindowSize(m_pWindow, &m_iWidth, &m_iHeight);
    m_pEntityManager->updateWidthAndHeight(m_iWidth, m_iHeight);
    m_pGameStats = nullptr;

    // Initialize Timer Variables
    m_fFrameTime = duration<float>(0.0f);
    m_fMaxDeltaTime = sixtieth_of_a_sec{ 1 };

    m_eKeyboardHovercraft = HOVERCRAFT_PLAYER_1;

    m_pCommandHandler = COMMAND_HANDLER;

    m_pPhysicsManager = PHYSICS_MANAGER;

    m_pSoundManager = SOUND_MANAGER;

    // Generate VAO and VBO for rendering SplitScreen Quads
    glGenVertexArrays(1, &m_iVertexArray);
    m_iVertexBuffer = m_pShaderManager->genVertexBuffer(m_iVertexArray, nullptr, (sizeof(vec4) << 4), GL_STATIC_DRAW);
    m_pShaderManager->setAttrib(m_iVertexArray, 0, 4, sizeof(vec4), nullptr);

    // Generate VAO and VBO for Blur Rendering Quad
    glGenVertexArrays(1, &m_iBlurVAO);
    m_iBlurVBO = m_pShaderManager->genVertexBuffer(m_iBlurVAO, BLUR_QUAD, (sizeof(vec4) << 2), GL_STATIC_DRAW);
    m_pShaderManager->setAttrib(m_iBlurVAO, 0, 4, sizeof(vec4), nullptr);

    // Generate VAO for rendering Map items
    glGenVertexArrays(1, &m_iMapVAO);
}

/*
Singleton Implementations
Requires Window to initialize 

@param rWindow to intialize
*/
GameManager* GameManager::getInstance(GLFWwindow *rWindow)
{
    if (nullptr == m_pInstance)
        m_pInstance = new GameManager(rWindow);

    return m_pInstance;
}

// Does not require window parameter, but assumes that Singleton instance has
// been instantiated beforehand.
GameManager* GameManager::getInstance()
{
    assert(nullptr != m_pInstance);
    return m_pInstance;
}

// Destruct Shaders, Buffers, Arrays and other GL stuff.
GameManager::~GameManager()
{
    // Let go of Window Handle
    m_pWindow = nullptr;

    // Clean up Frame Buffers
    for each (sRenderBlock pRenderBlock in m_pFrameBufferTextures)
    {
        // Delete Render and Frame Buffers
        glDeleteRenderbuffers(1, &pRenderBlock.iRenderBuffer);
        glDeleteFramebuffers(1, &pRenderBlock.iFrameBuffer);
        for (sRenderBlock::sPingPongBuffer pBuffer : pRenderBlock.pPingPongBuffers)
            glDeleteFramebuffers(1, &pBuffer.iFBO);
    }
    m_pFrameBufferTextures.clear();

    // Split Screen Clean Up
    glDeleteBuffers(1, &m_iVertexBuffer);
    glDeleteVertexArrays(1, &m_iVertexArray);

    // Map Clean up
    glDeleteBuffers(1, &m_iMapVBO);
    glDeleteVertexArrays(1, &m_iMapVAO);

    // Blur Clean up
    glDeleteBuffers(1, &m_iBlurVBO);
    glDeleteVertexArrays(1, &m_iBlurVAO);

    // Clean up Allocated Memory
    // NOTE: crash at glDeleteBuffers in destructor
    if (nullptr != m_pEntityManager)    // Entity Manager
        delete m_pEntityManager;

    if (nullptr != m_pShaderManager)    // Shader Manager
        delete m_pShaderManager;

    // User Interface
    for (UserInterface* ui : m_vInterfaceInstances) {
        delete ui;
    }
    m_vInterfaceInstances.clear();

    if (nullptr != m_pCommandHandler)   // Command Handler
        delete m_pCommandHandler;

    if (nullptr != m_pAIManager)        // AI Manager
        delete m_pAIManager;

    if (nullptr != m_pPhysicsManager)
        delete m_pPhysicsManager;

    if (nullptr != m_pGameStats)        // Game Stats
        delete m_pGameStats;
}


/*
    As UserInterface instances are generated, they are each added to
    the m_vInterfaceInstances list.
    At the end of the program, all interfaces are deleted.
*/
void GameManager::addInterface(UserInterface* ui)
{
    m_vInterfaceInstances.push_back(ui);
}
void GameManager::setCurrentInterface(PromptInterface* ui)
{
    m_pMenuInterface = ui;
}
/*
    Start rendering game to screen. This call with block until the game loop
    ends (it will hang the thread). When this function returns, the program
    should end.
*/
void GameManager::startRendering()
{
    m_pSoundManager->start();
    resetTime();

    while (renderGraphics());
}
/*
    Render the graphics of a single frame to the screen.
    Intended to be called every cycle, or when the graphics need to be updated

    @return true if the game should continue to run. In other words,
            renderGraphics() should continue to be called within the rendering
            loop.
*/
bool GameManager::renderGraphics()
{
    updateTime();
    // Execute all commands for this frame
    // These should be done before the EntityManager updates so that the
    // environment can respond to the commands issued this frame.
    m_pCommandHandler->update(m_fFrameDeltaTime);
    m_pMenuInterface->update(m_fFrameDeltaTime);
    checkIfStartedGameOver();

    if (m_bInGame)
    {
        updateInGame();
    }

    drawScene();

    // check for Window events
    glfwPollEvents();

    return !glfwWindowShouldClose(m_pWindow);
}

/*
    Calculate the height and width of the screens depending on the number of
    players in the game.
*/
void GameManager::calculateScreenDimensions(unsigned int playerCount)
{
    // Figure out Frame Buffer Sizes for multiple players
    m_iSplitHeight = m_iHeight;
    m_iSplitWidth = m_iWidth;
    if (playerCount > 1)
        m_iSplitHeight >>= 1;
    if (playerCount > 2)
        m_iSplitWidth >>= 1;

    // Set up EntitManager for Camera Frustums
    m_pEntityManager->updateWidthAndHeight(m_iSplitWidth, m_iSplitHeight);
}

/*
    Spawn hovercrafts of the specified player and bot count. Depending on the
    AI type, the bots may share a team color.
*/
void GameManager::spawnHovercrafts(unsigned int playerCount, unsigned int botCount, eGameMode gameMode)
{
    // Local Variables
    bool bTeamAI = (GAMEMODE_TEAM_AI_SOLO_PLAYERS == gameMode || GAMEMODE_TEAMS_AI_VS_PLAYERS == gameMode);
    bool bTeamPlayers = (GAMEMODE_TEAMS_AI_VS_PLAYERS == gameMode);
    bool bSplitTeamPlayers = (GAMEMODE_TEAMS_PLAYERS == gameMode);
    vector< vec3 > vColors(COLORS, COLORS + sizeof(COLORS) / sizeof(COLORS[0]));    // Load all colors into a set to pick from.

    m_vPlayerColors.clear();
    if (bSplitTeamPlayers)
    {
        selectColors(&m_vPlayerColors, &vColors, 2, true);
        selectColors(&m_vPlayerColors, &vColors, 2, true);
        // Bots are forced to a team
        // Select Bot Colors
        // Clear Colors to set.
        m_vBotColors.clear();
        selectColors(&m_vBotColors, &vColors, botCount, true);
    }
    else
    {
        // Select Player Colors First
        // Clear Colors to set.
        selectColors(&m_vPlayerColors, &vColors, playerCount, bTeamPlayers);
        // Select Bot Colors
        // Clear Colors to set.
        m_vBotColors.clear();
        selectColors(&m_vBotColors, &vColors, botCount, bTeamAI);
    }


    // Spawn the Bots and Players with their respective colors
    spawnPlayers(playerCount, m_vPlayerColors);
    spawnBots(botCount, m_vBotColors);
}

// Name: selectColors
// Written by: James Coté
// Description: Applies randomization algorithm to select random colors from a pool of colors.
void GameManager::selectColors(vector<vec3>* vReturnColors, vector<vec3>* vColorPool, unsigned int iNumPicks, bool bTeam)
{
    // Local Variables
    unsigned int iIndex;

    if (bTeam) // If the on the same team, they will all use the same chosen color
    {
        // Select Color
        iIndex = rand() % vColorPool->size();

        // Save Colors
        for (unsigned int i = 0; i < iNumPicks; ++i)
            vReturnColors->push_back((*vColorPool)[iIndex]);

        // Erase Color from the pool
        vColorPool->erase(vColorPool->begin() + iIndex);
    }
    else // If solo, they wil use the remaining unused colors.
    {
        for (unsigned int i = 0; i < iNumPicks; ++i)
        {
            // Get a random Index
            iIndex = rand() % vColorPool->size();

            // Store Color
            vReturnColors->push_back((*vColorPool)[iIndex]);

            // Remove Chosen Color for next pick.
            vColorPool->erase(vColorPool->begin() + iIndex);
        }
    }
}

void GameManager::spawnPlayers(unsigned int playerCount, const vector<vec3> &colors)
{
    for (unsigned int i = 0; i < playerCount; i++) {
        m_vPositions.push_back(SCENE_LOADER->createPlayer(i, &colors[i]));
        m_vColors.push_back(colors[i]);
        generateSplitScreen(i);
    }
}

void GameManager::spawnBots(unsigned int botCount, const vector<vec3> &colors)
{
    for (unsigned int i = 0; i < botCount; i++) {
        m_vPositions.push_back(SCENE_LOADER->createBot(&colors[i]));
        m_vColors.push_back(colors[i]);
    }
}

void GameManager::setupMapData()
{
    vector< vec3 > vCombinedData = m_vPositions;
    vCombinedData.insert(vCombinedData.end(), m_vColors.begin(), m_vColors.end());
    m_iMapVBO = m_pShaderManager->genVertexBuffer(m_iMapVAO,
                                                  vCombinedData.data(),
                                                  (vCombinedData.size() * sizeof(vec3)),
                                                  GL_DYNAMIC_DRAW);
    m_pShaderManager->setAttrib(m_iMapVAO, 0, 3, 0, nullptr);
    m_pShaderManager->setAttrib(m_iMapVAO, 1, 3, 0, (void*)(m_vPositions.size() * sizeof(vec3)));
    glBindVertexArray(0);
}

/*
    Update all time values for a given render update.
*/
void GameManager::updateTime()
{
    m_pTimer.updateTimeSinceLastFrame();
    m_fFrameDeltaTimePrecise = m_pTimer.getFrameTimeSinceLastFrame();
    m_fFrameTime += m_fFrameDeltaTimePrecise;
    m_fFrameDeltaTime = static_cast<float>(m_fFrameDeltaTimePrecise.count());
}

/*
    If the game has initiated game, either by time out, or the user ending the
    game manually from the pause menu, then the game will "start" game over.
    There may be some lag time between starting game over and ending the game.
*/
void GameManager::checkIfStartedGameOver()
{
    if (!startedGameOver && (m_fGameTime < 0))
    {
        startedGameOver = true;
    }
}

/*
    Update the game environment if in the game.
    This should only be called if in the game as it requires all the in game
    entities to already be instantiated.
*/
void GameManager::updateInGame()
{
    updateGameTime(m_fFrameDeltaTime);
    if (!m_bPaused)
    {
        updateEnvironment();
    }
    checkIfShouldEndGame();
}

/*
    The game should end if the startedGameOver flag has been enabled.
    Once enabled, there is a buffer time until the game actually ends.
*/
void GameManager::checkIfShouldEndGame()
{
    if (startedGameOver)
    {
        // Decrease real time
        m_fGameOverTime -= m_fFrameDeltaTime;
        if (m_fGameOverTime <= 0)
        {
            endGame();
        }
        else
        {
            // Do whatever you want for this duration.
            // Slow mo?
            // Music change/fade?
        }
    }
}

/*
    Once in game, calling this will update the environment of the game.
    If the game is paused, yet the environment is still rendering, then this
    should not be called.
*/
void GameManager::updateEnvironment()
{
    m_pAIManager->update(m_fFrameDeltaTime);
    m_pEntityManager->updateEnvironment(m_fFrameDeltaTimePrecise);

    // Sound needs to update after the EntityManager to reflect in game changes
    // Cannot be updated inside the EntityManager as sounds can play while game
    // is paused.
    m_pSoundManager->update();
    // The game interface should update after the EntityManager and
    // CommandHandler has changed in order to reflect their changes.
    // It also cannot update inside the EntityManager since it is able
    // to be updated while the EntityManager is paused.
    m_pGameInterface->update(m_fFrameDeltaTime);
}

/*
    Initialize everything necessary to start a new game.

    @param playerCount  player hovercrafts to register
    @param botCount     bot hovercrafts to register
    @param botDifficulty of bots
    @param gameTime     of game, in seconds
    @param eGameMode    of game
    @param mapNumber    of map
*/
void GameManager::initializeNewGame(unsigned int playerCount,
                                    unsigned int botCount,
                                    eBotDifficulty botDifficulty,
                                    float gameTime,
                                    eGameMode gameMode,
                                    unsigned int mapNumber,
                                    bool scoreLossEnabled)
{
    // Before we initialize, set to LoadingMenu and Interface
    m_pCommandHandler->setCurrentMenu(LoadingMenu::getInstance());

    // We intialize all values for the game to immediately start
    m_pPhysicsManager->initPhysics(true);

    string sFileName;
    switch (mapNumber)
    {
    case MAP_1_NUMBER:
        sFileName = MAP_1;
        break;
    case MAP_2_NUMBER:
        sFileName = MAP_2;
        break;
    default:
        sFileName = MAP_1;
    }

    startedGameOver = false;
    m_fGameTime = gameTime;
    m_bPaused = true;
    m_bInGame = true;
    m_fGameOverTime = GAME_OVER_TIME;
    m_pGameInterface = GameInterface::getInstance(m_iWidth, m_iHeight);
    m_pGameInterface->reinitialize(gameTime);
    m_pGameInterface->setDisplayCount(playerCount);
    TextureManager* pTxtMngr = TEXTURE_MANAGER;
    m_pEntityManager->initializeEnvironment(sFileName);

    calculateScreenDimensions(playerCount);

    spawnHovercrafts(playerCount, botCount, gameMode);

    setupMapData();

    // AFTER the players and bots have been made, the GameStats and AI
    // need to reinitialize to track the players and bots
    m_pGameStats->reinitialize(playerCount, botCount, gameMode, botDifficulty, scoreLossEnabled);
    m_pAIManager->reinitialize(gameMode, mapNumber);

    setKeyboardHovercraft(playerCount);

    m_pSoundManager->play(SoundManager::eSoundEvent::SOUND_HOVERCAR_ENGINE);

    // Only after everything has loaded, switch to the game menu.
    // Don't need to switch to GameInterface, as the GameInterface is directly
    // rendered for each player.
    m_pCommandHandler->setCurrentMenu(GameMenu::getInstance());
}

/*
    Generate each players' screen for split-screen multiplayer.
    If there is only 1 player, that player will take the entire screen.
    If there are 2 playere, the screen will split horizontally in half.
    If there are 3 or 4 players, the screen will split into 4 quadrants.

    @param iPlayer  to generate screen for. Value should correspond to
                    eHovecraft value.
*/
void GameManager::generateSplitScreen(unsigned int iPlayer)
{
    // Local Variables
    vec4 vCorners[4];
    memcpy(vCorners, BLUR_QUAD, (sizeof(vec4) << 2));

    /*
        (0, 1)-----(0.5, 1)------(1, 1)
          |            |           |
        (0, 0.5)---(0.5, 0.5)----(1, 0.5)
          |            |           |
        (0, 0)-----(0.5, 0)------(1, 0)
    */
    // Modify Screen Space Data
    switch (iPlayer)
    {
    case 0:
        if (m_iSplitHeight != m_iHeight)    // > 1 Player? -> compact the frame to half height
            vCorners[BOTTOM_LEFT].y = vCorners[BOTTOM_RIGHT].y = 0.0f;
        if (m_iSplitWidth != m_iWidth)      // > 2 Players? -> compact the frame to half width
            vCorners[BOTTOM_RIGHT].x = vCorners[TOP_RIGHT].x = 0.0f;
        break;
    case 1:
        if (m_iSplitWidth != m_iWidth)      // > 2 Players? compact frame to top right quadrant
        {
            vCorners[BOTTOM_LEFT].y = vCorners[BOTTOM_RIGHT].y = 0.0f;
            vCorners[BOTTOM_LEFT].x = vCorners[TOP_LEFT].x = 0.0f;
        }
        else                                // exactly 2 players? compact frame to bottom half
            vCorners[TOP_LEFT].y = vCorners[TOP_RIGHT].y = 0.0f;
        break;
    case 2:                                 // Compact to bottom left quadrant
        vCorners[TOP_LEFT].y = vCorners[TOP_RIGHT].y = 0.0f;
        vCorners[TOP_RIGHT].x = vCorners[BOTTOM_RIGHT].x = 0.0f;
        break;
    case 3:                                 // Compact to bottom right quadrant
        vCorners[TOP_LEFT].y = vCorners[TOP_RIGHT].y = 0.0f;
        vCorners[TOP_LEFT].x = vCorners[BOTTOM_LEFT].x = 0.0f;
        break;
    }

    // Store data in VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, iPlayer * FOUR_VEC4, FOUR_VEC4, vCorners);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate New Frame Buffer for Player
    generateFrameBuffer(iPlayer);
}

void GameManager::cleanupFrameBuffers()
{
    // Local Variables
    TextureManager* pTxtMngr = TEXTURE_MANAGER;

    // Clean up Frame Buffers
    for each (sRenderBlock pRenderBlock in m_pFrameBufferTextures)
    {
        // Delete Render and Frame Buffers
        glDeleteRenderbuffers(1, &pRenderBlock.iRenderBuffer);
        glDeleteFramebuffers(1, &pRenderBlock.iFrameBuffer);

        for( unsigned int i = 0; i < 2; ++i )
            pTxtMngr->unloadTexture(&(pRenderBlock.pColorBuffer[i]));

        // Clean up Ping Pong Buffers
        for (sRenderBlock::sPingPongBuffer pBuffer : pRenderBlock.pPingPongBuffers)
        {
            TEXTURE_MANAGER->unloadTexture(&pBuffer.pBuffer);
            glDeleteFramebuffers(1, &pBuffer.iFBO);
        }
    }
    m_pFrameBufferTextures.clear();
}

void GameManager::generateFrameBuffer(unsigned int iPlayer)
{
    // Set up Frame Buffer
    sRenderBlock sNewBlock;
    glGenFramebuffers(1, &sNewBlock.iFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, sNewBlock.iFrameBuffer);

    // Create color attachment texture
    // 2 Color Attachments are generated, one for FragColor and the other for BrightColors to apply Bloom
    for (unsigned int i = 0; i < 2; ++i)
    {
        sNewBlock.pColorBuffer[i] = TEXTURE_MANAGER->genFrameBufferTexture(m_iSplitWidth, m_iSplitHeight, iPlayer, i);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, sNewBlock.pColorBuffer[i]->getTextureID(), 0);
    }

    //create renderbuffer object for depth and stencil attachment
    glGenRenderbuffers(1, &sNewBlock.iRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, sNewBlock.iRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_iSplitWidth, m_iSplitHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sNewBlock.iRenderBuffer);

    // Verify that Framebuffer is set up correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR: Framebuffer is not complete.\n";

    // Tell Opengl that this Frame Buffer renders to two color buffers
    unsigned int iAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, iAttachments);

    // Generate Pingpong Buffers
    for (unsigned int i = 0; i < 2; ++i)
    {
        glGenFramebuffers(1, &sNewBlock.pPingPongBuffers[i].iFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, sNewBlock.pPingPongBuffers[i].iFBO);
        sNewBlock.pPingPongBuffers[i].pBuffer =
            TEXTURE_MANAGER->genFrameBufferTexture(m_iSplitWidth, m_iSplitHeight,
                                                   iPlayer, i + 2);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            sNewBlock.pPingPongBuffers[i].pBuffer->getTextureID(), 0);

        // Verify that Framebuffer is set up correctly
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "ERROR: Framebuffer is not complete.\n";
    }

    // Save Render Block and unbind frame buffer.
    m_pFrameBufferTextures.push_back(sNewBlock);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
    End the current game.

    - Stop the AI
    - Stop Physics
    - Determine the winner
    - Move to the PostGameMenu
*/
void GameManager::endGame()
{
    m_pSoundManager->setEndGame();

    // postgame menu
    cout << "GameManger::endGame()" << endl;
    m_bInGame = false;
    m_bPaused = true;
    m_pCommandHandler->setCurrentMenu(PostgameMenu::getInstance());
    m_pEntityManager->purgeEnvironment();

    cleanupFrameBuffers();

    // Clean up Map information
    glDeleteBuffers(1, &m_iMapVBO);
    m_vColors.clear();
    m_vPositions.clear();

    resizeWindow(m_iWidth, m_iHeight);

    m_pPhysicsManager->cleanupPhysics();
}

/*
    Draw the scene with a 60 fps lock.
    In other words, if drawScene() is called over 60 times per second, the
    scene will not be drawn more than it needs to.
*/
void GameManager::drawScene()
{
    if (m_fFrameTime >= sixtieth_of_a_sec{ 1 }) // This locks the framerate to 60 fps
    {
        m_fFrameTime = seconds{ 0 };

        // Render the Scene
        glEnable(GL_DEPTH_TEST);
        if (m_bInGame)
        {
            // Set up Render for this frame
            m_pEntityManager->setupRender();

            // Set Map Position Data.
            m_pEntityManager->getPlayerPositions(&m_vPositions);
            glBindBuffer(GL_ARRAY_BUFFER, m_iMapVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, m_vPositions.size() * sizeof(vec3), m_vPositions.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Render each screen
            for( unsigned int screen = 0; screen < m_pFrameBufferTextures.size(); ++screen)
            {
                // Bind Frame Buffer
                glBindFramebuffer(GL_FRAMEBUFFER, m_pFrameBufferTextures[screen].iFrameBuffer);
                glViewport(0, 0, m_iSplitWidth, m_iSplitHeight);

                // Render Frame
                m_pEntityManager->renderEnvironment(screen);                
                renderMap();
                // Render the UI
                // Since the GameInterface is rendering directly, we do not need to
                // switch to the GameInterface
                m_pGameInterface->setFocus(static_cast<eHovercraft>(screen));
                m_pGameInterface->render();
                // Blur the Bloom Buffer
                blurBloomBuffer(screen);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            // Render the Split Screen Quads
            renderSplitScreen();
        }

        glDisable(GL_DEPTH_TEST);
        if (!m_bInGame)
        {
            // In game check is to avoid double rendering the user interface.
            m_pMenuInterface->render();
        }
        

        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(m_pWindow);
    }
}

/*
    Set the keyboard hovercraft according to the number of connected joysticks
    and players in the game.
    This should be set at the initialization of a new game as the pregame menu
    determines how many players there will be in a given game.

    @param playerCount  for the given game
*/
void GameManager::setKeyboardHovercraft(int playerCount)
{
    int joystickCount = INPUT_HANDLER->getJoystickCount();

    if (playerCount == joystickCount)
    {
        // If all the players have joysticks, then the keyboard is not necessary.
        // Set it to control player 1 by default.
        m_eKeyboardHovercraft = HOVERCRAFT_PLAYER_1;
    }
    else
    {
        // Due to how the pregame menu is set up, the user can choose to have
        // one extra player more than there are connected joysticks, but less
        // than the max player count.
        // Under this assumption, the keyboard will control the last player,
        // which does not have a joystick.
        m_eKeyboardHovercraft = static_cast<eHovercraft>(joystickCount);
    }
}

/*
    Update the game time only while in game. This should only be called while
    in game.
*/
void GameManager::updateGameTime(float frameDeltaTime)
{
    checkIfShouldUpdateGameTime(frameDeltaTime);
    checkIfShouldResumeGame(frameDeltaTime);
}


// Name: blurBloomBuffer
// Description: Applies a gaussian blur shader to the bloom buffer
//      for the specified frame buffer.
//  Algorithm modified from: https://learnopengl.com/Advanced-Lighting/Bloom
void GameManager::blurBloomBuffer(unsigned int iScreen)
{
    // Bind Vertex Array and Use Blur Program
    glBindVertexArray(m_iBlurVAO);
    glUseProgram(m_pShaderManager->getProgram(ShaderManager::eShaderType::BLUR_SHDR));
    
    // Local variables
    unsigned int iAmount = BLUR_AMOUNT;
    bool bFirstIteration = true, bHorizontal = true;

    // Ping Pong between the blur buffers to apply gaussian blur in horizontal and vertical patterns.
    for (unsigned int i = 0; i < iAmount; ++i)
    {
        // Bind this Frame Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_pFrameBufferTextures[iScreen].pPingPongBuffers[bHorizontal].iFBO);

        // Set Horizontal/Vertical setting
        m_pShaderManager->setUniformBool(ShaderManager::eShaderType::BLUR_SHDR, "horizontal", bHorizontal);

        // This is first iteration? bind the Buffer to blur
        if (bFirstIteration)
        {
            m_pFrameBufferTextures[iScreen].pColorBuffer[1]->bindTexture(ShaderManager::eShaderType::BLUR_SHDR, "mTexture");
            bFirstIteration = false;
        }
        else    // Otherwise, apply opposite blur to previous output.
            m_pFrameBufferTextures[iScreen].pPingPongBuffers[!bHorizontal].pBuffer->bindTexture(ShaderManager::eShaderType::BLUR_SHDR, "mTexture");

        // Draw Screen Quad.
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        bHorizontal = !bHorizontal;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Name: renderSplitScreen
// Written by: James Coté
// Description: Draws each Frame Buffer into set up quads in screen space.
//              >2 Player Layout:
//                  Player 1        Player 2
//                  Player 3        Player 4
//              2 Player Layout:
//                  Player 1
//                  Player 2
void GameManager::renderSplitScreen()
{
    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShaderManager->getProgram(ShaderManager::eShaderType::SPLIT_SCREEN_SHDR));
    glViewport(0, 0, m_iWidth, m_iHeight);

    for (unsigned int i = 0; i < m_pFrameBufferTextures.size(); ++i)
    {
        m_pFrameBufferTextures[i].pColorBuffer[0]->bindTexture(ShaderManager::eShaderType::SPLIT_SCREEN_SHDR, "hdrBuffer");
        m_pFrameBufferTextures[i].pPingPongBuffers[BLUR_AMOUNT & 1].pBuffer->bindTexture(ShaderManager::eShaderType::SPLIT_SCREEN_SHDR, "bloomBuffer");
        glDrawArrays(GL_TRIANGLE_STRIP, (i << 2), 4);
    }
    
    // Bind each Frame Buffer and Render image
    // Debug for BrightBuffer
    // TODO: TEST WITH SEPERATE QUAD THAT TAKES UP SCREEN SPACE
    //for (unsigned int i = 0; i < 2; ++i)
    //{
    //    //if (i == 0)
    //    m_pFrameBufferTextures[0].pPingPongBuffers[i].pBuffer->bindTexture(ShaderManager::eShaderType::SPLIT_SCREEN_SHDR, "hdrBuffer");
    //    //else
    //    //    m_pFrameBufferTextures[0].pPingPongBuffers[1].pBuffer->bindTexture(ShaderManager::eShaderType::SPLIT_SCREEN_SHDR, "hdrBuffer");
    //    glDrawArrays(GL_TRIANGLE_STRIP, (i << 2), 4);
    //}

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

// Name: renderMap
// Written by: James Coté
// Description: Draws the Map triangles on the screen for directing toward other players that
//              aren't on screen.
void GameManager::renderMap()
{
    // Set up OpenGL for rendering
    glBindVertexArray(m_iMapVAO);
    glUseProgram(m_pShaderManager->getProgram(ShaderManager::eShaderType::MAP_SHDR));

    glPointSize(20.0);
    glDrawArrays(GL_POINTS, 0, m_vPositions.size());
    glPointSize(1.0);

    glBindVertexArray(0);
}

/*
    Initialize all start-of-program state.
    This should only be called once at the start of the program.

    Initialize shaders and geometry.
    Contains any initializion requirements in order to start drawing.

    Set the game to use the start menu and and start interface

    @return true if initialzation successful
*/
bool GameManager::initialize()
{
    // Shaders
    if (!m_pShaderManager->initializeShaders())
    {
        cout << "Couldn't initialize shaders." << endl;
        return false;
    }

    // Initialize Environment with a new scene      
    // For now we will initialize this here instead of the constructor as GameStats
    // uses GameInterface, which requires GameManager to already be instantiated.
    m_pGameStats     = GameStats::getInstance(m_iWidth, m_iHeight);


    // Game starts paused as the player starts in the start menu
    m_bInGame = false;
    startedGameOver = false;
    m_fGameOverTime = GAME_OVER_TIME;

    m_pCommandHandler->setCurrentMenu(StartMenu::getInstance());
    m_pMenuInterface = StartInterface::getInstance(m_iWidth, m_iHeight);

    // Return error results
    return true; 
}

/*******************************************************************************\
 * Camera Manipulation                                                         *
\*******************************************************************************/

void GameManager::rotateCamera(vec2 pDelta)
{
    if (m_bInGame) {
        m_pEntityManager->rotateCamera(pDelta);
    }
}

void GameManager::zoomCamera(float fDelta)
{
    if (m_bInGame) {
        m_pEntityManager->zoomCamera(fDelta);
    }
}

/*
Called from Window Resize callback.
Currently updates the Entity Manager with new Window Size, may require more
functionality for menus, etc.

@param iWidth   of the window after resizing
@param iHeight  of the window after resizing
*/
void GameManager::resizeWindow( int iWidth, int iHeight )
{
    // Store Screen Size
    m_iHeight = m_iSplitHeight = iHeight;
    m_iWidth = m_iSplitWidth = iWidth;
    unsigned int iSize = m_pFrameBufferTextures.size();

    // Calculate SplitScreen Size
    if (iSize > 1)
        m_iSplitHeight >>= 1;
    if (iSize > 2)
        m_iSplitWidth >>= 1;

    // Resize the Frame Buffers
    cleanupFrameBuffers();
    for (unsigned int i = 0; i < iSize; ++i)
        generateFrameBuffer(i);

    m_pEntityManager->updateWidthAndHeight(m_iSplitWidth, m_iSplitHeight);
    m_pMenuInterface->updateWidthAndHeight(iWidth, iHeight);
}

/*
Calculates an intersection given screen coordinates.
This is used for testing the particle emitter by spawning an emitter where
the mouse clicks the floor plane.

@param fX   x-coordinate of the mouse in window coordinates
@param fY   y-coordinate of the mouse in window coordinates
*/
void GameManager::intersectPlane(float fX, float fY)
{
    // Local Variables
    vec3 vRay = m_pEntityManager->getActiveCameraComponent()->getRay(fX, fY);
    vec3 vNormal = vec3(0.0, 1.0, 0.0); // normal of xz-plane
    vec3 vCameraPos = m_pEntityManager->getActiveCameraComponent()->getCameraWorldPos();
    vec3 vIntersection = vec3(-1.0f);
    float fT = dot(vRay, vNormal);

    // Calculate Intersection
    if ((fT > FLT_EPSILON) || (fT < -FLT_EPSILON))
    {
        // Is intersecting.
        fT = -(dot(vCameraPos, vNormal) / fT);

        // Not behind camera.
        if (fT >= 0)
            vIntersection = vCameraPos + (fT*vRay);
    }
}

void GameManager::setPaused(bool paused)
{
    if (paused)
    {
        pauseGame();
    }
    else
    {
        startResumeCountdown();
    }
}

/*
    When the user wants to resume (unpause) the game, there is a countdown
    before the environment begins again in order for the player to get ready.
*/
void GameManager::startResumeCountdown()
{
    m_bInGame = true;
    m_bQueueResume = true;
    m_fQueueResumeTime = GAME_RESUME_TIME;
    m_pSoundManager->setResumeCountdown();
    m_pGameInterface->startResumeCountdown();
}

/*
    Resume the game without delay.
*/
void GameManager::resumeGame()
{
    m_bPaused = false;
    m_bQueueResume = false;
    m_pSoundManager->setResumeGame();
}

/*
    When the game pauses, all the in-game sounds are paused and saved.
    If the game resumes, these saved paused events should reesume.
*/
void GameManager::pauseGame()
{
    m_bInGame = false;
    m_bPaused = true;
    m_pSoundManager->setPauseMenu();
}

/*
    Only update game time if the game is playing to ensure the GameInterface
    and the Game Time are in perfect sync.
*/
void GameManager::checkIfShouldUpdateGameTime(float frameDeltaTime)
{
    if (!m_bPaused)
    {
        m_fGameTime -= frameDeltaTime;
    }
}

/*
    If in game, check to see if queued to resume. This should be the case
    when resuming from pause or at the start of the game.
*/
void GameManager::checkIfShouldResumeGame(float frameDeltaTime)
{
    if (m_bQueueResume)
    {
        m_fQueueResumeTime -= frameDeltaTime;
        // Update countdown ticking for game UI
        m_pGameInterface->updateResumeCountdown(m_fFrameDeltaTime);
        if (m_fQueueResumeTime <= 0)
        {
            resumeGame();
        }
    }
}

