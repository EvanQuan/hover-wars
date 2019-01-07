#pragma once
#include "stdafx.h"
#include "Object3D.h"
#include "Light.h"
#include "BoidEngine.h"
#include "Entity.h"
#include "Camera.h"
#include "EntityComponentHeaders/EntityComponent.h"
#include "EntityComponentHeaders/CameraComponent.h"


// Environment Manager
// Manages all 3D objects in an environment
// Written by: James Coté
class EntityManager
{
public:
	static EntityManager* getInstance();
	~EntityManager();

	void initializeEnvironment(string sFileName);
	void pause() { m_bPause = !m_bPause; }

	// Entity Functions
	Camera* generateCameraEntity();
	vec3 getEntityPosition(int iEntityID);

	// Entity Component functions
	CameraComponent* generateCameraComponent(int iEntityID);

	// Get Camera Component
	CameraComponent* getActiveCamera() { return m_pActiveCamera; }

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

	// Boid Methods
	void initializeBoidEngine(vector< string >& sData);

private:
	EntityManager();
	EntityManager(const EntityManager* pCopy);
	static EntityManager* m_pInstance;

	// Object Managing
	vector<Object3D*>	m_pObjects;
	vector<Light*>		m_pLights;
	Object* getObject( long lID );
	BoidEngine* m_pBoidEngine;

	// Entity Managing
	int m_iEntityIDPool, m_iComponentIDPool;
	inline int getNewEntityID() { return ++m_iEntityIDPool; }
	inline int getNewComponentID() { return ++m_iComponentIDPool; }
	vector<unique_ptr<EntityComponent>>	m_pMasterComponentList;
	vector<unique_ptr<Entity>>	m_pMasterEntityList;
	vector<EntityComponent*>	m_pRenderingComponents;
	vector<CameraComponent*>	m_pCameraComponents;
	CameraComponent*			m_pActiveCamera;

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

