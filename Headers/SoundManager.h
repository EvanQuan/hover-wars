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
        SOUND_ROCKET_RECHARGE,
        SOUND_ROCKET_REFLECT,

        SOUND_SPIKES_ACTIVATE,
        SOUND_SPIKES_IMPACT,
        SOUND_SPIKES_RECHARGE,

        SOUND_TRAIL,
        SOUND_TRAIL_1,
        SOUND_TRAIL_2,
        SOUND_TRAIL_3,
        SOUND_TRAIL_4,
        SOUND_TRAIL_5,
        SOUND_TRAIL_6,
        SOUND_TRAIL_7,
        SOUND_TRAIL_8,
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

        SOUND_MUSIC_INGAME,
        SOUND_MUSIC_PAUSE,

        SOUND_UI_CURSOR_MOVE,
        SOUND_UI_CURSOR_SELECT,

        // For resuming the game
        SOUND_UI_END_GAME_CHEER,

        SOUND_UI_NEW_LEADER,
        SOUND_UI_TEAM_LEADER_1,
        SOUND_UI_TEAM_LEADER_2,
        SOUND_UI_TEAM_LEADER_BOTS,
        // For major time warnings
        SOUND_UI_TIME_REMAINING_WARNING,
        SOUND_UI_TIME_REMAINING_LOOP,
        // For resuming the game
        SOUND_UI_RESUME_COUNTDOWN,
        // For controller connecting
        SOUND_UI_CONTROLLER_CONNECT,

        SOUND_UI_INSERT_COIN,
    };

    void play(eSoundEvent sound);
    void play(eSoundEvent sound, bool shouldPlay);
    void handleCollisionSound(Entity* collider, Entity* collided);

    // TODO figure out sound at locations
    void play(eSoundEvent sound, vec3 location);

    void startLoop(eSoundEvent sound, eHovercraft hovercraft);
    void endLoop(eSoundEvent sound, eHovercraft hovercraft);

    void startLoop(eSoundEvent sound, vec3 location, int entityID, int loopID);
    void endLoop(eSoundEvent sound, vec3 location, int entityID, int loopID);
    // TODO
    // looping at different locations.
    // How do we make the sound follow the location as it moves?

    void stopAllEvents();
    void stopEvent(eSoundEvent soundEvent, bool bImmediate = true);

    static SoundManager* getInstance();

    void loadFiles();
    void shutDown();

    // TODO
    // Change to private method
    void setSpeedParameter(float speed);

    void setPauseMenu();
    void setResumeCountdown();
    void setResumeGame();
    void setEndGame();

    void upPosition();
    void downPosition();

    void start();
    void update();

    bool hasMusicEnabled() const { return m_bMusicEnabled; }
    void setMusicEnabled(bool bMusicEnabled) { m_bMusicEnabled = bMusicEnabled; }
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
    void pauseEvent(const string& sEventName);
    void resumeEvent(const string& sEventName);

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

    bool m_bMusicEnabled;

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
        {SOUND_ROCKET_RECHARGE,         { "event:/rocket/rocket_recharge",
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
        {SOUND_SPIKES_RECHARGE,           { "event:/spikes/spikes_recharge",
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
        {SOUND_TRAIL_1,                  { "event:/trail/trail_1",
                                         }},
        {SOUND_TRAIL_2,                  { "event:/trail/trail_2",
                                         }},
        {SOUND_TRAIL_3,                  { "event:/trail/trail_3",
                                         }},
        {SOUND_TRAIL_4,                  { "event:/trail/trail_4",
                                         }},
        {SOUND_TRAIL_5,                  { "event:/trail/trail_5",
                                         }},
        {SOUND_TRAIL_6,                  { "event:/trail/trail_6",
                                         }},
        {SOUND_TRAIL_7,                  { "event:/trail/trail_7",
                                         }},
        {SOUND_TRAIL_8,                  { "event:/trail/trail_8",
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
        {SOUND_MUSIC_INGAME,                   { "event:/background/music_loop_retro",
                                         }},
        {SOUND_MUSIC_PAUSE,                    { "event:/background/music_loop_pause",
                                         }},
        {SOUND_UI_CURSOR_MOVE,           { "event:/ui/cursor/cursor_move_01",
                                           "event:/ui/cursor/cursor_move_02",
                                           "event:/ui/cursor/cursor_move_03",
                                           "event:/ui/cursor/cursor_move_04",
                                           "event:/ui/cursor/cursor_move_05",
                                         }},
        {SOUND_UI_CURSOR_SELECT,         { "event:/ui/cursor/cursor_select_01",
                                           "event:/ui/cursor/cursor_select_02",
                                           "event:/ui/cursor/cursor_select_03",
                                           "event:/ui/cursor/cursor_select_04",
                                           "event:/ui/cursor/cursor_select_05",
                                         }},
        {SOUND_UI_END_GAME_CHEER,        { "event:/ui/notification/end_game_cheer",
                                         }},
        {SOUND_UI_TIME_REMAINING_WARNING,{ "event:/ui/notification/time_remaining_warning",
                                         }},
        {SOUND_UI_TIME_REMAINING_LOOP,   { "event:/ui/notification/time_remaining_warning_with_tick",
                                         }},
        {SOUND_UI_NEW_LEADER,            { "event:/ui/notification/new_leader",
                                         }},
        {SOUND_UI_TEAM_LEADER_1,         { "event:/ui/notification/team1_leader",
                                         }},
        {SOUND_UI_TEAM_LEADER_2,         { "event:/ui/notification/team2_leader",
                                         }},
        {SOUND_UI_TEAM_LEADER_BOTS,      { "event:/ui/notification/teambot_leader",
                                         }},
        {SOUND_UI_RESUME_COUNTDOWN,      { "event:/ui/notification/countdown_tick",
                                         }},
        {SOUND_UI_CONTROLLER_CONNECT,    { "event:/ui/notification/controller_connect",
                                         }},
        {SOUND_UI_INSERT_COIN,           { "event:/ui/insert_coin",
                                         }},
    };
};

#endif
