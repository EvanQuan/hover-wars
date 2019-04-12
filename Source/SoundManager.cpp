#include "stdafx.h"
#include "SoundManager.h"
#include "EntityHeaders/HovercraftEntity.h"
#include "EntityManager.h"
#include "EntityHeaders/Entity.h"


// Bank paths
#define MASTER_BANK_PATH         "Sound/Desktop/Master Bank.bank"
#define MASTER_BANK_STRINGS_PATH "Sound/Desktop/Master Bank.strings.bank"

#define MUSIC_BANK_PATH         "Sound/Desktop/Music Bank.bank"
#define MUSIC_BANK_STRINGS_PATH "Sound/Desktop/Music Bank.strings.bank"

/*
    @Stephen What does this impact? Describe why this is here.
*/
#define MAX_CHANNELS 2000 // 10 // 20
#define NO_EXTRA_DRIVER_DATA 0

// Initialize Static Instance Variable
SoundManager* SoundManager::m_pInstance = nullptr;


/*************************************************************************\
 * Constructors                                                          *
\*************************************************************************/
SoundManager::SoundManager() {
    m_pStudioSystem = nullptr;
    errorCheck(FMOD::Studio::System::create(&m_pStudioSystem));     // Create the studio system object.

    m_pSystem = nullptr;
    // Set a random seed, or the random music loop will be deterministic
    errorCheck(m_pStudioSystem->getLowLevelSystem(&m_pSystem));      // Setup low level system;
    advancedSettings = new FMOD_ADVANCEDSETTINGS();
    advancedSettings->cbSize = sizeof(FMOD_ADVANCEDSETTINGS);
    advancedSettings->randomSeed = FuncUtils::random(0, std::numeric_limits<int>::max());
    errorCheck(m_pSystem->setAdvancedSettings(advancedSettings));
    // Initialize system.
    errorCheck(m_pStudioSystem->initialize(MAX_CHANNELS,
                                           FMOD_STUDIO_INIT_NORMAL,
                                           FMOD_INIT_NORMAL,
                                           NO_EXTRA_DRIVER_DATA));

}

/*************************************************************************\
 * Destructor                                                            *
\*************************************************************************/
SoundManager::~SoundManager() {
    errorCheck(m_pStudioSystem->unloadAll());   // Unloads all currently loaded banks.
    errorCheck(m_pStudioSystem->release());     // Closes and frees a system object and its resources.
    delete advancedSettings;
}

void SoundManager::stopAllEvents()
{
    for (auto it : mEvents)
    {
        errorCheck(it.second->stop(FMOD_STUDIO_STOP_IMMEDIATE));
    }
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
    If the event does not have a valid path, no sound is played.

    @param sound    to play
*/
void SoundManager::play(eSoundEvent sound)
{
    string eventPath = getPath(sound);
    if (eventPath.empty())
    {
        return;
    }

    playEvent(eventPath);
    updateChannels();
}

/*
    Play a sound in the world, only if the specified boolean is true.

    @param sound        to play
    @param shouldPlay   to determin if the event should be played
*/
void SoundManager::play(eSoundEvent sound, bool shouldPlay)
{
    if (shouldPlay)
    {
        play(sound);
    }
}

/*
    Determine Entity Types resulting in Collision and play a resulting sound
    for the collision. Handle any logic between collisions to determine sound
    here.

    We want to ignore bot world collision sounds since world collision sounds
    are global.

    @param eColliderType    The Entity Type of the Collider
    @param eCollidedType    The Entity Type of the Collided
    @param colliderIsBot    if the collider is a bot
    @param collidedIsBot    if the collided is a bot
    @return true if both collider and collided are hovercrafts
*/
bool SoundManager::handleBaseCollisionSound(eEntityType eColliderType,
                                            eEntityType eCollidedType,
                                            bool colliderIsBot,
                                            bool collidedIsBot)
{
    // Context-specific sounds require specific information about the entities,
    // specifically if they are players
    switch (eColliderType)
    {
    case eEntityType::ENTITY_HOVERCRAFT:               
      // See what they collided with. Further collisions might be with pick ups
      // or other entities.
        switch (eCollidedType)
        {
        case eEntityType::ENTITY_HOVERCRAFT:
            // Collided with another Hovercar, play hovercar collision sound.
            play(eSoundEvent::SOUND_HOVERCAR_IMPACT_HOVERCAR,
                 !(colliderIsBot && collidedIsBot));
            return true;
            break;
        case eEntityType::ENTITY_STATIC:
            // Collided with Static Entity, impacted world
            play(eSoundEvent::SOUND_HOVERCAR_IMPACT_WORLD,
                 !colliderIsBot);
            break;
        }
        break;
    }
    return false;
}

void SoundManager::handleCollisionSound(Entity * collider, Entity * collided)
{
    eEntityType colliderType = collider->getType();
    eEntityType collidedType = collided->getType();

    HovercraftEntity* colliderHovercraft = nullptr;
    HovercraftEntity* collidedHovercraft = nullptr;
    if (colliderType == eEntityType::ENTITY_HOVERCRAFT)
    {
        colliderHovercraft = static_cast<HovercraftEntity*>(collider);
    }
    if (collidedType == eEntityType::ENTITY_HOVERCRAFT)
    {
        collidedHovercraft = static_cast<HovercraftEntity*>(collided);
    }
    // Base collision sounds only require type
    // To make the sound less busy, we will ignore bot base collisions, which
    // can be annoying if they get stuck on a wall
    bool colliderIsBot = GAME_STATS->isBot(colliderHovercraft);
    bool collidedIsBot = GAME_STATS->isBot(collidedHovercraft);
    if (handleBaseCollisionSound(colliderType, collidedType,
                                 colliderIsBot, collidedIsBot))
    {
        // For some reason this check is not enough?
        if (((colliderType == eEntityType::ENTITY_HOVERCRAFT))
            && (collidedType == eEntityType::ENTITY_HOVERCRAFT))
        {
            handleContextCollisionSound(colliderHovercraft, collidedHovercraft);
        }
    }
}

/*
    At this point, assumes both collider and collided are hovercrafts of some
    kind.

    @NOTE: This will be unneeded when interactable entities become further
           developed. As all abilities will be interactable entitise, context
           collisions will be determined in handleBaseCollisionSound.
*/
void SoundManager::handleContextCollisionSound(HovercraftEntity* collider,
                                               HovercraftEntity* collided)
{
    if (collider->hasSpikesActivated()
        || collided->hasSpikesActivated())
    {
        play(eSoundEvent::SOUND_SPIKES_IMPACT);
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
    auto eventInstance = mEvents[getPath(sound)];
    switch (entityID) {
    case 37:
        eventInstance = mEvents[getPath(SOUND_TRAIL_1)];
        break;
    case 40:
        eventInstance = mEvents[getPath(SOUND_TRAIL_2)];
        break;
    case 43:
        eventInstance = mEvents[getPath(SOUND_TRAIL_3)];
        break;
    case 46:
        eventInstance = mEvents[getPath(SOUND_TRAIL_4)];
        break;
    case 49:
        eventInstance = mEvents[getPath(SOUND_TRAIL_5)];
        break;
    case 52:
        eventInstance = mEvents[getPath(SOUND_TRAIL_6)];
        break;
    case 55:
        eventInstance = mEvents[getPath(SOUND_TRAIL_7)];
        break;
    case 58:
        eventInstance = mEvents[getPath(SOUND_TRAIL_8)];
        break;
    }
    const char* end = "shouldGoToEnd";
    eventInstance->setParameterValue(end, 0.0);
    eventInstance->start();
    this->updateChannels();
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
    auto eventInstance = mEvents[getPath(sound)];
    switch (entityID) {
    case 37:
        eventInstance = mEvents[getPath(SOUND_TRAIL_1)];
        break;
    case 40:
        eventInstance = mEvents[getPath(SOUND_TRAIL_2)];
        break;
    case 43:
        eventInstance = mEvents[getPath(SOUND_TRAIL_3)];
        break;
    case 46:
        eventInstance = mEvents[getPath(SOUND_TRAIL_4)];
        break;
    case 49:
        eventInstance = mEvents[getPath(SOUND_TRAIL_5)];
        break;
    case 52:
        eventInstance = mEvents[getPath(SOUND_TRAIL_6)];
        break;
    case 55:
        eventInstance = mEvents[getPath(SOUND_TRAIL_7)];
        break;
    case 58:
        eventInstance = mEvents[getPath(SOUND_TRAIL_8)];
        break;
    }
    const char* end = "shouldGoToEnd";
    eventInstance->setParameterValue(end, 1.0);
    this->updateChannels();
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

    For some reason, if this is called in the constructor, the program crashes.
    To avoid this, this is called after the Singleton instance is first
    instantiated in main().
*/
void SoundManager::loadFiles() {
    m_pInstance->loadBank(MASTER_BANK_STRINGS_PATH, FMOD_STUDIO_LOAD_BANK_NORMAL);
    m_pInstance->loadBank(MASTER_BANK_PATH, FMOD_STUDIO_LOAD_BANK_NORMAL);

    // m_pInstance->loadBank(MUSIC_BANK_STRINGS_PATH, FMOD_STUDIO_LOAD_BANK_NORMAL);
    m_pInstance->loadBank(MUSIC_BANK_PATH, FMOD_STUDIO_LOAD_BANK_NORMAL);

    loadAllEvents();
}

/*
    Update channels to start or stop playing sounds based on play() calls since
    the last time this was called. As a result, this sould be called AFTER
    every time play() called to ensure it is played immediately.
*/
void SoundManager::updateChannels() {
    vector<ChannelMap::iterator> vStoppedChannels;
    for (auto it = mChannels.begin(); it != mChannels.end(); ++it)
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
        mChannels.erase(it);
    }
    errorCheck(m_pStudioSystem->update());
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
    errorCheck(m_pSystem->createSound(sSoundName.c_str(), mode, nullptr, &pSound));

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
    updateChannels();       // Clear finished channels
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
    errorCheck(m_pSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));      // Play sound and paused at the beginning

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
    errorCheck(m_pStudioSystem->loadBankFile(sBankName.c_str(), flags, &pBank));

    if (pBank) {
        mBanks[sBankName] = pBank;
    }
}

/*
    Load all events
*/
void SoundManager::loadAllEvents()
{
    for (auto& it : eventToSound)
    {
        for (string sound : it.second)
        {
            loadEvent(sound);
        }
    }
}

void SoundManager::loadEvent(const string& sEventName) {
    // Exit early if event already loaded
    if (FuncUtils::contains(mEvents, sEventName)) {
        return;
        // cout << sEventName << " already loaded." << endl;
    }
    // cout << sEventName << " not loaded yet." << endl;

    //auto tFoundIt = mEvents.find(sEventName);
    //if (tFoundIt != mEvents.end()) {     // Event already loaded
        //return;
    //}
    FMOD::Studio::EventDescription* pEventDescription = nullptr;
    errorCheck(m_pStudioSystem->getEvent(sEventName.c_str(), &pEventDescription));
    if (nullptr != pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = nullptr;
        errorCheck(pEventDescription->createInstance(&pEventInstance));
        if (nullptr != pEventInstance) {
            mEvents[sEventName] = pEventInstance;
            // cout << sEventName << " successfully loaded with " << pEventInstance << endl;
        }
    }
}



/*
    Play an event.

    Multiple instances of each event can be played to allow for events to overlap.

    @param sEventName   path to event to play. All valid paths are recorded in
                        the eventToSound map.
*/
void SoundManager::playEvent(const string& sEventName) {
    auto tFoundIt = mEvents.find(sEventName);
    if (tFoundIt == mEvents.end()) {
        // cout << "event " << sEventName << " not loaded" << endl;
        loadEvent(sEventName);
        // cout << "event " << sEventName << " finished loading" << endl;
        tFoundIt = mEvents.find(sEventName);
        if (tFoundIt == mEvents.end()) {
            return;
            // cout << "event is not a valid event and so wasn't played" << endl;
        }
    }
    else {      // Event is in mEvents
        FMOD_STUDIO_PLAYBACK_STATE state;
        tFoundIt->second->getPlaybackState(&state);
        switch (state)
        {
        case FMOD_STUDIO_PLAYBACK_PLAYING: // Event currently playing
            // Copy from load event
            // Create new event instance with same event in bank file
            bool playable = false;
            int i = 0;
            string sNewEventName = sEventName + to_string(i);
            while (!playable) {
                auto found = mEvents.find(sNewEventName);
                if (found == mEvents.end()) {
                    playable = true;
                }
                else {
                    found->second->getPlaybackState(&state);
                    if (state == FMOD_STUDIO_PLAYBACK_STOPPED) {
                        playable = true;
                    }
                    i++;
                    sNewEventName = sEventName + to_string(i);
                }
            }
            // Create event instance with same event description
            FMOD::Studio::EventDescription* pEventDescription = nullptr;
            errorCheck(m_pStudioSystem->getEvent(sEventName.c_str(), &pEventDescription));
            if (nullptr != pEventDescription) {
                FMOD::Studio::EventInstance* pEventInstance = nullptr;
                errorCheck(pEventDescription->createInstance(&pEventInstance));
                if (nullptr != pEventInstance) {
                    mEvents[sNewEventName] = pEventInstance;
                    // cout << sNewEventName << " successfully loaded with " << pEventInstance << endl;
                }
            }
            tFoundIt = mEvents.find(sNewEventName);
            break;
        }
        // Event not playing, going play tFoundIt

        // 3D testing here
        testAttrubute.position = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
        tFoundIt->second->set3DAttributes(&testAttrubute);
    }
    tFoundIt->second->setPaused(false);
    tFoundIt->second->start();
    // cout << "event: " << tFoundIt->first << " played " << tFoundIt->second << endl;
}

/*
Testing things out
https://books.google.ca/books?id=VfxNDwAAQBAJ&pg=PT373&lpg=PT373&dq=fmod::studio:eventInstance+getplaybackstate&source=bl&ots=Dlb4f5O3pe&sig=ACfU3U3_eWkbAVazGIRgrqwPHPdr2_0CyA&hl=en&sa=X&ved=2ahUKEwi2lrnWvtbgAhWIuZ4KHXguDvsQ6AEwCHoECAIQAQ#v=onepage&q=fmod%3A%3Astudio%3AeventInstance%20getplaybackstate&f=false

@Deprecated
*/
void SoundManager::playEventDirect(const string& sEventName) {
    mEvents[sEventName]->start();

    // cout << "hahaevent: " << sEventName << " played " << mEvents[sEventName] << endl;
}

void SoundManager::stopEvent(eSoundEvent soundEvent, bool bImmediate) {
    auto tFoundIt = mEvents.find(getPath(soundEvent));
    if (tFoundIt == mEvents.end()) {
        return;
    }
    FMOD_STUDIO_STOP_MODE mode;
    mode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    errorCheck(tFoundIt->second->stop(mode));
}

void SoundManager::pauseEvent(const string & sEventName)
{
}

void SoundManager::resumeEvent(const string & sEventName)
{
}

bool SoundManager::isEventPlaying(eSoundEvent soundEvent) const {
    auto tFoundIt = mEvents.find(getPath(soundEvent));
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

// TODO: May need flag for pressing forward button, or not
// method should be private
void SoundManager::setSpeedParameter(float speedPercent) {
    auto tFoundIt = mEvents.find(getPath(SOUND_HOVERCAR_ENGINE));

    if ((0.0f <= speedPercent) && (speedPercent <= 1.0f)) {
        errorCheck(tFoundIt->second->setParameterValue("Speed", 5.0f * speedPercent));
    }
    updateChannels();
}

void SoundManager::setPauseMenu() {
    // Pause all the playing events
    for (auto it : mEvents)
    {
        it.second->setPaused(true);
    }
    // play pause music
    auto tFoundIt = mEvents.find(getPath(SOUND_MUSIC_PAUSE));
    tFoundIt->second->setPaused(false);
    play(SOUND_MUSIC_PAUSE);
    updateChannels();
}

void SoundManager::setResumeCountdown()
{
    stopEvent(SOUND_MUSIC_PAUSE);
    play(SOUND_UI_RESUME_COUNTDOWN);
    updateChannels();
}

void SoundManager::setResumeGame() {
    // Unpause all in game sound events that were previously paused and end
    // pause music
    for (auto it : mEvents)
    {
        it.second->setPaused(false);
    }
    updateChannels();
}

void SoundManager::setEndGame()
{
    stopAllEvents();
    play(SoundManager::eSoundEvent::SOUND_UI_END_GAME_CHEER);
    play(SOUND_MUSIC_INGAME);
}

// @Deprecated
void SoundManager::upPosition() {
    auto tFoundIt = mEvents.find(getPath(SOUND_HOVERCAR_ENGINE));
    vec3 testingP = ENTITY_MANAGER->getPlayer(HOVERCRAFT_PLAYER_1)->getPosition();
    testAttrubute.position.x = testingP.x;
    testAttrubute.position.y = testingP.y;
    testAttrubute.position.z = testingP.z;
    cout << "Car position: " << testAttrubute.position.x << ", "<< testAttrubute.position.y <<", "<< testAttrubute.position.z << endl;
    tFoundIt->second->set3DAttributes(&testAttrubute);
    updateChannels();
}

// @Deprecated
void SoundManager::downPosition() {
    auto tFoundIt = mEvents.find(getPath(SOUND_HOVERCAR_ENGINE));
    testAttrubute.position.x -= 10;
    tFoundIt->second->set3DAttributes(&testAttrubute);
    updateChannels();
}

void SoundManager::start() {
    play(SOUND_MUSIC_INGAME);
}

// Call every frame (or more often)
void SoundManager::update() {
    // make speed go from 0 to 1
    // Balance volume of engine sound with music
    // This seems to cause an out of range vector issue, even though tracking through
    // the code, it doesn't make sense. Perhaps it has to do with how the sound is threaded
    // and that this action is not thread-safe. I don't know.
    // For now, we will disable this.
    setSpeedParameter(ENTITY_MANAGER->getPlayer(HOVERCRAFT_PLAYER_1)->getSpeed() / 30.0f);
}
