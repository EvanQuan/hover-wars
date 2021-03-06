#pragma once

/*
Represents a power up in the game. These spawn on the map and can be picked up
by players. They provide temporary benefits to those who pick them up.
*/
enum ePowerup
{
    POWERUP_ROCKET_COOLDOWN,
    POWERUP_SPIKES_COOLDOWN,
    POWERUP_TRAIL_COOLDOWN,
    POWERUP_DASH_RECHARGE,
    POWERUP_SPEED_BOOST,
    POWERUP_COUNT,
    POWERUP_NONE,
    POWERUP_INVALID,
};
