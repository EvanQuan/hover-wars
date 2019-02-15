#pragma once
#include "stdafx.h"
#include "ShaderManager.h"
#include "Texture.h"

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
    void genCube(int iHeight, int iWidth, int iDepth, vec3 vPosition);
    void genBoundingBox(vec3 vDimensions, vec3 vPosition);
    void genBillboard();
    void initalizeVBOs();
    bool loadObj(const string& sFileName);
    void loadMaterial(const Material* pMaterial);

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
        vector<vec3> pVertices;
        vector< unsigned int> pIndices;
        GLuint iVertexBuffer, iInstancedBuffer, iVertexArray, iIndicesBuffer;

        void deleteBuffers()
        {
            glDeleteBuffers(1, &iIndicesBuffer);
            glDeleteBuffers(1, &iVertexBuffer);
            glDeleteBuffers(1, &iInstancedBuffer);
            glDeleteVertexArrays(1, &iVertexArray);
        }

        // Loads a new transformation Instance into the Instance buffer
        void loadInstance(const mat4* pTransform)
        {
            assert(nullptr != pTransform);
            glBindBuffer(GL_ARRAY_BUFFER, iInstancedBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(mat4), pTransform, GL_DYNAMIC_DRAW);
        }

        // Initializes VBOs for the Bounding Box.
        void initVBOs()
        {
            // Ensure that the Bounding Box was initialized with necessary data
            assert(!pVertices.empty());

            // Generate new vertex array for this Bounding Box
            glGenVertexArrays(1, &iVertexArray);

            // Generate Vertex Buffer
            iVertexBuffer = SHADER_MANAGER->genVertexBuffer(iVertexArray, pVertices.data(), pVertices.size() * sizeof(vec3), GL_STATIC_DRAW);
            SHADER_MANAGER->setAttrib(iVertexArray, 0, 3, sizeof(vec3), (void*)0);

            // Generate Indices Buffer
            iIndicesBuffer = SHADER_MANAGER->genIndicesBuffer(iVertexArray, pIndices.data(), pIndices.size() * sizeof(unsigned int), GL_STATIC_DRAW);

            // Set up Instance Buffer
            iInstancedBuffer = SHADER_MANAGER->genInstanceBuffer(iVertexArray, 1, (void*)0, 0, GL_DYNAMIC_DRAW);
        }
    } m_sBoundingBox;

    // Mesh Information and GPU VAO/VBOs
    vector<unsigned int> m_pIndices;
    vector< vec3 > m_pVertices, m_pNormals;
    vector< vec2 > m_pUVs;
    GLuint m_iVertexBuffer, m_iInstancedBuffer, m_iIndicesBuffer;
    GLuint m_iVertexArray;

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
    void updateBillboardVBO(unsigned int iIndex);
    unsigned int addBillboard(const vec3* vPosition, const vec3* vNormal, const vec2* vUVStart, const vec2* vUVEnd, float fHeight, float fWidth, float fDuration);
    void flushBillboards();

    // Friend Class: MeshManager to create Meshes.
    friend class MeshManager;
    friend class AnimationComponent;    // Friend class: Animation Component to manipulate Mesh information
    // Private Manager Cookie so only MeshManager can construct a Mesh, 
    //    but make_unique<Mesh> still has access to the constructor which it needs.
    struct manager_cookie {};

public:
    explicit Mesh(const string &sFileName, bool bStaticMesh, const Material* pMaterial, manager_cookie);
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
    void addInstance(const vec3* vPosition, const vec3* vNormal, float fScale);    // Specify particular components and a transformation matrix will be generated
    void addInstance(const mat4* m4Transform);                                    // Specify a previously generated transformation matrix

    // Getters for Mesh Data
    const vector<vec3>& getVertices() const { return m_pVertices; }
    const vector<vec3>& getNormals() const { return m_pNormals; }
    const vector<vec2>& getUVs() const { return m_pUVs; }
    GLuint getVertexArray() const { return m_iVertexArray; }

    // Functionality for Binding and Unbinding Textures
    void bindTextures(ShaderManager::eShaderType eShaderType) const ;
    void unbindTextures() const;

    // Gets the file name, only the MeshManager can set this variable.
    const string& getManagerKey() { return m_sManagerKey; }
};

