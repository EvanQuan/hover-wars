#include "EntityHeaders/StaticEntity.h"
#include "EntityManager.h"

// Default Constructor
StaticEntity::StaticEntity(int iID, const vec3* vPosition)
    : Entity( iID, *vPosition )
{
    
}

// Destructor
StaticEntity::~StaticEntity()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void StaticEntity::update(float fTimeInMilliseconds)
{
    /* Not Implemented */
}

/****************************************************************\
 * Load Functions                                                *
\****************************************************************/

// Load a Plane with a given Normal, Height and Width
void StaticEntity::loadAsPlane(const vec3* vNormal, int iHeight, int iWidth, const ObjectInfo* pObjectProperties, const string& sShaderType)
{
    m_pMesh = MESH_MANAGER->generatePlaneMesh(true, iHeight, iWidth, pObjectProperties, *vNormal);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLE_STRIP);

    // PHYSICSTODO: Set up Physics Component as a Static Plane Physics Object
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeComponent(true, m_pMesh); // PHYSICSTODO
}

// Load a Sphere with a given Radius
void StaticEntity::loadAsSphere(float fRadius, const ObjectInfo* pObjectProperties, const string& sShaderType)
{
    m_pMesh = MESH_MANAGER->generateSphereMesh(true, fRadius, pObjectProperties);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLE_STRIP);

    // PHYSICSTODO: Set up Physics Component as a Static Plane Physics Object
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeComponent(true, m_pMesh); // PHYSICSTODO
}

// Load a Static Mesh from a given file
void StaticEntity::loadFromFile(const string& sFileName, const ObjectInfo* pObjectProperties, const string& sShaderType, float fScale)
{
    // Grab the Mesh Object
    m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, pObjectProperties, fScale,  true);

    // Set up Render component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);

    // PHYSICSTODO: Set up Physics Component as a Static Plane Physics Object
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeComponent(true, m_pMesh); // PHYSICSTODO
}
