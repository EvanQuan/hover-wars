#include "stdafx.h"
#include "Physics/PhysicsCallBack.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "PxFiltering.h"
#include "PxSimulationEventCallback.h"
#include "Physics/PhysicsManager.h"
#include "SoundManager.h"
#include "GameStats.h"
#include "EntityManager.h"

void PhysicsCallBack::initObjects(physx::PxRigidDynamic *body1, physx::PxRigidDynamic *body2) {
    this->body1 = body1;
    this->body2 = body2;
}

/*
Detects when two actors have collided.

https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/AdvancedCollisionDetection.html
*/
void PhysicsCallBack::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {
    // std::cout << "Collision detected" << std::endl;
    for (PxU32 i = 0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];

        // @NOTE what is this if-statement checking for?
        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            // Get each of the two colliding actors
            physx::PxRigidActor* actor0 = pairHeader.actors[0];
            physx::PxRigidActor* actor1 = pairHeader.actors[1];

            // Determine the results by actor names
            const char* collider = actor0->getName();
            const char* collided = actor1->getName();

            std::cout << "\tactor 0: " << collider << std::endl;
            std::cout << "\tactor 1: " << collided << std::endl;

            // NOTE: Is there something more efficient than an if, else-if chain to do here?

            // If something hits something else, actor0 will always be a dynamic object:
            // - player
            // - bot
            // - rocket
            // So we need to distinguish between them
            if (collider[0] == 'P') {
                // Check what the player collided with...
                if (collided[0] == 'w') {   // World type (include wall, ground and other object)
                    SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_WORLD);
                }
                else if (collided[0] == 't' || collided[0] == 'r') {  // Trail, Rocket type (treat as same damage for now)
                    if (collided[0] == 'r') {   // Rocket explosion sound
                        SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_EXPLOSION);
                    }
                    if (collided[1] == '1') {
                        if (collider[1] == '2') {
                            GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_PLAYER_2);
                        }
                        else if (collider[1] == '3') {
                            GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_PLAYER_3);
                        }
                        else if (collider[1] == '4') {
                            GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_PLAYER_4);
                        }
                    }
                    else if (collided[1] == '2') {
                        if (collider[1] == '1') {
                            GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_PLAYER_1);
                        }
                        else if (collider[1] == '3') {
                            GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_PLAYER_3);
                        }
                        else if (collider[1] == '4') {
                            GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_PLAYER_4);
                        }
                    }
                    else if (collided[1] == '3') {
                        if (collider[1] == '1') {
                            GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_PLAYER_1);
                        }
                        else if (collider[1] == '2') {
                            GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_PLAYER_2);
                        }
                        else if (collider[1] == '4') {
                            GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_PLAYER_4);
                        }
                    }
                    else if (collided[1] == '4') {
                        if (collider[1] == '1') {
                            GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_PLAYER_1);
                        }
                        else if (collider[1] == '2') {
                            GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_PLAYER_2);
                        }
                        else if (collider[1] == '3') {
                            GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_PLAYER_3);
                        }
                    }
                }
                else if (collided[0] == 'p') { // Powerup type
                    if (collider[1] == '1') {
                        GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::PICKUP_POWERUP);
                    }
                    else if (collider[1] == '2') {
                        GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::PICKUP_POWERUP);
                    }
                    else if (collider[1] == '3') {
                        GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::PICKUP_POWERUP);
                    }
                    else if (collider[1] == '4') {
                        GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::PICKUP_POWERUP);
                    }
                }
                else if (collided[0] == 'b') {  // Bot type
                    // check if player spikes is on or not for both actor
                    // TODO: need to work on here
                    bool spikesON = false;
                    if (collider[1] == '1') {
                        spikesON = ENTITY_MANAGER->getPlayer(PLAYER_1)->hasSpikesActivated();
                    }
                    else if (collider[1] == '2') {
                        spikesON = ENTITY_MANAGER->getPlayer(PLAYER_2)->hasSpikesActivated();
                    }
                    else if (collider[1] == '3') {
                        spikesON = ENTITY_MANAGER->getPlayer(PLAYER_3)->hasSpikesActivated();
                    }
                    else if (collider[1] == '4') {
                        spikesON = ENTITY_MANAGER->getPlayer(PLAYER_4)->hasSpikesActivated();
                    }

                    if (spikesON) {
                        if (collider[1] == '1') {
                            GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_BOT);
                        }
                        else if (collider[1] == '2') {
                            GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_BOT);
                        }
                        else if (collider[1] == '3') {
                            GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_BOT);
                        }
                        else if (collider[1] == '4') {
                            GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_BOT);
                        }
                    }
                    else {
                        SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_HOVERCAR);
                    }
                }
                else if (collided[0] == 'P') {  // Player type
                    // check if player spikes is on or not for both actor
                    SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_HOVERCAR);
                    // TODO: need to work on here
                }
            }
            
            else if (collider[0] == 'b') {  // Bot type
                // Check what the bot collided with...
                if (collided[0] == 'w') {   // World type
                    SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_WORLD);
                }
                else if (collided[0] == 't' || collided[0] == 'r') {  // Trail, Rocket type (treat as same damage for now)
                    if (collided[1] == '1') {
                        GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_BOT);
                    }
                    else if (collided[1] == '2') {
                        GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_BOT);
                    }
                    else if (collided[1] == '3') {
                        GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_BOT);
                    }
                    else if (collided[1] == '4') {
                        GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_BOT);
                    }
                }
                else if (collided[0] == 'b') {  // Bot type
                    // Nothing happen
                    SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_HOVERCAR);
                }
                else if (collided[0] == 'P') {  // Player type
                    SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_HOVERCAR);
                    // TODO: need to work on here
                }
            }

            else if (collider[0] == 'r') {  // Rocket type
                // Check what the rocket collided with...
                if (collided[0] == 'w') {
                    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_EXPLOSION);
                }
                else if (collided[0] == 'b') {
                    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_EXPLOSION);
                    if (collider[1] == '1') {
                        GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_BOT);
                    }
                    else if (collider[1] == '2') {
                        GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_BOT);
                    }
                    else if (collider[1] == '3') {
                        GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_BOT);
                    }
                    else if (collider[1] == '4') {
                        GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_BOT);
                    }
                }
                else if (collided[0] == 'P') {
                    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_EXPLOSION);
                    if (collided[1] == '1') {
                        if (collider[1] == '2') {
                            GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_PLAYER_2);
                        }
                        else if (collider[1] == '3') {
                            GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_PLAYER_3);
                        }
                        else if (collider[1] == '4') {
                            GAME_STATS->addScore(PLAYER_1, GameStats::eAddScoreReason::HIT_PLAYER_4);
                        }
                    }
                    else if (collided[1] == '2') {
                        if (collider[1] == '1') {
                            GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_PLAYER_1);
                        }
                        else if (collider[1] == '3') {
                            GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_PLAYER_3);
                        }
                        else if (collider[1] == '4') {
                            GAME_STATS->addScore(PLAYER_2, GameStats::eAddScoreReason::HIT_PLAYER_4);
                        }
                    }
                    else if (collided[1] == '3') {
                        if (collider[1] == '1') {
                            GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_PLAYER_1);
                        }
                        else if (collider[1] == '2') {
                            GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_PLAYER_2);
                        }
                        else if (collider[1] == '4') {
                            GAME_STATS->addScore(PLAYER_3, GameStats::eAddScoreReason::HIT_PLAYER_4);
                        }
                    }
                    else if (collided[1] == '4') {
                        if (collider[1] == '1') {
                            GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_PLAYER_1);
                        }
                        else if (collider[1] == '2') {
                            GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_PLAYER_2);
                        }
                        else if (collider[1] == '3') {
                            GAME_STATS->addScore(PLAYER_4, GameStats::eAddScoreReason::HIT_PLAYER_3);
                        }
                    }
                }
            }

        }
    }
}
