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
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include "GameTime.h"
#include <string>
#include <glew.h>
#include <glfw3.h>
#include <random>


// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// PhysX
#include "PxPhysicsAPI.h"

// Game Enums
#include "Enums/eAbility.h"
#include "Enums/ePlayer.h"
#include "Enums/ePowerup.h"
#include "Enums/eFixedCommand.h"
#include "Enums/eVariableCommand.h"
#include "Enums/eBoundingBoxTypes.h"
#include "Enums/eEntityTypes.h"
#include "Enums/eCooldown.h"

// Custom Data Structures
#include "DataStructures/Bag.h"

// Function Utilities
#include "Utils/FuncUtils.h"

#define ERR_CODE -1

/* NAMESPACES */
using namespace std;
using namespace glm;
using namespace physx;

/* DEFINES */
#define INPUT_SIZE          128
#define RELEASE_ENV         "Scenes/release.scene" // file path
#define DEBUG_ENV           "Scenes/debug.scene"   // file path
#define LIGHT_MOVE_FACTOR   0.05f
#define PI                  3.14159265f
#define PI_2                6.28318530718f
#define DEGREES_TO_RADIANS  0.01745329251f

#define KEYS                349
#define MAX_PLAYER_JOYSTICK GLFW_JOYSTICK_4
#define MAX_PLAYER_COUNT    4
#define MAX_BOT_COUNT       4
#define MAX_HOVERCRAFT_COUNT MAX_PLAYER_COUNT + MAX_BOT_COUNT
#define XBOX_CONTROLLER     "Xbox"
#define EMPTY_CONTROLLER    "Empty Controller"
// m_pJoystickAxes
#define AXIS_LEFT_STICK_X   0
#define AXIS_LEFT_STICK_Y   1
#define AXIS_RIGHT_STICK_X  2
#define AXIS_RIGHT_STICK_Y  3
#define AXIS_LEFT_TRIGGER   4
#define AXIS_RIGHT_TRIGGER  5
// m_joystickButtons
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
#define MAX_BUTTON_INDEX    14
// XBox controllers seem to have 2 more buttons (14, 15)
// I have not been able to found out what these buttons actually represent.
// Instead we will use the triggers as button states
// so they can detect just pressed states
#define TRIGGER_LEFT        14
#define TRIGGER_RIGHT       15
//
#define MAX_BUTTON_COUNT    16
// Joystick/trigger values
#define JOYSTICK_IS_MAX     1.0f
#define JOYSTICK_IS_MIN     -1.0f
#define JOYSTICK_IS_NEUTRAL 0.0f
#define TRIGGER_IS_NETURAL  -1.0f
#define TRIGGER_IS_FULL     1.0f

/*
Actor names

Interactable entities have actor names so the game can discern what type of
object they are. This is especially important for collisions, as collisions
between different kinds of entities determine the results.

Actor names are composed of 2 characters. The order of tokens are:

1. Type

2. Subtype or Owner
*/
// Type
#define TYPE_WORLD      "w"
#define TYPE_POWERUP    "p"
#define TYPE_TRAIL      "t"
#define TYPE_PLAYER     "P"
#define TYPE_HOVERCRAFT "h"
#define TYPE_BOT        "b"
#define TYPE_ROCKET     "r"
#define TYPE_SPIKES     "s"

#define C_TYPE_WORLD      'w'
#define C_TYPE_POWERUP    'p'
#define C_TYPE_TRAIL      't'
#define C_TYPE_PLAYER     'P'
#define C_TYPE_BOT        'b'
#define C_TYPE_HOVERCRAFT 'h'
#define C_TYPE_ROCKET     'r'
#define C_TYPE_SPIKES     's'

// Subtype
#define SUBTYPE_WALL          "W"
#define SUBTYPE_GROUND        "g"
#define SUBTYPE_POWERUP_SPEED "S"

#define C_SUBTYPE_WALL          'W'
#define C_SUBTYPE_GROUND        'g'
#define C_SUBTYPE_POWERUP_SPEED 'S'

// Owner
#define OWNER_PLAYER_1 "1"
#define OWNER_PLAYER_2 "2"
#define OWNER_PLAYER_3 "3"
#define OWNER_PLAYER_4 "4"
#define OWNER_BOT_1    "5"
#define OWNER_BOT_2    "6"
#define OWNER_BOT_3    "7"
#define OWNER_BOT_4    "8"

#define C_OWNER_PLAYER_1 '1'
#define C_OWNER_PLAYER_2 '2'
#define C_OWNER_PLAYER_3 '3'
#define C_OWNER_PLAYER_4 '4'
#define C_OWNER_BOT_1    '5'
#define C_OWNER_BOT_2    '6'
#define C_OWNER_BOT_3    '7'
#define C_OWNER_BOT_4    '8'

// Don't use these
#define NAME_PLAYER  "p"
#define NAME_BOT     "b"
#define NAME_ROCKET  "r"
#define NAME_TRAIL   "t"
#define NAME_SPIKES  "s"
#define NAME_WALL    "w"
#define NAME_GROUND  "g"

// Don't use these
#define NAME_SPHERE  "sphere"
#define NAME_MESH    "mesh"


// Mapping potential types from the scene loader to corresponding enums
const std::unordered_map<string, eBoundingBoxTypes> BOUNDING_BOX_MAP =
{
    make_pair("box", CUBIC_BOX)
};

/* Manager Defines */
#define EMITTER_ENGINE      EmitterEngine::getInstance()
#define ENTITY_MANAGER      EntityManager::getInstance()
#define GAME_MANAGER        GameManager::getInstance()
#define GAME_STATS          GameStats::getInstance()
#define MESH_MANAGER        MeshManager::getInstance()
#define PHYSICS_MANAGER     PhysicsManager::getInstance()
#define SCENE_LOADER        SceneLoader::getInstance()
#define SHADER_MANAGER      ShaderManager::getInstance()
#define SPATIAL_DATA_MAP    SpatialDataMap::getInstance()
#define SOUND_MANAGER       SoundManager::getInstance()
#define TEXTURE_MANAGER     TextureManager::getInstance()
#define USER_INTERFACE      UserInterface::getInstance()

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
