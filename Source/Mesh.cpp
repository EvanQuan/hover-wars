#include "Mesh.h"
#include <sstream>
#include "TextureManager.h"

/****************************\
 * Constants: For Materials *
\****************************/
const string DEFAULT_DIFFUSE_MAP = "textures/defaultTexture.jpg";
const vec4 DEFAULT_SPEC_COLOR = vec4(vec3(0.f), 1.0f);

/************************************\
 * Defines: For Sphere Construction *
\************************************/
#define SLICE_SIZE 5.f
#define MAX_THETA_DEGS 360.f
#define MAX_PHI_DEGS 180.f
#define MAX_THETA_CUTS (int)(MAX_THETA_DEGS / SLICE_SIZE)
#define MAX_PHI_CUTS (int)(MAX_PHI_DEGS / SLICE_SIZE)

/**********************************************\
 * Defines: For Billboard Buffer Manipulation *
\**********************************************/
#define BILLBOARD_STRIDE	(sizeof(vec3) + /*Vertex*/ sizeof(vec3) + /*Normal*/ sizeof(vec2) + /*UVStart*/ sizeof(vec2) + /*UVEnd*/ sizeof(vec2) /*Height/Width*/ )
#define VERTEX_OFFSET		0
#define NORMAL_OFFSET		sizeof(vec3)
#define UV_START_OFFSET		(sizeof(vec3) << 1)
#define UV_END_OFFSET		((sizeof(vec3) << 1) + sizeof(vec2))
#define DIMENSION_OFFSET	((sizeof(vec3) << 1) + (sizeof(vec2) << 1))

// Basic Constructor
Mesh::Mesh( const string &sManagerKey, bool bStaticMesh, const Material* pMaterial, manager_cookie )
{
	m_sManagerKey = sManagerKey;
	m_bStaticMesh = bStaticMesh;
	m_pShdrMngr = SHADER_MANAGER;
	glGenVertexArrays(1, &m_iVertexArray);

	loadMaterial(pMaterial);
}

// Delete any buffers that we initialized
Mesh::~Mesh()
{
	glDeleteBuffers( 1, &m_iVertexBuffer );
	glDeleteBuffers( 1, &m_iIndicesBuffer );
	glDeleteBuffers(1, &m_iInstancedBuffer);
	glDeleteBuffers(1, &m_iScaleBuffer);
	glDeleteVertexArrays( 1, &m_iVertexArray );
}

// Load the Mesh from a given file name
//  Result: Stores the mesh variables into a set of vertices
bool Mesh::genMesh( const string& sFileName, vec3 vPosition, float fScale )
{
	// Return Value
	bool bReturnValue = true;

	// Load Mesh
	if (sFileName.substr(sFileName.find_last_of(".") + 1) == "obj")
		bReturnValue = loadObj(sFileName);		

	// Store Mesh in GPU
	if (bReturnValue)
	{
		// Apply Scale before Translation
		mat4 m4Transformation = scale(vec3(fScale)) * mat4(1.0f);

		// Translation
		if (vec3(0.f) != vPosition)
			m4Transformation = translate(vPosition) * m4Transformation;

		// Store initial transformation
		m_m4ListOfInstances.push_back(m4Transformation);

		// Initialize VBOs
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
	float fHalfHeight = static_cast<float>(iHeight) * 0.5f;
	float fHalfWidth = static_cast<float>(iWidth) * 0.5f;
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

	// Generate Indices
	m_pIndices = { 0, 1, 2, 1, 2, 3 };

	// Translation Matrix
	mat4 m4TranslationMatrix = getRotationMat4ToNormal(&vNormal);
	
	// If translation is necessary, translate plane.
	if (vec3(0.f) != vPosition)
		m4TranslationMatrix = translate(vPosition) * m4TranslationMatrix;

	// Store Initial Transformation Matrix
	m_m4ListOfInstances.push_back(m4TranslationMatrix);

	// Load Mesh into GPU
	initalizeVBOs();
}

// Generates a Sphere Mesh
void Mesh::genSphere(float fRadius, vec3 vPosition)
{
	// Algorithm pulled from: https://goo.gl/k9Q4mh
	float const R = 1.f / static_cast<float>(MAX_THETA_CUTS - 1);
	float const S = 1.f / static_cast<float>(MAX_PHI_CUTS - 1);
	int r, s;

	// Calculate Sizes of vectors ahead of time to avoid resize calls during loop
	m_pVertices.resize(MAX_THETA_CUTS * MAX_PHI_CUTS);
	m_pNormals.resize(MAX_THETA_CUTS * MAX_PHI_CUTS);
	m_pUVs.resize(MAX_THETA_CUTS * MAX_PHI_CUTS);

	// Populate Iterators as loop progresses
	vector<vec3>::iterator v = m_pVertices.begin();
	vector<vec3>::iterator n = m_pNormals.begin();
	vector<vec2>::iterator t = m_pUVs.begin();

	// Loop through Sphere and generate UVs, Vertices and Normals.
	for (r = 0; r < MAX_THETA_CUTS; r++) for (s = 0; s < MAX_PHI_CUTS; s++) {
		float const y = sinf(-(PI / 2) + PI * r * R);
		float const x = cosf(2 * PI * s * S) * sinf(PI * r * R);
		float const z = sinf(2 * PI * s * S) * sinf(PI * r * R);

		// UVs are inverted.
		*t++ = vec2( (MAX_PHI_CUTS-s) * S, (MAX_THETA_CUTS-r) * R);

		// Scale Sphere to Radius.
		*v++ = vec3(x * fRadius, y * fRadius, z * fRadius);

		// Store normal in Local space.
		*n++ = vec3(x, y, z);
	}

	// Generate indices
	m_pIndices.resize(MAX_THETA_CUTS * MAX_PHI_CUTS * 4);
	vector<unsigned int>::iterator i = m_pIndices.begin();
	unsigned int iWrapAroundMask = m_pVertices.size();

	// Ensure index storage creates counter clockwise pattern for back-face culling.
	for (r = 0; r < MAX_THETA_CUTS; r++) for (s = 0; s < MAX_PHI_CUTS; s++) {
		*i++ = (r * MAX_PHI_CUTS + s) % iWrapAroundMask;
		*i++ = ((r + 1) * MAX_PHI_CUTS + s) % iWrapAroundMask;
		*i++ = (r * MAX_PHI_CUTS + (s + 1)) % iWrapAroundMask;
		*i++ = ((r + 1) * MAX_PHI_CUTS + (s + 1)) % iWrapAroundMask;
	}

	// Initial Transformation Matrix
	mat4 m4InitialTransformation = mat4(1.0f);

	// Translate to Position if Sphere is a Static Mesh.
	if (vec3(0.f) != vPosition)
		m4InitialTransformation = translate(vPosition) * m4InitialTransformation;

	// Store Initial Transformation Matrix
	m_m4ListOfInstances.push_back(m4InitialTransformation);

	// Store Mesh in GPU
	initalizeVBOs();
}

// Generates a Cube object given a Height, Width and Depth dimension as well as a position.
void Mesh::genCube(int iHeight, int iWidth, int iDepth, vec3 vPosition)
{
	// Get half sizes of dimensions to set vertices wrt to origin.
	float iHalfHeight	= static_cast<float>(iHeight) * 0.5f;
	float iHalfWidth	= static_cast<float>(iWidth) * 0.5f;
	float iHalfDepth	= static_cast<float>(iDepth) * 0.5f;

	// Reserve Sizes ahead of time to speed up computation
	m_pVertices.reserve(24);
	m_pNormals.reserve(24);
	m_pUVs.reserve(24);
	m_pIndices.reserve(36);

	vec3 vIndexes[8] = {
		vec3(-iHalfWidth, iHalfHeight, iHalfDepth),
		vec3(iHalfWidth, iHalfHeight, iHalfDepth),
		vec3(iHalfWidth, -iHalfHeight, iHalfDepth),
		vec3(-iHalfWidth, -iHalfHeight, iHalfDepth),
		vec3(-iHalfWidth, iHalfHeight, -iHalfDepth),
		vec3(iHalfWidth, iHalfHeight, -iHalfDepth),
		vec3(-iHalfWidth, -iHalfHeight, -iHalfDepth),
		vec3(iHalfWidth, -iHalfHeight, -iHalfDepth)
	};

	vec3 vNormals[8] = {
		normalize(vIndexes[0] - vec3(0.f)),
		normalize(vIndexes[1] - vec3(0.f)),
		normalize(vIndexes[2] - vec3(0.f)),
		normalize(vIndexes[3] - vec3(0.f)),
		normalize(vIndexes[4] - vec3(0.f)),
		normalize(vIndexes[5] - vec3(0.f)),
		normalize(vIndexes[6] - vec3(0.f)),
		normalize(vIndexes[7] - vec3(0.f))
	};

	// Generate 24 vertices for all corners per face of the cube.
	// Face 1 - Front
	m_pVertices.push_back(vIndexes[0]);		// Index 0
	m_pVertices.push_back(vIndexes[1]);		// Index 1
	m_pVertices.push_back(vIndexes[2]);		// Index 2
	m_pVertices.push_back(vIndexes[3]);		// Index 3
	m_pNormals.push_back(vNormals[0]);
	m_pNormals.push_back(vNormals[1]);
	m_pNormals.push_back(vNormals[2]);
	m_pNormals.push_back(vNormals[3]);
	m_pUVs.push_back(vec2(0.0f));
	m_pUVs.push_back(vec2(1.0f, 0.f));
	m_pUVs.push_back(vec2(0.0f, 1.0f));
	m_pUVs.push_back(vec2(1.f));

	// Face 2 - Back
	m_pVertices.push_back(vIndexes[4]);		// Index 4
	m_pVertices.push_back(vIndexes[5]);		// Index 5
	m_pVertices.push_back(vIndexes[6]);		// Index 6
	m_pVertices.push_back(vIndexes[7]);		// Index 7
	m_pNormals.push_back(vNormals[4]);
	m_pNormals.push_back(vNormals[5]);
	m_pNormals.push_back(vNormals[6]);
	m_pNormals.push_back(vNormals[7]);
	m_pUVs.push_back(vec2(1.0f, 0.f));
	m_pUVs.push_back(vec2(0.0f));
	m_pUVs.push_back(vec2(1.f));
	m_pUVs.push_back(vec2(0.0f, 1.0f));

	// Face 3 - Left
	m_pVertices.push_back(vIndexes[0]);		// Index 1
	m_pVertices.push_back(vIndexes[3]);		// Index 2
	m_pVertices.push_back(vIndexes[4]);		// Index 5
	m_pVertices.push_back(vIndexes[6]);		// Index 7
	m_pNormals.push_back(vNormals[0]);
	m_pNormals.push_back(vNormals[3]);
	m_pNormals.push_back(vNormals[4]);
	m_pNormals.push_back(vNormals[6]);
	m_pUVs.push_back(vec2(0.0f));
	m_pUVs.push_back(vec2(0.0f, 1.0f));
	m_pUVs.push_back(vec2(1.0f, 0.f));
	m_pUVs.push_back(vec2(1.f));

	// Face 4 - Right
	m_pVertices.push_back(vIndexes[1]);		// Index 0
	m_pVertices.push_back(vIndexes[2]);		// Index 3
	m_pVertices.push_back(vIndexes[5]);		// Index 4
	m_pVertices.push_back(vIndexes[7]);		// Index 6
	m_pNormals.push_back(vNormals[1]);
	m_pNormals.push_back(vNormals[2]);
	m_pNormals.push_back(vNormals[5]);
	m_pNormals.push_back(vNormals[7]);
	m_pUVs.push_back(vec2(1.0f, 0.f));
	m_pUVs.push_back(vec2(1.f));
	m_pUVs.push_back(vec2(0.0f));
	m_pUVs.push_back(vec2(0.0f, 1.0f));

	// Face 5 - Bottom
	m_pVertices.push_back(vIndexes[2]);		// Index 2
	m_pVertices.push_back(vIndexes[3]);		// Index 3
	m_pVertices.push_back(vIndexes[6]);		// Index 6
	m_pVertices.push_back(vIndexes[7]);		// Index 7
	m_pNormals.push_back(vNormals[2]);
	m_pNormals.push_back(vNormals[3]);
	m_pNormals.push_back(vNormals[6]);
	m_pNormals.push_back(vNormals[7]);
	m_pUVs.push_back(vec2(1.0f, 0.f));
	m_pUVs.push_back(vec2(0.0f));
	m_pUVs.push_back(vec2(0.0f, 1.0f));
	m_pUVs.push_back(vec2(1.f));

	// Face 6 - Top
	m_pVertices.push_back(vIndexes[0]);		// Index 0
	m_pVertices.push_back(vIndexes[1]);		// Index 1
	m_pVertices.push_back(vIndexes[4]);		// Index 4
	m_pVertices.push_back(vIndexes[5]);		// Index 5
	m_pNormals.push_back(vNormals[0]);
	m_pNormals.push_back(vNormals[1]);
	m_pNormals.push_back(vNormals[4]);
	m_pNormals.push_back(vNormals[5]);
	m_pUVs.push_back(vec2(1.0f, 0.f));
	m_pUVs.push_back(vec2(1.f));
	m_pUVs.push_back(vec2(0.0f));
	m_pUVs.push_back(vec2(0.0f, 1.0f));

	m_pIndices = {
		0, 2, 1, 0, 3, 2,
		5, 6, 4, 5, 7, 6,
		10, 9, 8, 10, 11, 9,
		12, 15, 14, 12, 13, 15,
		17, 19, 16, 17, 18, 19,
		22, 21, 23, 22, 20, 21
	};

	// Initial Transformation Matrix
	mat4 m4InitialTransformationMatrix = mat4(1.0f);

	// Translation
	if (vec3(0.0f) != vPosition)
		m4InitialTransformationMatrix = translate(vPosition);

	// Store Initial Transformation Matrix in Transformation vector
	m_m4ListOfInstances.push_back(m4InitialTransformationMatrix);

	initalizeVBOs();
}

// This will store the position as a vertex with a given normal as the direction to draw the billboard in.
//	the height and width will be set up and stored in the VBO as well.
//	The billboard functionality of a Mesh will set up the VBOs in a very different manner:
//		- For each data entry:
//			* Vertex (vec3)
//			* Normal (vec3)
//			* UVStart (vec2)
//			* UVEnd	(vec2)
//			* Height (unsigned int)
//			* Width (unsigned int)
void Mesh::genBillboard(const vec3* vPosition, const vec3* vNormal, const vec2* vUVStart, const vec2* vUVEnd, int iHeight, int iWidth)
{
	// Generate VBO
	m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, nullptr, 0, GL_STATIC_DRAW);

	// Set up VBO Attributes
	m_pShdrMngr->setAttrib(m_iVertexArray, 0, 3, BILLBOARD_STRIDE, (void*)VERTEX_OFFSET);	/*Vertex*/
	m_pShdrMngr->setAttrib(m_iVertexArray, 1, 3, BILLBOARD_STRIDE, (void*)NORMAL_OFFSET);	/*Normal*/
	m_pShdrMngr->setAttrib(m_iVertexArray, 2, 2, BILLBOARD_STRIDE, (void*)UV_START_OFFSET);	/*UVStart*/
	m_pShdrMngr->setAttrib(m_iVertexArray, 3, 2, BILLBOARD_STRIDE, (void*)UV_END_OFFSET);	/*UVEnd*/
	m_pShdrMngr->setAttrib(m_iVertexArray, 4, 2, BILLBOARD_STRIDE, (void*)DIMENSION_OFFSET);/*Height/Width*/

	// Add first Billboard
	addBillboard(vPosition, vNormal, vUVStart, vUVEnd, iHeight, iWidth);
}

/****************************************************************************************\
 * Billboard Usage																		*
\****************************************************************************************/

// Adds a Billboard object to the Mesh.
unsigned int Mesh::addBillboard(const vec3* vPosition, const vec3* vNormal, const vec2* vUVStart, const vec2* vUVEnd, int iHeight, int iWidth)
{
	// Create new Billboard
	sBillboardInfo sNewBillboard;
	sNewBillboard.vPosition = *vPosition;
	sNewBillboard.vNormal = *vNormal;
	sNewBillboard.vUVStart = *vUVStart;
	sNewBillboard.vUVEnd = *vUVEnd;
	sNewBillboard.vDimensions = vec2(static_cast<float>(iHeight), static_cast<float>(iWidth));

	// add to main list
	m_pBillboardList.push_back(sNewBillboard);

	// Reload Data in GPU.
	glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_pBillboardList.size() * sizeof(sBillboardInfo), m_pBillboardList.data(), GL_DYNAMIC_DRAW);

	// Return the index for this billboard for later reference.
	return m_pBillboardList.size() - 1;
}

// Updates the UVs of a specified billboard, used for sprite animation.
void Mesh::updateBillboardUVs(unsigned int iIndex, const vec2* vNewUVStart, const vec2* vNewUVEnd)
{
	// Update in Billboard List
	m_pBillboardList[iIndex].vUVStart = *vNewUVStart;
	m_pBillboardList[iIndex].vUVEnd = *vNewUVEnd;
	vec2 pDataArray[] = { *vNewUVStart, *vNewUVEnd };

	// Update in VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, ((iIndex * BILLBOARD_STRIDE) + UV_START_OFFSET), sizeof(vec2) << 1, pDataArray);
}

// Clear VBO data and Clear the Billboard data internally.
void Mesh::flushBillboards()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 0, (void*)0, GL_DYNAMIC_DRAW);

	m_pBillboardList.clear();
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
	m_pNormals.push_back(normalize( pPoint ) );
}

// Initialize the GPU with Mesh Data and tell it how to read it.
void Mesh::initalizeVBOs()
{
	// This function shouldn't be called without passing in Vertex Data.
	assert(!m_pVertices.empty());

	// We can use just one VBO since it's assumed that the geometry won't update as often.
	//	Therefore, we'll combine all data into one VBO and set openGL to address the data accordingly.
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
	{
		m_pShdrMngr->setAttrib(m_iVertexArray, 1, 3, iStride, (void*)sizeof(vec3));
	}
	// UVs
	if (bHaveUVs) // Specified index could be 1 or 2 and Start location is Stride - sizeof(vec2) depending on if Normals exist. 
	{
		m_pShdrMngr->setAttrib(m_iVertexArray, 2, 2, iStride, (void*)(iStride - sizeof(vec2)));
	}

	// Initialize Instance Buffer
	setupInstanceBuffer(3);

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

void Mesh::setupInstanceBuffer(GLuint iStartSpecifiedIndex)
{
	// Set up Instanced Buffer for Instance Rendering
	m_iInstancedBuffer = SHADER_MANAGER->genVertexBuffer(
		m_iVertexArray, (void*)m_m4ListOfInstances.data(),
		sizeof(mat4) * m_m4ListOfInstances.size(), GL_DYNAMIC_DRAW);

	// Instance Rendering Attributes
	//	Set up openGL for referencing the InstancedBuffer as a Mat4
	// column 0
	glBindBuffer(GL_ARRAY_BUFFER, m_iInstancedBuffer);
	SHADER_MANAGER->setAttrib(
		m_iVertexArray, iStartSpecifiedIndex, 4, sizeof(vec4) * 4, (void*)0);
	// column 1
	SHADER_MANAGER->setAttrib(
		m_iVertexArray, iStartSpecifiedIndex + 1, 4, sizeof(vec4) * 4, (void*)sizeof(vec4));
	// column 2
	SHADER_MANAGER->setAttrib(
		m_iVertexArray, iStartSpecifiedIndex + 2, 4, sizeof(vec4) * 4, (void*)(2 * sizeof(vec4)));
	// column 3
	SHADER_MANAGER->setAttrib(
		m_iVertexArray, iStartSpecifiedIndex + 3, 4, sizeof(vec4) * 4, (void*)(3 * sizeof(vec4)));

	glBindVertexArray(m_iVertexArray);
	glVertexAttribDivisor(iStartSpecifiedIndex, 1);
	glVertexAttribDivisor(iStartSpecifiedIndex + 1, 1);
	glVertexAttribDivisor(iStartSpecifiedIndex + 2, 1);
	glVertexAttribDivisor(iStartSpecifiedIndex + 3, 1);
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

// Loads instance Data -> an array of transformation Matrices for updating the position of where to draw the mesh.
void Mesh::loadInstanceData(const void* pData, unsigned int iSize)
{
	if (nullptr != pData)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_iInstancedBuffer);
		glBufferData(GL_ARRAY_BUFFER, iSize * sizeof(mat4), pData, GL_DYNAMIC_DRAW);
	}
}

// Taking in a new Position a Rotation Quaternion and a Scale, add a new transformation matrix to the internal list and updates the VBO.
void Mesh::addInstance(const vec3* vPosition, const vec3* vNormal, float fScale)
{
	// Order as Scale -> Rotation -> Translation
	mat4 m4NewTransform = scale(vec3(fScale));
	m4NewTransform = getRotationMat4ToNormal(vNormal) * m4NewTransform;
	m4NewTransform = translate(*vPosition) * m4NewTransform;

	// Utilize Overloaded Function for functionality.
	addInstance(&m4NewTransform);
}

// Take in a new Transformation Matrix, update the internal transform list as well as the VBO
//	Dynamic Meshes will replace their old transformation and Static Meshes will add a transformation.
void Mesh::addInstance(const mat4* m4Transform)
{
	// If Dynamic, clear previous position.
	if (!m_bStaticMesh)
		m_m4ListOfInstances.clear();

	// Add new Transformation
	m_m4ListOfInstances.push_back(*m4Transform);

	// Load VBO with new Data.
	loadInstanceData(m_m4ListOfInstances.data(), m_m4ListOfInstances.size());
}

// Returns a Rotation Matrix to rotate an object from a World Coordinate System to a Local
//	coordinate system with a given y-axis normal.
mat4 Mesh::getRotationMat4ToNormal(const vec3* vNormal)
{
	// Initial Translation Matrix
	mat4 m4ReturnMatrix = mat4(1.0f);
	vec3 vYAxis(0.0f, 1.0f, 0.0f);

	float d = dot(*vNormal, vYAxis);
	if (d < 1.f) // If d >= 1.f, Vectors are the same.
	{
		// create Rotation quaternion.
		vec3 vCross = cross(*vNormal, vYAxis);
		quat q = angleAxis(acos(d), vCross);
		normalize(q);

		// Create Rotation Matrix from Quaternion
		m4ReturnMatrix = toMat4(q) * m4ReturnMatrix;
	}

	// Return Rotation Matrix
	return m4ReturnMatrix;
}

/************************************************************************************\
 * Texture Functionality															*
\************************************************************************************/

// Function to Bind the Mesh Material to the Shader for Rendering
//	To be called before the render function
void Mesh::bindTextures(ShaderManager::eShaderType eShaderType) const
{
	// Bind the Diffuse and Specular Maps
	m_sRenderMaterial.m_pDiffuseMap->bindTexture(eShaderType, "sMaterial.vDiffuse");
	m_sRenderMaterial.m_pSpecularMap->bindTexture(eShaderType, "sMaterial.vSpecular");

	// Set the Material's Shininess in the Material Uniform in the shader.
	m_pShdrMngr->setUniformFloat(eShaderType, "sMaterial.fShininess", m_sRenderMaterial.fShininess);
}

// Funtion to unbind textures. To be called after a render call on this mesh.
void Mesh::unbindTextures() const
{
	m_sRenderMaterial.m_pDiffuseMap->unbindTexture();
	m_sRenderMaterial.m_pSpecularMap->unbindTexture();
}

// Function to Load a given material to the internal Render Material struct.
//	This defines a specular and diffuse map as well as a shininess factor
//	for this mesh that is used for rendering.
void Mesh::loadMaterial(const Material* pMaterial)
{
	if (nullptr != pMaterial)
	{
		// Load Diffuse Texture if applicable
		if ("" != pMaterial->sDiffuseMap)
			m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->loadTexture(pMaterial->sDiffuseMap);
		else if (vec4(0.0f) != pMaterial->vOptionalDiffuseColor)	// No diffuse Texture applicable? then check if there's a specified diffuse color for the material.
			m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->genTexture(&pMaterial->vOptionalDiffuseColor);

		// Load Texture if applicable
		if ("" != pMaterial->sOptionalSpecMap)
			m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->loadTexture(pMaterial->sOptionalSpecMap);
		else	// "" as Spec Map Location? just generate a texture from whatever the Spec Shade is.
			m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->genTexture(&pMaterial->vOptionalSpecShade);

		// Store Shininess
		m_sRenderMaterial.fShininess = pMaterial->fShininess;
	}

	// Set some defaults if no Maps were specified.
	if (nullptr == m_sRenderMaterial.m_pDiffuseMap)
		m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->loadTexture(DEFAULT_DIFFUSE_MAP);
	if (nullptr == m_sRenderMaterial.m_pSpecularMap)
		m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->genTexture(&DEFAULT_SPEC_COLOR);
}

