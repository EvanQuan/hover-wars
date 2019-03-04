#pragma once

/*
Represents a player in the game.
*/
enum ePlayer
{
    PLAYER_1 = 0,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4,
    MAX_PLAYER_INDEX,
    PLAYER_INVALID,
};

/*
Represents a bot in the game.
*/
enum eBot
{
    BOT_1 = 0,
    BOT_2,
    BOT_3,
    BOT_4,
    MAX_BOT_INDEX,
    BOT_INVALID,
};

enum eHovercraft
{
    HOVERCRAFT_PLAYER_1 = 0,
    HOVERCRAFT_PLAYER_2,
    HOVERCRAFT_PLAYER_3,
    HOVERCRAFT_PLAYER_4,
    HOVERCRAFT_BOT_1,
    HOVERCRAFT_BOT_2,
    HOVERCRAFT_BOT_3,
    HOVERCRAFT_BOT_4,
    MAX_HOVERAFT_INDEX,
    HOVERCRAFT_INVALID,
};
