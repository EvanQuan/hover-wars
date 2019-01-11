#include "Mesh.h"
#include <sstream>

// Basic Constructor
Mesh::Mesh( const string &sManagerKey, manager_cookie )
{
	m_sManagerKey = sManagerKey;
	m_m4DefaultInstance =  mat4(1.0f);
	m_iNumInstances = 1;
	m_iVertexArray = -1;
}

// Delete any buffers that we initialized
Mesh::~Mesh()
{
	glDeleteBuffers( 1, &m_iVNBuffer );
	glDeleteBuffers( 1, &m_iIndicesBuffer );
	glDeleteBuffers(1, &m_iInstancedBuffer);
	glDeleteVertexArrays( 1, &m_iVertexArray );
}

// Load the Mesh from a given file name
//  Result: Stores the mesh variables into a set of vertices
bool Mesh::genMesh( const string& sFileName )
{
	bool bReturnValue = true;

	// Load Mesh
	if (sFileName.substr(sFileName.find_last_of(".") + 1) == "obj")
		bReturnValue = loadObj(sFileName);		

	if (bReturnValue)
	{
		// Compile Data into 1 buffer for GPU
		for (unsigned int i = 0; i < m_pVertices.size(); ++i)
		{
			m_pVNData.push_back(m_pVertices[i]);
			m_pVNData.push_back(m_pNormals[i]);
		}

		// Clear Normals and Verts to save space.
		m_pNormals.clear();
		m_pVertices.clear();
	}

	return bReturnValue;
}

// Generate a generic plane at the origin with a given Height and Width
//	Normals are along the y-axis.
void Mesh::genPlane(int iHeight, int iWidth, vec3 vPosition, vec3 vNormal)
{
	// Generate 4 vertices around the origin
	/***********************************
		Width = (x-axis)
		Height = (z-axis)
	*/
	float fHalfHeight = (float)(iHeight >> 1);
	float fHalfWidth = (float)(iWidth >> 1);
	m_pVertices.push_back(vec3(-fHalfWidth, 0.f, -fHalfHeight));
	m_pVertices.push_back(vec3(-fHalfWidth, 0.f, fHalfHeight));
	m_pVertices.push_back(vec3(fHalfWidth, 0.f, -fHalfHeight));
	m_pVertices.push_back(vec3(fHalfWidth, 0.f, fHalfHeight));

	// Generate Normals for each Vertex.
	m_pNormals.insert(m_pNormals.begin(), 4, vNormal);

	if ((vec3(0.f, 1.f, 0.f) != vNormal) && (vec3(0.0f, -1.f, 0.f) != vNormal))
	{
		// create Rotation quaternion to rotate plane.
		quat q = cross(vec3(0.f, 1.f, 0.f), vNormal);
		normalize(q);
		q.w = 1 + dot(vec3(0.f, 1.f, 0.f), vNormal);

		// Rotate Plane
		Mesh::rotate(q);
	}

	// If translation is necessary, translate plane.
	if (vec3(0.f) != vPosition)
		Mesh::translate(vPosition);

}

// Code for Loading a .obj file. Not comprehensive, will generate its own normals and will fail to load any
//		faces that are not quads or tris. Algorithm modified from original written by Andrew Robert Owens.
bool Mesh::loadObj(const string& sFileName)
{
	// Locals
	ifstream in(sFileName.c_str());
	bool bReturnValue = in.is_open();

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

			if ("v" == sToken)	// Vertex Data
			{
				ssLine >> vTempVec.x >> vTempVec.y >> vTempVec.z;
				m_pVertices.push_back(vTempVec);
			}
			else if ("vt" == sToken) // uv-coords
			{/* Ignored */}
			else if ("g" == sToken) // Group
			{/* Ignored */}
			else if ("o" == sToken) // Object Name
			{/* Ignored */}
			else if ("f" == sToken) // Face
			{
				// Local Variables for reading face.
				int iIndices[3] = { -1, -1, -1 };
				vector< int > vFaceVerts;

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
					iIndices[1] = (-1 == iIndices[1] ? iIndices[1] : iIndices[1] - 1); // UV-Coord; Not Used
					iIndices[2] = (-1 == iIndices[2] ? iIndices[2] : iIndices[2] - 1); // Normal; Not Used

					// Only Store the Vertex Indices
					vFaceVerts.push_back(iIndices[0]);
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
					else if( 3 != vFaceVerts.size() ) // Error
					{
						cerr << "Invalid Object in File: " << sFileName << ".\n";
						bReturnValue = false;
					}
				} // END Indices Store
			}// END Face
		} // END While

		// Close file when finished.
		in.close();

		// Vertices and Indices Loaded, need to compute Normals
		m_pNormals.resize(m_pVertices.size(), vec3(0.0));
		for (unsigned int i = 0; i < m_pIndices.size(); i += 3)
		{
			assert((i + 2) < m_pIndices.size());
			vec3 vTri[3] = { m_pVertices[m_pIndices[i]], m_pVertices[m_pIndices[i + 1]], m_pVertices[m_pIndices[i + 2]] };

			// Calculate Triangle Normal: (v1 - v0) x (v2 - v0)
			vec3 vTriNormal = cross((vTri[1] - vTri[0]), (vTri[2] - vTri[0]));

			// Accumulate Normals Per Vertex;
			m_pNormals[m_pIndices[i]] += vTriNormal;
			m_pNormals[m_pIndices[i+1]] += vTriNormal;
			m_pNormals[m_pIndices[i+2]] += vTriNormal;
		}

		// Normalize all Accumulated Normals
		for (vector< vec3 >::iterator vNormIter = m_pNormals.begin();
			vNormIter != m_pNormals.end();
			++vNormIter)
			(*vNormIter) = normalize((*vNormIter));
	}

	return bReturnValue;
}

void Mesh::loadInstanceData(const vector< mat4 >* pNewData)
{
	if (nullptr != pNewData)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_iInstancedBuffer);
		glBufferData(GL_ARRAY_BUFFER, pNewData->size() * sizeof(mat4), pNewData->data(), GL_STREAM_DRAW);
		m_iNumInstances = pNewData->size();
	}
}

// Initialize Open GL handles for drawing mesh.
void Mesh::initMesh()
{
	if (-1 == m_iVertexArray)
	{
		glGenVertexArrays(1, &m_iVertexArray);

		// Generate Buffer
		m_iVNBuffer = SHADER_MANAGER->genVertexBuffer(
			m_iVertexArray,
			m_pVNData.data(),
			m_pVNData.size() * sizeof(vec3),
			GL_STATIC_DRAW);

		// Set-up Attributes
		// Vertices
		SHADER_MANAGER->setAttrib(
			m_iVertexArray, 0, 3, sizeof(vec3) * 2, (void*)0);
		// Normals
		SHADER_MANAGER->setAttrib(
			m_iVertexArray, 1, 3, sizeof(vec3) * 2, (void*)sizeof(vec3));

		// Set up Instanced Buffer for Instance Rendering
		m_iInstancedBuffer = SHADER_MANAGER->genVertexBuffer(
			m_iVertexArray, (void*)&m_m4DefaultInstance,
			sizeof(mat4), GL_STREAM_DRAW);

		// Instance Rendering Attributes
		// column 0
		glBindBuffer(GL_ARRAY_BUFFER, m_iInstancedBuffer);
		SHADER_MANAGER->setAttrib(
			m_iVertexArray, 2, 4, sizeof(vec4) * 4, (void*)0);
		// column 1
		SHADER_MANAGER->setAttrib(
			m_iVertexArray, 3, 4, sizeof(vec4) * 4, (void*)sizeof(vec4));
		// column 2
		SHADER_MANAGER->setAttrib(
			m_iVertexArray, 4, 4, sizeof(vec4) * 4, (void*)(2 * sizeof(vec4)));
		// column 3
		SHADER_MANAGER->setAttrib(
			m_iVertexArray, 5, 4, sizeof(vec4) * 4, (void*)(3 * sizeof(vec4)));

		glBindVertexArray(m_iVertexArray);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);

		// Indices
		m_iIndicesBuffer = SHADER_MANAGER->genIndicesBuffer(
			m_iVertexArray,
			m_pIndices.data(),
			m_pIndices.size() * sizeof(unsigned int),
			GL_STATIC_DRAW);
	}
}

// draws the Mesh
void Mesh::drawMesh( GLuint iProgram )
{
	ShaderManager* pShdrMngr = SHADER_MANAGER;

	// Set Up Shader
	glBindVertexArray( m_iVertexArray );
	glUseProgram( iProgram );

	// Draw
	glDrawElementsInstanced( GL_TRIANGLES, m_pIndices.size(), GL_UNSIGNED_INT, 0, m_iNumInstances );

	// Unload Shader 
	glUseProgram(0);
	glBindVertexArray( 0 );
}

/****************************************************************************\
 * Static Mesh Manipulation													*
\****************************************************************************/

// Translates a Mesh from the origin to the given position.
//	Should only be used for static objects to set their static positions once.
void Mesh::translate(vec3 vPosition)
{
	// Generate translation matrix to translate plane to new position.
	mat4 m4TranslationMat = glm::translate(vPosition - vec3(0.f));

	// translate all vertices to new positions
	for (vector<vec3>::iterator iter = m_pVertices.begin();
		iter != m_pVertices.end();
		++iter)
	{
		(*iter) = m4TranslationMat * vec4((*iter), 1.f);
	}
}

// Given a Quaternion, rotate all vertices of mesh by quaternion value.
void Mesh::rotate(quat qRotationQuatern)
{
	// Rotate all Vertices by quaternion
	for (vector<vec3>::iterator iter = m_pVertices.begin();
		iter != m_pVertices.end();
		++iter)
	{
		(*iter) = (*iter) * qRotationQuatern;
	}
}

// Uniformally scales a mesh by a given float. Float should be represented as 1.0f == 100%
void Mesh::scale(float fScale)
{
	// Utilize GLM to generate a scale matrix
	mat4 m4ScaleMatrix = glm::scale(vec3(fScale));

	// Scale all Vertices by scale matrix.
	for (vector<vec3>::iterator iter = m_pVertices.begin();
		iter != m_pVertices.end();
		++iter)
	{
		(*iter) = vec4((*iter), 1.f) * m4ScaleMatrix;
	}
}