#include "Scene_Loader.h"
#include "EntityManager.h"
#include <sstream>
#include <iterator>

/***********\
* Defines *
\***********/
#define MAX_CHARS_PER_LINE 256
#define MAX_SPHERE_PARAMS 4
#define MAX_PLANE_PARAMS 8
#define MAX_TRI_PARAMS 12
#define MAX_LIGHT_PARAMS 6
#define MAX_MESH_PARAMS 3
#define MAX_TRACK_PARAMS 1
#define COMMENT_CHAR '#'

// Singleton Declaration
Scene_Loader* Scene_Loader::m_pInstance = nullptr;

// Private Constructor
Scene_Loader::Scene_Loader()
{
	m_lNextID = 0;

	m_pAnimProperty = nullptr;
	m_sMeshProperty = m_sTextureProperty = "";
}

// Returns the singleton instance of the Object Factory
Scene_Loader* Scene_Loader::getInstance()
{
	if ( nullptr == m_pInstance )
		m_pInstance = new Scene_Loader();

	return m_pInstance;
}


Scene_Loader::~Scene_Loader()
{
	if ( nullptr != m_pAnimProperty )
		delete m_pAnimProperty;
}

// Creation Functions
// Create a Sphere given a starting position, color and radius
void Scene_Loader::createSphere( vector< string > sData, int iLength )
{
	vec3 vPosition;

	// Create the Sphere if we're given the correct # of values
	if ( iLength == MAX_SPHERE_PARAMS )
	{
		vPosition = glm::vec3( stof( sData[ 0 ] )/*X*/, stof( sData[ 1 ] )/*Y*/, stof( sData[ 2 ] )/*Z*/ );	// Position of Sphere

		ENTITY_MANAGER->generateStaticSphere(stof(sData[3]), &vPosition, 
											 m_sTextureProperty, m_sShaderProperty);
	}
	else
		outputError("sphere", sData);
}

// Create a Plane given a normal, a position on the plane and a color
void Scene_Loader::createPlane( vector< string > sData, int iLength )
{
	// Local Parameters to pull out Plane Data.
	vec3 pPosition, vNormal;
	int iHeight, iWidth;

	if (iLength == MAX_PLANE_PARAMS)
	{
		pPosition = glm::vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);		// Position of Plane
		vNormal = vec3(stof(sData[3]),/*X*/ stof(sData[4]),/*Y*/ stof(sData[5]));
		iHeight = stoi(sData[6]);
		iWidth = stoi(sData[7]);
		ENTITY_MANAGER->generateStaticPlane(iHeight, iWidth, &pPosition, &vNormal, m_sTextureProperty, m_sShaderProperty);
	}
	else
		outputError("Plane", sData);
}

// Generates a Directional Light object given some input data
// sData -> String of inputs to parse
// iLength -> Number of Inputer to parse.
void Scene_Loader::createDirectionalLight( vector< string > sData, int iLength )
{
	vec3 vDirection, vDiffuseColor, vAmbientColor, vSpecularColor;

	if ( MAX_LIGHT_PARAMS == iLength )
	{
		vDirection = vec3( stof( sData[ 0 ] )/*dX*/, stof( sData[ 1 ] )/*dY*/, stof( sData[ 2 ] )/*dZ*/ );
		vAmbientColor = vec3( stof( sData[ 3 ] )/*aR*/, stof( sData[ 4 ] )/*aG*/, stof( sData[ 5 ] )/*aB*/ );
		vDiffuseColor = vec3(stof(sData[ 6 ])/*aR*/, stof(sData[ 7 ])/*aG*/, stof(sData[ 8 ])/*dB*/);
		vSpecularColor = vec3(stof(sData[ 9 ])/*sR*/, stof(sData[ 10 ])/*sG*/, stof(sData[ 11 ])/*sB*/);


		ENTITY_MANAGER->generateDirectionalLight(&vDirection, &vAmbientColor, &vDiffuseColor, &vSpecularColor);
	}
	else
		outputError("light", sData);
}

// Generates a Light object given some input data
// sData -> String of inputs to parse
// iLength -> Number of Inputer to parse.
void Scene_Loader::createPointLight(vector< string > sData, int iLength)
{
	vec3 pPosition, pColor;

	if (MAX_LIGHT_PARAMS == iLength)
	{
		pPosition = vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);
		pColor = vec3(stof(sData[3])/*R*/, stof(sData[4])/*G*/, stof(sData[5])/*B*/);

		ENTITY_MANAGER->generateStaticPointLight(&pPosition, &pColor, m_sMeshProperty, m_sTextureProperty);
	}
	else
		outputError("light", sData);
}

// Generates a Player Object at a given position
// NOTE: This is a temporary testing tool, it may not be possible in the final version of the game to generate this
//		object from a scene file.
void Scene_Loader::createPlayer(vector< string > sData, int iLength)
{
	vec3 vPosition = glm::vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);	// Position of Mesh

	ENTITY_MANAGER->generatePlayerEntity(&vPosition, m_sMeshProperty, m_sTextureProperty, m_sShaderProperty);
}

// Generates a Static Mesh Object at a specified location.
void Scene_Loader::createStaticMesh(vector< string > sData, int iLength)
{
	vec3 vPosition = glm::vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);	// Position of Mesh

	ENTITY_MANAGER->generateStaticMesh(m_sMeshProperty, &vPosition, m_sTextureProperty, m_sShaderProperty);
}

/**************************************************************************\
* File Reading                                                           *
\**************************************************************************/

void Scene_Loader::loadFromFile( string sFileName )
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
void Scene_Loader::outputError( string sName, vector<string> sData )
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
void Scene_Loader::pullData( ifstream& inFile, vector< string >& sReturnData )
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

void Scene_Loader::handleData( vector< string >& sData, const string& sIndicator )
{
	if ("sphere" == sIndicator)				// Parse Sphere
		createSphere(sData, sData.size());
	else if ("plane" == sIndicator)			// Parse Plane
		createPlane(sData, sData.size());
	else if ("point_light" == sIndicator)	// Parse Point Light
		createPointLight(sData, sData.size());
	else if ("player" == sIndicator)		// Parse Player
		createPlayer(sData, sData.size());
	else if ("static_mesh" == sIndicator)	// Parse Static Mesh
		createStaticMesh(sData, sData.size());
	else if ("boids" == sIndicator)	// Parse Mass Spring System
		ENTITY_MANAGER->initializeBoidEngine(sData);

	clearProperties();
}

// Set up internal property storage with property to load objects with.
void Scene_Loader::handleProperty( vector< string >& sData, const string& sIndicator )
{
	string sDataTrimmed = trimString( sData[ 0 ] );

	if ("anim_track" == sIndicator)
		m_pAnimProperty = new Anim_Track(getNewID(), &sDataTrimmed, &m_sMeshProperty, &m_sTextureProperty);
	else if ("texture" == sIndicator)
		m_sTextureProperty = sDataTrimmed;
	else if ("mesh" == sIndicator)
		m_sMeshProperty = sDataTrimmed;
	else if ("shader" == sIndicator)
		m_sShaderProperty = sDataTrimmed;
}

// Removes any tabs from the beginning or end of a given string.
string Scene_Loader::trimString( const string& sStr )
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

// Clear Any Properties that have been created on the last data parse.
void Scene_Loader::clearProperties() // Clear any properties
{
	if (nullptr != m_pAnimProperty)
		delete m_pAnimProperty;

	m_pAnimProperty = nullptr;
	m_sMeshProperty = m_sTextureProperty = m_sShaderProperty = "";
}
