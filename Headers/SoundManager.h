#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "stdafx.h"
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

class SoundManager
{
public:
    ~SoundManager();

    enum eSound
    {
        SOUND_ROCKET,

    };

    void play(eSound sound);

    // TODO figure out sound at locations
    void play(eSound sound, vec3 location);

    void startLoop(eSound sound, int entityID, int loopID);
    void endLoop(eSound sound, int entityID, int loopID);

    void startLoop(eSound sound, vec3 location, int entityID, int loopID);
    void endLoop(eSound sound, vec3 location, int entityID, int loopID);
    // TODO
    // looping at different locations.
    // How do we make the sound follow the location as it moves?

    static SoundManager* getInstance();

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
    void loadFiles();

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
};

#endif
