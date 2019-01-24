#pragma once

#include "stdafx.h"
#include "Anim_Track.h"

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
	void createPlayer(vector< string > sData, int iLength);
	void createStaticMesh(vector< string > sData, int iLength);

	void loadFromFile( string sFileName );

private:
	// Singleton Implementation
	Scene_Loader();
	Scene_Loader( Scene_Loader* pCopy );
	static Scene_Loader* m_pInstance;
	string m_sTextureProperty, m_sMeshProperty, m_sShaderProperty;
	Anim_Track* m_pAnimProperty;

	long m_lNextID;
	long getNewID() { return ++m_lNextID; }
	void outputError( string sName, vector<string> sData );
	void pullData( ifstream& inFile, vector< string >& sReturnData );
	void handleData( vector< string >& sData, const string& sIndicator );
	void handleProperty( vector< string >& sData, const string& sIndicator );
	void clearProperties(); // Clear any properties
	
	void saveProperties( string& sTextureProperty, string& sMeshProperty, Anim_Track* pAnimTrackProp )
	{
		sTextureProperty = m_sTextureProperty;
		sMeshProperty = m_sMeshProperty;
		pAnimTrackProp = m_pAnimProperty;

	}
	void restoreProperties( const string& sTextureProperty, const string& sMeshProperty, Anim_Track* pAnim )
	{
		m_sTextureProperty = (sTextureProperty == "") ? m_sTextureProperty : sTextureProperty;
		m_sMeshProperty = (sMeshProperty == "") ? m_sMeshProperty : sMeshProperty;
		m_pAnimProperty = (nullptr == pAnim) ? m_pAnimProperty : pAnim;
	}
	string trimString( const string& sStr );
};

