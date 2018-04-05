#include "Object_Factory.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Plane.h"
#include "MeshObject.h"
#include "Light.h"
#include "Anim_Track.h"
#include <sstream>
#include <iterator>

/***********\
* Defines *
\***********/
#define MAX_CHARS_PER_LINE 256
#define MAX_SPHERE_PARAMS 4
#define MAX_PLANE_PARAMS 16
#define MAX_TRI_PARAMS 12
#define MAX_LIGHT_PARAMS 6
#define MAX_MESH_PARAMS 3
#define MAX_TRACK_PARAMS 1
#define COMMENT_CHAR '#'

// Singleton Declaration
Object_Factory* Object_Factory::m_pInstance = nullptr;

// Private Constructor
Object_Factory::Object_Factory()
{
	m_lNextID = 0;

	m_pAnimProperty = nullptr;
	m_sMeshProperty = m_sTextureProperty = "";
}

// Returns the singleton instance of the Object Factory
Object_Factory* Object_Factory::getInstance()
{
	if ( nullptr == m_pInstance )
		m_pInstance = new Object_Factory();

	return m_pInstance;
}


Object_Factory::~Object_Factory()
{
	if ( nullptr != m_pAnimProperty )
		delete m_pAnimProperty;
}

// Creation Functions
// Create a Sphere given a starting position, color and radius
Object3D* Object_Factory::createSphere( vector< string > sData, int iLength )
{
	Sphere* pReturnSphere = nullptr;
	vec3 vPosition;

	// Create the Sphere if we're given the correct # of values
	if ( iLength == MAX_SPHERE_PARAMS )
	{
		vPosition = glm::vec3( stof( sData[ 0 ] )/*X*/, stof( sData[ 1 ] )/*Y*/, stof( sData[ 2 ] )/*Z*/ );	// Position of Sphere

		pReturnSphere = new Sphere( &vPosition,
									stof( sData[ 3 ] ),		// Radius of Sphere
									getNewID(),				// Unique ID
									&m_sTextureProperty,
									m_pAnimProperty );	// TextureName (if applicable)
	}

	return pReturnSphere;
}

// Create a Plane given a normal, a position on the plane and a color
Object3D* Object_Factory::createPlane( vector< string > sData, int iLength )
{
	vec3 pPosition;
	vector<glm::vec3> vCorners;
	Plane* pReturnPlane = nullptr;
	bool bUseEB;

	if ( iLength == MAX_PLANE_PARAMS )
	{
		pPosition = glm::vec3( stof( sData[ 0 ] )/*X*/, stof( sData[ 1 ] )/*Y*/, stof( sData[ 2 ] )/*Z*/ );		// Position of Plane
		vCorners.push_back( glm::vec3( stof( sData[ 3 ] )/*X*/, stof( sData[ 4 ] )/*Y*/, stof( sData[ 5 ] )/*Z*/ ) );		// First Corner of Plane 
		vCorners.push_back( glm::vec3( stof( sData[ 6 ] )/*X*/, stof( sData[ 7 ] )/*Y*/, stof( sData[ 8 ] )/*Z*/ ) );		// Second Corner of Plane
		vCorners.push_back( glm::vec3( stof( sData[ 9 ] )/*X*/, stof( sData[ 10 ] )/*Y*/, stof( sData[ 11 ] )/*Z*/ ) );		// Third Corner of Plane
		vCorners.push_back( glm::vec3( stof( sData[ 12 ] )/*X*/, stof( sData[ 13 ] )/*Y*/, stof( sData[ 14 ] )/*Z*/ ) );	// Fourth Corner of Plane
		bUseEB = sData[ 15 ] != "0";

		pReturnPlane = new Plane( &pPosition,
								  &vCorners,
								  getNewID(),
								  &m_sTextureProperty,
								  bUseEB, m_pAnimProperty );
	}

	return pReturnPlane;
}

// create a Triangle given 3 positions and a color.
Object3D* Object_Factory::createTriangle( vector< string > sData, int iLength )
{
	vec3 pPosition;
	vector<glm::vec3> vCorners;
	Triangle* pReturnTri = nullptr;

	if ( MAX_TRI_PARAMS == iLength )
	{
		pPosition = glm::vec3( stof( sData[ 0 ] )/*X*/, stof( sData[ 1 ] )/*Y*/, stof( sData[ 2 ] )/*Z*/ );		// Position of Triangle

		vCorners.push_back( glm::vec3( stof( sData[ 3 ] )/*X*/, stof( sData[ 4 ] )/*Y*/, stof( sData[ 5 ] )/*Z*/ ) );		// Point 1 of Triangle
		vCorners.push_back( glm::vec3( stof( sData[ 6 ] )/*X*/, stof( sData[ 7 ] )/*Y*/, stof( sData[ 8 ] )/*Z*/ ) );		// Point 2 of Triangle
		vCorners.push_back( glm::vec3( stof( sData[ 9 ] )/*X*/, stof( sData[ 10 ] )/*Y*/, stof( sData[ 11 ] )/*Z*/ ) );		// Point 3 of Triangle

		pReturnTri = new Triangle( &pPosition,
								   &vCorners,
								   getNewID(),
								   &m_sTextureProperty,
								   m_pAnimProperty );
	}

	return pReturnTri;
}

// Generates a Light object given some input data
// sData -> String of inputs to parse
// iLength -> Number of Inputer to parse.
Light* Object_Factory::createLight( vector< string > sData, int iLength )
{
	vec3 pPosition, pColor;
	Light* pReturnLight = nullptr;

	if ( MAX_LIGHT_PARAMS == iLength )
	{
		pPosition = vec3( stof( sData[ 0 ] )/*X*/, stof( sData[ 1 ] )/*Y*/, stof( sData[ 2 ] )/*Z*/ );
		pColor = vec3( stof( sData[ 3 ] )/*R*/, stof( sData[ 4 ] )/*G*/, stof( sData[ 5 ] )/*B*/ );

		pReturnLight = new Light( &pPosition,
								  &pColor,
								  getNewID(),
								  &m_sTextureProperty,
								  m_pAnimProperty );
	}

	return pReturnLight;
}

// Generate a Mesh object given an array of strings as data.
// sData -> Vector of inputs
// iLength -> Number of Inputs
Object3D* Object_Factory::createMesh( vector< string > sData, int iLength )
{
	vec3 pPosition;
	MeshObject* pReturnMesh = nullptr;

	if ( MAX_MESH_PARAMS == iLength )
	{
		pPosition = glm::vec3( stof( sData[ 0 ] )/*X*/, stof( sData[ 1 ] )/*Y*/, stof( sData[ 2 ] )/*Z*/ );	// Position of Mesh

		pReturnMesh = new MeshObject( &pPosition,
									  &m_sMeshProperty,
									  getNewID(),
									  &m_sTextureProperty,
									  m_pAnimProperty );
	}

	return pReturnMesh;
}

// Optional way to create a mesh, takes a position, mesh object location and a texture location
Object3D* Object_Factory::createMesh( const vec3* pPos,
									  const string* sLocation,
									  const string* sTexLocation )
{
	MeshObject* pNewMesh = nullptr;
	if ( sLocation->find( ".ply" ) != string::npos )
		pNewMesh = new MeshObject( pPos,
								   sLocation,
								   getNewID(),
								   sTexLocation,
								   nullptr );

	return pNewMesh;
}

/**************************************************************************\
* File Reading                                                           *
\**************************************************************************/

void Object_Factory::loadFromFile( string sFileName )
{
	ifstream inFile;
	string sBuffer;
	vector< string > sData;
	vector< string > sAdditionalData;
	string sIndicator, sParser;
	Object* pResultingObject = nullptr;

	// Open File
	inFile.open( sFileName );

	// Parse File if opened properly
	if ( inFile.good() )
	{
		// Parse to eof
		while ( !inFile.eof() )
		{
			// Get next line to parse
			getline( inFile, sBuffer );

			// Determine if we care about the line
			if ( "" != sBuffer && COMMENT_CHAR != sBuffer[ 0 ] )
			{
				// Determine keyword for the segment
				istringstream sStream( sBuffer );
				sStream >> sIndicator;

				pullData( inFile, sData );
				handleData( sData, sIndicator );

				// Debugging
			#ifdef DEBUG
				cout << sIndicator << endl;
				for ( unsigned int d = 0; d < sData.size(); ++d )
					cout << sData[ d ] << endl;
				cout << "\t\t END LINE" << endl;
			#endif

			}
		}
	}

	// Close File
	inFile.close();
}

// outputError - Outputs information about an error creating an object.  Usually if 
//					there were an incorrect number of parameters.
// Params: sName - The Name of the Object trying to be loaded
//		   sData - The Data read in when trying to load.
void Object_Factory::outputError( const string* sName, vector<string> sData )
{
	cout << "Error creating " << sName << " with the following data:" << endl;
	cout << "{";

	// Output Data
	for ( vector<string>::iterator pIter = sData.begin();
		  pIter != sData.end();
		  ++pIter )
	{
		cout << *pIter;
		if ( pIter != sData.end() - 1 )
			cout << ", ";
	}
	cout << "}" << endl;
}

// Pull a block of parameters for an object specified in the file.
void Object_Factory::pullData( ifstream& inFile, vector< string >& sReturnData )
{
	string sBuffer, sParser;
	string sTexturePropPH, sMeshPropPH;
	Anim_Track* pAnimTrackPropPH = nullptr;
	vector< string > sPropertyData;
	string sPropertyIndicator;
	stringstream sStream( sBuffer );

	// Store Data for the segment
	sReturnData.clear();
	do
	{
		getline( inFile, sBuffer );	// Next line of Data
		sBuffer = trimString( sBuffer );	// Clear any whitespace.
		sStream.str( sBuffer );		// Parse next Line
		sStream.clear();			// Clear Stream Flags
		while ( getline( sStream, sParser, ' ' ) )	// Extrapolate Data
		{
			if ( '+' == sParser[ 0 ] )
			{
				// Handle Property
				saveProperties( sTexturePropPH, sMeshPropPH, pAnimTrackPropPH );	// Save properties from being overwritten.
				m_sMeshProperty = m_sTextureProperty = "";
				sPropertyIndicator = sParser.substr( 1, sParser.size() - 1 );	// Grab property identifier without the '+'
				sStream.str( "" );			// Clear rest of stream.
				sStream.clear();			// clear flags
				pullData( inFile, sPropertyData );						// pull property information
				handleProperty( sPropertyData, sPropertyIndicator );	// store property internally
				restoreProperties( sTexturePropPH, sMeshPropPH, pAnimTrackPropPH );	// restore previous properties.
			}
			else if ( "" != sParser )					// Avoid Garbage
				sReturnData.push_back( sParser );
		}
	} while ( "}" != sReturnData.back() );				// Repeat until end delimiter

														// remove "}" from end of Data List
	sReturnData.pop_back();
}

void Object_Factory::handleData( vector< string >& sData, const string& sIndicator )
{
	Object* pResultingObject = nullptr;

	if ( "sphere" == sIndicator )			// Parse Sphere
		pResultingObject = createSphere( sData, sData.size() );
	else if ( "plane" == sIndicator )		// Parse Plane
		pResultingObject = createPlane( sData, sData.size() );
	else if ( "triangle" == sIndicator )	// Parse Triangle
		pResultingObject = createTriangle( sData, sData.size() );
	else if ( "light" == sIndicator )		// Parse Light
		pResultingObject = createLight( sData, sData.size() );
	else if ( "mesh_obj" == sIndicator )	// Parse Mesh
		pResultingObject = createMesh( sData, sData.size() );

	clearProperties();

	if ( nullptr == pResultingObject )
		outputError( &sIndicator, sData );

	pResultingObject = nullptr; // Don't do anything with the Object
							 // Handled by the Environment Manager
}

// Set up internal property storage with property to load objects with.
void Object_Factory::handleProperty( vector< string >& sData, const string& sIndicator )
{
	string sDataTrimmed = trimString( sData[ 0 ] );

	if ( "anim_track" == sIndicator )
		m_pAnimProperty = new Anim_Track( getNewID(), &sDataTrimmed, &m_sMeshProperty, &m_sTextureProperty );
	else if ( "texture" == sIndicator )
		m_sTextureProperty = sDataTrimmed;
	else if ( "mesh" == sIndicator )
		m_sMeshProperty = sDataTrimmed;
}

// Removes any tabs from the beginning or end of a given string.
string Object_Factory::trimString( const string& sStr )
{
	string sReturnString = sStr;

	if ( sStr != "" )
	{
		const string WHITESPACES( " \t\f\v\n\r" );
		const size_t strBegin = sStr.find_first_not_of( WHITESPACES );
		const size_t strRange = sStr.find_last_not_of( WHITESPACES ) - strBegin + 1;

		sReturnString = sStr.substr( strBegin, strRange );
	}

	return sReturnString;
}
