#pragma once
#include "stdafx.h"
#include "DataStructures/ObjectInfo.h"

/************************\
 * Forward Declarations *
\************************/
class Rocket;
class Spikes;
class EntityManager;

// Solely Generates Objects and assigns IDs to them.
class SceneLoader final
{
public:
    static SceneLoader* getInstance();
    ~SceneLoader();

    // Creation Functions
    void createSphere( vector< string > sData, int iLength );
    void createPlane( vector< string > sData, int iLength );
    void createPointLight( vector< string > sData, int iLength );
    void createDirectionalLight(vector< string > sData, int iLength);
    void createSpotLight(vector< string > sData, int iLength);
    void createPlayer(vector< string > sData, int iLength);
    void createCube(vector< string > sData, int iLength);
    void createBot(vector< string > sData, int iLength);
    void createStaticMesh(vector< string > sData, unsigned int iLength);
    Rocket* createRocketMesh(int iOwnerID, eHovercraft eOwnerHovercraft);
    Spikes* createSpikesMesh(int iOwnerID, eHovercraft eOwnerHovercraft);
    void initializeSpatialMap(vector< string > sData, unsigned int iLength);
    void loadFromFile( string sFileName );

private:
    // Singleton Implementation
    SceneLoader();
    SceneLoader( SceneLoader* pCopy );
    static SceneLoader* m_pInstance;
    EntityManager* m_pEntityManager;

    enum ePropertyTypes
    {
        CURRENT_PROPERTIES = 0,
        ROCKET_PROPERTIES,
        HC_PROPERTIES,
        SPIKES_PROPERTIES,
        MAX_PROPERTIES
    };

    // Structure for Mesh Properties
    struct sMeshProperties
    {
        // Variables for Structure
        bool        bMeshSaved;
        string      sMeshLocation, sShaderProperty;
        float       fScaleProperty;
        ObjectInfo  pObjectProperties;
        

        // Default Constructor
        sMeshProperties()
        {
            resetProperties();
        }

        // Reset the Properties to Default Settings
        void resetProperties()
        {
            bMeshSaved = false;
            sMeshLocation = sShaderProperty = "";
            fScaleProperty = 1.0f;
            pObjectProperties.loadDefaults();
        }

        // Assignment operator overload for Copying Structs
        sMeshProperties& operator=(const sMeshProperties& pCopy)
        {
            bMeshSaved          = pCopy.bMeshSaved;
            sMeshLocation       = pCopy.sMeshLocation;
            sShaderProperty     = pCopy.sShaderProperty;
            fScaleProperty      = pCopy.fScaleProperty;
            pObjectProperties   = pCopy.pObjectProperties;

            return *this;
        }
    };
    sMeshProperties m_sProperties[MAX_PROPERTIES];

    // Private Functions
    void outputError( string sName, vector<string> sData );
    void pullData( ifstream& inFile, vector< string >& sReturnData );
    void handleData( vector< string >& sData, const string& sIndicator );
    void handleProperty( vector< string >& sData, const string& sIndicator );
    void grabMaterial(vector< string >& sData);
    void grabBoundingBox(vector< string >& sData);
    void saveRocketInfo();
    void saveSpikesInfo();
    void clearProperties();     // Clear any properties
    void resetAllProperties();
    
    string trimString( const string& sStr );
};

