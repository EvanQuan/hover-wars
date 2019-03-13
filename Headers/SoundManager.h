#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "stdafx.h"
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "fmod_errors.h"

#include "EntityHeaders/Entity.h"

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

        SOUND_SPIKES_ACTIVATE,
        SOUND_SPIKES_IMPACT,

        SOUND_TRAIL,

        SOUND_PULSE_ACTIVATE,
        SOUND_PULSE_IMPACT,

        SOUND_HOVERCAR_LOOP,
        SOUND_HOVERCAR_IMPACT_HOVERCAR,
        SOUND_HOVERCAR_IMPACT_WORLD,
        SOUND_HOVERCAR_ENGINE,
        
        SOUND_HOVERCAR_DASH,

        SOUND_MUSIC_INGAME_LOOP,
        SOUND_MUSIC_PAUSE_LOOP,

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

        MUSIC_INGAME,
        MUSIC_PAUSE,
    };

    void play(eSoundEvent sound);
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

    void pauseAll();

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

    bool handleBaseCollisionSound(eEntityType eColliderType, eEntityType eCollidedType);
    void handleContextCollisionSound(Entity* collider, Entity* collided);

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
    void stopEvent(const string& sEventName, bool bImmediate = false);

    void getEventParameter(const string& sEventName, const string& sEventParameter, float* parameter);
    void setEventParameter(const string& sEventName, const string& sParamerterName, float fValue);

    //void stopAllChannels();
    void setChannel3dPosition(int iChannelId, const vec3& vPosition);
    void setChannelVolume(int iChannelId, float fVolumedB);
    //bool isPlaying(int iChannelId) const;
    bool isEventPlaying(const string& sEventName) const;
    float dbToVolume(float db);
    float volumeTodB(float volume);
    bool isPaused = false;

    FMOD_VECTOR vectorToFmod(const vec3& vPosition);

    const char* getPath(eSoundEvent event) {
        if (!FuncUtils::contains(eventToSound, event)) {
            return "";
        }
        vector<const char*> soundList = eventToSound.at(event);
        if (soundList.size() > 1)
        {
            return soundList[FuncUtils::random(0, soundList.size() - 1)];
        }
        return soundList[0]; // there is only 1 
    }

    const unordered_map<eSoundEvent, vector<const char*>> eventToSound =
    {
        {SOUND_ROCKET_ACTIVATE,          { "event:/rocket/rocket_activate",
                                         }},
        {SOUND_ROCKET_EXPLOSION,         { "event:/rocket/rocket_explosion_01",
                                           "event:/rocket/rocket_explosion_02",
                                           "event:/rocket/rocket_explosion_03",
                                         }},
        {SOUND_SPIKES_ACTIVATE,          { "event:/spikes/spikes_activate_01",
                                           "event:/spikes/spikes_activate_02",
                                           "event:/spikes/spikes_activate_03",
                                         }},
        {SOUND_SPIKES_IMPACT,            { "event:/spikes/spikes_impact",
                                         }},
        {SOUND_PULSE_ACTIVATE,           { "event:/pulse/pulse_activate",
                                         }},
        {SOUND_PULSE_IMPACT,             { "event:/pulse/pulse_impact_01",
                                           "event:/pulse/pulse_impact_02",
                                           "event:/pulse/pulse_impact_03",
                                         }},
        {SOUND_HOVERCAR_DASH,            { "event:/hovercraft/hovercraft_dash",
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
        {SOUND_MUSIC_INGAME_LOOP,        { "event:/background/ingame_music_loop_01",
                                         }},
        {SOUND_MUSIC_PAUSE_LOOP,         { "event:/background/music_loop_pause",
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
        {MUSIC_INGAME,                   { "event:/background/music_loop_retro",
                                         }},
        {MUSIC_PAUSE,                    { "event:/background/music_loop_pause",
                                         }},
    };
};

#endif
