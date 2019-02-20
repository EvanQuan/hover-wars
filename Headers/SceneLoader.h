#pragma once
#include "stdafx.h"
#include "DataStructures/ObjectInfo.h"

// Solely Generates Objects and assigns IDs to them.
class SceneLoader
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
    void createBot(vector< string > sData, int iLength);
    void createStaticMesh(vector< string > sData, unsigned int iLength);
    void createStaticCube(vector< string > sData, int iLength);
    void initializeSpatialMap(vector< string > sData, unsigned int iLength);
    void loadFromFile( string sFileName );

private:
    // Singleton Implementation
    SceneLoader();
    SceneLoader( SceneLoader* pCopy );
    static SceneLoader* m_pInstance;
    string m_sMeshProperty, m_sShaderProperty;
    float m_fMeshScaleProperty;
    ObjectInfo m_pObjectProperties;

    long m_lNextID;
    long getNewID() { return ++m_lNextID; }
    void outputError( string sName, vector<string> sData );
    void pullData( ifstream& inFile, vector< string >& sReturnData );
    void handleData( vector< string >& sData, const string& sIndicator );
    void handleProperty( vector< string >& sData, const string& sIndicator );
    void grabMaterial(vector< string >& sData);
    void grabBoundingBox(vector< string >& sData);
    void clearProperties(); // Clear any properties
    
    string trimString( const string& sStr );
};

