#pragma once
#include "stdafx.h"
#include "ShaderManager.h"

//////////////////////////////////////////////////////////////////
// Name: Mesh.h
// Class: Container for TriMesh Objects as well as buffers for normals
//			and indices.
// Written by: James Cote
//////////////////////////////////
class Mesh 
{
private:
	// Private Constructor and Copy Constructor to restrict usage to Object_Factory
	Mesh(const Mesh& pCopy);
	Mesh& operator= (const Mesh& pRHS);

	// Private Methods
	bool genMesh(const string& sFileName, vec3 vPosition);
	void genPlane(int iHeight, int iWidth, vec3 vPosition, vec3 vNormal);
	void genSphere(float fRadius, vec3 vPosition);
	void addCarteseanPoint(float fPhi, float fTheta, float fRadius);
	void initalizeVBOs();
	bool loadObj(const string& sFileName);

	// Mesh Manipulation - better used for Static Meshes that aren't intended to be moved often.
	void translate(vec3 vPosition);
	void rotate(quat qRotationQuatern);
	void scale(float fScale);

	// Indices for Faces of Mesh and Additional Buffer Addresses on the GPU for
	//	Indices and Normals
	vector<unsigned int> m_pIndices;
	vector< vec3 > m_pVertices, m_pNormals;
	vector< vec2 > m_pUVs;
	GLuint m_iVertexBuffer, m_iInstancedBuffer, m_iIndicesBuffer, m_iScaleBuffer;
	GLuint m_iVertexArray;
	string m_sManagerKey; // Used as key for finding Mesh in MeshManager
	ShaderManager* m_pShdrMngr;
	float m_fScale;
	GLuint m_iNumInstances;
	mat4 m_m4DefaultInstance;
	bool m_bStaticMesh;

	// Friend Class: Object_Factory to create Meshes.
	friend class MeshManager;
	// Private Manager Cookie so only MeshManager can construct a Mesh, 
	//	but make_unique<Mesh> still has access to the constructor which it needs.
	struct manager_cookie {};

public:
	explicit Mesh(const string &sFileName, bool bStaticMesh, manager_cookie);
	virtual ~Mesh();
	void loadInstanceData(const void* pData, unsigned int iSize);

	// Get Information for drawing elements.
	GLuint getNumInstances() const { return m_iNumInstances; }
	GLuint getCount() const {
		if (!m_pIndices.empty())
			return m_pIndices.size();
		else
			return m_pVertices.size();
	}

	// Checks for Render Component
	bool usingIndices() const { return !m_pIndices.empty(); }
	bool usingInstanced() const { return 0 != m_iInstancedBuffer; }

	// Getters for Mesh Data
	const vector<vec3>& getVertices() const { return m_pVertices; }
	const vector<vec3>& getNormals() const { return m_pNormals; }
	const vector<vec2>& getUVs() const { return m_pUVs; }
	float getScale() const { return m_fScale; }
	GLuint getVertexArray() const { return m_iVertexArray; }

	// Gets the file name, only the MeshManager can set this variable.
	const string& getManagerKey() { return m_sManagerKey; }
};

