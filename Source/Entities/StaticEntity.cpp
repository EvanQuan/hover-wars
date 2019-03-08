#include "EntityHeaders/StaticEntity.h"
#include "EntityManager.h"

// Default Constructor
StaticEntity::StaticEntity(int iID, const vec3* vPosition)
    : Entity( iID, *vPosition, STATIC_ENTITY )
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

// Gets the Spatial Dimensions of the Mesh.
//  The Corners are in local space and will need to be added to the Position of the mesh to convert to world coordinates.
void StaticEntity::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    m_pMesh->getSpatialDimensions(pNegativeCorner, pPositiveCorner);
}

/****************************************************************\
 * Load Functions                                                *
\****************************************************************/

// Load a Plane with a given Normal, Height and Width
void StaticEntity::loadAsPlane(const vec3* vNormal, int iHeight, int iWidth, const ObjectInfo* pObjectProperties, const string& sShaderType)
{
    m_pMesh = MESH_MANAGER->generatePlaneMesh(&m_iTransformationIndex, true, iHeight, iWidth, pObjectProperties, *vNormal);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLE_STRIP);

    m_eType = PLANE_ENTITY; // Set the Entity Type to Plane to avoid reference within the Spatial Map.

    // PHYSICSTODO: Set up Physics Component as a Static Plane Physics Object
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeComponent(true, m_pMesh); // PHYSICSTODO
}

// Load a Sphere with a given Radius
void StaticEntity::loadAsSphere(float fRadius, const ObjectInfo* pObjectProperties, const string& sShaderType)
{
    m_pMesh = MESH_MANAGER->generateSphereMesh(&m_iTransformationIndex, true, fRadius, pObjectProperties);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID,m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLE_STRIP);

    // PHYSICSTODO: Set up Physics Component as a Static Plane Physics Object
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeComponent(true, m_pMesh); // PHYSICSTODO
}

void StaticEntity::loadAsCube(const ObjectInfo* pObjectProperties, const vec3* vDimensions, const string& sShaderType)
{
    m_pMesh = MESH_MANAGER->generateCubeMesh(&m_iTransformationIndex, true, vDimensions->x, vDimensions->y, vDimensions->z, pObjectProperties);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);
    vec3 boundingBox = pObjectProperties->sObjBoundingBox.vDimensions;
    boundingBox *= 0.5; // TODO
    std::cout <<"here load As Cube: Static Entity: " << vDimensions->x << " " << vDimensions->y << " " << vDimensions->z << std::endl;
    PHYSICS_MANAGER->createCubeObject(this->getName(), pObjectProperties->vPosition.x , pObjectProperties->vPosition.y, pObjectProperties->vPosition.z, boundingBox.x, boundingBox .y, boundingBox.z);
}

// Load a Static Mesh from a given file
void StaticEntity::loadFromFile(const string& sFileName, const ObjectInfo* pObjectProperties, const string& sShaderType, float fScale)
{
    // Grab the Mesh Object
    m_pMesh = MESH_MANAGER->loadMeshFromFile(&m_iTransformationIndex, sFileName, pObjectProperties, fScale,  true);

    // Set up Render component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);

    // PHYSICSTODO: Set up Physics Component as a Static Plane Physics Object
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeComponent(true, m_pMesh); // PHYSICSTODO
}
