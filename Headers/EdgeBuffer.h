#pragma once

#include "stdafx.h"
#include "TriMesh.h"
#include "ShaderManager.h"
#include <unordered_map>
#include <bitset>

class EdgeBuffer
{
public:
	EdgeBuffer( GLuint iVertArray );
	~EdgeBuffer();

	void GenerateAdjListMesh( const vector<unsigned int>& m_pIndices,
							  const vector<trimesh::point>& pNormals,
							  int iNumVerts );
	void GenerateAdjListStrip( const vector<vec3>& pVerts,
							   const vector<vec3>& pNormals,
							   int iNumVerts );
	void CalculateEdgeBufferMesh( const vector<unsigned int>& m_pIndices,
								  const vector<trimesh::point>& pNormals,
								  const vec3* pLookAt );
	void CalculateEdgeBufferStrip( const vector<vec3>& pNormals, const vec3* pLookAt );

	void drawEdgeBuffer( float fScale, vec3& pPosition, float fMinThreshold, float fMaxThreshold );

private:
	// Edge Structure
	struct sEdge
	{
		unsigned int iVertex;					// Vertex, Connected to
		unsigned int iTriangle1, iTriangle2;	// Connected Triangles
		vec3 pNormal1, pNormal2;				// Normals for the Connected Triangles
		float fAngleDeg;						// Angle between 2 triangles (in degrees)
		byte bEdgeType;							// 0000 0000 = xxxA FBFaBa
		sEdge* pNext;							// Pointer to Next in Row.

		// Default Constructor, requires the Vertex.
		sEdge( unsigned int iVert, unsigned int iTri1 = 0, unsigned int iTri2 = 0, byte bEdge = 0, sEdge* next = nullptr )
		{
			iVertex = iVert;
			iTriangle1 = iTri1;
			iTriangle2 = iTri2;
			pNormal1 = vec3( 0.0 );
			pNormal2 = vec3( 0.0 );
			fAngleDeg = 0.0f;
			bEdgeType = bEdge;
			pNext = next;
		}

		// Destructor: Kill connected Nodes -> Only need to kill first one to kill row.
		~sEdge()
		{
			if( nullptr != pNext )
				delete pNext;
		}

		// Debug Print
		void printEdge()
		{
			cout << "{" << iVertex << "," << iTriangle1 << "," << iTriangle2
				 << " [" << pNormal1.x << ", " << pNormal1.y << ", " << pNormal1.z << "] ["
				 << pNormal2.x << ", " << pNormal2.y << ", " << pNormal2.z << "] @ " << fAngleDeg << "; "
				 << (bEdgeType >> 4) << std::bitset<4>( bEdgeType ) << "}";
		}
	};

	// Private Functions
	// Private Assignment Operator and Copy Constructor
	EdgeBuffer( const EdgeBuffer& pCopy );
	EdgeBuffer& operator=( const EdgeBuffer& pRHS ) {}
	void addTriangle( unsigned int iVert1, unsigned int iVert2, unsigned int iVert3, int iTriangle, const vec3& pNormal );
	void insertEdge( unsigned int iVertex1, unsigned int iVertex2, int iTriangle, const vec3& pNormal );
	void reset();
	void updateEB( int iVertex1, int iVertex2, bool bFront, bool bBack );
	void updateTriangle( unsigned int iVert1, unsigned int iVert2, unsigned int iVert3, bool bFront, bool bBack );
	void findIndices( byte bFilter, float fMinThreshold = 0.0f, float fMaxThreshold = 360.0f );
	void setUniforms( float fScale, vec3& pPosition );
	void drawEdges( ShaderManager::eShaderType eType );

	// Debug:
	void printEB();

	// Variables
	vector< sEdge* > m_pAdjacencyList;
	vec3 pPrevLookAt;
	GLuint m_iVertexArray, m_iIndicesBuffer;
	vector< uvec2 > m_vDrawIndices;

	// Bit Masks
	const byte Artist_Threshold = 1 << 5;
	const byte A_Bit = 1 << 4;
	const byte F_Bit = 1 << 3;
	const byte B_Bit = 1 << 2;
	const byte Fa_Bit = 1 << 1;
	const byte Ba_Bit = 1;
};

