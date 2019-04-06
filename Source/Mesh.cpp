#include "Mesh.h"
#include <sstream>
#include "TextureManager.h"

/****************************\
 * Constants: For Materials *
\****************************/
const string DEFAULT_DIFFUSE_MAP = "textures/defaultTexture.png";
const vec4 DEFAULT_SPEC_COLOR = vec4(vec3(0.f), 1.0f);

/************************************\
 * Defines: For Sphere Construction *
\************************************/
#define SLICE_SIZE 5.f
#define MAX_THETA_DEGS 360.f
#define MAX_PHI_DEGS 180.f
#define MAX_THETA_CUTS (int)(MAX_THETA_DEGS / SLICE_SIZE)
#define MAX_PHI_CUTS (int)(MAX_PHI_DEGS / SLICE_SIZE)

/**********************************************\
 * Defines: For Billboard Buffer Manipulation *
\**********************************************/
#define BILLBOARD_STRIDE    (sizeof(vec3) + /*Vertex*/ sizeof(vec3) + /*Normal*/ sizeof(vec2) + /*UVStart*/ sizeof(vec2) + /*UVEnd*/ sizeof(vec2) /*Height/Width*/ + sizeof(float) /*Duration*/ )
#define VERTEX_OFFSET       0
#define NORMAL_OFFSET       sizeof(vec3)
#define UV_START_OFFSET     (sizeof(vec3) << 1)
#define UV_END_OFFSET       ((sizeof(vec3) << 1) + sizeof(vec2))
#define DIMENSION_OFFSET    ((sizeof(vec3) << 1) + (sizeof(vec2) << 1))
#define DURATION_OFFSET     (DIMENSION_OFFSET + sizeof(vec2))

// Basic Constructor
Mesh::Mesh(const string &sManagerKey, bool bStaticMesh, float fScale, const ObjectInfo* pObjectProperties, string sHashKey, manager_cookie)
{
    m_sManagerKey = sManagerKey;
    m_bStaticMesh = bStaticMesh;
    m_pShdrMngr = SHADER_MANAGER;
    m_vNegativeOffset = m_vPositiveOffset = vec3(0.0f);
    glGenVertexArrays(1, &m_iVertexArray);
    m_m4ScaleMatrix = scale(vec3(fScale));
    m_m4InstanceMap.clear();

    loadObjectInfo(pObjectProperties, sHashKey);
}

// Delete any buffers that we initialized
Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_iVertexBuffer);
    glDeleteBuffers(1, &m_iIndicesBuffer);
    glDeleteBuffers(1, &m_iInstancedBuffer);
    glDeleteVertexArrays(1, &m_iVertexArray);

    // Delete the Buffers of the Bounding Box
    m_sBoundingBox.deleteBuffers();
}

// Load the Mesh from a given file name
//  Result: Stores the mesh variables into a set of vertices
bool Mesh::genMesh(const string& sFileName, vec3 vPosition, string sHashKey, float fScale)
{
    // Return Value
    bool bReturnValue = true;

    // Load Mesh
    if (sFileName.substr(sFileName.find_last_of(".") + 1) == "obj")
        bReturnValue = loadObj(sFileName);

    // Store Mesh in GPU
    if (bReturnValue)
    {
        // Save Scale Matrix
        m_m4ScaleMatrix = scale(vec3(fScale));

        // Set Bounding Box Rendering information if Spatial Calculation Type Specified
        if (SPATIAL_CALC == m_sBoundingBox.eType)
            m_sBoundingBox.generateCubicBox(&m_vNegativeOffset, &m_vPositiveOffset);

        // Add Transformation for Static Objects
        if (m_bStaticMesh)
        {
            mat4 m4Transformation = m_m4ScaleMatrix * mat4(1.0f);

            // Translation
            if (vec3(0.f) != vPosition)
                m4Transformation = translate(vPosition) * m4Transformation;

            // Store initial transformation
            //m_m4ListOfInstances.push_back(m4Transformation);
            m_m4InstanceMap.insert(make_pair(sHashKey, m4Transformation));
        }

        // Initialize VBOs
        //initalizeVBOs();
    }

    // Return result
    return bReturnValue;
}

// Generate a generic plane at the origin with a given Height and Width
//    Normals are along the y-axis.
void Mesh::genPlane(int iHeight, int iWidth, vec3 vPosition, vec3 vNormal, string sHashKey)
{
    // Generate 4 vertices around the origin
    /***********************************
        Width = (x-axis)
        Height = (z-axis)
    */
    float fHeight = static_cast<float>(iHeight);
    float fWidth = static_cast<float>(iWidth);
    float fHalfHeight = fHeight * 0.5f;
    float fHalfWidth = fWidth * 0.5f;
    vNormal = normalize(vNormal); // Normalize Normal Vector
    m_pVertices.push_back(vec3(-fHalfWidth, 0.f, -fHalfHeight));
    m_pVertices.push_back(vec3(-fHalfWidth, 0.f, fHalfHeight));
    m_pVertices.push_back(vec3(fHalfWidth, 0.f, -fHalfHeight));
    m_pVertices.push_back(vec3(fHalfWidth, 0.f, fHalfHeight));

    // Generate Normals for each Vertex.
    m_pNormals.insert(m_pNormals.begin(), 4, vNormal);

    fHalfHeight *= 0.1f;
    fHalfWidth *= 0.1f;

    // Generate UVs for Plane
    m_pUVs.push_back(vec2(0.0, 0.0));
    m_pUVs.push_back(vec2(0.0, fHalfHeight));
    m_pUVs.push_back(vec2(fHalfWidth, 0.0));
    m_pUVs.push_back(vec2(fHalfWidth, fHalfHeight));

    // Generate Indices
    m_pIndices = { 0, 1, 2, 1, 2, 3 };

    // Set Spatial Cube/Plane
    m_vNegativeOffset = m_pVertices.front();
    m_vPositiveOffset = m_pVertices.back();

    if (SPATIAL_CALC == m_sBoundingBox.eType)
        m_sBoundingBox.generateCubicBox(0.0f, static_cast<float>(iWidth), static_cast<float>(iHeight));

    // Store Initial Transformation Matrix for Static Meshes
    if (m_bStaticMesh)
    {
        // Translation Matrix
        mat4 m4TranslationMatrix = getRotationMat4ToNormal(&vNormal);

        // If translation is necessary, translate plane.
        if (vec3(0.f) != vPosition)
            m4TranslationMatrix = translate(vPosition) * m4TranslationMatrix;

        // Store Transformation to List
        //m_m4ListOfInstances.push_back(m4TranslationMatrix);
        m_m4InstanceMap.insert(make_pair(sHashKey, m4TranslationMatrix));
    }

    // Load Mesh into GPU
    initalizeVBOs();
}

// Generates a Sphere Mesh
void Mesh::genSphere(float fRadius, vec3 vPosition, string sHashKey)
{
    // Algorithm pulled from: https://goo.gl/k9Q4mh
    float const R = 1.f / static_cast<float>(MAX_THETA_CUTS - 1);
    float const S = 1.f / static_cast<float>(MAX_PHI_CUTS - 1);
    int r, s;

    // Calculate Sizes of vectors ahead of time to avoid resize calls during loop
    m_pVertices.resize(MAX_THETA_CUTS * MAX_PHI_CUTS);
    m_pNormals.resize(MAX_THETA_CUTS * MAX_PHI_CUTS);
    m_pUVs.resize(MAX_THETA_CUTS * MAX_PHI_CUTS);

    // Populate Iterators as loop progresses
    vector<vec3>::iterator v = m_pVertices.begin();
    vector<vec3>::iterator n = m_pNormals.begin();
    vector<vec2>::iterator t = m_pUVs.begin();

    // Loop through Sphere and generate UVs, Vertices and Normals.
    for (r = 0; r < MAX_THETA_CUTS; r++) for (s = 0; s < MAX_PHI_CUTS; s++) {
        float const y = sinf(-(PI / 2) + PI * r * R);
        float const x = cosf(2 * PI * s * S) * sinf(PI * r * R);
        float const z = sinf(2 * PI * s * S) * sinf(PI * r * R);

        // UVs are inverted.
        *t++ = vec2((MAX_PHI_CUTS - s) * S, (MAX_THETA_CUTS - r) * R);

        // Scale Sphere to Radius.
        *v++ = vec3(x * fRadius, y * fRadius, z * fRadius);

        // Store normal in Local space.
        *n++ = vec3(x, y, z);
    }

    // Generate indices
    m_pIndices.resize(MAX_THETA_CUTS * MAX_PHI_CUTS * 4);
    vector<unsigned int>::iterator i = m_pIndices.begin();
    unsigned int iWrapAroundMask = m_pVertices.size();

    // Ensure index storage creates counter clockwise pattern for back-face culling.
    for (r = 0; r < MAX_THETA_CUTS; r++) for (s = 0; s < MAX_PHI_CUTS; s++) {
        *i++ = (r * MAX_PHI_CUTS + s) % iWrapAroundMask;
        *i++ = ((r + 1) * MAX_PHI_CUTS + s) % iWrapAroundMask;
        *i++ = (r * MAX_PHI_CUTS + (s + 1)) % iWrapAroundMask;
        *i++ = ((r + 1) * MAX_PHI_CUTS + (s + 1)) % iWrapAroundMask;
    }

    // Compute Spatial Cube
    m_vNegativeOffset = vec3(-fRadius, -fRadius, -fRadius);
    m_vPositiveOffset = vec3(fRadius, fRadius, fRadius);

    // Generate Cubic Bounding Box if specified
    if (SPATIAL_CALC == m_sBoundingBox.eType)
    {
        float fRadius2 = fRadius * 2.0f;
        m_sBoundingBox.generateCubicBox(fRadius2, fRadius2, fRadius2);
    }

    // Store Initial Transformation if Static Mesh
    if (m_bStaticMesh)
    {
        // Initial Transformation Matrix
        mat4 m4InitialTransformation = mat4(1.0f);

        // Translate to Position if Sphere is a Static Mesh.
        if (vec3(0.f) != vPosition)
            m4InitialTransformation = translate(vPosition) * m4InitialTransformation;

        // Store Initial Transformation Matrix
        //m_m4ListOfInstances.push_back(m4InitialTransformation);
        m_m4InstanceMap.insert(make_pair(sHashKey, m4InitialTransformation));
    }

    // Store Mesh in GPU
    initalizeVBOs();
}

// Generates a Cube object given a Height, Width and Depth dimension as well as a position.
void Mesh::genCube(float fHeight, float fWidth, float fDepth, vec3 vPosition, string sHashKey)
{
    // Get half sizes of dimensions to set vertices wrt to origin.
    float iHalfHeight = fHeight * 0.5f;
    float iHalfWidth =  fWidth * 0.5f;
    float iHalfDepth =  fDepth * 0.5f;

    // Reserve Sizes ahead of time to speed up computation
    m_pVertices.reserve(24);
    m_pNormals.reserve(24);
    m_pUVs.reserve(24);
    m_pIndices.reserve(36);

    vec3 vIndexes[8] = {
        vec3(-iHalfWidth, iHalfHeight, iHalfDepth),
        vec3(iHalfWidth, iHalfHeight, iHalfDepth),
        vec3(iHalfWidth, -iHalfHeight, iHalfDepth),
        vec3(-iHalfWidth, -iHalfHeight, iHalfDepth),
        vec3(-iHalfWidth, iHalfHeight, -iHalfDepth),
        vec3(iHalfWidth, iHalfHeight, -iHalfDepth),
        vec3(-iHalfWidth, -iHalfHeight, -iHalfDepth),
        vec3(iHalfWidth, -iHalfHeight, -iHalfDepth)
    };

    vec3 vNormals[8] = {
        normalize(vIndexes[0] - vec3(0.f)),
        normalize(vIndexes[1] - vec3(0.f)),
        normalize(vIndexes[2] - vec3(0.f)),
        normalize(vIndexes[3] - vec3(0.f)),
        normalize(vIndexes[4] - vec3(0.f)),
        normalize(vIndexes[5] - vec3(0.f)),
        normalize(vIndexes[6] - vec3(0.f)),
        normalize(vIndexes[7] - vec3(0.f))
    };

    // Generate 24 vertices for all corners per face of the cube.
    // Face 1 - Front
    m_pVertices.push_back(vIndexes[0]);        // Index 0
    m_pVertices.push_back(vIndexes[1]);        // Index 1
    m_pVertices.push_back(vIndexes[2]);        // Index 2
    m_pVertices.push_back(vIndexes[3]);        // Index 3
    m_pNormals.push_back(vNormals[0]);
    m_pNormals.push_back(vNormals[1]);
    m_pNormals.push_back(vNormals[2]);
    m_pNormals.push_back(vNormals[3]);
    m_pUVs.push_back(vec2(0.f));
    m_pUVs.push_back(vec2(0.f, 1.0f));
    m_pUVs.push_back(vec2(1.f));
    m_pUVs.push_back(vec2(1.f, 0.0f));

    // Face 2 - Back
    m_pVertices.push_back(vIndexes[4]);        // Index 4
    m_pVertices.push_back(vIndexes[5]);        // Index 5
    m_pVertices.push_back(vIndexes[6]);        // Index 6
    m_pVertices.push_back(vIndexes[7]);        // Index 7
    m_pNormals.push_back(vNormals[4]);
    m_pNormals.push_back(vNormals[5]);
    m_pNormals.push_back(vNormals[6]);
    m_pNormals.push_back(vNormals[7]);
    m_pUVs.push_back(vec2(1.0f, 0.f));
    m_pUVs.push_back(vec2(0.0f));
    m_pUVs.push_back(vec2(1.f));
    m_pUVs.push_back(vec2(0.0f, 1.0f));

    // Face 3 - Left
    m_pVertices.push_back(vIndexes[0]);        // Index 1
    m_pVertices.push_back(vIndexes[3]);        // Index 2
    m_pVertices.push_back(vIndexes[4]);        // Index 5
    m_pVertices.push_back(vIndexes[6]);        // Index 7
    m_pNormals.push_back(vNormals[0]);
    m_pNormals.push_back(vNormals[3]);
    m_pNormals.push_back(vNormals[4]);
    m_pNormals.push_back(vNormals[6]);
    m_pUVs.push_back(vec2(0.0f));
    m_pUVs.push_back(vec2(0.0f, 1.0f));
    m_pUVs.push_back(vec2(1.0f, 0.f));
    m_pUVs.push_back(vec2(1.f));

    // Face 4 - Right
    m_pVertices.push_back(vIndexes[1]);        // Index 0
    m_pVertices.push_back(vIndexes[2]);        // Index 3
    m_pVertices.push_back(vIndexes[5]);        // Index 4
    m_pVertices.push_back(vIndexes[7]);        // Index 6
    m_pNormals.push_back(vNormals[1]);
    m_pNormals.push_back(vNormals[2]);
    m_pNormals.push_back(vNormals[5]);
    m_pNormals.push_back(vNormals[7]);
    m_pUVs.push_back(vec2(1.0f, 0.f));
    m_pUVs.push_back(vec2(1.f));
    m_pUVs.push_back(vec2(0.0f));
    m_pUVs.push_back(vec2(0.0f, 1.0f));

    // Face 5 - Bottom
    m_pVertices.push_back(vIndexes[2]);        // Index 2
    m_pVertices.push_back(vIndexes[3]);        // Index 3
    m_pVertices.push_back(vIndexes[6]);        // Index 6
    m_pVertices.push_back(vIndexes[7]);        // Index 7
    m_pNormals.push_back(vNormals[2]);
    m_pNormals.push_back(vNormals[3]);
    m_pNormals.push_back(vNormals[6]);
    m_pNormals.push_back(vNormals[7]);
    m_pUVs.push_back(vec2(0.0f));
    m_pUVs.push_back(vec2(1.0f, 0.f));
    m_pUVs.push_back(vec2(1.f));
    m_pUVs.push_back(vec2(0.0f, 1.0f));

    // Face 6 - Top
    m_pVertices.push_back(vIndexes[0]);        // Index 0
    m_pVertices.push_back(vIndexes[1]);        // Index 1
    m_pVertices.push_back(vIndexes[4]);        // Index 4
    m_pVertices.push_back(vIndexes[5]);        // Index 5
    m_pNormals.push_back(vNormals[0]);
    m_pNormals.push_back(vNormals[1]);
    m_pNormals.push_back(vNormals[4]);
    m_pNormals.push_back(vNormals[5]);
    m_pUVs.push_back(vec2(0.0f));
    m_pUVs.push_back(vec2(1.0f, 0.f));
    m_pUVs.push_back(vec2(0.0f, 1.0f));
    m_pUVs.push_back(vec2(1.f));

    m_pIndices = {
        0, 2, 1, 0, 3, 2,
        5, 6, 4, 5, 7, 6,
        10, 9, 8, 10, 11, 9,
        12, 15, 14, 12, 13, 15,
        17, 19, 16, 17, 18, 19,
        22, 21, 23, 22, 20, 21
    };

    // Compute Spatial Cube
    m_vNegativeOffset = vec3(-iHalfWidth, -iHalfHeight, -iHalfDepth);
    m_vPositiveOffset = vec3(iHalfWidth, iHalfHeight, iHalfDepth);

    // Generate Cubic Bounding Box using Spatial Cube settings if requested.
    if (SPATIAL_CALC == m_sBoundingBox.eType)   
        m_sBoundingBox.generateCubicBox(fHeight, fWidth, fDepth);

    // Store initial Transformation if Static Mesh
    if (m_bStaticMesh)
    {
        // Initial Transformation Matrix
        mat4 m4InitialTransformationMatrix = mat4(1.0f);

        // Translation
        if (vec3(0.0f) != vPosition)
            m4InitialTransformationMatrix = translate(vPosition);

        // Store Initial Transformation Matrix in Transformation vector
        // m_m4ListOfInstances.push_back(m4InitialTransformationMatrix);
        m_m4InstanceMap.insert(make_pair(sHashKey, m4InitialTransformationMatrix));
    }

    initalizeVBOs();
}

// This will store the position as a vertex with a given normal as the direction to draw the billboard in.
//    the height and width will be set up and stored in the VBO as well.
//    The billboard functionality of a Mesh will set up the VBOs in a very different manner:
//        - For each data entry:
//            * Vertex   (vec3)
//            * Normal   (vec3)
//            * UVStart  (vec2)
//            * UVEnd    (vec2)
//            * Height   (float)
//            * Width    (float)
//            * Duration (float)
void Mesh::genBillboard()
{
    // Generate VBO
    m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, nullptr, 0, GL_STATIC_DRAW);

    // Set up VBO Attributes
    m_pShdrMngr->setAttrib(m_iVertexArray, 0, 3, BILLBOARD_STRIDE, (void*)VERTEX_OFFSET);    /*Vertex*/
    m_pShdrMngr->setAttrib(m_iVertexArray, 1, 3, BILLBOARD_STRIDE, (void*)NORMAL_OFFSET);    /*Normal*/
    m_pShdrMngr->setAttrib(m_iVertexArray, 2, 2, BILLBOARD_STRIDE, (void*)UV_START_OFFSET);    /*UVStart*/
    m_pShdrMngr->setAttrib(m_iVertexArray, 3, 2, BILLBOARD_STRIDE, (void*)UV_END_OFFSET);    /*UVEnd*/
    m_pShdrMngr->setAttrib(m_iVertexArray, 4, 2, BILLBOARD_STRIDE, (void*)DIMENSION_OFFSET);/*Height/Width*/
    m_pShdrMngr->setAttrib(m_iVertexArray, 5, 1, BILLBOARD_STRIDE, (void*)DURATION_OFFSET); /*Duration*/
}

/****************************************************************************************\
 * Billboard Usage                                                                        *
\****************************************************************************************/

// Adds a Billboard object to the Mesh.
unsigned int Mesh::addBillboard(const vec3* vPosition, const vec3* vNormal, const vec2* vUVStart, const vec2* vUVEnd, float fHeight, float fWidth, float fDuration)
{
    // Create new Billboard
    sBillboardInfo sNewBillboard;
    sNewBillboard.vPosition = *vPosition;
    sNewBillboard.vNormal = *vNormal;
    sNewBillboard.vUVStart = *vUVStart;
    sNewBillboard.vUVEnd = *vUVEnd;
    sNewBillboard.vDimensions = vec2(fHeight, fWidth);
    sNewBillboard.fDuration = fDuration;

    // add to main list
    m_pBillboardList.push_back(sNewBillboard);

    // Reload Data in GPU.
    updateBillboardVBO();

    // Return the index for this billboard for later reference.
    return m_pBillboardList.size() - 1;
}

void Mesh::updateBillboardVBO()
{
    // Update in VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_pBillboardList.size() * sizeof(sBillboardInfo), m_pBillboardList.data(), GL_DYNAMIC_DRAW);
}

// Clear VBO data and Clear the Billboard data internally.
void Mesh::flushBillboards()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, (void*)0, GL_DYNAMIC_DRAW);

    m_pBillboardList.clear();
}

void Mesh::gatherVNData(vector<float>* vVNData, bool* bUsingNormals, bool* bUsingUVs)
{
    // This function shouldn't be called without passing in Vertex Data.
    assert(!m_pVertices.empty());

    // We can use just one VBO since it's assumed that the geometry won't update as often.
    //    Therefore, we'll combine all data into one VBO and set openGL to address the data accordingly.
    // Create a Vector to hold the combined data.
    vVNData->reserve(m_pVertices.size() * 3 +
        m_pNormals.size() * 3 +
        (m_pUVs.size() << 1));

    // Boolean values to determine if additional data exists.
    *bUsingNormals  = !m_pNormals.empty();
    *bUsingUVs      = !m_pUVs.empty();

    // Loop through the received input and set up the array of data for the VBO
    for (unsigned int i = 0; i < m_pVertices.size(); ++i)
    {
        // Vertex Data
        vVNData->push_back(m_pVertices[i].x);
        vVNData->push_back(m_pVertices[i].y);
        vVNData->push_back(m_pVertices[i].z);

        // Normal Data
        if (*bUsingNormals)
        {
            vVNData->push_back(m_pNormals[i].x);
            vVNData->push_back(m_pNormals[i].y);
            vVNData->push_back(m_pNormals[i].z);
        }

        // UV Data
        if (*bUsingUVs)
        {
            vVNData->push_back(m_pUVs[i].x);
            vVNData->push_back(m_pUVs[i].y);
        }
    }
}

// Initialize the GPU with Mesh Data and tell it how to read it.
void Mesh::initalizeVBOs(const vector<float>* vVNData, bool bUsingNormals, bool bUsingUVs)
{
    // Calculate Stride for setting up Attributes
    GLsizei iStride = sizeof(vec3) +
        (bUsingNormals ? sizeof(vec3) : 0) +
        (bUsingUVs ? sizeof(vec2) : 0);
    
    // Generate VBO
    m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, vVNData->data(), vVNData->size() * sizeof(float), GL_STATIC_DRAW);

    // Set-up Attributes
    // Vertices
    m_pShdrMngr->setAttrib(m_iVertexArray, 0, 3, iStride, (void*)0);
    // Normals
    if (bUsingNormals)
    {
        m_pShdrMngr->setAttrib(m_iVertexArray, 1, 3, iStride, (void*)sizeof(vec3));
    }
    // UVs
    if (bUsingUVs) // Specified index could be 1 or 2 and Start location is Stride - sizeof(vec2) depending on if Normals exist.
    {
        m_pShdrMngr->setAttrib(m_iVertexArray, 2, 2, iStride, (void*)(iStride - sizeof(vec2)));
    }

    // Initialize Instance Buffer
    m_iInstancedBuffer = SHADER_MANAGER->genInstanceBuffer(m_iVertexArray, 3, NULL,
                                                        m_m4InstanceMap.size() * sizeof(mat4), GL_DYNAMIC_DRAW);
    loadInstanceBuffer();

    // Set up Indices if applicable
    if (!m_pIndices.empty())
    {
        // Set up Index Buffer
        m_iIndicesBuffer = SHADER_MANAGER->genIndicesBuffer(
            m_iVertexArray,
            m_pIndices.data(),
            m_pIndices.size() * sizeof(unsigned int),
            GL_STATIC_DRAW);
    }
}

// Gathers and Initializes Generated VBO data
void Mesh::initalizeVBOs()
{
    // Gather Data and Initialize VBOs
    vector<float> vVNData;
    bool bUsingNormals, bUsingUVs;

    gatherVNData(&vVNData, &bUsingNormals, &bUsingUVs);
    initalizeVBOs(&vVNData, bUsingNormals, bUsingUVs);
}

// Code for Loading a .obj file. Not comprehensive, will generate its own normals and will fail to load any
//        faces that are not quads or tris. Algorithm modified from original written by Andrew Robert Owens.
bool Mesh::loadObj(const string& sFileName)
{
    // Locals
    ifstream in(sFileName.c_str());
    bool bReturnValue = in.is_open();
    vec2 vXRange, vYRange, vZRange;     // Compute Basic cubic convex hull for Spatial information.
    vector <float> vVNData;
    unordered_map<string, unsigned int> pIndexMap;
    unsigned int iIndexTick = 0;
    vZRange = vYRange = vXRange = vec2(numeric_limits<float>::max(), numeric_limits<float>::min());

    // Failed to load file.
    if (!bReturnValue)
        cerr << "Could not Load Mesh File: " << sFileName << ".\n";
    else
    {
        // Local for Extraction
        string sInput;
        vec3 vTempVec;

        // Read through file.
        while (getline(in, sInput))
        {
            stringstream ssLine(sInput);
            string sToken;

            // Read first token from line
            ssLine >> sToken;

            if ("v" == sToken)    // Vertex Data
            {
                ssLine >> vTempVec.x >> vTempVec.y >> vTempVec.z;
                m_pVertices.push_back(vTempVec);

                // Evaluate cubic convex hull
                vXRange.x = vTempVec.x < vXRange.x ? vTempVec.x : vXRange.x;    // X Range Min
                vXRange.y = vTempVec.x > vXRange.y ? vTempVec.x : vXRange.y;    // X Range Max

                vYRange.x = vTempVec.y < vYRange.x ? vTempVec.y : vYRange.x;    // Y Range Min
                vYRange.y = vTempVec.y > vYRange.y ? vTempVec.y : vYRange.y;    // Y Range Max

                vZRange.x = vTempVec.z < vZRange.x ? vTempVec.z : vZRange.x;    // Z Range Min
                vZRange.y = vTempVec.z > vZRange.y ? vTempVec.z : vZRange.y;    // Z Range Max
            }
            else if ("vt" == sToken) // uv-coords
            {
                ssLine >> vTempVec.x >> vTempVec.y >> vTempVec.z;
                m_pUVs.push_back(vec2(vTempVec));
            }
            else if ("vn" == sToken) // Normal
            {
                ssLine >> vTempVec.x >> vTempVec.y >> vTempVec.z;
                m_pNormals.push_back(vTempVec);
            }
            else if ("g" == sToken) // Group
            {/* Ignored */
            }
            else if ("o" == sToken) // Object Name
            {/* Ignored */
            }
            else if ("f" == sToken) // Face
            {
                // Local Variables for reading face.
                int iIndices[3] = { -1, -1, -1 };
                vector< unsigned int > vFaceVerts;
                string pTupleKey;

                // Read through rest of line
                while (ssLine >> sInput)
                {
                    // Set up parser and reset previous indices
                    stringstream ssItem(sInput);
                    iIndices[0] = iIndices[1] = iIndices[2] = -1;

                    // Read in face line
                    for (int j = 0; getline(ssItem, sInput, '/') && j < 3; ++j)
                    {
                        stringstream ssIndex(sInput);
                        ssIndex >> iIndices[j];
                    }

                    // Convert to 0-based indices
                    iIndices[0] = (-1 == iIndices[0] ? iIndices[0] : iIndices[0] - 1); // Vertex
                    iIndices[1] = (-1 == iIndices[1] ? iIndices[1] : iIndices[1] - 1); // UV-Coord
                    iIndices[2] = (-1 == iIndices[2] ? iIndices[2] : iIndices[2] - 1); // Normal
                    pTupleKey = to_string(iIndices[0]) + to_string(iIndices[1]) + to_string(iIndices[2]);

                    // If the Index for this tuple doesn't exist yet, add it to the map.
                    if (pIndexMap.find(pTupleKey) == pIndexMap.end())
                    {
                        // Add Vertex Data
                        vVNData.push_back(m_pVertices[iIndices[0]].x);
                        vVNData.push_back(m_pVertices[iIndices[0]].y);
                        vVNData.push_back(m_pVertices[iIndices[0]].z);

                        // Add Normal Data
                        if (!m_pNormals.empty())
                        {
                            vVNData.push_back(m_pNormals[iIndices[2]].x);
                            vVNData.push_back(m_pNormals[iIndices[2]].y);
                            vVNData.push_back(m_pNormals[iIndices[2]].z);
                        }

                        // Add UV Data
                        if (!m_pUVs.empty())
                        {
                            vVNData.push_back(m_pUVs[iIndices[1]].x);
                            vVNData.push_back(1.0f - m_pUVs[iIndices[1]].y);
                        }

                        // Store Index in Index Map
                        pIndexMap.insert(make_pair(pTupleKey, iIndexTick++));
                    }

                    // Collect the Face Indices for the Data
                    vFaceVerts.push_back(pIndexMap[pTupleKey]);
                }

                // Store Indices; Only handles 3-4 vert faces.
                if (3 <= vFaceVerts.size())
                {
                    // Triangle
                    for (int i = 0; i < 3; ++i)
                        m_pIndices.push_back(vFaceVerts[i]);
                
                    // Quad
                    if (4 == vFaceVerts.size())
                    {
                        // Store other Triangle
                        int j = 2;
                        for (int i = 0; i < 3; ++i)
                        {
                            m_pIndices.push_back(vFaceVerts[j]);
                            j = (j + 1) % 4;
                        }
                    }
                    else if (3 != vFaceVerts.size()) // Error
                    {
                        cerr << "Invalid Object in File: " << sFileName << ".\n";
                        bReturnValue = false;
                    }
                } // END Indices Store
            }// END Face
        } // END While

        // Close file when finished.
        in.close();

        // Store computed Spatial Range
        m_vNegativeOffset = m_m4ScaleMatrix * vec4(vXRange.x, vYRange.x, vZRange.x, 1.0f);  // Min
        m_vPositiveOffset = m_m4ScaleMatrix * vec4(vXRange.y, vYRange.y, vZRange.y, 1.0f);  // Max

        // Set up special VBOs for Mesh
        initalizeVBOs(&vVNData, !m_pNormals.empty(), !m_pUVs.empty());
    }

    return bReturnValue;
}

void Mesh::addInstance(const vec3* vPosition, const vec3* vNormal, float fScale, string sHashKey)
{
    // Order as Scale -> Rotation -> Translation
    mat4 m4NewTransform = scale(vec3(fScale));
    m4NewTransform = getRotationMat4ToNormal(vNormal) * m4NewTransform;
    m4NewTransform = translate(*vPosition) * m4NewTransform;

    // Utilize Overloaded Function for functionality.
    addInstance(&m4NewTransform, sHashKey);
}

// Take in a new Transformation Matrix, update the internal transform list as well as the VBO
void Mesh::addInstance(const mat4* m4Transform, string sHashKey)
{
    // Local Variables
    mat4 m4ScaledTransform = *m4Transform * m_m4ScaleMatrix;

    // Add new Transformation
    m_m4InstanceMap.insert(make_pair(sHashKey, m4ScaledTransform));   // Add the New instance to the Instance Buffer

    // Load VBO with new Data.
    loadInstanceBuffer();

    // Add the Same instance for the Bounding Box
    addBBInstance(m4Transform, sHashKey);
}

// Update the Instance with the specified HashKey.
void Mesh::updateInstance(const mat4* m4Transform, string sHashKey)
{
    // Ensure the specified index is a valid index.
    if (m_m4InstanceMap.find(sHashKey) != m_m4InstanceMap.end())
    {
        mat4 m4ScaledTransform = *m4Transform * m_m4ScaleMatrix;    // Scale transformation with Scale MAtrix for the model.

        // Update the local List of Instances with the updated Transformation.
        m_m4InstanceMap[sHashKey] = m4ScaledTransform;
        loadInstanceBuffer();

        // Update the Bounding Box Transformation if the Bounding Box is loaded.
        if (m_sBoundingBox.isLoaded())
            m_sBoundingBox.updateInstance(m4Transform, sHashKey);
    }
}

// Removes an Instance from the Transformation list at the specified index.
void Mesh::removeInstance(string sHashKey)
{
    if (m_m4InstanceMap.find(sHashKey) != m_m4InstanceMap.end())
    {
        // Remove Insatnce
        m_m4InstanceMap.erase(sHashKey);

        // Update the VBO with the new transformation
        loadInstanceBuffer();
        
        // Apply same to Bounding Box
        if (m_sBoundingBox.isLoaded())
            m_sBoundingBox.removeInstance(sHashKey);
    }
}

// Pulls Instances from the instance map and stores the data in the Instance Buffer of the GPU.
void Mesh::loadInstanceBuffer()
{
    // Local Variables
    vector<mat4> pInstanceList;
    pInstanceList.reserve(m_m4InstanceMap.size());  // Reserve size to avoid unnecessary resizing.

    // Populate Vector
    for (unordered_map<string, mat4>::iterator pIter = m_m4InstanceMap.begin();
        pIter != m_m4InstanceMap.end();
        ++pIter)
        pInstanceList.push_back(pIter->second);

    // Store data in GPU
    glBindBuffer(GL_ARRAY_BUFFER, m_iInstancedBuffer);
    glBufferData(GL_ARRAY_BUFFER, pInstanceList.size() * sizeof(mat4), pInstanceList.data(), GL_DYNAMIC_DRAW);
}

// Returns a Rotation Matrix to rotate an object from a World Coordinate System to a Local
//    coordinate system with a given y-axis normal.
mat4 Mesh::getRotationMat4ToNormal(const vec3* vNormal)
{
    // Initial Translation Matrix
    mat4 m4ReturnMatrix = mat4(1.0f);
    vec3 vYAxis(0.0f, 1.0f, 0.0f);

    float d = dot(*vNormal, vYAxis);
    if (d < 1.f) // If d >= 1.f, Vectors are the same.
    {
        // create Rotation quaternion.
        vec3 vCross = cross(*vNormal, vYAxis);
        quat q = angleAxis(acos(d), vCross);
        normalize(q);

        // Create Rotation Matrix from Quaternion
        m4ReturnMatrix = toMat4(q) * m4ReturnMatrix;
    }

    // Return Rotation Matrix
    return m4ReturnMatrix;
}

/************************************************************************************\
 * Object Properties Information                                                    *
\************************************************************************************/

// Function to Load Mesh Properties from a given ObjectInfo structure
void Mesh::loadObjectInfo(const ObjectInfo* pObjectProperties, string sHashkey)
{
    // Ensure the Object Properties pointer is valid
    if (nullptr != pObjectProperties)
    {
        loadMaterial(&pObjectProperties->sObjMaterial);                                         // Load Mesh Material
        loadBoundingBox(&pObjectProperties->sObjBoundingBox, &pObjectProperties->vPosition, sHashkey);    // Load Bounding Box
    }
}

// Load the Material for the Mesh
void Mesh::loadMaterial(const ObjectInfo::Material* pMaterial)
{
    if (nullptr != pMaterial)
    {
        // Load Diffuse Texture if applicable
        if ("" != pMaterial->sDiffuseMap)
            m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->loadTexture(pMaterial->sDiffuseMap);
        else if (vec4(0.0f) != pMaterial->vOptionalDiffuseColor)    // No diffuse Texture applicable? then check if there's a specified diffuse color for the material.
            m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->genTexture(&pMaterial->vOptionalDiffuseColor);

        // Load Texture if applicable
        if ("" != pMaterial->sOptionalSpecMap)
            m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->loadTexture(pMaterial->sOptionalSpecMap);
        else    // "" as Spec Map Location? just generate a texture from whatever the Spec Shade is.
            m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->genTexture(&pMaterial->vOptionalSpecShade);

        // Store Shininess
        m_sRenderMaterial.fShininess = pMaterial->fShininess;
    }

    // Set some defaults if no Maps were specified.
    if (nullptr == m_sRenderMaterial.m_pDiffuseMap)
        m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->loadTexture(DEFAULT_DIFFUSE_MAP);
    if (nullptr == m_sRenderMaterial.m_pSpecularMap)
        m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->genTexture(&DEFAULT_SPEC_COLOR);
}

// Load the Bounding Box for the Mesh
void Mesh::loadBoundingBox(const ObjectInfo::BoundingBox* pBoundingBox, const vec3* vStartingPosition, string sHashKey)
{
    if (nullptr != pBoundingBox && nullptr != vStartingPosition)
    {
        // Generate Translation Matrix for starting position
        mat4 m4Translation = translate(*vStartingPosition);
        m_sBoundingBox.eType = pBoundingBox->eType;

        // Nothing Set in the Bounding Box type? Don't evaluate further
        switch (pBoundingBox->eType)
        {
        case CUBIC_BOX:
            generateCubicBoundingBox(pBoundingBox->vDimensions.x, pBoundingBox->vDimensions.y, pBoundingBox->vDimensions.z);
            break;
        default:    // No Bounding Box specified
            return;
            break;
        }

        // Add initial translation for the Bounding Box.
        if (DEFAULT_TYPE != pBoundingBox->eType)
            addBBInstance(&m4Translation, sHashKey);
    }
}

// Returns calculated spatial dimensions for the mesh.
//  If the return values are vec3(0.0f), then no spatial information has been computed for
//      the bounding box nor the mesh.
void Mesh::getSpatialDimensions(vec3* pNegativeOffset, vec3* pPositiveOffset)
{
    // Ensure that proper pointers have been given.
    assert(nullptr != pNegativeOffset && nullptr != pPositiveOffset);

    if (m_sBoundingBox.isLoaded())  // Default to the Bounding Box Dimensions if available
    {
        *pNegativeOffset = m_sBoundingBox.m_vNegativeOffset;
        *pPositiveOffset = m_sBoundingBox.m_vPositiveOffset;
    }
    else    // Otherwise return the Mesh computed Offsets.
    {
        *pNegativeOffset = m_vNegativeOffset;
        *pPositiveOffset = m_vPositiveOffset;
    }
}

/************************************************************************************\
 * Texture Functionality                                                            *
\************************************************************************************/

// Function to Bind the Mesh Material to the Shader for Rendering
//    To be called before the render function
void Mesh::bindTextures(ShaderManager::eShaderType eShaderType) const
{
    // Bind the Diffuse and Specular Maps
    m_sRenderMaterial.m_pDiffuseMap->bindTexture(eShaderType, "sMaterial.vDiffuse");
    m_sRenderMaterial.m_pSpecularMap->bindTexture(eShaderType, "sMaterial.vSpecular");

    // Set the Material's Shininess in the Material Uniform in the shader.
    m_pShdrMngr->setUniformFloat(eShaderType, "sMaterial.fShininess", m_sRenderMaterial.fShininess);
}

// Funtion to unbind textures. To be called after a render call on this mesh.
void Mesh::unbindTextures() const
{
    m_sRenderMaterial.m_pDiffuseMap->unbindTexture();
    m_sRenderMaterial.m_pSpecularMap->unbindTexture();
}

/************************************************************************************\
 * Bounding Box Functionality                                                       *
\************************************************************************************/

// Deletes The VAO and VBOs used by the Mesh's Bounding Box.
void Mesh::sBoundingBox::deleteBuffers()
{
    glDeleteBuffers(1, &iIndicesBuffer);
    glDeleteBuffers(1, &iVertexBuffer);
    glDeleteBuffers(1, &iInstancedBuffer);
    glDeleteVertexArrays(1, &iVertexArray);
}

// Loads a new Transformation Instance into the Instance Map and stores it in the GPU
void Mesh::sBoundingBox::loadInstance(const mat4* pTransform, string sHashKey)
{
    // Ensure a valid transformation is passed in and that the InstancedBuffer is generated
    assert(nullptr != pTransform && 0 != iInstancedBuffer);

    // Add the Transformation to the Instance Vector and load the Instance Vector into the Instance VBO
    pInstanceMap.insert(make_pair(sHashKey, *pTransform));
    loadInstanceBuffer();
}

// Loads a new transformation Instance into the Instance buffer
void Mesh::sBoundingBox::updateInstance(const mat4* pTransform, string sHashKey)
{
    // Ensure a valid transformation is passed in and that the InstancedBuffer is generated
    assert(nullptr != pTransform && 0 != iInstancedBuffer && pInstanceMap.find(sHashKey) != pInstanceMap.end());

    // Add the Transformation to the Instance Vector and load the Instance Vector into the Instance VBO
    pInstanceMap[sHashKey] = *pTransform;
    loadInstanceBuffer();
}

// Remove Bounding Box instance 
void Mesh::sBoundingBox::removeInstance(string sHashKey)
{
    if (pInstanceMap.find(sHashKey) != pInstanceMap.end())
    {
        // Remove from Instance List
        pInstanceMap.erase(sHashKey);

        // Update GPU data.
        loadInstanceBuffer();
    }
}

// Initializes VBOs for the Bounding Box.
void Mesh::sBoundingBox::initVBOs()
{
    // Ensure that the Bounding Box was initialized with necessary data
    assert(!pVertices.empty());

    // Delete Current Buffers if they have already been initialized
    if (isLoaded())
        deleteBuffers();

    // Generate new vertex array for this Bounding Box
    glGenVertexArrays(1, &iVertexArray);

    // Generate Vertex Buffer
    iVertexBuffer = SHADER_MANAGER->genVertexBuffer(iVertexArray, pVertices.data(), pVertices.size() * sizeof(vec3), GL_STATIC_DRAW);
    SHADER_MANAGER->setAttrib(iVertexArray, 0, 3, sizeof(vec3), (void*)0);

    // Generate Indices Buffer
    iIndicesBuffer = SHADER_MANAGER->genIndicesBuffer(iVertexArray, pIndices.data(), pIndices.size() * sizeof(unsigned int), GL_STATIC_DRAW);

    // Generate Instance Buffer
    iInstancedBuffer = SHADER_MANAGER->genInstanceBuffer(iVertexArray, 1, (void*)0, 0, GL_DYNAMIC_DRAW);
}

// Generates a Cubic Bounding Box.
void Mesh::sBoundingBox::generateCubicBox(float fHeight, float fWidth, float fDepth)
{
    // Get half sizes of dimensions to set vertices wrt to origin.
    float iHalfHeight = fHeight * 0.5f;
    float iHalfWidth = fWidth * 0.5f;
    float iHalfDepth = fDepth * 0.5f;

    // Store all 8 Vertices for the Cubic Box
    pVertices = {
        vec3(-iHalfWidth, iHalfHeight, iHalfDepth),
        vec3(iHalfWidth, iHalfHeight, iHalfDepth),
        vec3(iHalfWidth, -iHalfHeight, iHalfDepth),
        vec3(-iHalfWidth, -iHalfHeight, iHalfDepth),
        vec3(-iHalfWidth, iHalfHeight, -iHalfDepth),
        vec3(iHalfWidth, iHalfHeight, -iHalfDepth),
        vec3(-iHalfWidth, -iHalfHeight, -iHalfDepth),
        vec3(iHalfWidth, -iHalfHeight, -iHalfDepth)
    };

    // Set up the Indices for Drawing lines
    pIndices = {
        0, 1, 0, 3, 0, 4, // Top Left Front Corner
        1, 2, 1, 5, 2, 3,
        2, 7, 3, 6, 4, 5,
        6, 7, 4, 6, 5, 7
    };

    // Store the Spatial information
    m_vNegativeOffset = vec3(-iHalfWidth, -iHalfHeight, -iHalfDepth);
    m_vPositiveOffset = vec3(iHalfWidth, iHalfHeight, iHalfDepth);

    // Initialize Bounding Box VBOs
    initVBOs();
}

void Mesh::sBoundingBox::generateCubicBox(const vec3* vNegativeOffset, const vec3* vPositiveOffset)
{
    // Store all 8 Vertices for the Cubic Box
    pVertices = {
        vec3(vNegativeOffset->x, vPositiveOffset->y, vPositiveOffset->z),
        vec3(vPositiveOffset->x, vPositiveOffset->y, vPositiveOffset->z),
        vec3(vPositiveOffset->x, vNegativeOffset->y, vPositiveOffset->z),
        vec3(vNegativeOffset->x, vNegativeOffset->y, vPositiveOffset->z),
        vec3(vNegativeOffset->x, vPositiveOffset->y, vNegativeOffset->z),
        vec3(vPositiveOffset->x, vPositiveOffset->y, vNegativeOffset->z),
        vec3(vNegativeOffset->x, vNegativeOffset->y, vNegativeOffset->z),
        vec3(vPositiveOffset->x, vNegativeOffset->y, vNegativeOffset->z)
    };

    // Set up the Indices for Drawing lines
    pIndices = {
        0, 1, 0, 3, 0, 4, // Top Left Front Corner
        1, 2, 1, 5, 2, 3,
        2, 7, 3, 6, 4, 5,
        6, 7, 4, 6, 5, 7
    };

    // Store the Spatial information
    this->m_vNegativeOffset = *vNegativeOffset;
    this->m_vPositiveOffset = *vPositiveOffset;

    // Initialize Bounding Box VBOs
    initVBOs();
}

// Set the Bounding Box Instance Matrix
//  If the Mesh is static, add multiple instances
//  dynamic? replace the current instance
void Mesh::addBBInstance(const mat4* m4Transformation, string sHashKey)
{
    if (m_sBoundingBox.isLoaded())
    {
        // Load the new instance
        m_sBoundingBox.loadInstance(m4Transformation, sHashKey);
    }
}

// Loads the Instance Array for the Bounding Box from the Bounding Box Instance Map.
void Mesh::sBoundingBox::loadInstanceBuffer()
{
    // Local Variables
    vector<mat4> pInstanceList;
    pInstanceList.reserve(pInstanceMap.size());  // Reserve size to avoid unnecessary resizing.

    // Populate Vector
    for (unordered_map<string, mat4>::iterator pIter = pInstanceMap.begin();
        pIter != pInstanceMap.end();
        ++pIter)
        pInstanceList.push_back(pIter->second);

    // Store data in GPU
    glBindBuffer(GL_ARRAY_BUFFER, iInstancedBuffer);
    glBufferData(GL_ARRAY_BUFFER, pInstanceList.size() * sizeof(mat4), pInstanceList.data(), GL_DYNAMIC_DRAW);
}
