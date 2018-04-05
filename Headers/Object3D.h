#pragma once

#include "stdafx.h"
#include "ShaderManager.h"
#include "Object.h"
#include "EdgeBuffer.h"

#define MAX_SPECULAR_EXP 1000.f
#define REFLECTIVITY_MOD 0.0001f

class Object3D : public Object
{
public:
	~Object3D();

	// Pure Virtual Intersection Function
	// All inherited classes must implement their own
	//		logic for this function.
	virtual void draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause ) = 0;
	virtual string getType() = 0;
	virtual string getDebugOutput() = 0;

protected:
	// Protected Variables
	vector<vec3> m_pVertices;
	vector<vec3> m_pNormals;
	vector<vec2> m_pUVs;
	vector<vec3[ 3 ]> m_pFaces;

	GLuint m_iVertexArray, m_iVertexBuffer, m_iNormalBuffer, m_iTextureBuffer;

	virtual void calculateUVs() = 0;

	// Scale and Position Value
	float m_fScale;

	// Constructors only accessable by Object Factory.
	Object3D( const vec3* pPosition, long lID, 
			  const string* sTexName, const Anim_Track* pAnimTrack );
	Object3D( const Object3D* pCopy );
};

