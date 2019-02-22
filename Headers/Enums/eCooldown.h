#pragma once

/*
Represents an ability cooldown.

This is used by hovercrafts, GameStats, and UserInterface.
It is separate from abilties due to shared cooldown between all dashes.
*/
enum eCooldown
{
    COOLDOWN_ROCKET = 0,
    COOLDOWN_SPIKES,
    COOLDOWN_TRAIL,
    COOLDOWN_DASH,
    COOLDOWN_COUNT,
};
