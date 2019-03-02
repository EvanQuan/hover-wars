#pragma once
#include "stdafx.h"
#include "ShaderManager.h"
#include "Texture.h"
#include "DataStructures/ObjectInfo.h"

//////////////////////////////////////////////////////////////////
// Name: Mesh.h
// Class: Container for TriMesh Objects as well as buffers for normals
//        and indices.
// Written by: James Cote
//////////////////////////////////
class Mesh
{
private:
    // Private Constructor and Copy Constructor to restrict usage to Object_Factory
    Mesh(const Mesh& pCopy);
    Mesh& operator= (const Mesh& pRHS);

    // Private Methods
    bool genMesh(const string& sFileName, vec3 vPosition, float fScale = 1.0f);
    void genPlane(int iHeight, int iWidth, vec3 vPosition, vec3 vNormal);
    void genSphere(float fRadius, vec3 vPosition);
    void genCube(float fHeight, float fWidth, float fDepth, vec3 vPosition);
    void genBillboard();
    void initalizeVBOs();
    bool loadObj(const string& sFileName);
    void loadObjectInfo(const ObjectInfo* pObjectProperties);
    void loadMaterial(const ObjectInfo::Material* pMaterial);
    void loadBoundingBox(const ObjectInfo::BoundingBox* pBoundingBox, const vec3* vStartingPosition);

    // function to generate a quaternion to rotate from y-axis normal to specified normal
    mat4 getRotationMat4ToNormal(const vec3* vNormal);

    // Material Struct for setting uniform in Lighting Shaders
    struct sRenderMaterial
    {
        Texture* m_pDiffuseMap;
        Texture* m_pSpecularMap;
        float fShininess;
    } m_sRenderMaterial;

    // The Bounding Box Drawing information
    struct sBoundingBox
    {
        // BoundingBox Variables
        vector<mat4> pInstances;
        vector<vec3> pVertices;
        vector< unsigned int> pIndices;
        GLuint iVertexBuffer, iInstancedBuffer, iVertexArray, iIndicesBuffer;
        vec3 vNegativeOffset, vPositiveOffset; // Specifies the dimensions of the Spacial cube for the Bounding Box.

        // Check to see if the Bounding Box is loaded.
        bool isLoaded() const { return 0 != iVertexArray; }

        // Initialization and cleaning of Buffers
        void deleteBuffers();   // Deletes the VAO and VBOs used by the Mesh's Bounding Box
        void initVBOs();        // Initializes VBOs for the Bounding Box.

        // Loads a new transformation Instance into the Instance buffer
        void loadInstance(const mat4* pTransform);
        void updateInstance(const mat4* pTransform, unsigned int iIndex);

        // Generation Functions
        void generateCubicBox(float fHeight, float fWidth, float fDepth);
    } m_sBoundingBox;

    // Mesh Information and GPU VAO/VBOs
    vector<unsigned int> m_pIndices;
    vector< vec3 > m_pVertices, m_pNormals;
    vector< vec2 > m_pUVs;
    GLuint m_iVertexBuffer, m_iInstancedBuffer, m_iIndicesBuffer;
    GLuint m_iVertexArray;

    // Spatial Information for Mesh
    vec3 m_vNegativeOffset, m_vPositiveOffset;

    string m_sManagerKey; // Used as key for finding Mesh in MeshManager
    ShaderManager* m_pShdrMngr;
    vector<mat4> m_m4ListOfInstances;
    bool m_bStaticMesh;
    mat4 m_m4ScaleMatrix;

    // Billboard Information
    struct sBillboardInfo
    {
        vec3 vPosition, vNormal;
        vec2 vUVStart, vUVEnd, vDimensions;
        float fDuration;
    };
    vector<sBillboardInfo> m_pBillboardList;

    // Billboard Functionality -> Only accessable within AnimationComponent
    void updateBillboardVBO();
    unsigned int addBillboard(const vec3* vPosition, const vec3* vNormal, const vec2* vUVStart, const vec2* vUVEnd, float fHeight, float fWidth, float fDuration);
    void flushBillboards();

    // Friend Class: MeshManager to create Meshes.
    friend class MeshManager;
    friend class AnimationComponent;    // Friend class: Animation Component to manipulate Mesh information
    // Private Manager Cookie so only MeshManager can construct a Mesh,
    //    but make_unique<Mesh> still has access to the constructor which it needs.
    struct manager_cookie {};

public:
    explicit Mesh(const string &sFileName, bool bStaticMesh, const ObjectInfo* pObjectProperties, manager_cookie);
    virtual ~Mesh();
    void loadInstanceData(const void* pData, unsigned int iSize);

    // Get Information for drawing elements.
    GLuint getNumInstances() const { return m_m4ListOfInstances.size(); }
    GLuint getCount() const {
        if (!m_pIndices.empty())
            return m_pIndices.size();
        else if (!m_pBillboardList.empty())
            return m_pBillboardList.size();
        else
            return m_pVertices.size();
    }

    // Checks for Render Component
    bool usingIndices() const { return !m_pIndices.empty(); }
    bool usingInstanced() const { return 0 != m_iInstancedBuffer; }

    // Function to add a new Instance Matrix for the Mesh. If the Mesh is dynamic, it will replace the current instance, static will add a new instance.
    unsigned int addInstance(const vec3* vPosition, const vec3* vNormal, float fScale);    // Specify particular components and a transformation matrix will be generated
    unsigned int addInstance(const mat4* m4Transform);                                     // Specify a previously generated transformation matrix
    void updateInstance(const mat4* m4Transform, unsigned int iTransformIndex);           // Updates a Transformation matrix at the given index.

    // Getters for Mesh Data
    const vector<vec3>& getVertices() const { return m_pVertices; }
    const vector<vec3>& getNormals() const { return m_pNormals; }
    const vector<vec2>& getUVs() const { return m_pUVs; }
    GLuint getVertexArray() const { return m_iVertexArray; }
    void getSpatialDimensions(vec3* pNegativeOffset, vec3* pPositiveOffset);    // Get Spatial Dimensions for the Mesh/BoundingBox.

    // Functionality for Binding and Unbinding Textures
    void bindTextures(ShaderManager::eShaderType eShaderType) const ;
    void unbindTextures() const;

    // Bounding Box Functionality
    void generateCubicBoundingBox(float fHeight, float fWidth, float fDepth) { m_sBoundingBox.generateCubicBox(fHeight, fWidth, fDepth); }
    void addBBInstance(const mat4* m4Transformation);
    bool usingBoundingBox() const { return m_sBoundingBox.isLoaded(); }
    GLuint getBBVertexArray() const { return m_sBoundingBox.iVertexArray; }
    GLuint getBBCount() const { return m_sBoundingBox.pIndices.size(); }

    // Gets the file name, only the MeshManager can set this variable.
    const string& getManagerKey() { return m_sManagerKey; }
};
