#include "SceneLoader.h"
#include "EntityManager.h"
#include <sstream>
#include <iterator>

/***********\
* Defines *
\***********/
#define CURRENT_PROPERTIES_DEF     SceneLoader::m_sProperties[SceneLoader::CURRENT_PROPERTIES]
#define ROCKET_PROPERTIES_DEF      SceneLoader::m_sProperties[SceneLoader::ROCKET_PROPERTIES]
#define SPIKES_PROPERTIES_DEF      SceneLoader::m_sProperties[SceneLoader::SPIKES_PROPERTIES]
#define HOVERCRAFT_PROPERTIES_DEF  SceneLoader::m_sProperties[SceneLoader::HC_PROPERTIES]

#define MAX_CHARS_PER_LINE     256
#define MAX_SPHERE_PARAMS      4
#define MAX_CUBE_PARAMS        6
#define MAX_PLANE_PARAMS       8
#define MAX_POINT_LIGHT_PARAMS 7
#define MAX_DIR_LIGHT_PARAMS   18
#define MAX_SPOTLIGHT_PARAMS   10
#define DEFAULT_SOFT_CUTOFF    5.f
#define MAX_MESH_PARAMS        3
#define MAX_TRACK_PARAMS       1
#define MAX_SPATIAL_MAP_PARAMS 3
#define COMMENT_CHAR           '#'
#define SPHERE                 "sphere"
#define PLANE                  "plane"
#define POINT_LIGHT            "point_light"
#define DIRECTIONAL_LIGHT      "directional_light"
#define SPOTLIGHT              "spotlight"
#define HC                     "hovercraft"
#define PLAYER_1               "player_1"
#define PLAYER_2               "player_2"
#define PLAYER_3               "player_3"
#define PLAYER_4               "player_4"
#define STATIC_MESH            "static_mesh"
#define SPATIAL_MAP            "spatial_map"
#define BOIDS                  "boids"
#define MATERIAL               "material"
#define BOUNDING               "bounding"
#define MESH                   "mesh"
#define CUBE                   "cube"
#define SHADER                 "shader"
#define CUBE                   "cube"
#define NAME                   "name"
#define ROCKET                 "rocket"
#define SPIKES                 "spikes"
#define SPAWNS                 "spawn_points"
#define SKYBOX                 "skybox"

// Singleton Declaration
SceneLoader* SceneLoader::m_pInstance = nullptr;

// Private Constructor
SceneLoader::SceneLoader()
{
    resetAllProperties();
}

// Returns the singleton instance of the Object Factory
SceneLoader* SceneLoader::getInstance()
{
    if (nullptr == m_pInstance)
        m_pInstance = new SceneLoader();

    return m_pInstance;
}


SceneLoader::~SceneLoader()
{
}

// Creation Functions
// Create a Sphere given a starting position, color and radius
void SceneLoader::createSphere( vector< string > sData, int iLength )
{
    // Create the Sphere if we're given the correct # of values
    if ( iLength == MAX_SPHERE_PARAMS )
    {
        CURRENT_PROPERTIES_DEF.pObjectProperties.vPosition = glm::vec3( stof( sData[ 0 ] )/*X*/, stof( sData[ 1 ] )/*Y*/, stof( sData[ 2 ] )/*Z*/ );    // Position of Sphere

        m_pEntityManager->generateStaticSphere(&CURRENT_PROPERTIES_DEF.pObjectProperties, stof(sData[3]), CURRENT_PROPERTIES_DEF.sShaderProperty);
    }
    else
        outputError(SPHERE, sData);
}

// Create a Plane given a normal, a position on the plane and a color
void SceneLoader::createPlane( vector< string > sData, int iLength )
{
    // Local Parameters to pull out Plane Data.
    vec3 vNormal;
    int iHeight, iWidth;

    if (iLength == MAX_PLANE_PARAMS)
    {
        CURRENT_PROPERTIES_DEF.pObjectProperties.vPosition = glm::vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);        // Position of Plane
        vNormal = vec3(stof(sData[3]),/*X*/ stof(sData[4]),/*Y*/ stof(sData[5]));
        iHeight = stoi(sData[6]);
        iWidth = stoi(sData[7]);
        m_pEntityManager->generateStaticPlane(&CURRENT_PROPERTIES_DEF.pObjectProperties, iHeight, iWidth, &vNormal, CURRENT_PROPERTIES_DEF.sShaderProperty);
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
    float fPosition, fNearPlane, fFarPlane, fShadowFrame;
    unsigned int iShadowHeight, iShadowWidth;

    if ( MAX_DIR_LIGHT_PARAMS == iLength )
    {
        vDirection      = normalize( vec3( stof( sData[ 0 ] )/*dX*/, stof( sData[ 1 ] )/*dY*/, stof( sData[ 2 ] )/*dZ*/ ) );
        vAmbientColor   = vec3( stof( sData[ 3 ] )/*aR*/, stof( sData[ 4 ] )/*aG*/, stof( sData[ 5 ] )/*aB*/ );
        vDiffuseColor   = vec3(stof(sData[ 6 ])/*aR*/, stof(sData[ 7 ])/*aG*/, stof(sData[ 8 ])/*dB*/);
        vSpecularColor  = vec3(stof(sData[ 9 ])/*sR*/, stof(sData[ 10 ])/*sG*/, stof(sData[ 11 ])/*sB*/);
        fPosition       = stof(sData[12]);
        fNearPlane      = stof(sData[13]);
        fFarPlane       = stof(sData[14]);
        iShadowHeight   = stoi(sData[15]);
        iShadowWidth    = stoi(sData[16]);
        fShadowFrame    = stof(sData[17]);

        m_pEntityManager->generateDirectionalLight(&vDirection, &vAmbientColor, &vDiffuseColor, &vSpecularColor,
                                                 fPosition, fNearPlane, fFarPlane, iShadowHeight, iShadowWidth, fShadowFrame);
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
    vec3 pColor;
    float fPower;

    if (MAX_POINT_LIGHT_PARAMS == iLength)
    {
        CURRENT_PROPERTIES_DEF.pObjectProperties.vPosition = vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);
        pColor = vec3(stof(sData[3])/*R*/, stof(sData[4])/*G*/, stof(sData[5])/*B*/);
        fPower = stof(sData[6])/*P*/;

        // Set the Diffuse Color of the Material
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.vOptionalDiffuseColor = vec4(pColor * fPower, 1.0);

        m_pEntityManager->generateStaticPointLight( &CURRENT_PROPERTIES_DEF.pObjectProperties, fPower, &pColor, CURRENT_PROPERTIES_DEF.sMeshLocation, CURRENT_PROPERTIES_DEF.fScaleProperty);
    }
    else
        outputError(POINT_LIGHT, sData);
}

// Generates a SpotLight object with a Position, Direction, Color and Phi angle of the spotlight.
//    There is also an optional soft edge cutoff able to be specified to simulate soft edges for the spotlight.
void SceneLoader::createSpotLight(vector< string > sData, int iLength)
{
    vec3 vDirection, vColor;
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
        CURRENT_PROPERTIES_DEF.pObjectProperties.vPosition = vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);
        vDirection = vec3( stof(sData[3])/*dX*/, stof(sData[4])/*dY*/, stof(sData[5])/*dZ*/);
        vColor = vec3( stof(sData[6])/*R*/, stof(sData[7])/*G*/, stof(sData[8])/*B*/);

        // Set the Diffuse color of the material.
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.vOptionalDiffuseColor = vec4(vColor, 1.0);

        m_pEntityManager->generateStaticSpotLight(&CURRENT_PROPERTIES_DEF.pObjectProperties, stof(sData[9]), fSoftCutoff, &vColor, &vDirection, CURRENT_PROPERTIES_DEF.sMeshLocation, CURRENT_PROPERTIES_DEF.fScaleProperty);
    }
    else
    {
        outputError(SPOTLIGHT, sData);
    }
}

// Generates a Cube from the given data from the file.
void SceneLoader::createCube(vector< string > sData, int iLength)
{
    vec3 vDimensions;

    if (MAX_CUBE_PARAMS == iLength)
    {
        CURRENT_PROPERTIES_DEF.pObjectProperties.vPosition = vec3(stof(sData[0])/*X*/, stof(sData[1])/*Y*/, stof(sData[2])/*Z*/);
        vDimensions = vec3(stof(sData[3])/*Height*/, stof(sData[4])/*Width*/, stof(sData[5])/*Depth*/);

        m_pEntityManager->generateStaticCube(&CURRENT_PROPERTIES_DEF.pObjectProperties, &vDimensions, CURRENT_PROPERTIES_DEF.sShaderProperty);
    }
    else
        outputError(CUBE, sData);
}

// Generates a Player Object at a given position
// NOTE: This is a temporary testing tool, it may not be possible in the final version of the game to generate this
//        object from a scene file.
vec3 SceneLoader::createPlayer(unsigned int iPlayerNumber, const vec3* vColor)
{
    getNextSpawnPoint(&m_sProperties[PLAYER_1_PROPERTIES + iPlayerNumber].pObjectProperties.vPosition);
    m_pEntityManager->generatePlayerEntity(&m_sProperties[PLAYER_1_PROPERTIES + iPlayerNumber].pObjectProperties, m_sProperties[PLAYER_1_PROPERTIES + iPlayerNumber].sMeshLocation, vColor, m_sProperties[PLAYER_1_PROPERTIES + iPlayerNumber].fScaleProperty, m_sProperties[PLAYER_1_PROPERTIES + iPlayerNumber].sShaderProperty);

    // Return starting position
    return m_sProperties[PLAYER_1_PROPERTIES + iPlayerNumber].pObjectProperties.vPosition;
}

// Generates a Bot Object at a given position
// NOTE: This is a temporary testing tool, it may not be possible in the final version of the game to generate this
//        object from a scene file.
vec3 SceneLoader::createBot(const vec3* vColor)
{
    getNextSpawnPoint(&HOVERCRAFT_PROPERTIES_DEF.pObjectProperties.vPosition);
    m_pEntityManager->generateBotEntity(&HOVERCRAFT_PROPERTIES_DEF.pObjectProperties, HOVERCRAFT_PROPERTIES_DEF.sMeshLocation, vColor, HOVERCRAFT_PROPERTIES_DEF.fScaleProperty, HOVERCRAFT_PROPERTIES_DEF.sShaderProperty);

    // Return starting position
    return HOVERCRAFT_PROPERTIES_DEF.pObjectProperties.vPosition;
}
// Generates a Static Mesh Object at a specified location.
void SceneLoader::createStaticMesh(vector< string > sData, unsigned int iLength)
{
    // Add a number of Static Meshes with the same Properties to all specified positions.
    for (unsigned int i = 0; i + 3 <= iLength; i += 3)
    {
        CURRENT_PROPERTIES_DEF.pObjectProperties.vPosition = vec3(stof(sData[i])/*X*/, stof(sData[i + 1])/*Y*/, stof(sData[i + 2])/*Z*/);    // Position of Mesh
        m_pEntityManager->generateStaticMesh(&CURRENT_PROPERTIES_DEF.pObjectProperties, CURRENT_PROPERTIES_DEF.sMeshLocation, CURRENT_PROPERTIES_DEF.fScaleProperty, CURRENT_PROPERTIES_DEF.sShaderProperty);
    }
}

// Generates a Rocket Entity based off the saved Rocket Properties loaded from the Scene.
Rocket* SceneLoader::createRocketMesh(int iOwnerID, const vec3* vColor)
{
    return m_pEntityManager->generateRocketEntity(&ROCKET_PROPERTIES_DEF.pObjectProperties, &ROCKET_PROPERTIES_DEF.sMeshLocation, vColor, ROCKET_PROPERTIES_DEF.fScaleProperty, &ROCKET_PROPERTIES_DEF.sShaderProperty, iOwnerID);
}

// Generates a Spikes Entity based off the saved Spikes Properties loaded from the Scene.
Mesh* SceneLoader::createSpikesMesh(string sHashKey)
{
    return MESH_MANAGER->loadMeshFromFile(SPIKES_PROPERTIES_DEF.sMeshLocation, &SPIKES_PROPERTIES_DEF.pObjectProperties, sHashKey,  SPIKES_PROPERTIES_DEF.fScaleProperty);
}

void SceneLoader::createSkybox(vector< string > sData)
{
    m_pEntityManager->loadSkyBox(&sData);
}

// Initializes the Spatial Data Map for the scene
void SceneLoader::initializeSpatialMap(vector< string > sData, unsigned int iLength)
{
    if (MAX_SPATIAL_MAP_PARAMS == iLength)
        m_pEntityManager->initializeSpatialMap(stof(sData[0]) /*Length*/, stof(sData[1]) /*Width*/, stof(sData[2]) /*Tile Size*/);
    else
        outputError(SPATIAL_MAP, sData);
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

    // Store Reference to Entity Manager
    if (nullptr == m_pEntityManager)
        m_pEntityManager = ENTITY_MANAGER;

    // Open File
    inFile.open( sFileName );

    // Parse File if opened properly
    if ( inFile.good() )
    {
        // Reset all Properties first
        resetAllProperties();

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
            #ifdef _DEBUG
                // cout << sIndicator << endl;
                //  for ( unsigned int d = 0; d < sData.size(); ++d )
                    // cout << sData[ d ] << endl;
                // cout << "\t\t END LINE" << endl;
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
    } while ( sReturnData.empty() || "}" != sReturnData.back() );                  // Repeat until end delimiter

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
    else if (HC == sIndicator)                          // Save Hovercraft Information
        HOVERCRAFT_PROPERTIES_DEF = CURRENT_PROPERTIES_DEF;
    else if (PLAYER_1 == sIndicator)                          // Save Hovercraft Information
        m_sProperties[PLAYER_1_PROPERTIES] = CURRENT_PROPERTIES_DEF;
    else if (PLAYER_2 == sIndicator)                          // Save Hovercraft Information
        m_sProperties[PLAYER_2_PROPERTIES] = CURRENT_PROPERTIES_DEF;
    else if (PLAYER_3 == sIndicator)                          // Save Hovercraft Information
        m_sProperties[PLAYER_3_PROPERTIES] = CURRENT_PROPERTIES_DEF;
    else if (PLAYER_4 == sIndicator)                          // Save Hovercraft Information
        m_sProperties[PLAYER_4_PROPERTIES] = CURRENT_PROPERTIES_DEF;
    else if (STATIC_MESH == sIndicator)                 // Parse Static Mesh
        createStaticMesh(sData, sData.size());
    else if (SPATIAL_MAP == sIndicator)                 // Parse Spatial Data Map Information
        initializeSpatialMap(sData, sData.size());
    else if (CUBE == sIndicator)                        // Parse Cube
        createCube(sData, sData.size());
    else if (ROCKET == sIndicator)                      // Parse Rocket information
        ROCKET_PROPERTIES_DEF = CURRENT_PROPERTIES_DEF;
    else if (SPIKES == sIndicator)                      // Parse Spikes information
        SPIKES_PROPERTIES_DEF = CURRENT_PROPERTIES_DEF;
    else if (SPAWNS == sIndicator)                      // Parse Spawn Point Information
        saveSpawnPoint(sData, sData.size());
    else if (SKYBOX == sIndicator)                      // Skybox Information
        createSkybox(sData);

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
        CURRENT_PROPERTIES_DEF.sMeshLocation = sDataTrimmed;
        CURRENT_PROPERTIES_DEF.fScaleProperty = sData.size() > 1 ? stof(sData[1]) : 1.0f;
    }
    else if (SHADER == sIndicator)
        CURRENT_PROPERTIES_DEF.sShaderProperty = sDataTrimmed;
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
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.sDiffuseMap = sData[0];
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.vOptionalSpecShade = vec4(0.0f);
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.fShininess = stof(sData[1]);
        break;
    case 3: // Diffuse Map, Spec Map location and Shininess Value
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.sDiffuseMap = sData[0];
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.sOptionalSpecMap = sData[1];
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.fShininess = stof(sData[2]);
        break;
    case 5: // Diffuse Map, Spec Map Shade and Shininess Value
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.sDiffuseMap = sData[0];
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.vOptionalSpecShade = vec4(stof(sData[1]), stof(sData[2]), stof(sData[3]), 0.0f);
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjMaterial.fShininess = stof(sData[4]);
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
        CURRENT_PROPERTIES_DEF.pObjectProperties.sObjBoundingBox.eType = BOUNDING_BOX_MAP.at(sData[0]);

        // Handle Bounding Box Type
        switch (CURRENT_PROPERTIES_DEF.pObjectProperties.sObjBoundingBox.eType)
        {
        case CUBIC_BOX:
            CURRENT_PROPERTIES_DEF.pObjectProperties.sObjBoundingBox.vDimensions = vec3(stof(sData[1])/*Height*/, stof(sData[2])/*Length*/, stof(sData[3])/*Depth*/);   // Dimensions of a Cubic Box
            break;
        case SPATIAL_CALC:
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
    CURRENT_PROPERTIES_DEF.resetProperties();
}

// Clear All Properties, designed to be called before loading a new scene
void SceneLoader::resetAllProperties() 
{
    for( unsigned int eIndex = 0; eIndex < MAX_PROPERTIES; ++eIndex )
        m_sProperties[eIndex].resetProperties();

    m_vSpawnPoints.clear();

    spawnIndex = 0;
}

/************************************************************************\
 * Spawn Points Functionality                                           *
\************************************************************************/
void SceneLoader::saveSpawnPoint(vector< string > sData, int iLength)
{
    for (int i = 0; i < iLength - 3; i += 3)
        m_vSpawnPoints.push_back(vec3(stoi(sData[i]), stoi(sData[i + 1]), stoi(sData[i + 2])));
}

void SceneLoader::getNextSpawnPoint(vec3* vPosition)
{
    spawnIndex = FuncUtils::addModulo(spawnIndex, 1, 0, m_vSpawnPoints.size() - 1);
    *vPosition = m_vSpawnPoints[spawnIndex];
}
