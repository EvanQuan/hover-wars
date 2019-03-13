#include "MeshManager.h"

// Constant Up Vector
const vec3 NORMAL_VECT(0.0f, 1.0f, 0.0f);

// Singleton Static Instance Definition
MeshManager* MeshManager::pInstance = nullptr;

MeshManager::MeshManager()
{
    // Nothing to Construct
}

MeshManager* MeshManager::getInstance()
{
    if (nullptr == pInstance)
    {
        pInstance = new MeshManager();
    }

    return pInstance;
}


// Destructor: Clear the Mesh Cache and release the memory.
MeshManager::~MeshManager()
{
    unloadAllMeshes();
}

// unloads all current Meshes to create a clean slate.
//        Since the meshes are unique pointers, calling clear on the container will properly
//            call the destructors of the meshes.
void MeshManager::unloadAllMeshes()
{
    m_pMeshCache.clear();
}

// loadMeshFromFile:    Takes in a FileName. Load in a mesh from that filename if possible.
// Return:                Returns a pointer to the desired mesh from the specified file.
// Parameters:            sFileName - The location of the file to load.
// Written by:            James Cote
Mesh* MeshManager::loadMeshFromFile(unsigned int *iTransformIndex, const string& sFileName, const ObjectInfo* pObjectProperties, float fScale, bool bStaticMesh)
{
    // Attempt to grab it from the texture cache if it already exists
    Mesh* pReturnMesh = nullptr;
    string sHashKey = sFileName + materialToString(&pObjectProperties->sObjMaterial);

    // Found an existing Mesh from that file.
    if ( m_pMeshCache.end() != m_pMeshCache.find( sHashKey ) )
    {
        // Grab the Mesh from the Cache
        pReturnMesh = m_pMeshCache[ sHashKey ].get();

        // Add the new instance to the mesh.
        if (bStaticMesh)
        {
            mat4 m4TranslationMat4 = translate(pObjectProperties->vPosition);
            *iTransformIndex = pReturnMesh->addInstance(&m4TranslationMat4);      // Add new Position for the Mesh
        }
    }
    else // Create the New Texture in the Texture Cache, attach the User to the Texture and return the newly created texture.
    {
        // Generate Mesh smart pointer
        unique_ptr<Mesh> pNewMesh = make_unique<Mesh>( sHashKey, bStaticMesh, fScale, pObjectProperties, Mesh::manager_cookie() );

        if ( !pNewMesh->genMesh(sFileName, pObjectProperties->vPosition, fScale) )
        {
            if (sFileName != "")
            {
                cout << "Error, unable to load mesh: " << sFileName << endl;
            }
            pNewMesh.reset();
        }
        else
        {
            // Return Newly Created Mesh.
            pReturnMesh = pNewMesh.get();
            *iTransformIndex = 0;   // Set the return index to 0 since it's the first instance of the Mesh;

            // Attach Mesh to the Cache
            m_pMeshCache.insert(make_pair( sHashKey, move(pNewMesh) ));
        }
    }

    return pReturnMesh;
}

// genereatePlaneMesh:    Searches the Cache for a similar mesh with handle "Plane<iHeight><iWidth>"
//                            or creates a new Plane Mesh if one hasn't been created yet.
// Returns:                Generated plane mesh or nullptr if no mesh was able to be generated.
// Written by:            James Cote
Mesh* MeshManager::generatePlaneMesh(unsigned int *iTransformIndex, bool bStaticMesh, int iHeight, int iWidth, const ObjectInfo* pObjectProperties, vec3 vNormal)
{
    // Local Variables
    string sHashHandle = "Plane" + to_string(iHeight) + to_string(iWidth) +
                            materialToString(&pObjectProperties->sObjMaterial);
    Mesh* pReturnMesh = nullptr;

    // Found a plane of this size that already exists, return that.
    if (m_pMeshCache.end() != m_pMeshCache.find(sHashHandle))
    {
        pReturnMesh = m_pMeshCache[sHashHandle].get();

        // Add the new instance to the mesh if it's a static mesh.
        if( bStaticMesh )
            *iTransformIndex = pReturnMesh->addInstance(&pObjectProperties->vPosition, &vNormal, 1.0f);
    }
    else // Generate a new Plane Mesh of height iHeight and width iWidth
    {
        unique_ptr<Mesh> pNewPlane = make_unique<Mesh>(sHashHandle, bStaticMesh, 1.0f, pObjectProperties, Mesh::manager_cookie());
        pNewPlane->genPlane(iHeight, iWidth, pObjectProperties->vPosition, vNormal);    // Generate Plane
        pReturnMesh = pNewPlane.get();                                                  // Return raw pointer to managed Mesh.
        *iTransformIndex = 0;                                                           // Set the return index to 0 since it's the first instance of the Mesh;
        m_pMeshCache.insert(make_pair(sHashHandle, move(pNewPlane)));                   // Insert into Mesh Cache
    }

    return pReturnMesh;
}

Mesh* MeshManager::generateSphereMesh(unsigned int *iTransformIndex, bool bStaticMesh, float fRadius, const ObjectInfo* pObjectProperties )
{
    // Local Variables
    string sHashHandle = "Sphere" + to_string(fRadius) + materialToString(&pObjectProperties->sObjMaterial);
    Mesh* pReturnMesh = nullptr;

    // Found a sphere of this radius at this position that already exists, return that.
    if (m_pMeshCache.end() != m_pMeshCache.find(sHashHandle))
    {
        pReturnMesh = m_pMeshCache[sHashHandle].get();

        // Add new Transformation Instance
        if (bStaticMesh)
            *iTransformIndex = pReturnMesh->addInstance(&pObjectProperties->vPosition, &NORMAL_VECT, 1.0f);
    }
    else // Generate a new Sphere Mesh of given Radius
    {
        unique_ptr<Mesh> pNewSphere = make_unique<Mesh>(sHashHandle, bStaticMesh, 1.0f, pObjectProperties, Mesh::manager_cookie());
        pNewSphere->genSphere(fRadius, pObjectProperties->vPosition);    // Generate Sphere
        pReturnMesh = pNewSphere.get();                                  // Return raw pointer to managed Mesh
        *iTransformIndex = 0;                                            // Set the return index to 0 since it's the first instance of the Mesh;
        m_pMeshCache.insert(make_pair(sHashHandle, move(pNewSphere)));   // Move Mesh to Cache
    }

    return pReturnMesh;
}

Mesh* MeshManager::generateCubeMesh(unsigned int *iTransformIndex, bool bStaticMesh, float fHeight, float fWidth, float fDepth, const ObjectInfo* pObjectProperties)
{
    // Local Variables
    string sHashHandle = "Cube" + to_string(fHeight) + to_string(fWidth) + to_string(fDepth) +
                            materialToString(&pObjectProperties->sObjMaterial);
    Mesh* pReturnMesh = nullptr;

    // Found a cube of these dimensions at this position? Return that.
    if (m_pMeshCache.end() != m_pMeshCache.find(sHashHandle))
    {
        pReturnMesh = m_pMeshCache[sHashHandle].get();

        // Add new Transformation Matrix
        if (bStaticMesh)
            *iTransformIndex = pReturnMesh->addInstance(&pObjectProperties->vPosition, &NORMAL_VECT, 1.0f);
    }
    else // Generate a new Cube Mesh with given dimensions
    {
        unique_ptr<Mesh> pNewCube = make_unique<Mesh>(sHashHandle, bStaticMesh, 1.0f, pObjectProperties, Mesh::manager_cookie());
        pNewCube->genCube(fHeight, fWidth, fDepth, pObjectProperties->vPosition);
        pReturnMesh = pNewCube.get();
        *iTransformIndex = 0;                                           // Set the return index to 0 since it's the first instance of the Mesh;
        m_pMeshCache.insert(make_pair(sHashHandle, move(pNewCube)));
    }

    return pReturnMesh;
}

// Provides a Generated Mesh as a Billboard Mesh
Mesh* MeshManager::generateBillboardMesh(const ObjectInfo* pObjectProperties, const void* pOwnerHandle)
{
    // Local Variables
    string sHashHandle = "Billboard" + materialToString(&pObjectProperties->sObjMaterial) + to_string(reinterpret_cast<intptr_t>(pOwnerHandle));  // This reinterpret_cast is used to make the billboard wholly unique to its owner.
                                                                                                                //  Therefore, any animation or rendering is unique to that owner. Necessary for Billboard uses.
    Mesh* pReturnMesh = nullptr;

    // Find another Billboard with the given hashhandle, if found, return the pre-existing Mesh
    if (m_pMeshCache.end() != m_pMeshCache.find(sHashHandle))
    {
        pReturnMesh = m_pMeshCache[sHashHandle].get();
    }
    else // generate a new Billboard Mesh
    {
        unique_ptr<Mesh> pNewMesh = make_unique<Mesh>(sHashHandle, false, 1.0f, pObjectProperties, Mesh::manager_cookie());
        pNewMesh->genBillboard();
        pReturnMesh = pNewMesh.get();
        m_pMeshCache.insert(make_pair(sHashHandle, move(pNewMesh)));
    }

    return pReturnMesh;
}

/****************************************************************************************************************\
 * Private Functions                                                                                            *
\****************************************************************************************************************/

// Generates a Hash string for the given material
// returns a default if pointer is a nullptr.
string MeshManager::materialToString(const ObjectInfo::Material* sMaterial)
{
    // Return String
    string sReturnString;

    if (nullptr != sMaterial)
    {
        // Append Diffuse Map Location or diffuse color if specified.
        if ("" != sMaterial->sDiffuseMap)
            sReturnString += sMaterial->sDiffuseMap;
        else
            sReturnString += glm::to_string( sMaterial->vOptionalDiffuseColor );

        // Store the location of the specified specular map or the color of the specular shade.
        if ("" != sMaterial->sOptionalSpecMap)
            sReturnString += sMaterial->sOptionalSpecMap;
        else    // "" as Spec Map Location? use spec shade as part of hash string.
            sReturnString += glm::to_string(sMaterial->vOptionalSpecShade);

        // add shininess to hash string
        sReturnString += to_string(sMaterial->fShininess);
    }
    else    // No material specified, just return a default hash string.
        sReturnString = "NoMaterial";

    // Return the Hash String for the material
    return sReturnString;
}
