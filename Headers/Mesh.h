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
    void genBillboard(const vec3* vPosition, const vec3* vNormal, const vec2* vUVStart, const vec2* vUVEnd, int iHeight, int iWidth );
    void addCarteseanPoint(float fPhi, float fTheta, float fRadius);
    void initalizeVBOs();
    bool loadObj(const string& sFileName);
    void loadMaterial(const Material* pMaterial);

    // function to generate a quaternion to rotate from y-axis normal to specified normal
    mat4 getRotationMat4ToNormal(const vec3* vNormal);

    // VBO Initialization
    void setupInstanceBuffer(GLuint iStartSpecifiedIndex);

    // Material Struct for setting uniform in Lighting Shaders
    struct sRenderMaterial
    {
        Texture* m_pDiffuseMap;
        Texture* m_pSpecularMap;
        float fShininess;
    } m_sRenderMaterial;

    // Indices for Faces of Mesh and Additional Buffer Addresses on the GPU for
    //    Indices and Normals
    vector<unsigned int> m_pIndices;
    vector< vec3 > m_pVertices, m_pNormals;
    vector< vec2 > m_pUVs;
    GLuint m_iVertexBuffer, m_iInstancedBuffer, m_iIndicesBuffer, m_iScaleBuffer;
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
    };
    vector<sBillboardInfo> m_pBillboardList;

    // Friend Class: Object_Factory to create Meshes.
    friend class MeshManager;
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

    // Billboard Usage
    unsigned int addBillboard(const vec3* vPosition, const vec3* vNormal, const vec2* vUVStart, const vec2* vUVEnd, int iHeight, int iWidth);
    void updateBillboardUVs(unsigned int iIndex, const vec2* vNewUVStart, const vec2* vNewUVEnd);
    void flushBillboards();

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

