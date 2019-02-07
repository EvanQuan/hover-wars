// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include "EnvSpec.h"
#include "GameTime.h"


#ifdef USING_LINUX
#include <string.h>
#include <memory>
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#endif
#ifdef USING_WINDOWS
#include <string>
#include <glew.h>
#endif
#include <glfw3.h>

#define ERR_CODE -1

/* NAMESPACES */
using namespace std;
using namespace glm;

/* DEFINES */
#define INPUT_SIZE			128
#define START_HEIGHT		1080
#define START_WIDTH			1920
#define STARTING_ENV		"scene2.txt"
#define LIGHT_MOVE_FACTOR	0.05f
#define PI					3.14159265f

/* GLOBAL STRUCTS */
struct Material
{
	string sDiffuseMap;
	string sOptionalSpecMap;
	vec4 vOptionalSpecShade;
	float fShininess;
};


/* Manager Defines */
#define ENTITY_MANAGER	EntityManager::getInstance()
#define SHADER_MANAGER	ShaderManager::getInstance()
#define MESH_MANAGER	MeshManager::getInstance()
#define TEXTURE_MANAGER TextureManager::getInstance()
#define SCENE_LOADER	Scene_Loader::getInstance()

// From Boilerplate code,
// Shouldn't need to modify this.
// Accessible anywhere stdafx.h is included and GL Error Checking is required.
inline bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}
