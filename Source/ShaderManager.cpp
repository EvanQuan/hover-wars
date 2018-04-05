#include "ShaderManager.h"

/////////////
// Defines //
/////////////

// Singleton Variable initialization
ShaderManager* ShaderManager::m_pInstance = nullptr;

// Public - Not a singleton
// Designed mainly to manage different shaders between assignments.  
ShaderManager::ShaderManager()
{
	m_bInitialized = false;

	// Set Edge Shader Locations

	// Silhouette Edge
	m_pShader[ eShaderType::SILH_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,   "Shaders/silhouette.vert" );
	m_pShader[ eShaderType::SILH_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/silhouette.frag" );
	m_pShader[ eShaderType::SILH_SHDR ].storeShadrLoc( Shader::eShader::GEOMETRY, "Shaders/silhouette.geo" );

	// Inside Edge
	m_pShader[ eShaderType::INSIDE_EDGE_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,	 "Shaders/insideEdge.vert" );
	m_pShader[ eShaderType::INSIDE_EDGE_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/insideEdge.frag" );
	m_pShader[ eShaderType::INSIDE_EDGE_SHDR ].storeShadrLoc( Shader::eShader::GEOMETRY, "Shaders/insideEdge.geo" );

	// Outside Edge
	m_pShader[ eShaderType::OUTSIDE_EDGE_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,	  "Shaders/outsideEdge.vert" );
	m_pShader[ eShaderType::OUTSIDE_EDGE_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/outsideEdge.frag" );
	m_pShader[ eShaderType::OUTSIDE_EDGE_SHDR ].storeShadrLoc( Shader::eShader::GEOMETRY, "Shaders/outsideEdge.geo" );

	// Inside Boundary Edge
	m_pShader[ eShaderType::INSIDE_BOUNDRY_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,	"Shaders/insideBoundEdge.vert" );
	m_pShader[ eShaderType::INSIDE_BOUNDRY_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/insideBoundEdge.frag" );
	m_pShader[ eShaderType::INSIDE_BOUNDRY_SHDR ].storeShadrLoc( Shader::eShader::GEOMETRY, "Shaders/insideBoundEdge.geo" );

	// Outside Boundary Edge
	m_pShader[ eShaderType::OUTSIDE_BOUNDRY_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,	 "Shaders/outsideBoundEdge.vert" );
	m_pShader[ eShaderType::OUTSIDE_BOUNDRY_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/outsideBoundEdge.frag" );
	m_pShader[ eShaderType::OUTSIDE_BOUNDRY_SHDR ].storeShadrLoc( Shader::eShader::GEOMETRY, "Shaders/outsideBoundEdge.geo" );

	m_pShader[ eShaderType::LIGHT_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,   "Shaders/light.vert" );
	m_pShader[ eShaderType::LIGHT_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/light.frag" );

	m_pShader[ eShaderType::MESH_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,   "Shaders/mesh.vert" );
	m_pShader[ eShaderType::MESH_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/mesh.frag" );

	m_pShader[ eShaderType::PLANE_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,   "Shaders/plane.vert" );
	m_pShader[ eShaderType::PLANE_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/plane.frag" );

	m_pShader[ eShaderType::WORLD_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,   "Shaders/world.vert" );
	m_pShader[ eShaderType::WORLD_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/world.frag" );

	// RC_Track Shaders
	m_pShader[ eShaderType::RC_TRACK_SHDR ].storeShadrLoc( Shader::eShader::VERTEX,	  "Shaders/rc_track.vert" );
	m_pShader[ eShaderType::RC_TRACK_SHDR ].storeShadrLoc( Shader::eShader::FRAGMENT, "Shaders/rc_track.frag" );

	m_pShader[eShaderType::BOID_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/boid.vert");
	m_pShader[eShaderType::BOID_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/boid.frag");
}

// Get the Singleton ShaderManager Object.  Initialize it if nullptr.
ShaderManager* ShaderManager::getInstance()
{
	if (nullptr == m_pInstance)
		m_pInstance = new ShaderManager();

	return m_pInstance;
}

// Destructor - Kill any shaders that we've been using.
ShaderManager::~ShaderManager()
{
	// unbind any shader programs
	glUseProgram(0);
}

/*******************************************************************\
 * Set up Shaders												   *
\*******************************************************************/

// Inializes shaders. 
bool ShaderManager::initializeShaders()
{
	// Initialize Shaders
	m_bInitialized = true;
	for ( int eIndex = LIGHT_SHDR; eIndex < MAX_SHDRS; eIndex++ )
		m_bInitialized &= m_pShader[eIndex].initializeShader( );

	// return False if not all Shaders Initialized Properly
	return m_bInitialized;
}


/*******************************************************************************\
* Shader manipulation														   *
\*******************************************************************************/

// Set Projection Matrix for all Shaders
void ShaderManager::setProjectionModelViewMatrix( const mat4* pProjMat, const mat4* pModelViewMat )
{
	for ( int i = 0; i < MAX_SHDRS; ++i )
	{
		setUnifromMatrix4x4( (eShaderType) i, "projection", pProjMat );
		setUnifromMatrix4x4( (eShaderType) i, "modelview", pModelViewMat );
	}
}

// Binds and creates a buffer on the GPU.  Sets the data into the buffer and returns the location of the buffer.
GLuint ShaderManager::genVertexBuffer(GLuint iVertArray, const void* pData, GLsizeiptr pSize, GLenum usage )
{
	GLuint iVertexBufferLoc;
	glBindVertexArray( iVertArray );
	glGenBuffers( 1, &iVertexBufferLoc );
	glBindBuffer( GL_ARRAY_BUFFER, iVertexBufferLoc );
	glBufferData( GL_ARRAY_BUFFER, pSize, pData, usage );

	glBindVertexArray( 0 );
	return iVertexBufferLoc;
}

// Enables an Attribute Pointer for a specified Vertex Array
void ShaderManager::setAttrib(GLuint iVertArray, GLuint iSpecifiedIndex, GLint iChunkSize, GLsizei iStride, const void *pOffset)
{
	glBindVertexArray(iVertArray);
	glVertexAttribPointer(iSpecifiedIndex, iChunkSize, GL_FLOAT, GL_FALSE, iStride, pOffset);
	glEnableVertexAttribArray(iSpecifiedIndex);

	glBindVertexArray(0);
}

// Binds and creates an Element Array Buffer on the GPU.  Sets the data into the buffer and returns the location.
GLuint ShaderManager::genIndicesBuffer( GLuint iVertArray, 
										const void* pData, GLsizeiptr pSize, GLenum usage )
{
	GLuint iIndicesBufferLoc;

	glBindVertexArray( iVertArray );

	glGenBuffers( 1, &iIndicesBufferLoc );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iIndicesBufferLoc );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, pSize, pData, usage );

	glBindVertexArray( 0 );

	return iIndicesBufferLoc;
}

// given a glm 4x4 Matrix, a specifed shader and a variablename, attempt to set the given matrix into that uniform variable.
void ShaderManager::setUnifromMatrix4x4( eShaderType eType, string sVarName, const mat4* pResultingMatrix )
{
	GLint iVariableLocation;
	GLint iProgram, iCurrProgram;
	
	if ( eType < eShaderType::MAX_SHDRS && eType >= 0 )
	{
		iProgram = getProgram( eType );

		glGetIntegerv( GL_CURRENT_PROGRAM, &iCurrProgram );
		glUseProgram( iProgram );
		iVariableLocation = glGetUniformLocation( iProgram, sVarName.c_str() );
		if ( ERR_CODE != iVariableLocation )
			glUniformMatrix4fv( iVariableLocation, 1, GL_FALSE, value_ptr( *pResultingMatrix ) );
		glUseProgram( iCurrProgram );

		#ifdef DEBUG
			CheckGLErrors();
		#endif // DEBUG
	}
}

// given a glm vec3 set it as the unifrom light position in the mesh shader
void ShaderManager::setUniformVec3( eShaderType eType, string sVarName, const glm::vec3* pResultingVector )
{
	GLint iVariableLocation;
	GLint iProgram, iCurrProgram;
	
	if ( eType < eShaderType::MAX_SHDRS && eType >= 0 )
	{
		iProgram = getProgram( eType );

		glGetIntegerv( GL_CURRENT_PROGRAM, &iCurrProgram );
		glUseProgram( iProgram );
		iVariableLocation = glGetUniformLocation( iProgram, sVarName.c_str() );
		if ( ERR_CODE != iVariableLocation )
			glUniform3fv( iVariableLocation, 1, glm::value_ptr( *pResultingVector ) );
		glUseProgram( iCurrProgram );

		#ifdef DEBUG
			CheckGLErrors();
		#endif // DEBUG
	}
}

// Sets a single uniform floating value in the specified shader to the given value.
void ShaderManager::setUniformFloat(eShaderType eType, string sVarName, float fVal)
{
	GLint iVariableLocation;
	GLint iProgram, iCurrProgram;

	if (eType < eShaderType::MAX_SHDRS && eType >= 0)
	{
		iProgram = getProgram(eType);

		glGetIntegerv( GL_CURRENT_PROGRAM, &iCurrProgram );
		glUseProgram(iProgram);
		iVariableLocation = glGetUniformLocation(iProgram, sVarName.c_str());
		if (ERR_CODE != iVariableLocation)
			glUniform1f(iVariableLocation, fVal);
		glUseProgram(iCurrProgram);
		
		#ifdef DEBUG
			CheckGLErrors();
		#endif // DEBUG
	}
}

// Sets a uniform integer value in the specified shader program to the given value.
void ShaderManager::setUniformInt( eShaderType eType, string sVarName, int iVal )
{
	GLint iVariableLocation;
	GLint iProgram, iCurrProgram;

	if ( eType < eShaderType::MAX_SHDRS && eType >= 0 )
	{
		iProgram = getProgram( eType );

		glGetIntegerv( GL_CURRENT_PROGRAM, &iCurrProgram );
		glUseProgram( iProgram );
		iVariableLocation = glGetUniformLocation( iProgram, sVarName.c_str() );
		if ( ERR_CODE != iVariableLocation )
			glUniform1i( iVariableLocation, iVal );
		glUseProgram( iCurrProgram );

	#ifdef DEBUG
		CheckGLErrors();
	#endif // DEBUG
	}
}

// Set a Uniform Boolean Value within a given Shader.
void ShaderManager::setUniformBool( eShaderType eType, string sVarName, bool bVal )
{
	GLint iVariableLocation;
	GLint iProgram, iCurrProgram;

	if ( eType < eShaderType::MAX_SHDRS && eType >= 0 )
	{
		iProgram = getProgram( eType );
		iVariableLocation = glGetUniformLocation( iProgram, sVarName.c_str() );

		if ( ERR_CODE != iVariableLocation )
		{
			glGetIntegerv( GL_CURRENT_PROGRAM, &iCurrProgram );
			glUseProgram( iProgram );
			glUniform1i( iVariableLocation, bVal );
			glUseProgram( iCurrProgram );
		}
	}
}

// Toggles a Boolean Value in the given shader.
void ShaderManager::toggleUniformBool( eShaderType eType, string sVarName )
{
	GLint iVariableLocation;
	GLint iProgram, iCurrProgram;
	GLint bCurrSetting;

	if ( eType < eShaderType::MAX_SHDRS && eType >= 0 )
	{
		iProgram = getProgram( eType );
		iVariableLocation = glGetUniformLocation( iProgram, sVarName.c_str() );

		if ( ERR_CODE != iVariableLocation )
		{
			glGetUniformiv( iProgram, iVariableLocation, &bCurrSetting );

			bCurrSetting = !bCurrSetting;

			glGetIntegerv( GL_CURRENT_PROGRAM, &iCurrProgram );
			glUseProgram( iProgram );
			glUniform1i( iVariableLocation, bCurrSetting );
			glUseProgram( iCurrProgram );
		}
	}
}