#pragma once
#include "stdafx.h"
#include "Object3D.h"
#include "Light.h"

// Environment Manager
// Manages all 3D objects in an environment
// Written by: James Coté
class EnvironmentManager
{
public:
	static EnvironmentManager* getInstance();
	~EnvironmentManager();

	void initializeEnvironment(string sFileName);
	void pause() { m_bPause = !m_bPause; }

	// Clears the Environment so a new one can be loaded.
	void purgeEnvironment();
	void killObject( long lID );
	void killLight( long lID );
	void listEnvironment();
	void renderEnvironment( const vec3& vCamLookAt );

	// Texture Manipulation
	void switchTexture( const string* sTexLocation, long lObjID );

	// Light Manipulation
	void moveLight(vec3 pMoveVec);
	mat4 getFrenetFrame();

	// Edge Threshold Getters/Setters
	void setMinThreshold( float fMin ) { m_fMinEdgeThreshold = fMin; }
	float getMinThreshold() { return m_fMinEdgeThreshold; }
	void setMaxThreshold( float fMax ) { m_fMaxEdgeThreshold = fMax; }
	float getMaxThreshold() { return m_fMaxEdgeThreshold; }

private:
	EnvironmentManager();
	EnvironmentManager(const EnvironmentManager* pCopy);
	static EnvironmentManager* m_pInstance;

	// Object Managing
	vector<Object3D*>	m_pObjects;
	vector<Light*>		m_pLights;
	Object* getObject( long lID );

	// Edge Threshold Implementation
	float m_fMinEdgeThreshold, m_fMaxEdgeThreshold;
	bool m_bPause;

	// Declare friend class for Objects and lights so they can add themselves to the manager.
	friend class Object3D;
	friend class Light;
	// Set-up Utility - Add an object to the Environment.
	// Accessable only by the manager and Objects themselves.
	void addObject( Object3D* pNewObject );
	void addLight( Light* pNewLight );
};

