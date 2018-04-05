#include "Shader.h"

// Shader Compiler Enums
const GLenum gleShaderTypes[ Shader::eShader::MAX_PARTS ] =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER
};

// Default Constructor
Shader::Shader()
{
	for ( int i = 0; i < MAX_PARTS; ++i )
	{
		m_uiShader[ i ] = 0;
		m_uiShaderLocations[ i ] = "";
	}

	m_uiProgram		= 0;
	m_bInitialized	= false;
}

// Destructor
Shader::~Shader()
{
	// Delete Program and any created Shader
	glDeleteProgram(m_uiProgram);

	for ( int i = 0; i < MAX_PARTS; ++i )
		glDeleteShader( m_uiShader[ i ] );
}

// Set Shader Location
void Shader::storeShadrLoc( eShader shaderType, const string& sShaderLoc )
{
	if ( shaderType >= 0 && shaderType < MAX_PARTS )
		m_uiShaderLocations[ shaderType ] = sShaderLoc;
}

// Shader Initialization with Tesselation Shaders
bool Shader::initializeShader( )
{
	string sSources[ MAX_PARTS ];
	bool bError = false;

	// Don't initialize more than once.
	if (!m_bInitialized)
	{
		// Load from Source
		for ( int i = 0; i < MAX_PARTS; ++i )
			sSources[ i ] = LoadSource( i, &bError );
		
		// Ensure at least the Vertex and Fragment Shaders are loaded.
		if ( sSources[ VERTEX ].empty() || sSources[ FRAGMENT ].empty() )
			return false;

		// Compile Shaders
		for ( int i = 0; i < MAX_PARTS; ++i )
			m_uiShader[ i ] = CompileShader( gleShaderTypes[ i ], sSources[ i ], &bError );

		// Link Shaders
		m_uiProgram = LinkProgram( &bError );

		// Set that it's been initialized
		m_bInitialized = !CheckGLErrors() && !bError;
	}

	// Return result of initialization
	return m_bInitialized;
}

// reads a text file with the given name into a string
string Shader::LoadSource( int iShaderType, bool* bError)
{
	string source = "";

	// Some Error Checking
	if ( iShaderType == VERTEX || iShaderType == FRAGMENT )
		*bError = m_uiShaderLocations[ iShaderType ].empty();
	else if ( m_uiShaderLocations[ iShaderType ].empty() )
		return source;

	// No Errors Yet? Pull Shader Source.
	if ( !(*bError) )
	{
		ifstream input( m_uiShaderLocations[ iShaderType ] );
		if ( input )
		{
			copy( istreambuf_iterator<char>( input ),
				istreambuf_iterator<char>(),
				back_inserter( source ) );
			input.close();
		}
		else
		{
			cout << "ERROR: Could not load shader source from file "
				<< m_uiShaderLocations[ iShaderType ] << endl;
			*bError |= true;
		}
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint Shader::CompileShader(GLenum shaderType, const string &source, bool* bError)
{
	// Return Variable
	GLuint shaderObject = 0;

	if ( !source.empty() )
	{
		// allocate shader object name
		shaderObject = glCreateShader( shaderType );

		// try compiling the source as a shader of the given type
		const GLchar *source_ptr = source.c_str();
		glShaderSource( shaderObject, 1, &source_ptr, 0 );
		glCompileShader( shaderObject );

		// retrieve compile status
		GLint status;
		glGetShaderiv( shaderObject, GL_COMPILE_STATUS, &status );
		if ( status == GL_FALSE )
		{
			GLint length;
			glGetShaderiv( shaderObject, GL_INFO_LOG_LENGTH, &length );
			string info( length, ' ' );
			glGetShaderInfoLog( shaderObject, info.length(), &length, &info[ 0 ] );
			cout << "ERROR compiling shader:" << endl << endl;
			cout << source << endl;
			cout << info << endl;
			*bError |= true;
		}
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint Shader::LinkProgram( bool* bError )
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	for ( int i = 0; i < MAX_PARTS; ++i )
		if ( m_uiShader[ i ] )
			glAttachShader( programObject, m_uiShader[ i ] );

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
		*bError |= true;
	}

	return programObject;
}
