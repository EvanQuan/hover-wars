#include "Anim_Track.h"
#include "ShaderManager.h"
#include <sstream>

/***********\
 * DEFINES *
\***********/
#define SUBDIVISION_COUNT 4
#define DELTA_ST 0.01f
#define DELTA_T 0.009f
#define MIN_VELOCITY 0.1f
#define TRACK_WIDTH 0.05f
#define DECEL_THRESHOLD(fD) (fD * 0.9f)
#define GRAVITY 9.81f
#define POSITION_OFFSET 0.025f
#define LEFT_TRACK m_vTrackFrames[0]
#define RIGHT_TRACK m_vTrackFrames[1]

// Default Constructor
Anim_Track::Anim_Track( long lID, const string* sContourFile, const string* pMeshLoc,
						const string* pTexLoc, bool bOpen )
{
	// Store generation values
	m_lID = lID;
	m_sContourFile = *sContourFile;
	m_sMeshFile = *pMeshLoc;
	m_sTextureFile = *pTexLoc;
	m_bOpenCurve = bOpen;

	// Initialize Animation Track
	loadAnimTrack( m_sContourFile );
	initializeTrack();
}

// Copy Constructor Overload
Anim_Track::Anim_Track( const Anim_Track& pRHS )
{
	// Utilize operator= overload.
	(*this) = pRHS;
}

// Assignment operator overload.
Anim_Track& Anim_Track::operator=( const Anim_Track& pRHS )
{
	// Copy over initialization parameters
	this->m_lID = pRHS.m_lID;
	this->m_sContourFile = pRHS.m_sContourFile;
	this->m_sMeshFile = pRHS.m_sMeshFile;
	this->m_sTextureFile = pRHS.m_sTextureFile;
	this->m_bOpenCurve = pRHS.m_bOpenCurve;
	this->m_vKeyFrames = pRHS.m_vKeyFrames;
	this->m_vKeyFrameTable = pRHS.m_vKeyFrameTable;
	this->m_fCurveLength = pRHS.m_fCurveLength;
	this->m_fDistToFreeFall = pRHS.m_fDistToFreeFall;
	this->m_fMaxHeight = pRHS.m_fMaxHeight;
	this->m_fMinHeight = pRHS.m_fMinHeight;
	this->m_vDecelStartPosition = pRHS.m_vDecelStartPosition;

	for ( int i = 0; i < 2; ++i )
		this->m_vTrackFrames[ i ] = pRHS.m_vTrackFrames[ i ];

	this->m_vTracks = pRHS.m_vTracks;


	// Initialize Track based on same input files.
	initializeTrack();

	return (*this);
}

void Anim_Track::initializeTrack()
{
	assert( m_vKeyFrameTable.size() != 0 );
	m_fCurrDist = 0.f;
	m_fCurrHeight = m_vKeyFrameTable.front().vPosition.y;
	m_eCurrentState = LIFTING_STATE;

	// Create Vertex Array for Rendering Track
	glGenVertexArrays( 1, &m_iVertexArray );

	// Load a Mesh if mesh is specified.
	if ( !m_sMeshFile.empty() )
	{
		m_pMesh = MESH_MANAGER->loadMeshFromFile( m_sMeshFile );

		if ( nullptr != m_pMesh )
			m_pMesh->initMesh();
	}
	else m_pMesh = nullptr;

	if ( !m_sTextureFile.empty() )
		m_pTexture = TextureManager::getInstance()->loadTexture( m_sTextureFile, m_lID );
	else m_pTexture = nullptr;

	// Generate Vertex buffer for curve.
	m_iVertexBuffer = SHADER_MANAGER->genVertexBuffer( m_iVertexArray, m_vTracks.data(), 
																	 m_vTracks.size() * sizeof( vec3 ), GL_DYNAMIC_DRAW );
	SHADER_MANAGER->setAttrib(m_iVertexArray, 0, 3, 0, nullptr);
}

// Destructor
Anim_Track::~Anim_Track()
{
	if (nullptr != m_pMesh)
		m_pMesh = nullptr;

	if ( nullptr != m_pTexture )
		TextureManager::getInstance()->unloadTexture( m_pTexture->getFileName(), m_lID );

	glDeleteBuffers( 1, &m_iVertexBuffer );
	glDeleteVertexArrays( 1, &m_iVertexArray );
}

// render
// Renders the Animation track.  Can be turned on or off depending on whether it is necessary to render the track.
// TODO: want to extend to be able to repeat a model along the track such as a rollercoaster track.
void Anim_Track::draw( )
{
	// Get current bindings as a restore point
	GLint iCurrProgBinding = 0, iCurrVABinding = 0;
	glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &iCurrVABinding );
	glGetIntegerv( GL_CURRENT_PROGRAM, &iCurrProgBinding );

	glBindVertexArray( m_iVertexArray );


	// Upload new Indices
#ifdef DEBUG	// For Debugging
	vec3 vColor( 0.5 );
	mat4 vCurrFrenetFrame = getFreNetFrames();
	glPointSize( 10.f );
	glUseProgram( SHADER_MANAGER->getProgram( ShaderManager::eShaderType::WORLD_SHDR ) );
	vector< vec3 > vTemps = { vec3( 0.0f ), getPosition() };
	glBindBuffer( GL_ARRAY_BUFFER, m_iVertexBuffer );
	//glBufferData( GL_ARRAY_BUFFER, vTemps.size() * sizeof( vec3 ), vTemps.data(), GL_DYNAMIC_DRAW );
	//glDrawArrays( GL_POINTS, 0, 2 );
	//glDrawArrays( GL_LINES, 0, 2 );

	vTemps.clear();
	vTemps.push_back( vec3( vCurrFrenetFrame[ 3 ] ) );
	vTemps.push_back( vec3( vCurrFrenetFrame[ 3 ] ) + getCentripetalAcce( m_fCurrDist ) );
	glBufferData( GL_ARRAY_BUFFER, vTemps.size() * sizeof( vec3 ), vTemps.data(), GL_DYNAMIC_DRAW );
	SHADER_MANAGER->setUniformVec3( ShaderManager::eShaderType::WORLD_SHDR, "vColor", &vColor );
	//glDrawArrays( GL_POINTS, 0, vTemps.size() );
	//glDrawArrays( GL_LINES, 0, vTemps.size() );

	// Prev Pos
	//vTemps.clear();
	//vTemps.push_back( vec3( m_m4CurrentFrenetFrame[ 3 ] ) );
	//vTemps.push_back( vec3( getPosition( m_fCurrDist - (100.f * DELTA_S) ) ) );
	//glBufferData( GL_ARRAY_BUFFER, vTemps.size() * sizeof( vec3 ), vTemps.data(), GL_DYNAMIC_DRAW );
	//SHADER_MANAGER->setUniformVec3( ShaderManager::eShaderType::WORLD_SHDR, "vColor", &vec3( 0.25f ) );
	//glDrawArrays( GL_POINTS, 0, vTemps.size() );
	//glDrawArrays( GL_LINES, 0, vTemps.size() );

	// Next Pos
	//vTemps.clear();
	//vTemps.push_back( vec3( m_m4CurrentFrenetFrame[ 3 ] ) );
	//vTemps.push_back( vec3( getPosition( m_fCurrDist + (100.f * DELTA_S) ) ) );
	//glBufferData( GL_ARRAY_BUFFER, vTemps.size() * sizeof( vec3 ), vTemps.data(), GL_DYNAMIC_DRAW );
	//SHADER_MANAGER->setUniformVec3( ShaderManager::eShaderType::WORLD_SHDR, "vColor", &vec3( 0.75f ) );
	//glDrawArrays( GL_POINTS, 0, vTemps.size() );
	//glDrawArrays( GL_LINES, 0, vTemps.size() );

	mat3 mTemp = mat3( 1.0f );

	for ( int i = 0; i < 3; ++i )
	{
		// Debug FreNet Frame Lines:
		//	Red = BiNormal
		//	Green = Normal
		//	Blue = Tangent
		vTemps.clear();
		vTemps.push_back( vec3( vCurrFrenetFrame[ 3 ] ) );
		vTemps.push_back( vec3( vCurrFrenetFrame[ 3 ] + (vCurrFrenetFrame[ i ] * 0.5) ) );
		glBufferData( GL_ARRAY_BUFFER, vTemps.size() * sizeof( vec3 ), vTemps.data(), GL_DYNAMIC_DRAW );
		SHADER_MANAGER->setUniformVec3( ShaderManager::eShaderType::WORLD_SHDR, "vColor", &mTemp[ i ] );
		glDrawArrays( GL_POINTS, 0, vTemps.size() );
		glDrawArrays( GL_LINES, 0, vTemps.size() );
	}
	glPointSize( 1.0f );
#endif

	// Defined Colors for Track.
	vec3 vBrown( 0.545, 0.270, 0.074 );
	vec3 vSilver( 0.752, 0.752, 0.752 );

	// Draw Main Curve
	glUseProgram( SHADER_MANAGER->getProgram( ShaderManager::eShaderType::RC_TRACK_SHDR ) );
	SHADER_MANAGER->setUniformVec3( ShaderManager::eShaderType::RC_TRACK_SHDR, "vColor", &vBrown );
	glBindBuffer( GL_ARRAY_BUFFER, m_iVertexBuffer );

	// Draw connecting lines
	glBufferData( GL_ARRAY_BUFFER, m_vTracks.size() * sizeof( vec3 ), m_vTracks.data(), GL_DYNAMIC_DRAW );
	glDrawArrays( GL_LINES, 0, m_vTracks.size() );

	SHADER_MANAGER->setUniformVec3( ShaderManager::eShaderType::RC_TRACK_SHDR, "vColor", &vSilver );
	// Draw Track
	for ( int i = 0; i < 2; ++i )
	{
		glBufferData( GL_ARRAY_BUFFER, m_vTrackFrames[i].size() * sizeof( vec3 ), m_vTrackFrames[ i ].data(), GL_DYNAMIC_DRAW );
		glDrawArrays( GL_LINE_LOOP, 0, m_vTrackFrames[ i ].size() );
	}

	glUseProgram( iCurrProgBinding );
	glBindVertexArray( iCurrVABinding );
}


// animate
//		Called from container object to update the Animation.
//		Updates the Current Distance by Velocity and evaluates any stat changes.
void Anim_Track::animate()
{
	m_fCurrDist = wrap( m_fCurrDist + (getVelocity( m_fCurrDist ) * DELTA_T) );
	m_fCurrHeight = getPosition().y;
	m_eCurrentState = getState( m_fCurrDist );
}

/*******************************************************************************\
* Private Functions  														   *
\*******************************************************************************/

// loadAnimTrack
// Takes in a set of keyframes loaded from a file.  Temporary function, may switch to take in a file to load the keyframes from.
// Function taken from provided Vec3f_FileIO.h:
// i) Reads in file (e.g. text file (.txt) or contour file (.con)).
// ii) Parses it line by line.
// iii) Extracts the first three floating point values (ignoring all other values)
// iv) Stores these values as a Vec3f in a vector container
void Anim_Track::loadAnimTrack( const string& pContourFile )
{
	using std::string;
	using std::stringstream;
	using std::istream_iterator;

	// Set up input stream
	std::ifstream file( pContourFile );

	// Error opening file.
	if ( !file )
	{
		cout << "Could not open " << pContourFile << " for Animation Track." << endl;
		return;
	}

	// Local Variables.
	vec3 v;
	string line;
	size_t index;
	stringstream ss( std::ios_base::in );
	TableEntry vNewEntry;

	// Clear keyframes
	m_vKeyFrames.clear();

	// Get line by line
	while ( getline( file, line ) )
	{
		// remove comments
		index = line.find_first_of( "#" );
		if ( index != string::npos )
		{
			line.erase( index, string::npos );
		}

		// removes leading/tailing junk
		line.erase( 0, line.find_first_not_of( " \t\r\n\v\f" ) );
		index = line.find_last_not_of( " \t\r\n\v\f" ) + 1;
		if ( index != string::npos )
		{
			line.erase( index, string::npos );
		}

		if ( line.empty() )
		{
			continue; // empty or commented out line
		}

		// read line into string stream and clear any flags
		ss.str( line );
		ss.clear();

		// store the position into the vec3f
		ss >> v.x;
		ss >> v.y;
		ss >> v.z;

		// No issues, store the position
		if ( !(ss.fail() || ss.bad()) )
			m_vKeyFrames.push_back( v );
	}
	file.close();

	// Smooth out curve.
	for ( int i = 0; i < SUBDIVISION_COUNT; ++i )
		smoothCurve();

	// PreProcess for easier position access and evaluation
	arcLengthParameterize();

	// Calculate Total Curve Length by stepping along the curve.
	vector<vec3>::iterator iPrev = m_vKeyFrames.begin();
	m_fCurveLength = 0.0f;
	for ( vector<vec3>::iterator iter = m_vKeyFrames.begin() + 1;
		  iter != m_vKeyFrames.end();
		  ++iter )
	{
		m_fCurveLength += length( (*iter) - (*iPrev) );
		iPrev = iter;
	}

	// Set up Key frame table, storing FreNet Frames
	for ( unsigned int i = 0; i < m_vKeyFrames.size(); ++i )
	{
		// Store Position
		vNewEntry.vPosition = m_vKeyFrames[ i ];
		m_fCurrHeight = vNewEntry.vPosition.y;	// Track Current Height for Velocity Calculation
		computeFreNetFrames( (float) i * DELTA_ST, vNewEntry.m4FrenetFrame );
		m_vKeyFrameTable.push_back( vNewEntry );	// Store Entry
	}

	// Generate Tracks for the Car.
	generateTrackFrames();
}

// smoothCurve: pre-processes the curve by splitting the curve into several smaller segments
//		Each of which have, at most, size delta which is evaluated as the smallest distance between
//		two points on the initial set of key frames.
void Anim_Track::smoothCurve()
{
	// Local Variables
	vector< vec3 > vNewCurve, vShiftedCurve;		// New working curve
	vNewCurve.reserve( m_vKeyFrames.size() << 1 );	// Resize to 2n
	vector< vec3 >::const_iterator v_iCurrPoint, v_iNextPoint;	// Point iterators.
	v_iNextPoint = m_vKeyFrames.begin();
	v_iCurrPoint = v_iNextPoint++;
	vNewCurve.push_back( (*v_iCurrPoint) );

	// Steps:
	//	add new points between every 2 points on the curve
	//  move each point halfway forward between each set of points

	// 1st pass
	while ( v_iNextPoint != m_vKeyFrames.end() )
	{
		vNewCurve.push_back( ((*v_iNextPoint) + (*v_iCurrPoint)) * 0.5f );
		vNewCurve.push_back( (*v_iNextPoint) );
		v_iCurrPoint = v_iNextPoint++;
	}

	// Add one last point between the first and last points of the curve (if it's a closed curve)
	if ( !m_bOpenCurve )
		vNewCurve.push_back( (m_vKeyFrames.front() + (*v_iCurrPoint)) * 0.5f );
	else
		vShiftedCurve.push_back( vNewCurve.front() );

	// 2nd pass: shift everything over. Repeat 3 Times.
	for ( int i = 0; i < 3; ++i )
	{
		// Set up
		v_iNextPoint = vNewCurve.begin();
		v_iCurrPoint = v_iNextPoint++;

		// Loop through and shift.
		while ( v_iNextPoint != vNewCurve.end() )
		{
			vShiftedCurve.push_back( ((*v_iCurrPoint) + (*v_iNextPoint)) * 0.5f );
			v_iCurrPoint = v_iNextPoint++;
		}

		// Shift point over between first and last points on a closed curve.
		if ( !m_bOpenCurve )
			vShiftedCurve.push_back( ((*v_iCurrPoint) + vNewCurve.front()) * 0.5f );

		// Store generated curve and reset for potential next loop
		vNewCurve.clear();
		vNewCurve = vShiftedCurve;
		vShiftedCurve.clear();
	}

	// Finally, swap new set to stored set
	m_vKeyFrames.swap( vNewCurve );
}

// preProcessCurve: creates a new curve with a uniform distance of Delta_S between points.
void Anim_Track::arcLengthParameterize( )
{
	// Locals
	vector< vec3 > vNewCurve;
	vector< vec3 >::iterator v_iNextPos;
	float fCurrDist = 0.0f;
	vec3 vCurrPos = m_vKeyFrames.front();
	v_iNextPos = m_vKeyFrames.begin() + 1;
	m_fMaxHeight = -numeric_limits<float>::infinity();
	m_fMinHeight = numeric_limits<float>::infinity();
	m_fDistToFreeFall = 0.0f;

	// Push on current point, evaluate from here
	vNewCurve.push_back( vCurrPos );

	// Go through each point to set a set distance between each point.
	while ( v_iNextPos != m_vKeyFrames.end() )
	{
		// Create Direction Vector, add length and normalize for stepping.
		vec3 vTarget = (*v_iNextPos) - vCurrPos;
		fCurrDist += length( vTarget );
		vTarget = normalize( vTarget );

		// Loop through Vector at set rate.
		while ( fCurrDist >= DELTA_ST )
		{
			// Next step marked.
			fCurrDist -= DELTA_ST;
			vNewCurve.push_back( (vCurrPos) + (DELTA_ST) * vTarget );	// Store Point
			
			// Set new point 
			vCurrPos = vNewCurve.back();

			// Evaluate Height information of track for velocity.
			evalHeight( vNewCurve.back().y, vNewCurve.size() * DELTA_ST );
		}

		// Iterate.
		v_iNextPos++;
	}

	// Close off curve.
	if ( !m_bOpenCurve )
	{
		vec3 vTarget = m_vKeyFrames.front() - vCurrPos;
		fCurrDist += length( vTarget );
		vTarget = normalize( vTarget );

		while ( fCurrDist >= DELTA_ST )
		{
			fCurrDist -= DELTA_ST;
			vNewCurve.push_back( (vCurrPos) + (DELTA_ST) * vTarget );

			vCurrPos = vNewCurve.back();

			evalHeight( vNewCurve.back().y, vNewCurve.size() * DELTA_ST );
		}
	}

	// Store the new curve.
	m_vKeyFrames.swap( vNewCurve );
}

// After everything has been pre-processed, evaluate the tracks using the binormal at each position on the curve.
void Anim_Track::generateTrackFrames()
{
	// Locals
	LEFT_TRACK.reserve( m_vKeyFrames.size() );		// Reserve necessary space for the tracks
	RIGHT_TRACK.reserve( m_vKeyFrames.size() );		// Reserve necessary space for the tracks
	m_vTracks.reserve( m_vKeyFrames.size() << 1 );	// Reserve Necessary space for track bars
	float fHalfWidth = TRACK_WIDTH * 0.5f;

	for ( vector<TableEntry>::const_iterator iter = m_vKeyFrameTable.begin();
		 iter != m_vKeyFrameTable.end();
		 ++iter )
	{
		// Index 0 of the frenet frame matrix is the binormal
		LEFT_TRACK.push_back( iter->vPosition + vec3(normalize( iter->m4FrenetFrame[0] ) * -fHalfWidth) );
        RIGHT_TRACK.push_back( iter->vPosition + vec3(normalize( iter->m4FrenetFrame[0] ) * fHalfWidth ) );

		// Store both points for track drawing
		m_vTracks.push_back( LEFT_TRACK.back() );
		m_vTracks.push_back( RIGHT_TRACK.back() );
	}
}

// Returns an Arc-Length distance at a certain point along the curve.
vec3 Anim_Track::getPosition( float sDist )
{
	// Locals
	vec3 vReturnPos = vec3( 0.f );

	// Evaluate the index from the current distance on the track
	sDist = wrap( sDist );
	float fIndexedDist = (float)sDist / (float)DELTA_ST; // Divide to get index to array
	int iIndex = (unsigned int) fIndexedDist; // Store index
	int iNextIndex = (unsigned int) (iIndex + 1) < m_vKeyFrames.size() ? (iIndex + 1) : 0; // evaluate next index
	float t = (fIndexedDist - (float)iIndex); // get value for interpolation.

	// Get interpolated distance
	vReturnPos = (1-t) * m_vKeyFrames[ iIndex ] + t * m_vKeyFrames[ iNextIndex ];

	// Return Position.
	return vReturnPos;
}

// Evaluate the current roller coaster state.
Anim_Track::eCurrVelocityState Anim_Track::getState( float fDist )
{
	// Get Current position and set default return.
	vec3 vPosAtDist = getPosition( fDist );
	eCurrVelocityState eReturnState = GRAVITY_FREE_FALL;

	if ( fDist >= DECEL_THRESHOLD( m_fCurveLength ) )	// In the Deceleration zone
		eReturnState = DECELERATION;
	else if ( fDist >= m_fDistToFreeFall )				// In the Free Fall zone
		eReturnState = GRAVITY_FREE_FALL;
	else												// Anything else is the Lifting Stage
		eReturnState = LIFTING_STATE;

	//eReturnState = GRAVITY_FREE_FALL;
	return eReturnState;
}

// Returns the Velocity at a given position.
float Anim_Track::getVelocity( float fDist )
{
	// Locals
	float fReturnVelocity = MIN_VELOCITY;
	float fDecel_Start_Velocity;
	float fDecelDistance;

	// Evaluate Velocity based on current state.
	switch ( getState( fDist ) )
	{
		case GRAVITY_FREE_FALL:	// Free Fall, just use Gravity
			fReturnVelocity = sqrt( (2.0f * GRAVITY) * abs(m_fMaxHeight - m_fCurrHeight) );
			break;
		case DECELERATION:	// Deceleration Stage -> coming to a stop.
			fDecel_Start_Velocity =
				sqrt( (2.0f * GRAVITY) * (m_fMaxHeight - m_fCurrHeight ) ); // Get beginning Velocity (Free Fall)
			fDecelDistance = m_fCurveLength - DECEL_THRESHOLD( m_fCurveLength ); // Total Deceleration Distance
			fReturnVelocity = fDecel_Start_Velocity * ((m_fCurveLength - m_fCurrDist) / fDecelDistance); // Slow Down velocity
			break;
	}

	// Set a minimum velocity 
	fReturnVelocity = fReturnVelocity < MIN_VELOCITY ? MIN_VELOCITY : fReturnVelocity;

	// Return
	return fReturnVelocity;
}

// Get the interpolated frenetframe for the current position.
mat4 Anim_Track::getFreNetFrames()
{
	// Local Variables
    mat4 m4CurrFrame;
    float fPos = wrap( m_fCurrDist );
    float t = fPos / DELTA_ST;
    unsigned int iIndex = (unsigned int) t;
    unsigned int iNextIndex = (unsigned int) (iIndex + 1) < m_vKeyFrameTable.size() ? iIndex + 1 : 0;
    t = t - (float)iIndex;

	// Interpolate between Frenet frames
    m4CurrFrame = (1-t) * m_vKeyFrameTable[ iIndex ].m4FrenetFrame +
                  t * m_vKeyFrameTable[ iNextIndex ].m4FrenetFrame;

	// Return result.
    return m4CurrFrame;
}

// Compute the frenetframes for a given position.
void Anim_Track::computeFreNetFrames( float fCurrPos, mat4& m4ReturnFrames )
{
	// Find Tangent, BiNormal and Normal
	vec3 vTangent = normalize( getTangent( fCurrPos ) );
	vec3 vBiNormal = normalize( computeBiNormal( fCurrPos, vTangent ) );
	vec3 vNormal = normalize( cross( vTangent, vBiNormal ) );

	// mat4 is column wise
	m4ReturnFrames[ 0 ] = vec4( vBiNormal, 0.0f );		// Column 1: Binormal
	m4ReturnFrames[ 1 ] = vec4( vNormal, 0.0f );		// Column 2: Normal
	m4ReturnFrames[ 2 ] = vec4( vTangent, 0.0f );		// Column 3: Tangent
	m4ReturnFrames[ 3 ] = vec4( getPosition( fCurrPos ) + (vNormal * POSITION_OFFSET), 1.0f );	// Column 4: Translation to Position
}

// Returns the Centripetal Acceleration based on the curvature at the given position on the animation track
vec3 Anim_Track::getCentripetalAcce( float fDist )
{
	// Locals
	vec3 a = getTangent( fDist + (getVelocity( fDist ) * DELTA_T) );
	vec3 b = getTangent( fDist );
	vec3 vReturn = vec3( 0.0f );

	// Ignore Centripetal Acceleration on linear sections of curve ||A x B|| ~= 0
	if ( length( cross( a, b ) ) >= FLT_EPSILON )
		vReturn = (a - b) / DELTA_T;

	// Return
	return vReturn;
}

// Cross between Track Up Vector and Tangent
vec3 Anim_Track::computeBiNormal( float fDist, const vec3& vTangent )
{
	return cross( computeNormal( fDist ), vTangent );
}

// Computes the Normal at a given point on the track => centripetal acceleration + gravity.
vec3 Anim_Track::computeNormal( float fDist )
{
	return getCentripetalAcce( fDist ) + vec3( 0.0f, GRAVITY, 0.0f );
}

// Return estimated tangent at point fDist.
vec3 Anim_Track::getTangent( float fDist )
{
	return  (getPosition( fDist + (getVelocity(fDist) * DELTA_T) ) - getPosition( fDist )) / DELTA_T;
}
