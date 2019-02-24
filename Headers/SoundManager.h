#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "stdafx.h"
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "fmod_errors.h"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

/*
Manages all sounds. Provides an interfaces for other classes to play sound
events.

@author Evan Quan
@author Jianan Ding
*/
class SoundManager
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

        SOUND_TRAIL,        // combine as one

        // Not used
        SOUND_TRAIL_START,
        SOUND_TRAIL_LOOP,
        SOUND_TRAIL_END,

        SOUND_HOVERCAR_LOOP,
        SOUND_HOVERCAR_IMPACT_HOVERCAR,
        SOUND_HOVERCAR_IMPACT_WORLD,
        
        SOUND_HOVERCAR_DASH,

        SOUND_MUSIC_INGAME_LOOP,
        SOUND_MUSIC_PAUSE_LOOP,
    };

    void play(eSoundEvent sound);

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
    static void update();
    void shutDown();
private:
    
    static SoundManager* m_pInstance;

    FMOD::Studio::System* mpStudioSystem;   // FMOD Studio API
    FMOD::System* mpSystem;  // FMOD Low Level API

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

    int errorCheck(FMOD_RESULT result);

    void loadBank(const string& sBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void loadEvent(const string& sEvenName);
    void loadSound(const string& sSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);

    void unloadSound(const string& sSoundName);

    // void set3dListenerAndOrientation(const vec3& vPos = vec3(0, 0, 0), float fVoumedB = 0.0f);

    int playSounds(const string& sSoundName, const vec3& vPos = vec3(0, 0, 0), float fVolumedB = 0.0f);
    void playEvent(const string& sEventName);

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

    FMOD_VECTOR vectorToFmod(const vec3& vPosition);

    const char* getPath(eSoundEvent event) {
        vector<const char*> soundList = eventToSound.at(event);
        if (soundList.size() > 1)
        {
            std::random_device rd; // get a random number for the seed
            std::mt19937 eng(rd()); // seed the generator
            std::uniform_int_distribution<> distr(0, soundList.size() - 1); // 0 to (length of list - 1)
            return soundList[distr(eng)];
        }
        return soundList[0]; // there is only 1 element to return
    }

    const unordered_map<eSoundEvent, vector<const char*>> eventToSound =
    {
        {SOUND_ROCKET_ACTIVATE,          { "event:/rocket/rocket_activate",
                                         }},
        {SOUND_ROCKET_EXPLOSION,         { "event:/rocket/rocket_explosion_01",
                                           "event:/rocket/rocket_explosion_02",
                                           "event:/rocket/rocket_explosion_03",
                                         }},
        {SOUND_ROCKET_ACTIVATE,          { "event:/rocket/rocket_activate",
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
    };
};

#endif
