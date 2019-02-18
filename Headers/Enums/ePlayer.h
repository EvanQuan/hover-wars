#pragma once

/*
Represents a player in the game. May be a human player (PLAYER) or a bot (BOT).
*/
enum ePlayer
{
    PLAYER_1 = 0,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4,
    MAX_PLAYER_INDEX,
    BOT_1,
    BOT_2,
    BOT_3,
    BOT_4,
    MAX_BOT_INDEX,
};
