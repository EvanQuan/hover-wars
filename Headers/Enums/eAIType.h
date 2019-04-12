#pragma once

/*
    Represents the game rule configuration. Once the AIManager is configured
    with a game mode, all bots will behave in that manner.
    GameStats will also choose to ignore certain hits if hovercrafts are on the
    same team.
*/
enum eGameMode
{
    // AI will team up to only target players and share a score at the end of
    // the game, but players are still solo.
    GAMEMODE_TEAM_AI_SOLO_PLAYERS = 0,
    // AI are on a team, and players are on a team. Both teams have a shared
    // score at the end of the game.
    GAMEMODE_TEAMS_AI_VS_PLAYERS = 1,
    // All players for themselves.
    GAMEMODE_FREE_FOR_ALL = 2,

    GAMEMODE_COUNT = 3,
};
