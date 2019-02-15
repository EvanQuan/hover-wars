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
#include "EntityHeaders/Camera.h"
#include "EntityComponentHeaders/EntityComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/LightingComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "EntityComponentHeaders/AnimationComponent.h"
#include "Physics/PhysicsManager.h"

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
    void pause() { m_bPause = !m_bPause; }

    // Entity Functions
    Camera* generateCameraEntity();
    void generateStaticPlane(int iHeight, int iWidth, const vec3* vPosition, const vec3* vNormal, const Material* sMaterial, const string& sShaderType = "");
    void generateStaticSphere(float fRadius, const vec3* vPosition, const Material* sMaterial, const string& sShaderType = "");
    void generateStaticMesh(const string& sMeshLocation, const vec3* vPosition, const Material* sMaterial, float fScale, const string& sShaderType = "" );
    void generateStaticPointLight( float fPower, const vec3* vPosition, const vec3* vColor, const Material* sMaterial, const string& sMeshLocation = "", float m_fMeshScale = 1.0);
    void generateDirectionalLight( const vec3* vDirection, const vec3* vAmbientColor, const vec3* vDiffuseColor, const vec3* vSpecularColor );
    void generateStaticSpotLight(float fPhi, float fSoftPhi, const vec3* vPosition, const vec3* vColor, const vec3* vDirection, const Material* sMaterial, const string& sMeshLocation = "", float m_fMeshScale = 1.0);
    void generatePlayerEntity(const vec3* vPosition, const string& sMeshLocation, const Material* sMaterial, float fScale, const string& sShaderType = "");
    InteractableEntity* generateInteractableEntity(const vec3* vPosition);
    vec3 getEntityPosition(int iEntityID);

    // Entity Component functions
    CameraComponent* generateCameraComponent(int iEntityID);
    RenderComponent* generateRenderComponent(int iEntityID, Mesh const* pMeshKey, bool bStaticDraw, ShaderManager::eShaderType eType, GLenum eMode);
    LightingComponent* generateLightingComponent(int iEntityID);
    PhysicsComponent* generatePhysicsComponent(int iEntityID);
    AnimationComponent* generateAnimationComponent(int iEntityID);

    // Camera Management
    void updateHxW(int iHeight, int iWidth);

    // The GameManager instead retrieves the camera components from the player
    // entities to set as its active cameras
    const CameraComponent* getActiveCameraComponent() { return m_pActiveCameraComponent; }

    // Clears the Environment so a new one can be loaded.
    void purgeEnvironment();
    void renderEnvironment( );
    void updateEnvironment(const Time& pTimer);
    void changeFrameRate(duration<float> pNewFrameRate) { m_pMaxDeltaTime = pNewFrameRate; }
    
    /*
    The command handler can get all the players to directly communicate to.
    */
    PlayerEntity* getPlayer(ePlayer player);
    bool playerExists(ePlayer);

private:
    EntityManager();
    EntityManager(const EntityManager* pCopy);
    static EntityManager* m_pInstance;

    // Entity Managing
    int m_iEntityIDPool, m_iComponentIDPool;
    int m_iHeight, m_iWidth;
    inline int getNewEntityID() { return ++m_iEntityIDPool; }
    inline int getNewComponentID() { return ++m_iComponentIDPool; }
    vector<PhysicsComponent*>                       m_pPhysicsComponents;   // PHYSICSTODO: If this isn't necessary, remove it.
    vector<unique_ptr<EntityComponent>>             m_pMasterComponentList;
    vector<PlayerEntity*>                           m_pPlayerEntityList;    
    vector<unique_ptr<Entity>>                      m_pMasterEntityList;
    unordered_map<Mesh const*, RenderComponent*>    m_pRenderingComponents;
    vector<CameraComponent*>                        m_pCameraComponents;
    vector<LightingComponent*>                      m_pLights;
    CameraComponent*                                m_pActiveCameraComponent;
    vector<AnimationComponent*>                     m_pAnimationComponents;
    DirectionalLight*                               m_pDirectionalLight;
    InteractableEntity*                             m_pBillboardTesting;
    PointLight*                                     m_pTestingLight;        // Temporary, Needs to be removed.

    // Manage Pointers for Deletion.
    MeshManager*                m_pMshMngr;
    TextureManager*             m_pTxtMngr;
    SceneLoader*                m_pScnLdr;
    EmitterEngine*              m_pEmtrEngn;
    PhysicsManager*             m_pPhysxMngr;

    // Edge Threshold Implementation
    float m_fMinEdgeThreshold, m_fMaxEdgeThreshold;
    bool m_bPause;

    /*
    Represents the time of the current frame
    This value should be changed every time the environment is updated.

    Unit: second
    */
    duration<float> m_pFrameTime;
    /*
    This determines the environment update, and thus the frame rate.

    Unit: second
    */
    duration<float> m_pMaxDeltaTime;
};

