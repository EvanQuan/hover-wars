// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

/*
    Game version. This is displayed in the rules interface.
*/
#define GAME_VERSION "v1.2.0"

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
#include "Enums/eAIType.h"
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
#define MAP_1               "Scenes/release.scene" // file path
#define MAP_2               "Scenes/newScene.scene" // file path
#define MAP_1_NUMBER        0
#define MAP_2_NUMBER        1
#define MIN_MAP_NUMBER      MAP_1_NUMBER
#define MAX_MAP_NUMBER      MAP_2_NUMBER
#define DEBUG_ENV           "Scenes/debug.scene"   // file path
#define DEBUG_NO_AI_ENV     "Scenes/debug_no_ai.scene"   // file path
#define LIGHT_MOVE_FACTOR   0.05f
#define PI                  3.1415926535f
#define PI_2                6.28318530718f
#define PI_HALF             1.57079632679f
#define DEGREES_TO_RADIANS  0.01745329251f
#define SECONDS_PER_MINUTE  60

#define KEYS                349
#define MAX_JOYSTICK_COUNT  4
#define MIN_PLAYER_COUNT    1
#define MAX_PLAYER_COUNT    4
#define MIN_BOT_COUNT       0
#define MAX_BOT_COUNT       4
#define MAX_HOVERCRAFT_COUNT MAX_PLAYER_COUNT + MAX_BOT_COUNT
#define XBOX_CONTROLLER     "Xbox"
#define EMPTY_CONTROLLER    "Empty Controller"

// XBOX Joystick Axes
#define XBOX_MASK                0x00
#define XBOX_BUTTON_A            0
#define XBOX_BUTTON_B            1
#define XBOX_BUTTON_X            2
#define XBOX_BUTTON_Y            3
#define XBOX_BUTTON_LEFT_BUMPER  4
#define XBOX_BUTTON_RIGHT_BUMPER 5
#define XBOX_BUTTON_BACK         6
#define XBOX_BUTTON_START        7
#define XBOX_BUTTON_LEFT_STICK   8
#define XBOX_BUTTON_RIGHT_STICK  9
#define XBOX_BUTTON_UP           10
#define XBOX_BUTTON_RIGHT        11
#define XBOX_BUTTON_DOWN         12
#define XBOX_BUTTON_LEFT         13
#define XBOX_TRIGGER_LEFT        14
#define XBOX_TRIGGER_RIGHT       15
#define AXIS_LEFT_STICK_X        0
#define AXIS_LEFT_STICK_Y        1
#define AXIS_RIGHT_STICK_X       2
#define AXIS_RIGHT_STICK_Y       3
#define AXIS_LEFT_TRIGGER        4
#define AXIS_RIGHT_TRIGGER       5

#define PS4_MASK                 0x80000000
#define PS4_BUTTON_X             1
#define PS4_BUTTON_O             2
#define PS4_BUTTON_SQUARE        0
#define PS4_BUTTON_TRIANGLE      3
#define PS4_BUTTON_OPTIONS       9
#define PS4_BUTTON_SHARE         8
#define PS4_BUTTON_R1            5
#define PS4_BUTTON_R2            7
#define PS4_BUTTON_R3            11
#define PS4_BUTTON_L1            4
#define PS4_BUTTON_L2            6 
#define PS4_BUTTON_L3            10
#define PS4_BUTTON_LEFT          17
#define PS4_BUTTON_RIGHT         15
#define PS4_BUTTON_UP            14
#define PS4_BUTTON_DOWN          16
#define PS4_BUTTON_TRACKPAD      13
#define PS4_BUTTON_PS            12
#define PS4_AXIS_LEFT_X          0
#define PS4_AXIS_LEFT_Y          1
#define PS4_AXIS_RIGHT_X         2
#define PS4_AXIS_RIGHT_Y         5
#define PS4_AXIS_L2              3
#define PS4_AXIS_R2              4

/*
    The actual button count for XBOX controllers is 14.
    As we treat the left and right triggers (which are actually axes)
    as buttons, we increase the value to 16.
    This is important for iterating through the joystick buttons
    in Menu::updateJoystickCommands() to ensure we check the trigger
    input states.

    As a result, MIN_BUTTON_COUNT will be 2 more than the XBOX_BUTTON_COUNT to
    ensure we iterate over the xbox triggers in Menu::updateJoystickCommands().
*/
#define XBOX_BUTTON_COUNT       14
#define PS4_BUTTON_COUNT        18
#define MIN_BUTTON_COUNT        XBOX_BUTTON_COUNT + 2
#define MAX_BUTTON_COUNT        PS4_BUTTON_COUNT
// Joystick/trigger values
#define JOYSTICK_IS_MAX         1.0f
#define JOYSTICK_IS_MIN         -1.0f
#define JOYSTICK_IS_NEUTRAL     0.0f
#define TRIGGER_IS_NETURAL      -1.0f
#define TRIGGER_IS_FULL         1.0f


// Mapping potential types from the scene loader to corresponding enums
const std::unordered_map<string, eBoundingBoxTypes> BOUNDING_BOX_MAP =
{
    make_pair("box", CUBIC_BOX),
    make_pair("spatial", SPATIAL_CALC)
};

/* Manager Defines */
#define MENU_MANAGER        MenuManager::getInstance()
#define EMITTER_ENGINE      EmitterEngine::getInstance()
#define ENTITY_MANAGER      EntityManager::getInstance()
#define GAME_MANAGER        GameManager::getInstance()
#define GAME_STATS          GameStats::getInstance()
#define INPUT_HANDLER       InputHandler::getInstance()
#define MESH_MANAGER        MeshManager::getInstance()
#define PHYSICS_MANAGER     PhysicsManager::getInstance()
#define SCENE_LOADER        SceneLoader::getInstance()
#define SHADER_MANAGER      ShaderManager::getInstance()
#define SOUND_MANAGER       SoundManager::getInstance()
#define SPATIAL_DATA_MAP    SpatialDataMap::getInstance()
#define TEXTURE_MANAGER     TextureManager::getInstance()

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
