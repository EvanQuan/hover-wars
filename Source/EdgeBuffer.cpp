#include "EdgeBuffer.h"
#include <sstream>

// DEFINES
#define SILHOUETTE_FLAG 0x2f
#define INSIDE_EDGE_FLAG 0x22
#define OUTSIDE_EDGE_FLAG 0x21
#define INSIDE_BOUNDARY_FLAG 0x1a
#define OUTSIDE_BOUNDARY_FLAG 0x15
#define ARTIST_MASK 0xf0
#define NUM_EDGES(n) 2*n + 1
#define EST_SIZE( n ) 0.05 * NUM_EDGES(n)

#define PI					3.14159265f
#define RAD_TO_DEG( n ) n * (180.f/PI)

// Constants
const float AVG_MULTIPLIER = 1.0f / 3.0f;

EdgeBuffer::EdgeBuffer( GLuint iVertArray )
{
	m_iVertexArray = iVertArray;

	m_iIndicesBuffer	= ShaderManager::getInstance()->genIndicesBuffer( m_iVertexArray, m_vDrawIndices.data(), m_vDrawIndices.size() * sizeof( uvec2 ), GL_DYNAMIC_DRAW );
}

EdgeBuffer::~EdgeBuffer()
{
	m_pAdjacencyList.clear();
	glDeleteBuffers( 1, &m_iIndicesBuffer );
}


void EdgeBuffer::GenerateAdjListMesh( const vector<unsigned int>& m_pIndices,
									  const vector<trimesh::point>& pNormals,
									  int iNumVerts )
{
	// Clear any data we had previously and resize for the new adjacency list.
	m_pAdjacencyList.clear();
	m_pAdjacencyList.resize( iNumVerts, nullptr );

	vec3 Normal1, Normal2, Normal3, pAvgNormal;

	// Populate Adjacency List
	for ( unsigned int i = 0; i < m_pIndices.size(); i += 3 )
	{
		Normal1 = vec3( pNormals[ m_pIndices[ i ] ][ 0 ],
						pNormals[ m_pIndices[ i ] ][ 1 ],
						pNormals[ m_pIndices[ i ] ][ 2 ] );
		Normal2 = vec3( pNormals[ m_pIndices[ i + 1 ] ][ 0 ],
						pNormals[ m_pIndices[ i + 1 ] ][ 1 ],
						pNormals[ m_pIndices[ i + 1 ] ][ 2 ] );
		Normal3 = vec3( pNormals[ m_pIndices[ i + 2 ] ][ 0 ],
						pNormals[ m_pIndices[ i + 2 ] ][ 1 ],
						pNormals[ m_pIndices[ i + 2 ] ][ 2 ] );

		pAvgNormal = (Normal1 + Normal2 + Normal3) * AVG_MULTIPLIER;
		addTriangle( m_pIndices[ i ], m_pIndices[ i + 1 ], m_pIndices[ i + 2 ], i, pAvgNormal );
	}

	printEB();
}

void EdgeBuffer::GenerateAdjListStrip( const vector<vec3>& pVerts,
									   const vector<vec3>& pNormals,
									   int iNumVerts )
{
	// Clear any data we had previously and resize for the new adjacency list.
	m_pAdjacencyList.clear();
	m_pAdjacencyList.resize( iNumVerts, nullptr );
	vec3 pAvgNormal;

	int iTriNum = 1;

	// Populate Adjacency List
	for ( unsigned int i = 0; i + 3 < pVerts.size(); i = i + 2 )
	{
		// Store the Normal of the Triangle.
		pAvgNormal = (pNormals[ i ] + pNormals[ i + 1 ] + pNormals[ i + 2 ]) * AVG_MULTIPLIER;
		addTriangle( i, i + 1, i + 2, iTriNum++, pAvgNormal );

		pAvgNormal = (pNormals[ i + 1 ] + pNormals[ i + 2 ] + pNormals[ i + 3 ]) * AVG_MULTIPLIER;
		addTriangle( i + 1, i + 2, i + 3, iTriNum++, pAvgNormal );
	}	

	printEB();
}

// Calculates the EdgeBuffer information for a trimesh object.  Only calculates if the camera has
//	moved since last calculation.
void EdgeBuffer::CalculateEdgeBufferMesh( const vector<unsigned int>& m_pIndices,
										  const vector<trimesh::point>& pNormals,
										  const vec3* pLookAt )
{
	// Compare new LookAt with previous one.
	if ( !all( equal( pPrevLookAt, *pLookAt ) ) )
	{
		// Calculate from fresh slate.
		reset();
		vec3 Normal1, Normal2, Normal3, pAvgNormal;
		bool bFront;
		unsigned int iSize = m_pIndices.size();

		// For Every Triangle: calculate face and update EB
		for ( unsigned int i = 0; i < iSize; i += 3 )
		{
			Normal1 = vec3( pNormals[ m_pIndices[ i ] ][ 0 ],
							pNormals[ m_pIndices[ i ] ][ 1 ],
							pNormals[ m_pIndices[ i ] ][ 2 ] );
			Normal2 = vec3( pNormals[ m_pIndices[ i + 1 ] ][ 0 ],
							pNormals[ m_pIndices[ i + 1 ] ][ 1 ],
							pNormals[ m_pIndices[ i + 1 ] ][ 2 ] );
			Normal3 = vec3( pNormals[ m_pIndices[ i + 2 ] ][ 0 ],
							pNormals[ m_pIndices[ i + 2 ] ][ 1 ],
							pNormals[ m_pIndices[ i + 2 ] ][ 2 ] );
		
			pAvgNormal = (Normal1 + Normal2 + Normal3) * AVG_MULTIPLIER;
		
			// Is it Front or BackFacing?
			float angle = dot( pAvgNormal, *pLookAt );
			bFront = angle < 0.0;
		
			// Update Edges
			updateTriangle( m_pIndices[ i ], m_pIndices[ i + 1 ], m_pIndices[ i + 2 ],
							bFront, !bFront );
		}

		pPrevLookAt = *pLookAt;

		//printEB();
	}
}


void EdgeBuffer::CalculateEdgeBufferStrip( const vector<vec3>& pNormals, const vec3* pLookAt )
{
	if( !all(equal( pPrevLookAt, *pLookAt)) )
	{
		reset();
		vec3 Normal1, Normal2, Normal3, pAvgNormal;
		float angle;
		bool bFront;

		// Populate Adjacency List
		for ( unsigned int i = 0; i + 3 <= pNormals.size(); i = i + 2 )
		{
			Normal1 = vec3( pNormals[ i ][ 0 ],
							pNormals[ i ][ 1 ],
							pNormals[ i ][ 2 ] );
			Normal2 = vec3( pNormals[ i + 1 ][ 0 ],
							pNormals[ i + 1 ][ 1 ],
							pNormals[ i + 1 ][ 2 ] );
			Normal3 = vec3( pNormals[ i + 2 ][ 0 ],
							pNormals[ i + 2 ][ 1 ],
							pNormals[ i + 2 ][ 2 ] );

			pAvgNormal = (Normal1 + Normal2 + Normal3) * AVG_MULTIPLIER;

			// Is it Front or BackFacing?
			angle = dot( pAvgNormal, *pLookAt );
			bFront = angle < 0.0;

			// Update Edges
			updateTriangle( i, i + 1, i + 2, bFront, !bFront );

			Normal1 = vec3( pNormals[ i + 2 ][ 0 ],
							pNormals[ i + 2 ][ 1 ],
							pNormals[ i + 2 ][ 2 ] );
			Normal2 = vec3( pNormals[ i + 1 ][ 0 ],
							pNormals[ i + 1 ][ 1 ],
							pNormals[ i + 1 ][ 2 ] );
			Normal3 = vec3( pNormals[ i + 3 ][ 0 ],
							pNormals[ i + 3 ][ 1 ],
							pNormals[ i + 3 ][ 2 ] );

			pAvgNormal = (Normal1 + Normal2 + Normal3) * AVG_MULTIPLIER;

			// Is it Front or BackFacing?
			angle = dot( pAvgNormal, *pLookAt );
			bFront = angle < 0.0;

			// Update Edges
			updateTriangle( i + 1, i + 2, i + 3, bFront, !bFront );
		}

		pPrevLookAt = *pLookAt;

		//printEB();
	}
}


/*******************************************************************************\
 * Private Functions														   *
\*******************************************************************************/

void EdgeBuffer::drawEdgeBuffer( float fScale, vec3& pPosition, float fMinThreshold, float fMaxThreshold )
{
	// Get current bindings as a restore point
	GLint iCurrProgBinding = 0, iCurrVABinding = 0;
	glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &iCurrVABinding );
	glGetIntegerv( GL_CURRENT_PROGRAM, &iCurrProgBinding );

	// Set Uniforms
	setUniforms( fScale, pPosition );

	// Bind Edge Buffer data
	glBindVertexArray( m_iVertexArray );

	// Draw All Edges - TODO: Add flags to toggle different edges to draw.
	findIndices( SILHOUETTE_FLAG );
	drawEdges( ShaderManager::eShaderType::SILH_SHDR );
	findIndices( INSIDE_EDGE_FLAG, fMinThreshold, fMaxThreshold );
	drawEdges( ShaderManager::eShaderType::INSIDE_EDGE_SHDR );
	findIndices( INSIDE_BOUNDARY_FLAG );
	drawEdges( ShaderManager::eShaderType::INSIDE_BOUNDRY_SHDR );
	findIndices( OUTSIDE_EDGE_FLAG );
	drawEdges( ShaderManager::eShaderType::OUTSIDE_EDGE_SHDR );
	findIndices( OUTSIDE_BOUNDARY_FLAG );
	drawEdges( ShaderManager::eShaderType::OUTSIDE_BOUNDRY_SHDR );

	glUseProgram( iCurrProgBinding );
	glBindVertexArray( iCurrVABinding );
}

// Binds program and indices to draw edges of a given type.
void EdgeBuffer::drawEdges( ShaderManager::eShaderType eType )
{
	glUseProgram( ShaderManager::getInstance()->getProgram( eType ) );

	// Upload new Indices
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iIndicesBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_vDrawIndices.size() * sizeof( uvec2 ), m_vDrawIndices.data(), GL_DYNAMIC_DRAW );

	glDrawElements( GL_LINES, m_vDrawIndices.size() * 2, GL_UNSIGNED_INT, nullptr );
}

// Set Uniforms to all edge shaders: Scale, Position and Edge Width.
void EdgeBuffer::setUniforms( float fScale, vec3& pPosition )
{
	// Translation Matrix to translate edge to object position.
	mat4 pTranslateMat = (glm::mat4)glm::translate( pPosition );

	// Iterate through all Edge Shaders using ShaderManager defined bounds.
	for ( int eEdgeType = ShaderManager::eDefaultEdgeShader;
		  eEdgeType <= ShaderManager::eLastEdgeShader;
		  eEdgeType++ )
	{
		ShaderManager::getInstance()->setUniformFloat(	   (ShaderManager::eShaderType)eEdgeType, "fWidth", 0.006f );
		ShaderManager::getInstance()->setUniformFloat(	   (ShaderManager::eShaderType)eEdgeType, "fScale", fScale );
		ShaderManager::getInstance()->setUnifromMatrix4x4( (ShaderManager::eShaderType)eEdgeType, "translate", &pTranslateMat );
	}
}

// Find every Index corresponding to a specified Edge Value.
void EdgeBuffer::findIndices( byte bFilter, float fMinThreshold, float fMaxThreshold )
{
	// Clear Drawing Index Buffer
	int iDrawSize = m_vDrawIndices.size();
	m_vDrawIndices.clear();
	m_vDrawIndices.reserve( iDrawSize );

	for ( unsigned int i = 0;
		 i < m_pAdjacencyList.size();
		 ++i )
	{
		sEdge* pEdgeIter = m_pAdjacencyList[i];
		while ( pEdgeIter != nullptr )
		{
			// Is this one a desired edge?
			if ( bFilter == pEdgeIter->bEdgeType )
			{
				// Check Dihedral Threshold if applicable (for Inside Edge only)
				if ( INSIDE_EDGE_FLAG != bFilter )
					m_vDrawIndices.push_back( uvec2( i, pEdgeIter->iVertex ) );
				else if ( pEdgeIter->fAngleDeg >= fMinThreshold && pEdgeIter->fAngleDeg <= fMaxThreshold )
					m_vDrawIndices.push_back( uvec2( i, pEdgeIter->iVertex ) );
			}
	
			pEdgeIter = pEdgeIter->pNext;
		}
	}
}


// printEB
// - For Debugging: Output Edge Buffer to the Console.
void EdgeBuffer::printEB()
{
	// Iterator through the Row
	sEdge* pIter = nullptr;
	bool bShortView = true;
	unsigned int uiShortRange = 10;
	bShortView = uiShortRange < m_pAdjacencyList.size();

	// Loop through and Print Adjacency List (first 3)
	for ( unsigned int i = 0; i < (bShortView ? uiShortRange : m_pAdjacencyList.size()); ++i )
	{
		cout << i << "->";
		pIter = m_pAdjacencyList[ i ];
		
		while ( nullptr != pIter )
		{
			pIter->printEdge();
			cout << (nullptr != pIter->pNext ? ", " : ".");
			pIter = pIter->pNext;
		}

		cout << endl;
	}
	
	// Show last 3 if we want a shorter printout.
	if ( bShortView )
	{
		cout << endl << "..." << endl << endl;

		// Loop through last 3
		for ( unsigned int i = m_pAdjacencyList.size() - 3; i < m_pAdjacencyList.size(); ++i )
		{
			cout << i << "->";
			pIter = m_pAdjacencyList[ i ];

			while ( nullptr != pIter )
			{
				pIter->printEdge();
				cout << (nullptr != pIter->pNext ? ", " : ".");
				pIter = pIter->pNext;
			}

			cout << endl;
		}
	}
}

// addTriangle
// - Adds 3 edges of a given triangle.
void EdgeBuffer::addTriangle( unsigned int iVert1, unsigned int iVert2, unsigned int iVert3, int iTriangle, const vec3& pNormal )
{
	insertEdge( iVert1, iVert2, iTriangle, pNormal );
	insertEdge( iVert1, iVert3, iTriangle, pNormal );
	insertEdge( iVert2, iVert3, iTriangle, pNormal );
}

// insertEdge
// - Inserts an Edge into the Edge buffer between 2 vertices.  Adjacency List is indexed via iVertex1
//		and connecting edge is denoted by iVertex2
void EdgeBuffer::insertEdge( unsigned int iVertex1, unsigned int iVertex2, int iTriangle, const vec3& pNormal )
{
	if ( nullptr == m_pAdjacencyList[ iVertex1 ] )	// No Edges connected to this Vertex, create first edge
	{
		m_pAdjacencyList[ iVertex1 ] = new sEdge( iVertex2, iTriangle );
		m_pAdjacencyList[ iVertex1 ]->pNormal1 = vec3( pNormal );
	}
	else
	{
		// Set Adjacency Row iterator
		sEdge* pIter = m_pAdjacencyList[ iVertex1 ];

		// Iterate through: result in either end of adjacency row or before insertion spot
		while ( nullptr != pIter->pNext && iVertex2 > pIter->pNext->iVertex )
			pIter = pIter->pNext;

		// Equal Case: Set Second Triangle of Edge
		if ( iVertex2 == pIter->iVertex )
		{
			pIter->iTriangle2 = iTriangle;
			pIter->pNormal2 = vec3( pNormal );
			pIter->fAngleDeg = RAD_TO_DEG( acos( dot( pIter->pNormal1, pIter->pNormal2 ) ) );
		}
		else  // Add New Edge
		{
			sEdge* pNewEdge = new sEdge( iVertex2, iTriangle );
			pNewEdge->pNext = pIter->pNext;
			pIter->pNext = pNewEdge;
			pIter->pNormal1 = vec3( pNormal );
		}
	}
}

// reset
// - Resets the Edgebuffer masks without clearing the data
void EdgeBuffer::reset()
{
	// Row Iterator
	sEdge* pRowIter = nullptr;

	// For loop through Adjacency List
	for ( vector< sEdge* >::iterator pIter = m_pAdjacencyList.begin();
		 pIter != m_pAdjacencyList.end();
		 ++pIter )
	{
		// Clear all the Edge Flags in the row.
		pRowIter = (*pIter);
		while ( nullptr != pRowIter )
		{
			pRowIter->bEdgeType = 0;
			pRowIter = pRowIter->pNext;
		}
	}
}

// updateTriangle
// - Updates a given triangle (3 verts) with new values for the edgebuffer.
void EdgeBuffer::updateTriangle( unsigned int iVert1, unsigned int iVert2, unsigned int iVert3, bool bFront, bool bBack )
{ 
	updateEB( iVert1, iVert2, bFront, bBack );
	updateEB( iVert1, iVert3, bFront, bBack );
	updateEB( iVert2, iVert3, bFront, bBack );
}


// updateEB
// - Updates an Edge with a given Front and Back Flag.
void EdgeBuffer::updateEB( int iVertex1, int iVertex2, bool bFront, bool bBack )
{
	sEdge* pUpdatingEdge = m_pAdjacencyList[ iVertex1 ];
	unsigned int iEdgeBufferIndex = (iVertex1 + iVertex2) - 1;
	uvec2 uvHashKey = uvec2( iVertex1, iVertex2 );

	while ( nullptr != pUpdatingEdge && iVertex2 != pUpdatingEdge->iVertex )
		pUpdatingEdge = pUpdatingEdge->pNext;

	if ( nullptr != pUpdatingEdge )
	{
		if ( (pUpdatingEdge->bEdgeType & ARTIST_MASK) < Artist_Threshold )
			pUpdatingEdge->bEdgeType += A_Bit;

		// Create Mask
		// F and B Bits
		byte bMask = (bFront ? F_Bit : 0);	// xxxx Fxxx
		bMask |= (bBack ? B_Bit : 0);		// xxxx xBxx
		pUpdatingEdge->bEdgeType ^= bMask;

		// Fa and Ba Bits
		bMask = (bFront ? Fa_Bit : 0);		// xxxx xxFax
		bMask |= (bBack ? Ba_Bit : 0);		// xxxx xxxBa
		pUpdatingEdge->bEdgeType |= bMask;
	}
	else
		cout << "Error: Tried to update Edge (" << iVertex1 << "->" << iVertex2 << ") which doesn't exist!" << endl;
}