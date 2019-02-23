#include "stdafx.h"
#include "SoundManager.h"

using namespace std;

// Initialize Static Instance Variable
SoundManager* SoundManager::m_pInstance = nullptr;


/*************************************************************************\
 * Constructors                                                          *
\*************************************************************************/
SoundManager::SoundManager() {
    mpStudioSystem = NULL;
    errorCheck(FMOD::Studio::System::create(&mpStudioSystem));     // Create the studio system object.
    errorCheck(mpStudioSystem->initialize(5, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));   // Initialize system.
    // 5 - max channels
    // FMOD_STUDIO_INIT_LIVEUPDATE
    // 0 - no extra driver data

    mpSystem = NULL;
    errorCheck(mpStudioSystem->getLowLevelSystem(&mpSystem));      // Setup low level system;
}

/*************************************************************************\
 * Destructor                                                            *
\*************************************************************************/
SoundManager::~SoundManager() {
    errorCheck(mpStudioSystem->unloadAll());   // Unloads all currently loaded banks.
    errorCheck(mpStudioSystem->release());     // Closes and frees a system object and its resources.
}

SoundManager* SoundManager::getInstance() {
    if (nullptr == m_pInstance) {
        m_pInstance = new SoundManager();
    }
    return m_pInstance;
}

/*************************************************************************\
 * Public                                                                *
\*************************************************************************/

/*
Play a sound in the world. That sound can have any number of
instances played simultaneously.

@param sound    to play
*/
void SoundManager::play(eSoundEvent sound)
{
    m_pInstance->update();
    if (sound == SOUND_ROCKET_ACTIVATE) {
        //SOUND_MANAGER->playSounds("Sound/hovercraft/bumper_car_go_loop.wav", vec3(0, 0, 0), SOUND_MANAGER->volumeTodB(1.0f));
        m_pInstance->playEvent("event:/rocket_activate");
    }
    else if (sound == SOUND_ROCKET_EXPLOSION) {
        m_pInstance->playEvent("event:/rocket_explosion");
    }
    else if (sound == SOUND_SPIKES_ACTIVATE) {
        //m_pInstance->playEvent("event:/");
    }
    else if (sound == SOUND_SPIKES_IMPACT) {
        //m_pInstance->playEvent("event:/");
    }
    else if (sound == SOUND_TRAIL) {
        m_pInstance->playEvent("event:/trail");
    }
    else if (sound == SOUND_HOVERCAR_LOOP) {
        m_pInstance->playEvent("event:/hovercraft_move");   // need flag
    }
    else if (sound == SOUND_HOVERCAR_IMPACT_HOVERCAR) {
        m_pInstance->playEvent("event:/hovercraft_hit_hovercraft");
    }
    else if (sound == SOUND_HOVERCAR_IMPACT_WORLD) {
        m_pInstance->playEvent("event:/hovercraft_hit_world");
    }
    else if (sound == SOUND_MUSIC_INGAME_LOOP) {
        m_pInstance->playEvent("event:/background_2");
    }
    else if (sound == SOUND_MUSIC_PAUSE_LOOP) {
        //m_pInstance->playEvent("");
    }

}

/*
Play a sound at a specified location in the world.

@param sound    to play
@param location in world-space
*/
void SoundManager::play(eSoundEvent sound, vec3 location)
{
}

/*
Begin looping a sound for a specified entityID. That entity can only loop one
instance of each sound for each loopID.

@param sound    to play
@param entityID that the sound belongs to
@param loopID   to start looping
*/
void SoundManager::startLoop(eSoundEvent sound, int entityID, int loopID)
{

}

/*
If the sound is looping for a specified entityID for the specified loopID, end
that loop.

@param sound    to play
@param entityID that the sound belongs to
@param loopID   to stop looping
*/
void SoundManager::endLoop(eSoundEvent sound, int entityID, int loopID)
{

}

/*
Begin looping a sound for a specified entityID at a specified location. That
entity can only loop one instance of each sound for each loopID.

@param sound    to play
@param location in world-space
@param entityID that the sound belongs to
@param loopID   to start looping
*/
void SoundManager::startLoop(eSoundEvent sound, vec3 location, int entityID, int loopID)
{

}

/*
If the sound is looping for a specified entityID for a specified loopID, end that loop.

@param sound    to play
@param location in world-space
@param entityID that the sound belongs to
@param loopID   to stop looping
*/
void SoundManager::endLoop(eSoundEvent sound, vec3 location, int entityID, int loopID)
{

}

/*************************************************************************\
 * Private                                                               *
\*************************************************************************/
/*
Load all the audio files needed.

For some reason, if this is called in the constructor, the program crashes. To
avoid this, this is called after the Singleton instance is first instantiated
in main().
*/
void SoundManager::loadFiles() {
    m_pInstance->loadBank("Sound/Master Bank.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
    m_pInstance->loadBank("Sound/Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);

    //m_pInstance->loadEvent("event:/hovercraft/bumper_car_go_loop.wav");
    //m_pInstance->loadEvent("event:/SwordBattle");
    //m_pInstance->loadSound("Sound/hovercraft/bumper_car_go_loop.wav", false);

    //m_pInstance->loadEvent("event:/rocket/rocket_shoot.wav");
    //m_pInstance->loadSound("Sound/rocket/rocket_shoot.wav", false);
}

void SoundManager::update() {
    vector<ChannelMap::iterator> vStoppedChannels;
    for (auto it = m_pInstance->mChannels.begin(); it != m_pInstance->mChannels.end(); ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
        {
            vStoppedChannels.push_back(it);
        }
    }
    for (auto& it : vStoppedChannels)
    {
        m_pInstance->mChannels.erase(it);
    }
    m_pInstance->errorCheck(m_pInstance->mpStudioSystem->update());
}

/*
Load a sound.

@param sSoundName   file path to the sound
@param b3d          true if 3D sound, else 2D
@param bLooping     true if sound is to be looped, else not looped
@param bStream      ???????
*/
void SoundManager::loadSound(const string& sSoundName, bool b3d, bool bLooping, bool bStream) {
    auto tFoundIt = mSounds.find(sSoundName);
    if (tFoundIt != mSounds.end()) {       // Founded (has been loaded)
        return;
    }

    FMOD_MODE mode = FMOD_DEFAULT;      // 0x00000000
    mode |= b3d ? FMOD_3D : FMOD_2D;
    mode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;      // Stream is more for background music?

    FMOD::Sound* pSound = nullptr;
    errorCheck(mpSystem->createSound(sSoundName.c_str(), mode, nullptr, &pSound));

    // Store new sound
    if (pSound) {
        mSounds[sSoundName] = pSound;
    }
}

/*
Unload a sound. If the sound was not loaded, do nothing.

@param sSoundName   filepath of sound to unload
*/
void SoundManager::unloadSound(const string& sSoundName) {
    auto tFoundIt = mSounds.find(sSoundName);
    if (tFoundIt == mSounds.end()) {      // Not found
        return;
    }

    errorCheck(tFoundIt->second->release());
    mSounds.erase(tFoundIt);
}

int SoundManager::playSounds(const string& sSoundName, const vec3& vPosition, float fVolumedB) {
    update();       // Clear finished channels
    int iChannelId = mnNextChannelId++;
    auto tFoundIt = mSounds.find(sSoundName);
    if (tFoundIt == mSounds.end()) {       // Not found in sound map
        loadSound(sSoundName);
        tFoundIt = mSounds.find(sSoundName);
        if (tFoundIt == mSounds.end()) {       // Not found after relaod
            return iChannelId;
        }
    }

    FMOD::Channel* pChannel = nullptr;
    errorCheck(mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));      // Play sound and paused at the beginning

    if (pChannel) {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            FMOD_VECTOR position = vectorToFmod(vPosition);
            errorCheck(pChannel->set3DAttributes(&position, nullptr));
        }
        errorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
        errorCheck(pChannel->setPaused(false));
        mChannels[iChannelId] = pChannel;
    }
    return iChannelId;
}

void SoundManager::setChannel3dPosition(int iChannelId, const vec3& vPosition) {
    auto tFoundIt = mChannels.find(iChannelId);
    if (tFoundIt == mChannels.end()) {     // Not found
        return;
    }

    FMOD_VECTOR position = vectorToFmod(vPosition);
    errorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void SoundManager::setChannelVolume(int iChannelId, float fVolumedB) {
    auto tFoundIt = mChannels.find(iChannelId);
    if (tFoundIt == mChannels.end()) {     // Not found
        return;
    }
    errorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

void SoundManager::loadBank(const string& sBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
    auto tFoundIt = mBanks.find(sBankName);
    if (tFoundIt != mBanks.end()) {     // Bank already loaded
        return;
    }
    FMOD::Studio::Bank* pBank;
    errorCheck(mpStudioSystem->loadBankFile(sBankName.c_str(), flags, &pBank));

    if (pBank) {
        mBanks[sBankName] = pBank;
    }
}

void SoundManager::loadEvent(const string& sEventName) {
    auto tFoundIt = mEvents.find(sEventName);
    if (tFoundIt != mEvents.end()) {     // Event already loaded
        return;
    }
    FMOD::Studio::EventDescription* pEventDescription = NULL;
    errorCheck(mpStudioSystem->getEvent(sEventName.c_str(), &pEventDescription));
    if (pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = NULL;
        errorCheck(pEventDescription->createInstance(&pEventInstance));
        if (pEventInstance) {
            mEvents[sEventName] = pEventInstance;
        }
    }
}

void SoundManager::playEvent(const string& sEventName) {
    auto tFoundIt = mEvents.find(sEventName);
    if (tFoundIt == mEvents.end()) {
        loadEvent(sEventName);
        tFoundIt = mEvents.find(sEventName);
        if (tFoundIt == mEvents.end()) {
            return;
        }
    }
    tFoundIt->second->start();
}

void SoundManager::stopEvent(const string& sEventName, bool bImmediate) {
    auto tFoundIt = mEvents.find(sEventName);
    if (tFoundIt == mEvents.end()) {
        return;
    }
    FMOD_STUDIO_STOP_MODE mode;
    mode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    errorCheck(tFoundIt->second->stop(mode));
}

bool SoundManager::isEventPlaying(const string& sEventName) const {
    auto tFoundIt = mEvents.find(sEventName);
    if (tFoundIt == mEvents.end()) {
        return false;
    }
    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
    if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}

void SoundManager::getEventParameter(const string& sEventName, const string& sParameterName, float* parameter) {
    auto tFoundIt = mEvents.find(sEventName);
    if (tFoundIt == mEvents.end()) {
        return;
    }
    FMOD::Studio::ParameterInstance* pParameter = NULL;
    errorCheck(tFoundIt->second->getParameter(sParameterName.c_str(), &pParameter));
    errorCheck(pParameter->getValue(parameter));
}

void SoundManager::setEventParameter(const string& sEventName, const string& sParameterName, float fValue) {
    auto tFoundIt = mEvents.find(sEventName);
    if (tFoundIt == mEvents.end()) {
        return;
    }
    FMOD::Studio::ParameterInstance* pParameter = NULL;
    errorCheck(tFoundIt->second->getParameter(sParameterName.c_str(), &pParameter));
    errorCheck(pParameter->setValue(fValue));
}

FMOD_VECTOR SoundManager::vectorToFmod(const vec3& vPosition) {
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}

float SoundManager::dbToVolume(float dB) {
    return powf(10.0f, 0.05f * dB);
}

float SoundManager::volumeTodB(float volume) {
    return 20.0f * log10f(volume);
}

int SoundManager::errorCheck(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        cout << "FMOD error: " << result << " (" << FMOD_ErrorString(result) << ")"<<endl;
        return 1;   // Error
    }
    return 0;   // No error
}

void SoundManager::shutDown() {
    delete m_pInstance;
}
