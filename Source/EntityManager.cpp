#include "EntityManager.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "EntityHeaders/StaticEntity.h"
#include "EntityHeaders/PlayerEntity.h"

// Initialize Static Instance Variable
EntityManager* EntityManager::m_pInstance = nullptr;

EntityManager::EntityManager()
{
    // Initialize ID Pools
    m_iComponentIDPool = m_iEntityIDPool = 0;

    // Initialize Local Variables
    m_iHeight = START_HEIGHT;
    m_iWidth = START_WIDTH;
    m_bPause = false;
    m_pMshMngr = MESH_MANAGER;
    m_pTxtMngr = TEXTURE_MANAGER;
    m_pScnLdr = SCENE_LOADER;
    m_pEmtrEngn = EMITTER_ENGINE;
    m_pPhysxMngr = PHYSICS_MANAGER;
}

// Gets the instance of the environment manager.
EntityManager* EntityManager::getInstance()
{
    if ( nullptr == m_pInstance )
        m_pInstance = new EntityManager();
    return m_pInstance;
}

// Destructor.
EntityManager::~EntityManager()
{
    purgeEnvironment();

    // Delete Mesh Manager
    if (nullptr != m_pMshMngr)
        delete m_pMshMngr;
    
    // Delete Texture Manager
    if (nullptr != m_pTxtMngr)
        delete m_pTxtMngr;

    // Delete Scene Loader
    if (nullptr != m_pScnLdr)
        delete m_pScnLdr;

    // Delete Emitter Engine
    if (nullptr != m_pEmtrEngn)
        delete m_pEmtrEngn;
}

// Clears Environment and loads a new environment from specified file.
void EntityManager::initializeEnvironment(string sFileName)
{
    SceneLoader* pObjFctry = SceneLoader::getInstance();

    purgeEnvironment();
    pObjFctry->loadFromFile(sFileName);

    // TESTING: To Be Removed
    vec3 vNormal(0.0f, 1.0f, 0.0f);
    vec3 vPosition(5.0f, 5.0f, 5.0f);
    unique_ptr<InteractableEntity> pTestingEntity = make_unique<InteractableEntity>(getNewEntityID(), &vPosition);
    pTestingEntity->loadAsBillboard(&vNormal, 2.0f, 1.5f, nullptr);
    m_pBillboardTesting = pTestingEntity.get();
    m_pMasterEntityList.push_back(move(pTestingEntity));
}

// Remove Object from List with given ID
//void EntityManager::killObject( long lID )
//{
//    unsigned int i = 0;
//
//    // Iterate to find Object
//    while ( i < m_pObjects.size() && nullptr != m_pObjects[i] && lID != m_pObjects[i]->ID() )
//        ++i;
//
//    // Delete Object and remove it from list.
//    if ( i < m_pObjects.size() )
//    {
//        swap( m_pObjects[i], m_pObjects.back() );
//        delete m_pObjects.back();
//        m_pObjects.pop_back();
//    }
//}

// Outputs all the objects in the environment for debugging.
//void EntityManager::listEnvironment()
//{
//    cout << "Environment:" << endl;
//    for ( vector<Object3D*>::iterator pIter = m_pObjects.begin();
//          pIter != m_pObjects.end();
//          ++pIter )
//        cout << "\t" << (*pIter)->getDebugOutput() << endl;
//
//    for ( vector<Light*>::iterator pIter = m_pLights.begin();
//          pIter != m_pLights.end();
//          ++pIter )
//        cout << "\t" << (*pIter)->getDebugOutput() << endl;
//
//    cout << endl;
//}

// Clears out the entire environment
void EntityManager::purgeEnvironment()
{
    // Clear unique_ptrs of Components and Entities
    m_pMasterComponentList.clear();
    m_pMasterEntityList.clear();
    m_pEmtrEngn->clearAllEmitters();

    m_pMshMngr->unloadAllMeshes();
    m_pTxtMngr->unloadAllTextures();
    m_pPhysxMngr->cleanupPhysics(); // Clean up current Physics Scene
    m_pDirectionalLight = nullptr;
    m_pTestingLight = nullptr;
    m_pActiveCamera = nullptr;
}

// Fetch the Frenet Frame of the first MeshObject found (Hack for assignment)
//mat4 EntityManager::getFrenetFrame()
//{ 
//    mat4 pReturnVal = mat4( 1.0 );    // Default: return Identity Matrix
//
//    for ( vector<Object3D*>::iterator pObjIter = m_pObjects.begin();
//         pObjIter != m_pObjects.end();
//         ++pObjIter )
//    {
//        if ( !(*pObjIter)->getType().compare( "MeshObject" ) )
//        {
//            pReturnVal = (*pObjIter)->getFreNetFrames();
//            break;
//        }
//    }
//
//    // Return
//    return pReturnVal;
//}

void EntityManager::renderEnvironment( const vec3& vCamLookAt )
{
    // Local Variables
    ShaderManager* pShdrMngr = SHADER_MANAGER;
    const LightingComponent* pDirectionalLightComponent = nullptr;

    if (nullptr != m_pDirectionalLight)
        pDirectionalLightComponent = m_pDirectionalLight->getLightingComponent();
    
    // Calculate information for each Light in the scene (Current max = 4 + 1 Directional Light)
    pShdrMngr->setLightsInUniformBuffer(pDirectionalLightComponent, &m_pLights);

    for (unordered_map<Mesh const*, RenderComponent*>::iterator pIter = m_pRenderingComponents.begin();
        pIter != m_pRenderingComponents.end();
        ++pIter)
        (*pIter).second->render();

    if (nullptr != m_pEmtrEngn)
        m_pEmtrEngn->renderEmitters();
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
void EntityManager::generateStaticPlane(int iHeight, int iWidth, const vec3* vPosition, const vec3* vNormal, const Material* sMaterial, const string& sShaderType)
{
    unique_ptr<StaticEntity> pNewPlane = make_unique<StaticEntity>(getNewEntityID(), vPosition);
    pNewPlane->loadAsPlane(vNormal, iHeight, iWidth, sMaterial, sShaderType);
    m_pMasterEntityList.push_back(move(pNewPlane));
}

// Generates a Static Plane Entity into the world.
void EntityManager::generateStaticSphere(float fRadius, const vec3* vPosition, const Material* sMaterial, const string& sShaderType)
{
    unique_ptr<StaticEntity> pNewSphere = make_unique<StaticEntity>(getNewEntityID(), vPosition);
    pNewSphere->loadAsSphere(fRadius, sMaterial, sShaderType);
    m_pMasterEntityList.push_back(move(pNewSphere));
}

// Generates a Static Mesh at a given location
void EntityManager::generateStaticMesh(const string& sMeshLocation, const vec3* vPosition, const Material* sMaterial, float fScale, const string& sShaderType )
{
    unique_ptr<StaticEntity> pNewMesh = make_unique<StaticEntity>(getNewEntityID(), vPosition);
    pNewMesh->loadFromFile(sMeshLocation, sMaterial, sShaderType, fScale);
    m_pMasterEntityList.push_back(move(pNewMesh));
}

void EntityManager::generatePlayerEntity(const vec3* vPosition, const string& sMeshLocation, const Material* sMaterial, float fScale, const string& sShaderType)
{
    unique_ptr<PlayerEntity> pNewPlayer = make_unique<PlayerEntity>(getNewEntityID(), vPosition);
    pNewPlayer->initializePlayer(sMeshLocation, sMaterial, sShaderType, fScale);
    m_pPlayerEntityList.push_back(pNewPlayer.get()); // TODO this retrieves a raw pointer from the unique pointer. Is this okay?
    m_pMasterEntityList.push_back(move(pNewPlayer));
}

// Generates a Static light at a given position. Position and Color are required, but default meshes and textures are available.
void EntityManager::generateStaticPointLight( float fPower, const vec3* vPosition, const vec3* vColor, const Material* sMaterial, const string& sMeshLocation, float m_fMeshScale)
{
    unique_ptr<PointLight> pNewLight = make_unique<PointLight>(getNewEntityID(), vPosition);
    pNewLight->initialize(fPower, vColor, true, sMaterial, sMeshLocation, m_fMeshScale);

    if (nullptr == m_pTestingLight)
        m_pTestingLight = pNewLight.get();

    m_pMasterEntityList.push_back(move(pNewLight));
}

// Generates a New Directional Light and stores it in the Entity Manager.
//    If a Directional Light already exists, no new Directional Light will be created.
void EntityManager::generateDirectionalLight(const vec3* vDirection, const vec3* vAmbientColor, const vec3* vDiffuseColor, const vec3* vSpecularColor)
{
    if (nullptr == m_pDirectionalLight)
    {
        unique_ptr<DirectionalLight> pNewDirectionalLight = make_unique<DirectionalLight>(getNewEntityID());
        pNewDirectionalLight->initialize(vDirection, vAmbientColor, vDiffuseColor, vSpecularColor);

        // Set the current Directional Light and store inside the Master Entity List.
        m_pDirectionalLight = pNewDirectionalLight.get();
        m_pMasterEntityList.push_back(move(pNewDirectionalLight));
    }
}

// Generates a new Spot Light Entity and stores it in the Entity Manager.
void EntityManager::generateStaticSpotLight(float fPhi, float fSoftPhi, const vec3* vPosition, const vec3* vColor, const vec3* vDirection, const Material* sMaterial, const string& sMeshLocation, float m_fMeshScale)
{
    unique_ptr<SpotLight> pNewLight = make_unique<SpotLight>(getNewEntityID(), vPosition);
    pNewLight->initialize(fPhi, fSoftPhi, true, vColor, vDirection, sMeshLocation, sMaterial, m_fMeshScale);
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
void EntityManager::updateEnvironment(const Time& pTimer)
{
    // Get Total Frame Time and Benchmark for 60 fps
    duration<float> pFrameTime = pTimer.getFrameTime();
    constexpr auto pMaxDeltaTime = sixtieths_of_a_sec{ 1 };

    // Loop updates to maintain 60 fps
    while (pFrameTime > milliseconds(0))
    {
        // Get the Delta of this time step <= 1/60th of a second (60 fps)
        // Interpolate on steps < 1/60th of a second
        duration<float> pDeltaTime = 
            std::min<common_type<decltype(pFrameTime),decltype(pMaxDeltaTime)>::type>(pFrameTime, pMaxDeltaTime);

        pFrameTime -= pDeltaTime;
        float fDeltaTime = static_cast<float>(pDeltaTime.count());
        
        // UPDATES GO HERE
        m_pEmtrEngn->update(fDeltaTime);
        m_pPhysxMngr->update(fDeltaTime); // PHYSICSTODO: This is where the Physics Update is called.

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
}

/*********************************************************************************\
* Entity Component Management                                                    *
\*********************************************************************************/

// Generates a new Camera Component. Stores it in the Camera Component and Master component lists.
CameraComponent* EntityManager::generateCameraComponent( int iEntityID )
{
    // Generate new Camera Component
    unique_ptr<CameraComponent> pNewCameraPtr = make_unique<CameraComponent>(iEntityID, getNewComponentID(), m_iHeight, m_iWidth);

    // Store new Camera Component
    m_pCameraComponents.push_back(pNewCameraPtr.get());
    m_pMasterComponentList.push_back(move(pNewCameraPtr));

    // Set the active Camera if no camera is currently active.
    if (NULL == m_pActiveCamera)
        m_pActiveCamera = m_pCameraComponents.back();

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
    if (m_pRenderingComponents.end() != m_pRenderingComponents.find(pMeshKey))
        pReturnComponent = m_pRenderingComponents[pMeshKey];
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
* Command Management                                                    *
\*********************************************************************************/

void EntityManager::execute(ePlayer player, eVariableCommand command, float x, float y)
{
    switch (command)
    {
    case COMMAND_MOVE:
        PHYSICS_MANAGER->handleControllerInputMove(x, y);
        break;
    case COMMAND_TURN:
        // TODO make this a different method
        PHYSICS_MANAGER->handleControllerInputRotate(x, y);
        break;
    }
}

bool EntityManager::playerExists(ePlayer player)
{
    return (int) m_pPlayerEntityList.size() > player;
}

PlayerEntity* EntityManager::getPlayer(ePlayer player)
{
    return m_pPlayerEntityList.at(player);
}
