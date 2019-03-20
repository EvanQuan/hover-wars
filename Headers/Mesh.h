#pragma once
#include "stdafx.h"
#include "ShaderManager.h"
#include "Texture.h"
#include "DataStructures/ObjectInfo.h"

//////////////////////////////////////////////////////////////////
// Name: Mesh.h
// Class: Container for Meshes as well as buffers for normals, UVs,
//        indices, Materials, Bounding Boxes and Shader variables.
// Written by: James Coté
//////////////////////////////////
class Mesh
{
private:
    // Private Constructor and Copy Constructor to restrict usage to Object_Factory
    Mesh(const Mesh& pCopy);
    Mesh& operator= (const Mesh& pRHS);

    // Private Methods
    bool genMesh(const string& sFileName, vec3 vPosition, string sHashKey, float fScale = 1.0f);
    void genPlane(int iHeight, int iWidth, vec3 vPosition, vec3 vNormal, string sHashKey);
    void genSphere(float fRadius, vec3 vPosition, string sHashKey);
    void genCube(float fHeight, float fWidth, float fDepth, vec3 vPosition, string sHashKey);
    void genBillboard();
    void initalizeVBOs();
    void initalizeVBOs(const vector<float>* vVNData, bool bUsingNormals, bool bUsingUVs);
    void gatherVNData(vector<float>* vVNData, bool* bUsingNormals, bool* bUsingUVs);
    bool loadObj(const string& sFileName);
    void loadObjectInfo(const ObjectInfo* pObjectProperties, string sHashKey);
    void loadMaterial(const ObjectInfo::Material* pMaterial);
    void loadBoundingBox(const ObjectInfo::BoundingBox* pBoundingBox, const vec3* vStartingPosition, string sHashKey);
    void loadInstanceBuffer();

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
        //vector<mat4>            pInstances;
        unordered_map<string, mat4> pInstanceMap;
        vector<vec3>                pVertices;
        vector< unsigned int>       pIndices;
        GLuint                      iVertexBuffer,
                                    iInstancedBuffer,
                                    iVertexArray,
                                    iIndicesBuffer;
        vec3                        vNegativeOffset, vPositiveOffset; // Specifies the dimensions of the Spacial cube for the Bounding Box.
        eBoundingBoxTypes           eType;

        // Check to see if the Bounding Box is loaded.
        bool isLoaded() const { return 0 != iVertexArray; }

        // Initialization and cleaning of Buffers
        void deleteBuffers();   // Deletes the VAO and VBOs used by the Mesh's Bounding Box
        void initVBOs();        // Initializes VBOs for the Bounding Box.

        // Loads a new transformation Instance into the Instance buffer
        void loadInstance(const mat4* pTransform, string sHashKey);
        void updateInstance(const mat4* pTransform, string sHashKey);
        void removeInstance(string sHashKey);
        void loadInstanceBuffer();

        // Generation Functions
        void generateCubicBox(float fHeight, float fWidth, float fDepth);
        void generateCubicBox(const vec3* vNegativeOffset, const vec3* vPositiveOffset);
    } m_sBoundingBox;

    // Adds an Instance for Bounding Box Drawing
    void addBBInstance(const mat4* m4Transformation, string sHashKey);

    // Mesh Information and GPU VAO/VBOs
    vector<unsigned int>        m_pIndices;
    vector< vec3 >              m_pVertices,
                                m_pNormals;
    vector< vec2 >              m_pUVs;
    GLuint                      m_iVertexBuffer,
                                m_iInstancedBuffer,
                                m_iIndicesBuffer,
                                m_iVertexArray;

    // Spatial Information for Mesh
    vec3 m_vNegativeOffset, m_vPositiveOffset;

    string                      m_sManagerKey;          // Used as key for finding Mesh in MeshManager
    ShaderManager*              m_pShdrMngr;            // Pointer to Shader Manager for GPU/Shader Interaction
    unordered_map<string, mat4> m_m4InstanceMap;        // Map for handled Instance Management among multiple objects
    bool                        m_bStaticMesh;          // Boolean to differentiate between Static and Dynamic Meshes (one is updated on load, others are updated frequently)
    mat4                        m_m4ScaleMatrix;        // Scale Matrix that is set on load for any updates that need to maintain the scale of the mesh.

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
    explicit Mesh(const string &sFileName, bool bStaticMesh, float fScale, const ObjectInfo* pObjectProperties, string sHashKey, manager_cookie);
    virtual ~Mesh();

    // Get Information for drawing elements.
    GLuint getNumInstances() const { return m_m4InstanceMap.size(); }
    GLuint getCount() const {
        if (!m_pIndices.empty())                // Using Indices takes priority
            return m_pIndices.size();
        else if (!m_pBillboardList.empty())     // For Billboards
            return m_pBillboardList.size();
        else                                    // Otherwise, assume drawing non-instanced
            return m_pVertices.size();
    }

    // Checks for Render Component
    bool usingIndices() const { return !m_pIndices.empty(); }
    bool usingInstanced() const { return 0 != m_iInstancedBuffer; }

    // Function to add a new Instance Matrix for the Mesh. If the Mesh is dynamic, it will replace the current instance, static will add a new instance.
    void addInstance(const vec3* vPosition, const vec3* vNormal, float fScale, string sHashKey);    // Specify particular components and a transformation matrix will be generated
    void addInstance(const mat4* m4Transform, string sHashKey);                                             // Add an Instance with a given HashKey
    void updateInstance(const mat4* m4Transform, string sHashKey);                                          // Updates a Transformation matrix at a given hashkey.
    void removeInstance(string sHashKey);                                                                   // Removes a transformation matrix at a given hashkey.

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
    bool usingBoundingBox() const { return m_sBoundingBox.isLoaded(); }
    GLuint getBBVertexArray() const { return m_sBoundingBox.iVertexArray; }
    GLuint getBBCount() const { return m_sBoundingBox.pIndices.size(); }

    // Gets the file name, only the MeshManager can set this variable.
    const string& getManagerKey() { return m_sManagerKey; }
};
