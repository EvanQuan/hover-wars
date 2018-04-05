#pragma once
#include "stdafx.h"
#include "Object.h"

// Infinitely small point that emits a light source.
//	Managed by the Environment Manager.
class Light :
	 public Object
{
public:
	~Light();

	// Virtual inherited functions
	void draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause );
	string getType() { return "Light"; }

	// Light Manipulation
	void move( vec3 pMoveVec ) { m_pPosition += pMoveVec; }

	// Ouptut Debug information for Light
	string getDebugOutput()
	{
		string sOutput = "";

		sOutput += "Light";
		sOutput += "/ID:" + to_string( m_lID );
		sOutput += "/Position:" + glm::to_string(m_pPosition);
		sOutput += "/Color:" + glm::to_string(m_pColor);

		return sOutput;
	}

private:
	// Protected Constructors, only accessible by the Object Factory.
	Light( const glm::vec3* pPos,
		   const glm::vec3* pColor,
		   long lID, const string* sTexName, const Anim_Track* pAnimTrack );
	Light( const Light* newLight );

	// Private Variables
	vec3 m_pColor;
	GLuint m_iVertexArray, m_iVertexBuffer;
	
	// Shake hands with the Object Factory so that Lights can only be made from the Object Factory.
	friend class Object_Factory;
};

