#include "Mesh.h"
#include <sstream>

/************************************\
 * Defines: For Sphere Construction *
\************************************/
#define SLICE_SIZE 15.f
#define MAX_THETA_DEGS 360.f
#define MAX_PHI_DEGS 180.f
#define MAX_THETA_CUTS (int)(MAX_THETA_DEGS / SLICE_SIZE)
#define MAX_PHI_CUTS (int)(MAX_PHI_DEGS / SLICE_SIZE)

// Basic Constructor
Mesh::Mesh( const string &sManagerKey, bool bStaticMesh, manager_cookie )
{
	m_sManagerKey = sManagerKey;
	m_bStaticMesh = bStaticMesh;
	m_m4DefaultInstance =  mat4(1.0f);
	m_iNumInstances = 1;
	glGenVertexArrays(1, &m_iVertexArray);
}

// Delete any buffers that we initialized
Mesh::~Mesh()
{
	glDeleteBuffers( 1, &m_iVertexBuffer );
	glDeleteBuffers( 1, &m_iIndicesBuffer );
	glDeleteBuffers(1, &m_iInstancedBuffer);
	glDeleteVertexArrays( 1, &m_iVertexArray );
}

// Load the Mesh from a given file name
//  Result: Stores the mesh variables into a set of vertices
bool Mesh::genMesh( const string& sFileName, vec3 vPosition )
{
	// Return Value
	bool bReturnValue = true;

	// Load Mesh
	if (sFileName.substr(sFileName.find_last_of(".") + 1) == "obj")
		bReturnValue = loadObj(sFileName);		

	// Store Mesh in GPU
	if (bReturnValue)
	{
		if (m_bStaticMesh)
		{
			if (vec3(0.f) != vPosition)
				Mesh::translate(vPosition);
		}
		initalizeVBOs();
	}

	// Return result
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
	vNormal = normalize(vNormal); // Normalize Normal Vector
	m_pVertices.push_back(vec3(-fHalfWidth, 0.f, -fHalfHeight));
	m_pVertices.push_back(vec3(-fHalfWidth, 0.f, fHalfHeight));
	m_pVertices.push_back(vec3(fHalfWidth, 0.f, -fHalfHeight));
	m_pVertices.push_back(vec3(fHalfWidth, 0.f, fHalfHeight));

	// Generate Normals for each Vertex.
	m_pNormals.insert(m_pNormals.begin(), 4, vNormal);

	// Generate UVs for Plane
	m_pUVs.push_back(vec2(0.0, 0.0));
	m_pUVs.push_back(vec2(0.0, 1.0));
	m_pUVs.push_back(vec2(1.0, 0.0));
	m_pUVs.push_back(vec2(1.0, 1.0));

	float d = dot(vNormal, vec3(0.f, 1.f, 0.f) );
	if (d < 1.f) // If d >= 1.f, Vectors are the same.
	{
		// create Rotation quaternion to rotate plane.
		vec3 vCross = cross(vNormal, vec3(0.f, 1.f, 0.f));// *invs;
		quat q = angleAxis(acos(d), vCross);
		normalize(q);
	
		// Rotate Plane
		Mesh::rotate(q);
	}

	// If translation is necessary, translate plane.
	if (vec3(0.f) != vPosition)
		Mesh::translate(vPosition);

	// Load Mesh into GPU
	initalizeVBOs();
}

// Generates a Sphere Mesh
void Mesh::genSphere(float fRadius, vec3 vPosition)
{
	// Generate Vertices and Normals for the Mesh.
	float fCurrParallel, fNextParallel, fCurrMeridian;

	// Slice up Sphere PHI = vertical
	for (unsigned int p = 0; p < MAX_PHI_CUTS; ++p)
	{
		// Compute radial position of current parallel and next parallel
		fCurrParallel = PI * p / MAX_PHI_CUTS;
		fNextParallel = PI * (p + 1) / MAX_PHI_CUTS;

		// Horizontal Slicing, add Cartesian Points.
		for (unsigned int m = 0; m < MAX_THETA_CUTS; ++m)
		{
			fCurrMeridian = (2.f * PI * m) / MAX_THETA_CUTS;
			addCarteseanPoint(fCurrParallel, fCurrMeridian, fRadius);
			addCarteseanPoint(fNextParallel, fCurrMeridian, fRadius);
		}
	}

	// Compute UVs
	vec2 pUV;
	vec3 pToOrigin;

	for (vector<vec3>::iterator pIter = m_pVertices.begin();
		pIter != m_pVertices.end();
		++pIter)
	{
		// Inverse Normal to get a vector to the Origin
		pToOrigin = normalize( vec3(0.f) - (*pIter));

		// Compute UV using trig
		pUV.x = 0.5f + (atan2(pToOrigin.z, pToOrigin.x) / 2.f * PI);
		pUV.y = 0.5f - asin(pToOrigin.y) / PI;

		// Store UV
		m_pUVs.push_back(pUV);
	}

	// Translate to Position
	if (vec3(0.f) != vPosition)
		Mesh::translate(vPosition);

	// Store Mesh in GPU
	initalizeVBOs();
}

// Generates a Caresean point and normal for that point based on a given spherical coord
void Mesh::addCarteseanPoint(float fPhi, float fTheta, float fRadius)
{
	// Local Variables
	float fPhi_Rads = fPhi;
	float fTheta_Rads = fTheta;
	vec3 pPoint;

	// Generate the Cartesian Point
	pPoint.x = fRadius * sin(fPhi_Rads);
	pPoint.y = pPoint.x * sin(fTheta_Rads);
	pPoint.x *= cos(fTheta_Rads);
	pPoint.z = fRadius * cos(fPhi_Rads);

	// Store Vertex and Normal
	m_pVertices.push_back(pPoint);
	m_pNormals.push_back(normalize( pPoint - vec3(0.f)) );
}

void Mesh::initalizeVBOs()
{
	// This function shouldn't be called without passing in Vertex Data.
	assert(!m_pVertices.empty());

	// For Static objects, we can use just one VBO since it won't be updated as often.
	//	Therefore, we'll combine all data into one VBO and set openGL to address the data accordingly.
	if (m_bStaticMesh)
	{
		// Create a Vector to hold the combined data.
		vector<float> vVNdata;
		vVNdata.reserve(m_pVertices.size() * 3 +
			m_pNormals.size() * 3 +
			(m_pUVs.size() << 1));

		// Boolean values to determine if additional data exists.
		bool bHaveNormals = !m_pNormals.empty();
		bool bHaveUVs = !m_pUVs.empty();

		// Calculate Stride for setting up Attributes
		GLsizei iStride = sizeof(vec3) +
			(bHaveNormals ? sizeof(vec3) : 0) +
			(bHaveUVs ? sizeof(vec2) : 0);

		// Loop through the received input and set up the array of data for the VBO
		for (unsigned int i = 0; i < m_pVertices.size(); ++i)
		{
			// Vertex Data
			vVNdata.push_back(m_pVertices[i].x);
			vVNdata.push_back(m_pVertices[i].y);
			vVNdata.push_back(m_pVertices[i].z);

			// Normal Data
			if (bHaveNormals)
			{
				vVNdata.push_back(m_pNormals[i].x);
				vVNdata.push_back(m_pNormals[i].y);
				vVNdata.push_back(m_pNormals[i].z);
			}

			// UV Data
			if (bHaveUVs)
			{
				vVNdata.push_back(m_pUVs[i].x);
				vVNdata.push_back(m_pUVs[i].y);
			}
		}

		// Generate VBO
		m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, vVNdata.data(), vVNdata.size() * sizeof(float), GL_STATIC_DRAW);

		// Set-up Attributes
		// Vertices
		m_pShdrMngr->setAttrib(m_iVertexArray, 0, 3, iStride, (void*)0);
		// Normals
		if (bHaveNormals)
			m_pShdrMngr->setAttrib(m_iVertexArray, 1, 3, iStride, (void*)sizeof(vec3));
		// UVs
		if (bHaveUVs) // Specified index could be 1 or 2 and Start location is Stride - sizeof(vec2) depending on if Normals exist. 
			m_pShdrMngr->setAttrib(m_iVertexArray, 2, 2, iStride, (void*)(iStride - sizeof(vec2)));

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
	// HACK:  Will be removed later.
	vector<float> vVNdata;

	for (unsigned int i = 0; i < m_pVertices.size(); ++i)
	{
		// Vertex Data
		vVNdata.push_back(m_pVertices[i].x);
		vVNdata.push_back(m_pVertices[i].y);
		vVNdata.push_back(m_pVertices[i].z);

		// Normal Data
		if (!m_pNormals.empty())
		{
			vVNdata.push_back(m_pNormals[i].x);
			vVNdata.push_back(m_pNormals[i].y);
			vVNdata.push_back(m_pNormals[i].z);
		}
	}
	// Generate Buffer
	m_iVertexBuffer = SHADER_MANAGER->genVertexBuffer(
		m_iVertexArray,
		vVNdata.data(),
		vVNdata.size() * sizeof(float),
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
		(*iter) = vec3(qRotationQuatern * vec4((*iter), 1.f));
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