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
    AI_ON_SAME_TEAM,
    // AI are on a team, and players are on a team. Both teams have a shared
    // score at the end of the game.
    AI_VS_PLAYERS,
    // All players for themselves.
    GAMEMODE_DEATH_MATCH,
};
