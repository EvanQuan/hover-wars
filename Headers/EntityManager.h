#pragma once
#include "stdafx.h"
#include "EntityHeaders/PointLight.h"
#include "EntityHeaders/DirectionalLight.h"
#include "EntityHeaders/SpotLight.h"
#include "SceneLoader.h"
#include "EmitterEngine.h"
#include "EntityHeaders/InteractableEntity.h"
#include "EntityHeaders/Entity.h"
#include "EntityHeaders/PlayerEntity.h"
#include "EntityHeaders/BotEntity.h"
#include "EntityHeaders/Camera.h"
#include "EntityComponentHeaders/EntityComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/LightingComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "EntityComponentHeaders/AnimationComponent.h"
#include "Physics/PhysicsManager.h"
#include "SpatialDataMap.h"
#include "UserInterface/UserInterface.h"

// Environment Manager
// Manages all objects in an environment
// Written by: James Coté, Evan Quan
class EntityManager
{
public:
    // Singleton instance retrieval and Destructor
    static EntityManager* getInstance();
    ~EntityManager();

    // Init and Pause Functionality
    void initializeEnvironment(string sFileName);

    // Scene Management Functionality
    void pause() { m_bPause = !m_bPause; }
    void toggleBBDrawing() { m_bDrawBoundingBoxes = !m_bDrawBoundingBoxes; }
    void toggleSpatialMapDrawing() { m_bDrawSpatialMap = !m_bDrawSpatialMap; }
    bool doBoundingBoxDrawing() { return m_bDrawBoundingBoxes; }
    bool doShadowDraw() { return m_bShadowDraw; }
    void toggleDebugCamera() { m_bUseDebugCamera = !m_bUseDebugCamera; }
    void initializeSpatialMap(float fLength, float fWidth, float fTileSize);

    // Entity Functions
    Camera* generateCameraEntity();
    void generateStaticPlane(const char* name, const ObjectInfo* pObjectProperties, int iHeight, int iWidth, const vec3* vNormal, const string& sShaderType = "");
    void generateStaticSphere(const char* name, const ObjectInfo* pObjectProperties, float fRadius, const string& sShaderType = "");
    void generateStaticCube(const char* name, const ObjectInfo* pObjectProperties, const vec3* vDimensions, const string& sShaderType = "");
    void generateStaticMesh(const char* name, const ObjectInfo* pObjectProperties, const string& sMeshLocation, float fScale, const string& sShaderType = "" );
    void generateStaticPointLight(const ObjectInfo* pObjectProperties, float fPower, const vec3* vColor, const string& sMeshLocation = "", float m_fMeshScale = 1.0);
    void generateDirectionalLight( const vec3* vDirection, const vec3* vAmbientColor, const vec3* vDiffuseColor, const vec3* vSpecularColor,
                                    float fPosition, float fNearPlane, float fFarPlane, unsigned int iShadowHeight, unsigned int iShadowWidth, float fShadowFrame);
    void generateStaticSpotLight(const ObjectInfo* pObjectProperties, float fPhi, float fSoftPhi, const vec3* vColor, const vec3* vDirection, const string& sMeshLocation = "", float m_fMeshScale = 1.0);
    void generatePlayerEntity(const ObjectInfo* pObjectProperties, const string& sMeshLocation, float fScale, const string& sShaderType = "");
    void generateBotEntity(const ObjectInfo* pObjectProperties, const string& sMeshLocation, float fScale, const string& sShaderType = "");
    InteractableEntity* generateInteractableEntity(const vec3* vPosition);
    vec3 getEntityPosition(int iEntityID);

    // Entity Component functions
    CameraComponent* generateCameraComponent(int iEntityID);
    RenderComponent* generateRenderComponent(int iEntityID, Mesh const* pMeshKey, bool bStaticDraw, ShaderManager::eShaderType eType, GLenum eMode);
    LightingComponent* generateLightingComponent(int iEntityID);
    PhysicsComponent* generatePhysicsComponent(int iEntityID);
    AnimationComponent* generateAnimationComponent(int iEntityID);

    // Camera Management
    void updateWidthAndHeight(int iWidth, int iHeight);
    void rotateCamera(vec2 pDelta); 
    void zoomCamera(float fDelta);

    // The GameManager instead retrieves the camera components from the player
    // entities to set as its active cameras
    const CameraComponent* getActiveCameraComponent() { return m_pActiveCameraComponent; }

    // Clears the Environment so a new one can be loaded.
    void purgeEnvironment();
    void renderEnvironment( );
    void updateEnvironment(const GameTime* pTimer);
    
    /*
    The command handler can get all the players to directly communicate to.
    */
    PlayerEntity* getPlayer(ePlayer player);
    bool playerExists(ePlayer player);
    BotEntity* getBot(eBot bot);
    bool botExists(eBot bot);

private:
    EntityManager();
    EntityManager(const EntityManager* pCopy);
    static EntityManager* m_pInstance;

    // Entity Managing
    int m_iEntityIDPool, m_iComponentIDPool;
    int m_iHeight, m_iWidth;
    duration<float> m_fGameTime;
    inline int getNewEntityID() { return ++m_iEntityIDPool; }
    inline int getNewComponentID() { return ++m_iComponentIDPool; }

    // Master
    vector<unique_ptr<Entity>>                      m_pMasterEntityList;
    vector<unique_ptr<EntityComponent>>             m_pMasterComponentList;
    // Phycis
    vector<PhysicsComponent*>                       m_pPhysicsComponents;   // PHYSICSTODO: If this isn't necessary, remove it.
    // Players
    vector<PlayerEntity*>                           m_pPlayerEntityList;    
    // Bots
    vector<BotEntity*>                              m_pBotEntityList;    
    // Rendering
    unordered_map<Mesh const*, RenderComponent*>    m_pRenderingComponents;
    // Cameras
    vector<CameraComponent*>                        m_pCameraComponents;
    CameraComponent*                                m_pActiveCameraComponent;
    //  Lighting
    vector<LightingComponent*>                      m_pLights;
    vector<AnimationComponent*>                     m_pAnimationComponents;
    DirectionalLight*                               m_pDirectionalLight;
    // Interactable
    InteractableEntity*                             m_pBillboardTesting;

    // Manage Pointers for Deletion.
    MeshManager*                m_pMshMngr;
    TextureManager*             m_pTxtMngr;
    SceneLoader*                m_pScnLdr;
    EmitterEngine*              m_pEmtrEngn;
    PhysicsManager*             m_pPhysxMngr;
    SpatialDataMap*             m_pSpatialMap;
    ShaderManager*              m_pShdrMngr;
    
    // Private Functions
    void doRender();
    void resetFBO();
    void renderAxis();
    void setCameraPMVMatrices();

    // Camera
    bool m_bUseDebugCamera;
    Camera* m_pCamera;

    // Rendering World Axis
    // Axis Buffer/Array Containers
    GLuint m_pVertexArray;
    GLuint m_pVertexBuffer;

    // Scene Management toggling
    bool m_bPause, m_bDrawBoundingBoxes, m_bDrawSpatialMap, m_bShadowDraw;
};

