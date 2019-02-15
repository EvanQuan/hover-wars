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
Mesh* MeshManager::loadMeshFromFile( const string& sFileName, const Material* pMaterial, float fScale, vec3 vPosition, bool bStaticMesh)
{
    // Attempt to grab it from the texture cache if it already exists
    Mesh* pReturnMesh = nullptr;
    string sHashKey = sFileName + materialToString(pMaterial);

    // Found an existing Mesh from that file.
    if ( bStaticMesh && m_pMeshCache.end() != m_pMeshCache.find( sHashKey ) )
    {
        // Grab the Mesh from the Cache
        pReturnMesh = m_pMeshCache[ sHashKey ].get();

        // Add the new instance to the mesh.
        mat4 m4NewTransformInstance = translate(vPosition) * scale(vec3(fScale)) * mat4(1.0f);
        pReturnMesh->addInstance(&m4NewTransformInstance);
    }
    else // Create the New Texture in the Texture Cache, attach the User to the Texture and return the newly created texture.
    {
        // Generate Mesh smart pointer
        unique_ptr<Mesh> pNewMesh = make_unique<Mesh>( sHashKey, bStaticMesh, pMaterial, Mesh::manager_cookie() );

        if ( !pNewMesh->genMesh(sFileName, vPosition, fScale) )
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
Mesh* MeshManager::generatePlaneMesh(bool bStaticMesh, int iHeight, int iWidth, const Material* pMaterial, 
                                     vec3 vPosition, vec3 vNormal)
{
    // Local Variables
    string sHashHandle = "Plane" + to_string(iHeight) + to_string(iWidth) + 
                            materialToString(pMaterial);
    Mesh* pReturnMesh = nullptr;

    // Found a plane of this size that already exists, return that.
    if (m_pMeshCache.end() != m_pMeshCache.find(sHashHandle))
    {
        pReturnMesh = m_pMeshCache[sHashHandle].get();

        // Add the new instance to the mesh.
        pReturnMesh->addInstance(&vPosition, &vNormal, 1.0f);
    }
    else // Generate a new Plane Mesh of height iHeight and width iWidth
    {
        unique_ptr<Mesh> pNewPlane = make_unique<Mesh>(sHashHandle, bStaticMesh, pMaterial, Mesh::manager_cookie());
        pNewPlane->genPlane(iHeight, iWidth, vPosition, vNormal);    // Generate Pane
        pReturnMesh = pNewPlane.get();    // Return raw pointer to managed Mesh.
        m_pMeshCache.insert(make_pair(sHashHandle, move(pNewPlane)));    // Insert into Mesh Cache
    }

    return pReturnMesh;
}

Mesh* MeshManager::generateSphereMesh(bool bStaticMesh, float fRadius, const Material* pMaterial, vec3 vPosition)
{
    // Local Variables
    string sHashHandle = "Sphere" + to_string(fRadius) + materialToString(pMaterial);
    Mesh* pReturnMesh = nullptr;

    // Found a sphere of this radius at this position that already exists, return that.
    if (m_pMeshCache.end() != m_pMeshCache.find(sHashHandle))
    {
        pReturnMesh = m_pMeshCache[sHashHandle].get();

        // Add new Transformation Instance
        pReturnMesh->addInstance(&vPosition, &NORMAL_VECT, 1.0f);
    }
    else // Generate a new Sphere Mesh of given Radius
    {
        unique_ptr<Mesh> pNewSphere = make_unique<Mesh>(sHashHandle, bStaticMesh, pMaterial, Mesh::manager_cookie());
        pNewSphere->genSphere(fRadius, vPosition, false);    // Generate Sphere
        pReturnMesh = pNewSphere.get();                            // Return raw pointer to managed Mesh
        m_pMeshCache.insert(make_pair(sHashHandle, move(pNewSphere)));    // Move Mesh to Cache
    }

    return pReturnMesh;
}

Mesh* MeshManager::generateCubeMesh(bool bStaticMesh, int iHeight, int iWidth, int iDepth, const Material* pMaterial, vec3 vPosition)
{
    // Local Variables
    string sHashHandle = "Cube" + to_string(iHeight) + to_string(iWidth) + to_string(iDepth) + 
                            materialToString(pMaterial);
    Mesh* pReturnMesh = nullptr;

    // Found a cube of these dimensions at this position? Return that.
    if (m_pMeshCache.end() != m_pMeshCache.find(sHashHandle))
    {
        pReturnMesh = m_pMeshCache[sHashHandle].get();

        // Add new Transformation Matrix
        pReturnMesh->addInstance(&vPosition, &NORMAL_VECT, 1.0f);
    }
    else // Generate a new Cube Mesh with given dimensions
    {
        unique_ptr<Mesh> pNewCube = make_unique<Mesh>(sHashHandle, bStaticMesh, pMaterial, Mesh::manager_cookie());
        pNewCube->genCube(iHeight, iWidth, iDepth, vPosition, false);
        pReturnMesh = pNewCube.get();
        m_pMeshCache.insert(make_pair(sHashHandle, move(pNewCube)));
    }

    return pReturnMesh;
}

// Provides a Generated Mesh as a Billboard Mesh
Mesh* MeshManager::generateBillboardMesh(const Material* pMaterial, const void* pOwnerHandle)
{
    // Local Variables
    string sHashHandle = "Billboard" + materialToString(pMaterial) + to_string(reinterpret_cast<intptr_t>(pOwnerHandle));  // This reinterpret_cast is used to make the billboard wholly unique to its owner.
                                                                                                                //  Therefore, any animation or rendering is unique to that owner. Necessary for Billboard uses.
    Mesh* pReturnMesh = nullptr;

    // Find another Billboard with the given hashhandle, if found, return the pre-existing Mesh
    if (m_pMeshCache.end() != m_pMeshCache.find(sHashHandle))
    {
        pReturnMesh = m_pMeshCache[sHashHandle].get();
    }
    else // generate a new Billboard Mesh
    {
        unique_ptr<Mesh> pNewMesh = make_unique<Mesh>(sHashHandle, false, pMaterial, Mesh::manager_cookie());
        pNewMesh->genBillboard();
        pReturnMesh = pNewMesh.get();
        m_pMeshCache.insert(make_pair(sHashHandle, move(pNewMesh)));
    }

    return pReturnMesh;
}


// Generates a Hash string for the given material
// returns a default if pointer is a nullptr.
string MeshManager::materialToString(const Material* sMaterial)
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
