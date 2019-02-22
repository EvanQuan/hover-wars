#include "EntityManager.h"
#include "EntityHeaders/StaticEntity.h"

/***********\
 * Defines *
\***********/
#define DEFAULT_HXW 1024

/*************\
 * Constants *
\*************/
const vec3 WORLD_CENTER = vec3(0.0);
const mat3 WORLD_COORDS = mat3(1.0);
const vector<vec3> AXIS_VERTS = { WORLD_CENTER, WORLD_COORDS[0],
                                  WORLD_CENTER, WORLD_COORDS[1],
                                  WORLD_CENTER, WORLD_COORDS[2] };
const GLfloat color[] = { 0.3215f, 0.3411f, 0.4352f, 1.0f };
const GLfloat DEPTH_ZERO = 1.0f;

// Initialize Static Instance Variable
EntityManager* EntityManager::m_pInstance = nullptr;

// Default Constructor
EntityManager::EntityManager()
{
    // Initialize ID Pools
    m_iComponentIDPool = m_iEntityIDPool = 0;

    // Initialize Local Variables
    m_iHeight = m_iWidth = DEFAULT_HXW;
    m_bPause             = false;
    m_bDrawBoundingBoxes = false;
    m_bDrawSpatialMap    = false;
    m_bShadowDraw        = false;
    m_bUseDebugCamera    = false;
    m_pMshMngr           = MESH_MANAGER;
    m_pTxtMngr           = TEXTURE_MANAGER;
    m_pScnLdr            = SCENE_LOADER;
    m_pEmtrEngn          = EMITTER_ENGINE;
    m_pPhysxMngr         = PHYSICS_MANAGER;
    m_pSpatialMap        = SPATIAL_DATA_MAP;
    m_pShdrMngr          = SHADER_MANAGER;

    // For Rendering the World Axis
    glGenVertexArrays(1, &m_pVertexArray);

    // Generate Buffer and Set Attribute
    m_pVertexBuffer = m_pShdrMngr->genVertexBuffer(m_pVertexArray, AXIS_VERTS.data(), AXIS_VERTS.size() * sizeof(vec3), GL_STATIC_DRAW);
    m_pShdrMngr->setAttrib(m_pVertexArray, 0, 3, 0, nullptr);
}

/*
Does not requires window parameter. Assumes the Singleton instance has been
instantiated beforehand.
*/
EntityManager* EntityManager::getInstance()
{
    if (nullptr == m_pInstance)
        m_pInstance = new EntityManager();

    return m_pInstance;
}

// Destructor.
EntityManager::~EntityManager()
{
    purgeEnvironment();

    // Delete World Axis Buffers
    glDeleteBuffers(1, &m_pVertexBuffer);
    glDeleteVertexArrays(1, &m_pVertexArray);

    // Delete Mesh Manager
    if (nullptr != m_pMshMngr) { delete m_pMshMngr; }
    
    // Delete Texture Manager
    if (nullptr != m_pTxtMngr) { delete m_pTxtMngr; }

    // Delete Scene Loader
    if (nullptr != m_pScnLdr) { delete m_pScnLdr; }

    // Delete Emitter Engine
    if (nullptr != m_pEmtrEngn) { delete m_pEmtrEngn; }

    if (nullptr != m_pSpatialMap) { delete m_pSpatialMap; }
}

// Clears Environment and loads a new environment from specified file.
void EntityManager::initializeEnvironment(string sFileName)
{
    SceneLoader* pObjFctry = SceneLoader::getInstance();

    purgeEnvironment();
    pObjFctry->loadFromFile(sFileName);
    // Generate Debug Camera
    m_pCamera = generateCameraEntity();

    // Populate the Spatial Data Map now that everything has been loaded.
    m_pSpatialMap->populateStaticMap(&m_pMasterEntityList);
}

// Initializes the SpatialDataMap with a given length, width and tilesize
void EntityManager::initializeSpatialMap(float fLength, float fWidth, float fTileSize)
{
    if (!m_pSpatialMap->isInitialized())     // Only Initialize the Spatial Map once. Unload everything first to initialize again.
    {
        m_pSpatialMap->initializeMap(fLength, fWidth, fTileSize);
    }
}

// Clears out the entire environment
void EntityManager::purgeEnvironment()
{
    // Clear unique_ptrs of Components and Entities
    m_pMasterComponentList.clear();
    m_pMasterEntityList.clear();
    m_pEmtrEngn->clearAllEmitters();
    m_pSpatialMap->clearMap();           // Unload the Spatial Data Map

    // Reset ID Pools
    m_iComponentIDPool = m_iEntityIDPool = 0;

    m_pMshMngr->unloadAllMeshes();
    m_pTxtMngr->unloadAllTextures();
    m_pPhysxMngr->cleanupPhysics(); // Clean up current Physics Scene
    m_pDirectionalLight = nullptr;
    m_pActiveCameraComponent = nullptr;
}

// Resets the FBO to the default settings for a new render.
void EntityManager::resetFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_iWidth, m_iHeight);
    glClearBufferfv(GL_COLOR, 0, color);
    glClearBufferfv(GL_DEPTH, 0, &DEPTH_ZERO);
    m_bShadowDraw = false;
}

// Name: renderEnvironment
// Written by: James Coté
// Description: Sets necessary lights into Shader Uniform Buffers and Renders
//      all active Render Components.
// TODO: Have Lighting load based on Spatial Data Structure
void EntityManager::renderEnvironment( )
{    
    // Local Variables
    LightingComponent* pDirectionalLightComponent = nullptr;

    // Get Directional Light
    if (nullptr != m_pDirectionalLight)
    {
        pDirectionalLightComponent = m_pDirectionalLight->getLightingComponent();
        pDirectionalLightComponent->setupShadowFBO();       // Set up Frame Buffer for Shadow
        pDirectionalLightComponent->setupPMVMatrices();     // Set the Lighting ModelView and Projection Matrices for generating a Depth Buffer from Light Position
        m_bShadowDraw = true;                               // Render For Shadow Map
        glCullFace(GL_FRONT);
        doRender();                                         // Do the Render
        glCullFace(GL_BACK);
        pDirectionalLightComponent->setupShadowUniforms();  // Set the Shadow Map in the Shaders.
        resetFBO();                                         // Reset the Frame Buffer for typical rendering.
    }
    
    // Calculate information for each Light in the scene (Current max = 4 + 1 Directional Light)
    m_pShdrMngr->setLightsInUniformBuffer(pDirectionalLightComponent, &m_pLights);

    // Perform Final Render
    setCameraPMVMatrices();
    doRender();
}

// Performs Rendering of scene
void EntityManager::doRender()
{
    // Render all render components
    for (unordered_map<Mesh const*, RenderComponent*>::iterator pIter = m_pRenderingComponents.begin();
        pIter != m_pRenderingComponents.end();
        ++pIter)
        (*pIter).second->render();

    // Don't render these if only doing a shadow pass
    if (!m_bShadowDraw)
    {
        // Render Emitters
        if (nullptr != m_pEmtrEngn)
        {
           m_pEmtrEngn->renderEmitters();
        }

        // Draw the Spatial Map for debuggin
        if (m_bDrawSpatialMap)
        {
            m_pSpatialMap->drawMap();
        }

#ifdef _DEBUG
        renderAxis();
#endif
    }
}

// Draw World Axis Lines
void EntityManager::renderAxis()
{
    // Increase Point Size for the Axis
    glPointSize(10.f);

    // Set up Shader and Vertex Array
    glBindVertexArray(m_pVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::WORLD_SHDR));

    // Render
    glDrawArrays(GL_LINES, 0, AXIS_VERTS.size());
    glDrawArrays(GL_POINTS, 0, AXIS_VERTS.size());

    // Reset Point size
    glPointSize(1.f);
}

// Sets the Camera Projection, Model and View Matrices from the active camera.
void EntityManager::setCameraPMVMatrices()
{
    // Set Debug Camera to follow player. Copy the Rotation Quaternion to the Camera which will rotate the camera using the same quaternion before
    //  translating the camera to world coordinates. TODO: Re-evaluate this methodology.
    m_pCamera->setLookAt(m_pPlayerEntityList[PLAYER_1]->getCameraPosition());
    quat pQuat = m_pPlayerEntityList[PLAYER_1]->getCameraRotation();
    m_pCamera->setRotationQuat(pQuat);

    // Get player 1's active camera to show
    // TODO for multiplayer or spectator mode, GameManager needs multiple active camera's
    // each with their own camera components. The game will render 4 times, each switching
    // the player to retrieve the active camera.
    const CameraComponent* pCamera = m_bUseDebugCamera ?
        m_pCamera->getCameraComponent() : m_pPlayerEntityList[PLAYER_1]->getActiveCameraComponent();

    mat4 pModelViewMatrix = pCamera->getToCameraMat();
    mat4 pProjectionMatrix = pCamera->getPerspectiveMat();

    // Set camera information in Shaders before rendering
    m_pShdrMngr->setProjectionModelViewMatrix(&pProjectionMatrix, &pModelViewMatrix);
}

/*********************************************************************************\
* Entity Management                                                              *
\*********************************************************************************/

// Fetches the current position of Entity with ID: iEntityID
vec3 EntityManager::getEntityPosition(int iEntityID)
{
    // Return Value: Origin as a default if entity not found.
    vec3 vReturn = vec3(0.0f);

    // Iterate through all Entities.
    for (vector<unique_ptr<Entity>>::iterator iter = m_pMasterEntityList.begin();
        iter != m_pMasterEntityList.end();
        ++iter)
    {
        // If Entity found, get the position and break from the loop
        if (iEntityID == (*iter)->getID())
        {
            vReturn = (*iter)->getPosition();
            break;
        }
    }

    // Return result.
    return vReturn;
}

// Generates a Camera Entity and stores it in the Master Entity List.
Camera* EntityManager::generateCameraEntity()
{
    unique_ptr<Camera> pNewCamera = make_unique<Camera>(getNewEntityID());
    Camera* pReturnCamera = pNewCamera.get();
    m_pMasterEntityList.push_back(move(pNewCamera));

    return pReturnCamera;
}

// Generates a Static Plane Entity into the world.
void EntityManager::generateStaticPlane(const ObjectInfo* pObjectProperties, int iHeight, int iWidth, const vec3* vNormal, const string& sShaderType)
{
    unique_ptr<StaticEntity> pNewPlane = make_unique<StaticEntity>(getNewEntityID(), &pObjectProperties->vPosition);
    pNewPlane->loadAsPlane(vNormal, iHeight, iWidth, pObjectProperties, sShaderType);
    m_pMasterEntityList.push_back(move(pNewPlane));
}

// Generates a Static Plane Entity into the world.
void EntityManager::generateStaticSphere(const ObjectInfo* pObjectProperties, float fRadius, const string& sShaderType)
{
    unique_ptr<StaticEntity> pNewSphere = make_unique<StaticEntity>(getNewEntityID(), &pObjectProperties->vPosition);
    pNewSphere->loadAsSphere(fRadius, pObjectProperties, sShaderType);
    m_pMasterEntityList.push_back(move(pNewSphere));
}

void EntityManager::generateStaticCube(const ObjectInfo* pObjectProperties, const vec3* vDimensions, const string& sShaderType)
{
    unique_ptr<StaticEntity> pNewCube = make_unique<StaticEntity>(getNewEntityID(), &pObjectProperties->vPosition);
    pNewCube->loadAsCube(pObjectProperties, vDimensions, sShaderType);
    m_pMasterEntityList.push_back(move(pNewCube));
}

// Generates a Static Mesh at a given location
void EntityManager::generateStaticMesh(const ObjectInfo* pObjectProperties, const string& sMeshLocation, float fScale, const string& sShaderType )
{
    unique_ptr<StaticEntity> pNewMesh = make_unique<StaticEntity>(getNewEntityID(), &pObjectProperties->vPosition);
    pNewMesh->loadFromFile(sMeshLocation, pObjectProperties, sShaderType, fScale);
    m_pMasterEntityList.push_back(move(pNewMesh));
}

/*
Generate a Player Entity at a starting position with a given Material, scale,
mesh location and shader type.
*/
void EntityManager::generatePlayerEntity(const ObjectInfo* pObjectProperties, const string& sMeshLocation, float fScale, const string& sShaderType)
{
    unique_ptr<PlayerEntity> pNewPlayer = make_unique<PlayerEntity>(getNewEntityID(), &pObjectProperties->vPosition);
    pNewPlayer->initialize(sMeshLocation, pObjectProperties, sShaderType, fScale, static_cast<ePlayer>(m_pPlayerEntityList.size()));
    m_pPlayerEntityList.push_back(pNewPlayer.get()); 
    m_pMasterEntityList.push_back(move(pNewPlayer));
}

/*
Generate a Bot Entity at a starting position with a given Material, scale,
mesh location and shader type.
*/
void EntityManager::generateBotEntity(const ObjectInfo* pObjectProperties, const string& sMeshLocation, float fScale, const string& sShaderType)
{
    unique_ptr<BotEntity> pNewBot = make_unique<BotEntity>(getNewEntityID(), &pObjectProperties->vPosition);
    pNewBot->initialize(sMeshLocation, pObjectProperties, sShaderType, fScale, static_cast<eBot>(m_pBotEntityList.size()));
    m_pBotEntityList.push_back(pNewBot.get()); 
    m_pMasterEntityList.push_back(move(pNewBot));
}

// Generates and Returns an Interactable Entity with a specified Position.
InteractableEntity* EntityManager::generateInteractableEntity(const vec3* vPosition)
{
    unique_ptr<InteractableEntity> pNewEntity = make_unique<InteractableEntity>(getNewEntityID(), vPosition);
    InteractableEntity* pReturnEntity = pNewEntity.get();
    m_pMasterEntityList.push_back(move(pNewEntity));

    // Return InteractableEntity
    return pReturnEntity;
}

// Generates a Static light at a given position. Position and Color are required, but default meshes and textures are available.
void EntityManager::generateStaticPointLight( const ObjectInfo* pObjectProperties, float fPower, const vec3* vColor, const string& sMeshLocation, float m_fMeshScale)
{
    unique_ptr<PointLight> pNewLight = make_unique<PointLight>(getNewEntityID(), &pObjectProperties->vPosition);
    pNewLight->initialize(fPower, vColor, true, pObjectProperties, sMeshLocation, m_fMeshScale);
    m_pMasterEntityList.push_back(move(pNewLight));
}

// Generates a New Directional Light and stores it in the Entity Manager.
//    If a Directional Light already exists, no new Directional Light will be created.
void EntityManager::generateDirectionalLight(const vec3* vDirection, const vec3* vAmbientColor, const vec3* vDiffuseColor, const vec3* vSpecularColor,
                                            float fPosition, float fNearPlane, float fFarPlane, unsigned int iShadowHeight, unsigned int iShadowWidth, float fShadowFrame)
{
    if (nullptr == m_pDirectionalLight)
    {
        unique_ptr<DirectionalLight> pNewDirectionalLight = make_unique<DirectionalLight>(getNewEntityID());
        pNewDirectionalLight->initialize(vDirection, vAmbientColor, vDiffuseColor, vSpecularColor, fPosition, fNearPlane, fFarPlane, iShadowHeight, iShadowWidth, fShadowFrame);

        // Set the current Directional Light and store inside the Master Entity List.
        m_pDirectionalLight = pNewDirectionalLight.get();
        m_pMasterEntityList.push_back(move(pNewDirectionalLight));
    }
}

// Generates a new Spot Light Entity and stores it in the Entity Manager.
void EntityManager::generateStaticSpotLight(const ObjectInfo* pObjectProperties, float fPhi, float fSoftPhi, const vec3* vColor, const vec3* vDirection, const string& sMeshLocation, float m_fMeshScale)
{
    unique_ptr<SpotLight> pNewLight = make_unique<SpotLight>(getNewEntityID(), &pObjectProperties->vPosition);
    pNewLight->initialize(fPhi, fSoftPhi, true, vColor, vDirection, sMeshLocation, pObjectProperties, m_fMeshScale);
    m_pMasterEntityList.push_back(move(pNewLight));
}

// Goes through all Existing Camera Components and updates their aspect ratio.
void EntityManager::updateHxW(int iHeight, int iWidth)
{
    for (vector<CameraComponent*>::iterator iter = m_pCameraComponents.begin();
        iter != m_pCameraComponents.end();
        ++iter)
    {
        (*iter)->updateHxW(iHeight, iWidth);
    }

    // Store new Height and Width in case another Camera Component is created.
    m_iHeight = iHeight;
    m_iWidth = iWidth;
}

// Main Update Function
// This function checks the timer and updates necessary components
//    in the game world. No rendering is done here.
void EntityManager::updateEnvironment(const GameTime pTimer)
{
    /*
    Get the delta since the last frame and update based on that delta.

    Unit: seconds
    */
    float fDeltaTime = static_cast<float>(pTimer.getFrameTimeSinceLastFrame().count());
        
    // UPDATES GO HERE
    m_pPhysxMngr->update(fDeltaTime); // PHYSICSTODO: This is where the Physics Update is called.
    m_pEmtrEngn->update(fDeltaTime);

    // Iterate through all Entities and call their update with the current time.
    for (vector<unique_ptr<Entity>>::iterator iter = m_pMasterEntityList.begin();
        iter != m_pMasterEntityList.end();
        ++iter)
        (*iter)->update(fDeltaTime);

    // Iteratre through all Animation Components to update their animations
    for (vector<AnimationComponent*>::iterator iter = m_pAnimationComponents.begin();
        iter != m_pAnimationComponents.end();
        ++iter)
        (*iter)->update(fDeltaTime);
}

/*********************************************************************************\
* Entity Component Management                                                    *
\*********************************************************************************/

/*
Generates a new Camera Component. Stores it in the Camera Component and Master component lists.
@param int iEntityID the ID of the entity for which this component corresponds. This allows us
to correspond camera components to their "owner" entity.
*/
CameraComponent* EntityManager::generateCameraComponent( int iEntityID )
{
    // Generate new Camera Component
    unique_ptr<CameraComponent> pNewCameraComponentPtr = make_unique<CameraComponent>(iEntityID, getNewComponentID(), m_iHeight, m_iWidth);

    // Store new Camera Component
    m_pCameraComponents.push_back(pNewCameraComponentPtr.get());
    m_pMasterComponentList.push_back(move(pNewCameraComponentPtr));

    // Set the active Camera if no camera is currently active.
    if (NULL == m_pActiveCameraComponent)
    {
        m_pActiveCameraComponent = m_pCameraComponents.back();
    }

    return m_pCameraComponents.back();
}

// Generates a new Render Component, stores it in the Rendering Components list and Master Components list.
//    Manages component with a unique pointer stored internally in the Master Components list.
RenderComponent* EntityManager::generateRenderComponent(int iEntityID, Mesh const* pMeshKey, bool bStaticDraw, ShaderManager::eShaderType eType, GLenum eMode)
{
    // Generate new Render Component
    RenderComponent* pReturnComponent;

    // Since Meshes are more static with their material, if there is a rendering component
    //    for that mesh already, return that instead since that Mesh will probably have multiple instances
    //    per render.
    if (FuncUtils::contains(m_pRenderingComponents, pMeshKey))
    {
        pReturnComponent = m_pRenderingComponents[pMeshKey];
    }
    else    // Otherwise, if it hasn't been found, create the new render component and associate it with that Mesh Pointer.
    {
        // Initialize new Unique_Ptr for Render Component.
        unique_ptr<RenderComponent> pNewRenderComponent = make_unique<RenderComponent>(iEntityID, getNewComponentID(), bStaticDraw, eType, eMode);
        pNewRenderComponent->initializeComponent(pMeshKey);    // Initialize Render Component

        // Grab return Pointer and store Component within Entity Manager.
        pReturnComponent = pNewRenderComponent.get();    // Return pointer
        m_pRenderingComponents.insert(make_pair(pMeshKey, pReturnComponent));
        m_pMasterComponentList.push_back(move(pNewRenderComponent));    // move to Master Components list.
    }

    // Return newly created component.
    return pReturnComponent;
}

// Generates a Lighting Component, stores the light in the Lights list as well as manages it in the Master Components list.
//    Creates a Barebone light component, up to the caller to initialize the light how they desire.
LightingComponent* EntityManager::generateLightingComponent(int iEntityID)
{
    // Generate new Lighting Component
    unique_ptr<LightingComponent> pNewComponent = make_unique<LightingComponent>(iEntityID, getNewComponentID());
    LightingComponent* pReturnComponent = pNewComponent.get();
    m_pLights.push_back(pReturnComponent);
    m_pMasterComponentList.push_back(move(pNewComponent));

    // Return newly created component
    return pReturnComponent;
}

// This function will generate a new Physics component, store it within the internal
//    Master Component list as well as a separate PhysicsComponent* list that can be
//    managed by the Entity Manager. PHYSICSTODO: Maybe the separate list isn't necessary
//                                                and only the Physics Manager needs to be updated
//                                                or modified on a frame by frame basis?
PhysicsComponent* EntityManager::generatePhysicsComponent(int iEntityID)
{
    // Generate new Physics Component
    unique_ptr<PhysicsComponent> pNewComponent = make_unique<PhysicsComponent>(iEntityID, getNewComponentID());
    PhysicsComponent* pReturnComponent = pNewComponent.get();
    m_pPhysicsComponents.push_back(pReturnComponent);
    m_pMasterComponentList.push_back(move(pNewComponent));

    // Return newly created component
    return pReturnComponent;
}

// This function will generate a new Animation Component, store it within the internal
//      Master Component list as well as a separate AnimationComponent* list that can be
//      managed by the Entity Manager and prompted for updates separate from other Components.
AnimationComponent* EntityManager::generateAnimationComponent(int iEntityID)
{
    // Generate and store new Animation Component
    unique_ptr<AnimationComponent> pNewAnimCmp = make_unique<AnimationComponent>(iEntityID, getNewComponentID());
    AnimationComponent* pReturnComponent = pNewAnimCmp.get();
    m_pAnimationComponents.push_back(pReturnComponent);
    m_pMasterComponentList.push_back(move(pNewAnimCmp));

    // Return newly created component
    return pReturnComponent;
}

/*********************************************************************************\
* Command Management                                                             *
\*********************************************************************************/

bool EntityManager::playerExists(ePlayer player)
{
    return m_pPlayerEntityList.size() > static_cast<unsigned int>(player);
}

PlayerEntity* EntityManager::getPlayer(ePlayer player)
{
    return m_pPlayerEntityList.at(player);
}

bool EntityManager::botExists(eBot bot)
{
    return m_pBotEntityList.size() > static_cast<unsigned int>(bot);
}

BotEntity* EntityManager::getBot(eBot bot)
{
    return m_pBotEntityList.at(bot);
}



/*********************************************************************************\
* Camera Management                                                              *
\*********************************************************************************/
void EntityManager::rotateCamera(vec2 pDelta)
{
    m_pCamera->orbit(pDelta);
}

void EntityManager::zoomCamera(float fDelta)
{
    m_pCamera->zoom(fDelta);
}
