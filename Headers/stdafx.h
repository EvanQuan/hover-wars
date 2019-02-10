// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include "EnvSpec.h"
#include "GameTime.h"
#include "Physics/PhysicsManager.h"

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// FTGL
// #include <FTGL/ftgl.h>

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
using namespace physx;

/* DEFINES */
#define INPUT_SIZE			128
#define START_HEIGHT		1080
#define START_WIDTH			1920
#define STARTING_ENV		"scene2.scene"
#define LIGHT_MOVE_FACTOR	0.05f
#define PI					3.14159265f
#define PI_2				6.28318530718f
#define PI_OVER_180			0.01745f

#define KEYS 349
#define MAX_PLAYER_COUNT GLFW_JOYSTICK_4
#define XBOX_CONTROLLER "Xbox"
#define EMPTY_CONTROLLER "Empty Controller"
// m_pJoystickAxes
#define AXIS_LEFT_STICK_X   0
#define AXIS_LEFT_STICK_Y   1
#define AXIS_RIGHT_STICK_X  2
#define AXIS_RIGHT_STICK_Y  3
#define AXIS_LEFT_TRIGGER   4
#define AXIS_RIGHT_TRIGGER  5
// m_pJoystickButtons
#define BUTTON_A            0
#define BUTTON_B            1
#define BUTTON_X            2
#define BUTTON_Y            3
#define BUTTON_LEFT_BUMPER  4
#define BUTTON_RIGHT_BUMPER 5
#define BUTTON_BACK         6
#define BUTTON_START        7
#define BUTTON_LEFT_STICK   8
#define BUTTON_RIGHT_STICK  9
#define BUTTON_UP           10
#define BUTTON_RIGHT        11
#define BUTTON_DOWN         12
#define BUTTON_LEFT         13
#define BUTTON_UNKNOWN1     14
#define BUTTON_UNKNOWN2     15
// TODO What are buttons 14, 15?
// Is it the centre xbox button (home)?
// Joystick/trigger values
#define JOYSTICK_MAX        1.0f
#define JOYSTICK_MIN        -1.0f
#define JOYSTICK_NEUTRAL    0.0f
#define TRIGGER_NETURAL     -1.0f
#define TRIGGER_FULL        1.0f


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
#define GAME_MANAGER	GameManager::getInstance()
#define MESH_MANAGER	MeshManager::getInstance()
#define SHADER_MANAGER	ShaderManager::getInstance()
#define TEXTURE_MANAGER TextureManager::getInstance()
#define PHYSICS_MANAGER PhysicsManager::getInstance()
#define SCENE_LOADER	Scene_Loader::getInstance()
#define EMITTER_ENGINE	EmitterEngine::getInstance()

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
