#include "SceneLoader.h"
#include "EntityManager.h"
#include <sstream>
#include <iterator>

/***********\
* Defines *
\***********/
#define MAX_CHARS_PER_LINE     256
#define MAX_SPHERE_PARAMS      4
#define MAX_PLANE_PARAMS       8
#define MAX_POINT_LIGHT_PARAMS 7
#define MAX_DIR_LIGHT_PARAMS   12
#define MAX_SPOTLIGHT_PARAMS   10
#define DEFAULT_SOFT_CUTOFF    5.f
#define MAX_MESH_PARAMS        3
#define MAX_TRACK_PARAMS       1
#define COMMENT_CHAR           '#'
#define SPHERE                 "sphere"
#define PLANE                  "plane"
#define POINT_LIGHT            "point_light"
#define DIRECTIONAL_LIGHT      "directional_light"
#define SPOTLIGHT              "spotlight"
#define PLAYER                 "player"
#define STATIC_MESH            "static_mesh"
#define BOIDS                  "boids"
#define MATERIAL               "material"
#define BOUNDING               "bounding"
#define MESH                   "mesh"
#define SHADER                 "shader"


// Singleton Declaration
SceneLoader* SceneLoader::m_pInstance = nullptr;

// Private Constructor
SceneLoader::SceneLoader()
{
    m_lNextID = 0;

    clearProperties();
}

// Returns the singleton instance of the Object Factory
SceneLoader* SceneLoader::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new SceneLoader();
    }

    return m_pInstance;
}


SceneLoader::~SceneLoader()
{
}

// Creation Functions
// Create a Sphere given a starting position, color and radius
void SceneLoader::createSphere( vector< string > sData, int iLength )
{
    vec3 vPosition;

    // Create the Sphere if we're given the correct # of values
    if ( iLength == MAX_SPHERE_PARAMS )
    {
        vPosition = glm::vec3( stof( sData[ 0 ] )/*X*/, stof( sData[ 1 ] )/*Y*/, stof( sData[ 2 ] )/*Z*/ );    // Position of Sphere

        ENTITY_MANAGER->generateStaticSphere(stof(sData[3]), &vPosition, 
                                             & m_pObjectProperties.sObjMaterial, &m_pBBProperty, m_sShaderProperty);
    }
    else
    {
        outputError(SPHERE, sData);
    }
}

// Create a Plane given a normal, a position on the plane and a color
void SceneLoader::createPlane( vector< string > sData, int iLength )
{
    // Local Parameters to pull out Plane Data.
    vec3 pPosition, vNormal;
    int iHeight, iWidth;

    if (iLength == MAX_PLANE_PARAMS)
    {
        pPosition = glm::vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);        // Position of Plane
        vNormal = vec3(stof(sData[3]),/*X*/ stof(sData[4]),/*Y*/ stof(sData[5]));
        iHeight = stoi(sData[6]);
        iWidth = stoi(sData[7]);
        ENTITY_MANAGER->generateStaticPlane(iHeight, iWidth, &pPosition, &vNormal, & m_pObjectProperties.sObjMaterial, &m_pBBProperty, m_sShaderProperty);
    }
    else
    {
        outputError(PLANE, sData);
    }
}

// Generates a Directional Light object given some input data
// sData -> String of inputs to parse
// iLength -> Number of Inputer to parse.
void SceneLoader::createDirectionalLight( vector< string > sData, int iLength )
{
    vec3 vDirection, vDiffuseColor, vAmbientColor, vSpecularColor;

    if ( MAX_DIR_LIGHT_PARAMS == iLength )
    {
        vDirection = normalize( vec3( stof( sData[ 0 ] )/*dX*/, stof( sData[ 1 ] )/*dY*/, stof( sData[ 2 ] )/*dZ*/ ) );
        vAmbientColor = vec3( stof( sData[ 3 ] )/*aR*/, stof( sData[ 4 ] )/*aG*/, stof( sData[ 5 ] )/*aB*/ );
        vDiffuseColor = vec3(stof(sData[ 6 ])/*aR*/, stof(sData[ 7 ])/*aG*/, stof(sData[ 8 ])/*dB*/);
        vSpecularColor = vec3(stof(sData[ 9 ])/*sR*/, stof(sData[ 10 ])/*sG*/, stof(sData[ 11 ])/*sB*/);

        ENTITY_MANAGER->generateDirectionalLight(&vDirection, &vAmbientColor, &vDiffuseColor, &vSpecularColor);
    }
    else
    {
        outputError(DIRECTIONAL_LIGHT, sData);
    }
}

// Generates a Light object given some input data
// sData -> String of inputs to parse
// iLength -> Number of Inputer to parse.
void SceneLoader::createPointLight(vector< string > sData, int iLength)
{
    // Local Variables
    vec3 pPosition, pColor;
    float fPower;

    if (MAX_POINT_LIGHT_PARAMS == iLength)
    {
        pPosition = vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);
        pColor = vec3(stof(sData[3])/*R*/, stof(sData[4])/*G*/, stof(sData[5])/*B*/);
        fPower = stof(sData[6])/*P*/;

        // Set the Diffuse Color of the Material
        m_pMaterialProperty.vOptionalDiffuseColor = vec4(pColor * fPower, 1.0);

        ENTITY_MANAGER->generateStaticPointLight( &m_pObjectProperties, fPower, &pColor, m_sMeshProperty, m_fMeshScaleProperty);
    }
    else
    {
        outputError(POINT_LIGHT, sData);
    }
}

// Generates a SpotLight object with a Position, Direction, Color and Phi angle of the spotlight.
//    There is also an optional soft edge cutoff able to be specified to simulate soft edges for the spotlight.
void SceneLoader::createSpotLight(vector< string > sData, int iLength)
{
    vec3 vPosition, vDirection, vColor;
    float fSoftCutoff = DEFAULT_SOFT_CUTOFF;
    
    // Check for Optional Parameter
    if ((MAX_SPOTLIGHT_PARAMS + 1) == iLength)
    {
        fSoftCutoff = stof(sData[MAX_SPOTLIGHT_PARAMS]);
        --iLength;    // Handle the Optional and resolve the rest as normal
    }

    // Grab Data and give to Entity Manager
    if (MAX_SPOTLIGHT_PARAMS == iLength)
    {
        vPosition = vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);
        vDirection = vec3( stof(sData[3])/*dX*/, stof(sData[4])/*dY*/, stof(sData[5])/*dZ*/);
        vColor = vec3( stof(sData[6])/*R*/, stof(sData[7])/*G*/, stof(sData[8])/*B*/);

        // Set the Diffuse color of the material.
        m_pMaterialProperty.vOptionalDiffuseColor = vec4(vColor, 1.0);

        ENTITY_MANAGER->generateStaticSpotLight(stof(sData[9]), fSoftCutoff, &vPosition, &vColor, &vDirection, &m_pMaterialProperty, &m_pBBProperty, m_sMeshProperty, m_fMeshScaleProperty);
    }
    else
    {
        outputError(SPOTLIGHT, sData);
    }
}

// Generates a Player Object at a given position
// NOTE: This is a temporary testing tool, it may not be possible in the final version of the game to generate this
//        object from a scene file.
void SceneLoader::createPlayer(vector< string > sData, int iLength)
{
    vec3 vPosition = glm::vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);    // Position of Mesh

    ENTITY_MANAGER->generatePlayerEntity(&vPosition, m_sMeshProperty, &m_pMaterialProperty, m_fMeshScaleProperty, &m_pBBProperty, m_sShaderProperty );
}

// Generates a Static Mesh Object at a specified location.
void SceneLoader::createStaticMesh(vector< string > sData, unsigned int iLength)
{
    // Local Variables
    vec3 vPosition;

    // Add a number of Static Meshes with the same Properties to all specified positions.
    for (unsigned int i = 0; i + 3 <= iLength; i += 3)
    {
        vPosition = vec3(stof(sData[i])/*X*/, stof(sData[i + 1])/*Y*/, stof(sData[i + 2])/*Z*/);    // Position of Mesh
        ENTITY_MANAGER->generateStaticMesh(m_sMeshProperty, &vPosition, &m_pMaterialProperty, &m_pBBProperty, m_fMeshScaleProperty, m_sShaderProperty);
    }
}

/**************************************************************************\
* File Reading                                                           *
\**************************************************************************/

void SceneLoader::loadFromFile( string sFileName )
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
//                    there were an incorrect number of parameters.
// Params: sName - The Name of the Object trying to be loaded
//           sData - The Data read in when trying to load.
void SceneLoader::outputError( string sName, vector<string> sData )
{
    cout << "Error creating " << sName << " with the following data:" << endl;
    cout << "{";

    // Output Data
    for ( vector<string>::iterator pIter = sData.begin();
          pIter != sData.end();
          ++pIter )
    {
        cout << *pIter;
        if (pIter != sData.end() - 1)
        {
            cout << ", ";
        }
    }
    cout << "}" << endl;
}

// Pull a block of parameters for an object specified in the file.
void SceneLoader::pullData( ifstream& inFile, vector< string >& sReturnData )
{
    string sBuffer, sParser;
    string sTexturePropPH, sMeshPropPH;
    vector< string > sPropertyData;
    string sPropertyIndicator;
    stringstream sStream( sBuffer );

    // Store Data for the segment
    sReturnData.clear();
    do
    {
        getline( inFile, sBuffer );                     // Next line of Data
        sBuffer = trimString( sBuffer );                // Clear any whitespace.
        sStream.str( sBuffer );                         // Parse next Line
        sStream.clear();                                // Clear Stream Flags
        while ( getline( sStream, sParser, ' ' ) )      // Extrapolate Data
        {
            if ( '+' == sParser[ 0 ] )
            {
                // Handle Property
                sPropertyIndicator = sParser.substr( 1, sParser.size() - 1 );   // Grab property identifier without the '+'
                sStream.str( "" );                                              // Clear rest of stream.
                sStream.clear();                                                // clear flags
                pullData( inFile, sPropertyData );                              // pull property information
                handleProperty( sPropertyData, sPropertyIndicator );            // store property internally
            }
            else if ("" != sParser)                         // Avoid Garbage
            {
                sReturnData.push_back( sParser );
            }
        }
    } while ( "}" != sReturnData.back() );                  // Repeat until end delimiter

                                                            // remove "}" from end of Data List
    sReturnData.pop_back();
}

void SceneLoader::handleData( vector< string >& sData, const string& sIndicator )
{
    if (SPHERE == sIndicator)                           // Parse Sphere
        createSphere(sData, sData.size());
    else if (PLANE == sIndicator)                       // Parse Plane
        createPlane(sData, sData.size());
    else if (POINT_LIGHT == sIndicator)                 // Parse Point Light
        createPointLight(sData, sData.size());
    else if (DIRECTIONAL_LIGHT == sIndicator)           // Parse Directional Light
        createDirectionalLight(sData, sData.size());
    else if (SPOTLIGHT == sIndicator)                   // Parse Spotlight
        createSpotLight(sData, sData.size());
    else if (PLAYER == sIndicator)                      // Parse Player
        createPlayer(sData, sData.size());
    else if (STATIC_MESH == sIndicator)                 // Parse Static Mesh
        createStaticMesh(sData, sData.size());

    clearProperties();
}

// Set up internal property storage with property to load objects with.
void SceneLoader::handleProperty( vector< string >& sData, const string& sIndicator )
{
    string sDataTrimmed = trimString( sData[ 0 ] );

    if (MATERIAL == sIndicator)
        grabMaterial(sData);
    else if (BOUNDING == sIndicator)
        grabBoundingBox(sData);
    else if (MESH == sIndicator)
    {
        m_sMeshProperty = sDataTrimmed;
        m_fMeshScaleProperty = sData.size() > 1 ? stof(sData[1]) : 1.0f;
    }
    else if (SHADER == sIndicator)
        m_sShaderProperty = sDataTrimmed;
}

// Grabs a Material from the given data
//    format for Material:
//        - sData[0] = string location for Diffuse Map (Required)
//        - sData[1] = (Optional) string location for Specular Map
//        - sData[1], sData[2], sData[3] = (Optional) float values for a generated specular map.
//        - sData[1]/sData[2]/sData[4] = (Required) float value for Shininess of Material.
// Possible sizes for sData = 
//        - 2: Diffuse Map and Shininess value. Specular Map defaults to 0.0
//        - 3: Diffuse Map, Spec Map location, Shininess Value.
//        - 5: Diffuse Map, Spec Map Shade, Shininess Value
void SceneLoader::grabMaterial(vector< string >& sData)
{
    switch (sData.size())
    {
    case 2:    // Diffuse Map and Shininess Value
        m_pMaterialProperty.sDiffuseMap = sData[0];
        m_pMaterialProperty.vOptionalSpecShade = vec4(0.0f);
        m_pMaterialProperty.fShininess = stof(sData[1]);
        break;
    case 3: // Diffuse Map, Spec Map location and Shininess Value
        m_pMaterialProperty.sDiffuseMap = sData[0];
        m_pMaterialProperty.sOptionalSpecMap = sData[1];
        m_pMaterialProperty.fShininess = stof(sData[2]);
        break;
    case 5: // Diffuse Map, Spec Map Shade and Shininess Value
        m_pMaterialProperty.sDiffuseMap = sData[0];
        m_pMaterialProperty.vOptionalSpecShade = vec4(stof(sData[1]), stof(sData[2]), stof(sData[3]), 0.0f);
        m_pMaterialProperty.fShininess = stof(sData[4]);
        break;
    default:
        cout << "Error: Double Check Material parameters:{ \n";
        for (vector<string>::iterator iter = sData.begin();
            iter != sData.end();
            ++iter)
        {
            cout << (*iter) << " ";
        }
        cout << "}\n";
        break;
    }
        
}

// Grabs the Bounding Box information from the string of data
//  Order of data (index):
//      0 - string identifier ("box")
//      1 - height  <float>
//      2 - width   <float>
//      3 - depth   <float>
void SceneLoader::grabBoundingBox(vector< string >& sData)
{
    // If no corresponding identifier is found, output Error
    if (BOUNDING_BOX_MAP.end() == BOUNDING_BOX_MAP.find(sData[0]))
        cout << "Error: \"" << sData[0] << "\" is not a valid Bounding Box type.\n";
    else
    {
        // Grab eNum via the string identifier read in from scene file.
        m_pBBProperty.eType = BOUNDING_BOX_MAP.at(sData[0]);

        // Handle Bounding Box Type
        switch (m_pBBProperty.eType)
        {
        case CUBIC_BOX:
            m_pBBProperty.vDimensions = vec3(stof(sData[1]), stof(sData[2]), stof(sData[3]));   // Dimensions of a Cubic Box
            break;
        default:    // Unsure how this happened, output an error message.
            cout << "Error: Double Check Bounding Box parameters:{ \n";
            for (vector<string>::iterator iter = sData.begin();
                iter != sData.end();
                ++iter)
            {
                cout << (*iter) << " ";
            }
            cout << "}\n";
            break;
        }
    }
}

// Removes any tabs from the beginning or end of a given string.
string SceneLoader::trimString( const string& sStr )
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
void SceneLoader::clearProperties() // Clear any properties
{
    // Clear Basic Properties
    m_sMeshProperty = m_sShaderProperty = "";
    m_fMeshScaleProperty = 1.0f;

    // Clear Material Properties
    m_pObjectProperties.sObjMaterial.fShininess = 0.0f;
     m_pObjectProperties.sObjMaterial.sDiffuseMap =  m_pObjectProperties.sObjMaterial.sOptionalSpecMap = "";
     m_pObjectProperties.sObjMaterial.vOptionalDiffuseColor = vec4(0.0f);
     m_pObjectProperties.sObjMaterial.vOptionalSpecShade = vec4(0.0f);

    // Clear Bounding Box Properties
    m_pBBProperty.eType = DEFAULT_TYPE;
    m_pBBProperty.vDimensions = vec3(0.0f);
}
