#include "EntityHeaders/StaticEntity.h"
#include "EntityManager.h"

// Default Constructor
StaticEntity::StaticEntity(int iID, const vec3* vPosition)
    : Entity( iID, *vPosition, ENTITY_STATIC )
{
    m_pPhysicsActor = nullptr;
}

// Destructor
StaticEntity::~StaticEntity()
{
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void StaticEntity::update(float fTimeInSeconds)
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
    m_pMesh = MESH_MANAGER->generatePlaneMesh(true, iHeight, iWidth, pObjectProperties, m_sName, *vNormal);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, false, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLE_STRIP);

    m_eType = ENTITY_PLANE; // Set the Entity Type to Plane to avoid reference within the Spatial Map.

    // PHYSICSTODO: Set up Physics Component as a Static Plane Physics Object
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeVehicle(true, m_pMesh); // PHYSICSTODO
}

// Load a Sphere with a given Radius
void StaticEntity::loadAsSphere(float fRadius, const ObjectInfo* pObjectProperties, const string& sShaderType)
{
    m_pMesh = MESH_MANAGER->generateSphereMesh(true, fRadius, pObjectProperties, m_sName);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID,m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLE_STRIP);

    // PHYSICSTODO: Set up Physics Component as a Static Plane Physics Object
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeVehicle(true, m_pMesh); // PHYSICSTODO
}

void StaticEntity::loadAsCube(const ObjectInfo* pObjectProperties, const vec3* vDimensions, const string& sShaderType)
{
    m_pMesh = MESH_MANAGER->generateCubeMesh(true, vDimensions->x, vDimensions->y, vDimensions->z, pObjectProperties, m_sName);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);
    vec3 boundingBox = pObjectProperties->sObjBoundingBox.vDimensions;
    boundingBox *= 0.5; // TODO
    //std::cout <<"here load As Cube: Static Entity: " << vDimensions->x << " " << vDimensions->y << " " << vDimensions->z << std::endl;
    m_pPhysicsActor = PHYSICS_MANAGER->createCubeObject(this->getName(), pObjectProperties->vPosition.x , pObjectProperties->vPosition.y, pObjectProperties->vPosition.z, boundingBox.x, boundingBox.y, boundingBox.z);
}

// Load a Static Mesh from a given file
void StaticEntity::loadFromFile(const string& sFileName, const ObjectInfo* pObjectProperties, const string& sShaderType, float fScale)
{
    // Grab the Mesh Object
    m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, pObjectProperties, m_sName, fScale, true);

    // Set up Render component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);

    // Set up Rigid Static Actor for Mesh.
    m_pPhysicsActor = PHYSICS_MANAGER->createStaticMeshObject(m_sName.c_str(), m_pMesh, &pObjectProperties->vPosition);
}
