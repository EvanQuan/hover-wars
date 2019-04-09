#pragma once

/*
    Represents an AI configuration. Once the AIManager is configured with an AI
    type, all bots will behave in that manner.
*/
enum eAIType
{
    // AI will team up to only target players
    AI_ON_SAME_TEAM,
    // AI will act alone and target both players and bots
    AI_SOLO,
};
