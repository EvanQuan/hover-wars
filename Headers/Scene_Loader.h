#pragma once
#include "stdafx.h"

// Solely Generates Objects and assigns IDs to them.
class Scene_Loader
{
public:
	static Scene_Loader* getInstance();
	~Scene_Loader();

	// Creation Functions
	void createSphere( vector< string > sData, int iLength );
	void createPlane( vector< string > sData, int iLength );
	void createPointLight( vector< string > sData, int iLength );
	void createDirectionalLight(vector< string > sData, int iLength);
	void createSpotLight(vector< string > sData, int iLength);
	void createPlayer(vector< string > sData, int iLength);
	void createStaticMesh(vector< string > sData, int iLength);

	void loadFromFile( string sFileName );
	// .dll for freetype?
	// no, for glfw3

	// Where's the .dll?


private:
	// Singleton Implementation
	Scene_Loader();
	Scene_Loader( Scene_Loader* pCopy );
	static Scene_Loader* m_pInstance;
	string m_sMeshProperty, m_sShaderProperty;
	Material m_pMaterialProperty;

	long m_lNextID;
	long getNewID() { return ++m_lNextID; }
	void outputError( string sName, vector<string> sData );
	void pullData( ifstream& inFile, vector< string >& sReturnData );
	void handleData( vector< string >& sData, const string& sIndicator );
	void handleProperty( vector< string >& sData, const string& sIndicator );
	void grabMaterial(vector< string >& sData);
	void clearProperties(); // Clear any properties
	
	string trimString( const string& sStr );
};

