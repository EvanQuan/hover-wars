#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "stdafx.h"
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "fmod_errors.h"

class Entity;
class HovercraftEntity;

/*
Manages all sounds. Provides an interfaces for other classes to play sound
events.

@author Evan Quan
@author Jianan Ding
*/
class SoundManager final
{
public:
    ~SoundManager();

    /*
    Sound events correspond to world events.
    They may be composed of multiple individual sounds.
    */
    enum eSoundEvent
    {
        SOUND_ROCKET_ACTIVATE,
        SOUND_ROCKET_EXPLOSION,
        SOUND_ROCKET_REFLECT,

        SOUND_SPIKES_ACTIVATE,
        SOUND_SPIKES_IMPACT,

        SOUND_TRAIL,
        SOUND_TRAIL_IMPACT,

        SOUND_PULSE_ACTIVATE,
        SOUND_PULSE_IMPACT,

        SOUND_HOVERCAR_LOOP,
        SOUND_HOVERCAR_IMPACT_HOVERCAR,
        SOUND_HOVERCAR_IMPACT_WORLD,
        SOUND_HOVERCAR_ENGINE,
        
        SOUND_DASH_ACTIVATE,
        SOUND_DASH_RECHARGE,

        SOUND_KILL_FIRST_BLOOD,
        SOUND_KILL_DOMINATION,
        SOUND_KILL_REVENGE,
        SOUND_KILL_NEMESIS,
        SOUND_KILL_STREAK,

        // Honks, for fun
        SOUND_HONK_UP,
        SOUND_HONK_RIGHT,
        SOUND_HONK_DOWN,
        SOUND_HONK_LEFT,

        SOUND_POWERUP_SPAWN,
        SOUND_POWERUP_PICKUP,
        SOUND_POWERUP_SPEED_ACTIVATE,
        SOUND_POWERUP_SPEED_DEACTIVATE,

        MUSIC_INGAME,
        MUSIC_PAUSE,

        SOUND_UI_CURSOR_MOVE,
        SOUND_UI_CURSOR_SELECT,

        // For resuming the game
        SOUND_UI_END_GAME_CHEER,
        SOUND_UI_NEW_LEADER,
        SOUND_UI_TIME_REMAINING_LOOP,
        SOUND_UI_COUNTDOWN_TICK,
    };

    void play(eSoundEvent sound);
    void play(eSoundEvent sound, bool shouldPlay);
    void handleCollisionSound(Entity* collider, Entity* collided);

    // TODO figure out sound at locations
    void play(eSoundEvent sound, vec3 location);

    void startLoop(eSoundEvent sound, int entityID, int loopID);
    void endLoop(eSoundEvent sound, int entityID, int loopID);

    void startLoop(eSoundEvent sound, vec3 location, int entityID, int loopID);
    void endLoop(eSoundEvent sound, vec3 location, int entityID, int loopID);
    // TODO
    // looping at different locations.
    // How do we make the sound follow the location as it moves?

    static SoundManager* getInstance();

    void loadFiles();
    void shutDown();

    // TODO
    // Change to private method
    void setSpeedParameter(float speed);

    void togglePaused();

    void upPosition();
    void downPosition();

    void start();
    void update();
private:

    FMOD_3D_ATTRIBUTES testAttrubute;
    static SoundManager* m_pInstance;

    FMOD_ADVANCEDSETTINGS* advancedSettings;

    FMOD::Studio::System* m_pStudioSystem;   // FMOD Studio API
    FMOD::System* m_pSystem;  // FMOD Low Level API

    int mnNextChannelId;

    typedef map<string, FMOD::Sound*> SoundMap;
    typedef map<int, FMOD::Channel*> ChannelMap;
    typedef map<string, FMOD::Studio::EventInstance*> EventMap;
    typedef map<string, FMOD::Studio::Bank*> BankMap;

    SoundMap mSounds;
    ChannelMap mChannels;
    EventMap mEvents;
    BankMap mBanks;

    SoundManager();
    void updateChannels();

    bool handleBaseCollisionSound(eEntityType eColliderType, eEntityType eCollidedType, bool colliderIsBot, bool collidedIsBot);
    void handleContextCollisionSound(HovercraftEntity* collider, HovercraftEntity* collided);

    int errorCheck(FMOD_RESULT result);

    void loadBank(const string& sBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void loadEvent(const string& sEvenName);
    void loadSound(const string& sSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);

    void loadAllEvents();

    void unloadSound(const string& sSoundName);

    // void set3dListenerAndOrientation(const vec3& vPos = vec3(0, 0, 0), float fVoumedB = 0.0f);

    int playSounds(const string& sSoundName, const vec3& vPos = vec3(0, 0, 0), float fVolumedB = 0.0f);
    void playEvent(const string& sEventName);
    void playEventDirect(const string& sEventName);

    // void stopChannel(int iChannelId);
    void stopEvent(eSoundEvent soundEvent, bool bImmediate = true);
    void pauseEvent(const string& sEventName);
    void resumeEvent(const string& sEventName);

    void setPauseMenu();
    void setResumeGame();

    void getEventParameter(const string& sEventName, const string& sEventParameter, float* parameter);
    void setEventParameter(const string& sEventName, const string& sParamerterName, float fValue);

    //void stopAllChannels();
    void setChannel3dPosition(int iChannelId, const vec3& vPosition);
    void setChannelVolume(int iChannelId, float fVolumedB);
    //bool isPlaying(int iChannelId) const;
    bool isEventPlaying(eSoundEvent soundEvent) const;
    float dbToVolume(float db);
    float volumeTodB(float volume);
    bool isPaused = false;

    FMOD_VECTOR vectorToFmod(const vec3& vPosition);

    /*
        Get the event path of the event. If there are multiple event paths for
        the specified event, pick a random one from the list of valid ones.

        @param event    to get event path from
        @return path of event in the Master Bank file if the event exists
        @return "" if the event does not exist
    */
    string getPath(eSoundEvent event) const {
        if (!FuncUtils::contains(eventToSound, event)) {
            return "";
        }
        vector<string> soundList = eventToSound.at(event);
        if (soundList.size() > 1)
        {
            return soundList[FuncUtils::random(0, soundList.size() - 1)];
        }
        return soundList[0]; // there is only 1 
    }

    const unordered_map<eSoundEvent, vector<string>> eventToSound =
    {
        {SOUND_ROCKET_ACTIVATE,          { "event:/rocket/rocket_activate",
                                         }},
        {SOUND_ROCKET_EXPLOSION,         { "event:/rocket/rocket_explosion_01",
                                           "event:/rocket/rocket_explosion_02",
                                           "event:/rocket/rocket_explosion_03",
                                           "event:/rocket/rocket_explosion_04",
                                           "event:/rocket/rocket_explosion_05",
                                           "event:/rocket/rocket_explosion_06",
                                         }},
        {SOUND_ROCKET_REFLECT,          { "event:/rocket/rocket_reflect",
                                         }},
        {SOUND_SPIKES_ACTIVATE,          { "event:/spikes/spikes_activate_01",
                                           "event:/spikes/spikes_activate_02",
                                           "event:/spikes/spikes_activate_03",
                                         }},
        {SOUND_SPIKES_IMPACT,            { "event:/spikes/spikes_impact_01",
                                           "event:/spikes/spikes_impact_02",
                                           "event:/spikes/spikes_impact_03",
                                           "event:/spikes/spikes_impact_04",
                                         }},
        {SOUND_PULSE_ACTIVATE,           { "event:/pulse/pulse_activate",
                                         }},
        {SOUND_PULSE_IMPACT,             { "event:/pulse/pulse_impact_01",
                                           "event:/pulse/pulse_impact_02",
                                           "event:/pulse/pulse_impact_03",
                                         }},
        {SOUND_DASH_ACTIVATE,            { "event:/dash/dash_start",
                                         }},
        {SOUND_DASH_RECHARGE,            { "event:/dash/dash_gain_charge",
                                         }},
        {SOUND_HOVERCAR_IMPACT_HOVERCAR, { "event:/hovercraft/hovercraft_hit_hovercraft_01",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_02",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_03",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_04",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_05",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_06",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_07",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_08",
                                         }},
        {SOUND_HOVERCAR_IMPACT_WORLD,    { "event:/hovercraft/hovercraft_hit_hovercraft_01",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_02",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_03",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_04",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_05",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_06",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_07",
                                           "event:/hovercraft/hovercraft_hit_hovercraft_08",
                                         }},
        {SOUND_HOVERCAR_ENGINE,          { "event:/hovercraft/hovercraft_engine",
                                         }},
        {SOUND_TRAIL,                    { "event:/trail/trail",
                                         }},
        {SOUND_TRAIL_IMPACT,             { "event:/trail/trail_hit",
                                         }},
        {SOUND_KILL_FIRST_BLOOD,         { "event:/kill/firstblood",
                                         }},
        {SOUND_KILL_DOMINATION,          { "event:/kill/domination",
                                         }},
        {SOUND_KILL_NEMESIS,             { "event:/kill/nemesis",
                                         }},
        {SOUND_KILL_REVENGE,             { "event:/kill/revenge",
                                         }},
        {SOUND_KILL_STREAK,              { "event:/kill/killstreak",
                                         }},
        {SOUND_HONK_UP,                  { "event:/honk/honk_01",
                                         }},
        {SOUND_HONK_RIGHT,               { "event:/honk/honk_02",
                                         }},
        {SOUND_HONK_DOWN,                { "event:/honk/honk_03",
                                         }},
        {SOUND_HONK_LEFT,                { "event:/honk/honk_04",
                                         }},
        {SOUND_POWERUP_SPAWN,            { "event:/powerup/powerup_spawn",
                                         }},
        {SOUND_POWERUP_PICKUP,           { "event:/powerup/powerup_pickup",
                                         }},
        {SOUND_POWERUP_SPEED_ACTIVATE,   { "event:/powerup/speedboost_start",
                                         }},
        {SOUND_POWERUP_SPEED_DEACTIVATE, { "event:/powerup/speedboost_end",
                                         }},
        {MUSIC_INGAME,                   { "event:/background/music_loop_retro",
                                         }},
        {MUSIC_PAUSE,                    { "event:/background/music_loop_pause",
                                         }},
        {SOUND_UI_CURSOR_MOVE,           { "event:/ui/cursor/cursor_move_01",
                                           "event:/ui/cursor/cursor_move_02",
                                           "event:/ui/cursor/cursor_move_03",
                                           "event:/ui/cursor/cursor_move_04",
                                           "event:/ui/cursor/cursor_move_05",
                                         }},
        {SOUND_UI_CURSOR_SELECT,         { "event:/ui/cursor/cursor_move_01",
                                           "event:/ui/cursor/cursor_move_02",
                                           "event:/ui/cursor/cursor_move_03",
                                           "event:/ui/cursor/cursor_move_04",
                                           "event:/ui/cursor/cursor_move_05",
                                         }},
        {SOUND_UI_END_GAME_CHEER,        { "event:/ui/notification/end_game_cheer",
                                         }},
        {SOUND_UI_TIME_REMAINING_LOOP,   { "event:/ui/notification/time_remaining_warning",
                                         }},
        {SOUND_UI_NEW_LEADER,            { "event:/ui/notification/new_leader",
                                         }},
        {SOUND_UI_COUNTDOWN_TICK,        { "event:/ui/notification/countdown_tick",
                                         }},
    };
};

#endif
