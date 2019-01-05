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
public:
	void initMesh( );	// Binds mesh information to a specified Vertex Array.
	void loadInstanceData(const vector< mat4 >* pNewData);

	// Get Information for drawing elements.
	void bindIndicesBuffer() const { glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iIndicesBuffer ); }
	GLuint getCount() { return m_pIndices.size(); }

	// Gets the file name, only the MeshManager can set this variable.
	const string& getFileName() { return m_sFileName; }

	void drawMesh( GLuint iProgram );

private:
	// Private Constructor and Copy Constructor to restrict usage to Object_Factory
	Mesh( const string &sFileName );
	Mesh( const Mesh& pCopy );
	Mesh& operator= ( const Mesh& pRHS );
	~Mesh();

	// Private Methods
	bool genMesh( const string& sFileName );
	bool loadObj(const string& sFileName);

	// Indices for Faces of Mesh and Additional Buffer Addresses on the GPU for
	//	Indices and Normals
	vector<unsigned int> m_pIndices;
	vector< vec3 > m_pVertices, m_pNormals;
	vector< vec3 > m_pVNData;
	GLuint m_iVNBuffer, m_iInstancedBuffer, m_iIndicesBuffer;
	GLuint m_iVertexArray;
	string m_sFileName;
	ShaderManager* m_pShdrMngr;
	GLuint m_iNumInstances;
	mat4 m_m4DefaultInstance;

	// Friend Class: Object_Factory to create Meshes.
	friend class MeshManager;
};

