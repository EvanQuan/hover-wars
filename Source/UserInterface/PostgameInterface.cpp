#include "UserInterface/PostgameInterface.h"
#include "GameManager.h"
#include "Menus/PostgameMenu.h"

// Singleton instance
PostgameInterface* PostgameInterface::m_pInstance = nullptr;

/*
    Placements                  Awards
    Player #    <score>         Award1 name <award1 points>
    Player #    <score>         Award1 description
    Bot    #    <score>         
    Player #    <score>         Award2 name <points>
    Bot    #    <score>         Award2 descrption


                        Main Menu
*/
PostgameInterface::PostgameInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // Rank 1
        {0.15f, 0.8f},
        // Rank 2
        {0.15f, 0.7f},
        // Rank 3
        {0.15f, 0.6f},
        // Rank 4
        {0.15f, 0.5f},
        // Rank 5
        {0.15f, 0.4f},
        // Rank 6
        {0.15f, 0.3f},
        // Rank 7
        {0.15f, 0.2f},
        // Rank 8
        {0.15f, 0.1f},
        // 0 Background
        {0.0f, 0.0f},
        // 1 Placement
        {0.15f, 0.9f},
        // 2 Awards
        {0.7f, 0.7f},
        // Number 1
        {0.05f, 0.8f},
        // Number 2
        {0.05f, 0.7f},
        // Numebr 3
        {0.05f, 0.6f},

        // Rank 1 score thousand
        {0.40f, 0.8f},
        // Rank 1 score hundred
        {0.43f, 0.8f},
        // Rank 1 score ten
        {0.46f, 0.8f},
        // Rank 1 score one
        {0.49f, 0.8f},
        // Rank 1 pts icon
        {0.52f, 0.8f},

        // Rank 2 score thousand
        {0.40f, 0.7f},
        // Rank 2 score hundred
        {0.43f, 0.7f},
        // Rank 2 score ten
        {0.46f, 0.7f},
        // Rank 2 score one
        {0.49f, 0.7f},
        // Rank 2 pts icon
        {0.52f, 0.7f},

        // Rank 3 score thousand
        {0.40f, 0.6f},
        // Rank 3 score hundred
        {0.43f, 0.6f},
        // Rank 3 score ten
        {0.46f, 0.6f},
        // Rank 3 score one
        {0.49f, 0.6f},
        // Rank 3 pts icon
        {0.52f, 0.6f},

        // Rank 4 score thousand
        {0.40f, 0.5f},
        // Rank 4 score hundred
        {0.43f, 0.5f},
        // Rank 4 score ten
        {0.46f, 0.5f},
        // Rank 4 score one
        {0.49f, 0.5f},
        // Rank 4 pts icon
        {0.52f, 0.5f},

        // Rank 5 score thousand
        {0.40f, 0.4f},
        // Rank 5 score hundred
        {0.43f, 0.4f},
        // Rank 5 score ten
        {0.46f, 0.4f},
        // Rank 5 score one
        {0.49f, 0.4f},
        // Rank 5 pts icon
        {0.52f, 0.4f},

        // Rank 6 score thousand
        {0.40f, 0.3f},
        // Rank 6 score hundred
        {0.43f, 0.3f},
        // Rank 6 score ten
        {0.46f, 0.3f},
        // Rank 6 score one
        {0.49f, 0.3f},
        // Rank 6 pts icon
        {0.52f, 0.3f},

        // Rank 7 score thousand
        {0.40f, 0.2f},
        // Rank 7 score hundred
        {0.43f, 0.2f},
        // Rank 7 score ten
        {0.46f, 0.2f},
        // Rank 7 score one
        {0.49f, 0.2f},
        // Rank 7 pts icon
        {0.52f, 0.2f},

        // Rank 8 score thousand
        {0.40f, 0.1f},
        // Rank 8 score hundred
        {0.43f, 0.1f},
        // Rank 8 score ten
        {0.46f, 0.1f},
        // Rank 8 score one
        {0.49f, 0.1f},
        // Rank 8 pts icon
        {0.52f, 0.1f},

        // Main menu option
        {0.7f, 0.3f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // Rank 1
        {0.0f, 0.0f},
        // Rank 2
        {0.0f, 0.0f},
        // Rank 3
        {0.0f, 0.0f},
        // Rank 4
        {0.4f, 0.0f},
        // Rank 5
        {0.0f, 0.0f},
        // Rank 6
        {0.0f, 0.0f},
        // Rank 7
        {0.0f, 0.0f},
        // Rank 8
        {0.0f, 0.0f},
        // 0 Background
        {0.0f, 0.0f},
        // 1 Placement
        {0.47f, 0.1f},
        // 2 Awards
        {0.0f, 0.0f},
        // Number 1
        {0.0f, 0.0f},
        // Number 2
        {0.0f, 0.0f},
        // Number 3
        {0.0f, 0.0f},

        // Rank 1 score thousand
        {0.0f, 0.0f},
        // Rank 1 score hundred
        {0.0f, 0.0f},
        // Rank 1 score ten
        {0.0f, 0.0f},
        // Rank 1 score one
        {0.0f, 0.0f},
        // Rank 1 pts icon
        {0.0f, 0.0f},

        // Rank 2 score thousand
        {0.0f, 0.0f},
        // Rank 2 score hundred
        {0.0f, 0.0f},
        // Rank 2 score ten
        {0.0f, 0.0f},
        // Rank 2 score one
        {0.0f, 0.0f},
        // Rank 2 pts icon
        {0.0f, 0.0f},

        // Rank 3 score thousand
        {0.0f, 0.0f},
        // Rank 3 score hundred
        {0.0f, 0.0f},
        // Rank 3 score ten
        {0.0f, 0.0f},
        // Rank 3 score one
        {0.0f, 0.0f},
        // Rank 3 pts icon
        {0.0f, 0.0f},

        // Rank 4 score thousand
        {0.0f, 0.0f},
        // Rank 4 score hundred
        {0.0f, 0.0f},
        // Rank 4 score ten
        {0.0f, 0.0f},
        // Rank 4 score one
        {0.0f, 0.0f},
        // Rank 4 pts icon
        {0.0f, 0.0f},

        // Rank 5 score thousand
        {0.0f, 0.0f},
        // Rank 5 score hundred
        {0.0f, 0.0f},
        // Rank 5 score ten
        {0.0f, 0.0f},
        // Rank 5 score one
        {0.0f, 0.0f},
        // Rank 5 pts icon
        {0.0f, 0.0f},

        // Rank 6 score thousand
        {0.0f, 0.0f},
        // Rank 6 score hundred
        {0.0f, 0.0f},
        // Rank 6 score ten
        {0.0f, 0.0f},
        // Rank 6 score one
        {0.0f, 0.0f},
        // Rank 6 pts icon
        {0.0f, 0.0f},

        // Rank 7 score thousand
        {0.0f, 0.0f},
        // Rank 7 score hundred
        {0.0f, 0.0f},
        // Rank 7 score ten
        {0.0f, 0.0f},
        // Rank 7 score one
        {0.0f, 0.0f},
        // Rank 7 pts icon
        {0.0f, 0.0f},

        // Rank 8 score thousand
        {0.0f, 0.0f},
        // Rank 8 score hundred
        {0.0f, 0.0f},
        // Rank 8 score ten
        {0.0f, 0.0f},
        // Rank 8 score one
        {0.0f, 0.0f},
        // Rank 8 pts icon
        {0.0f, 0.0f},

        // Main menu option
        {0.4f, 0.0f},
    }
)
{
    GAME_MANAGER->addInterface(this);
}

PostgameInterface* PostgameInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new PostgameInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void PostgameInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void PostgameInterface::reinitialize(float gameTime)
{
}

void PostgameInterface::renderOverride()
{
    int playerCount = GAME_STATS->getPlayerCount();
    int botCount = GAME_STATS->getBotCount();
    int hovercraftCount = playerCount + botCount;
    eGameMode gameMode = GAME_STATS->getGameMode();

    bool renderPlacement2 = shouldRenderPlacement2(gameMode, hovercraftCount, playerCount, botCount);
    bool renderPlacement3 = shouldRenderPlacement3(gameMode, hovercraftCount, playerCount, botCount);

    renderBackgroundImage(IMAGE_BACKGROUND_POST_MENU);
    // Placement title
    renderImage(IMAGE_PLACEMENT, PLACEMENT);
    // There is always at least 1 player or team
    renderImage(IMAGE_NUMBER_1, NUMBER_1);

    if (renderPlacement2)
    {
        renderImage(IMAGE_NUMBER_2, NUMBER_2);
    } 
    if (renderPlacement3)
    {
        renderImage(IMAGE_NUMBER_3, NUMBER_3);
    }

    renderPlacement(botCount > 0);
    renderOption();
}

/*
    @param[in] renderBots   true if bot scores should be rendered. Typically if
                            there are no bots in the game, their score (team or
                            individual) should be ignored.
*/
void PostgameInterface::renderPlacement(bool renderBots)
{
    PostgameMenu* m = (PostgameMenu*)PostgameMenu::getInstance();
    string option = m->getCurrentPrompt();

    vector<EndGameStat> endGameStats = GAME_STATS->getEndGameStats();
    switch (GAME_STATS->getGameMode())
    {
    case GAMEMODE_FREE_FOR_ALL:
        renderFreeForAll(endGameStats);
        break;
    case GAMEMODE_TEAM_BOTS_VS_SOLO_PLAYERS:
        renderTeamBots(endGameStats, renderBots);
        break;
    case GAMEMODE_TEAMS_BOTS_VS_PLAYERS:
        renderPlayersVsBots(endGameStats, renderBots);
        break;
    case GAMEMODE_TEAMS_PLAYERS1_VS_PLAYERS2_VS_BOTS:
        renderTeamPlayers(endGameStats, renderBots);
        break;
    default:
        renderFreeForAll(endGameStats);
        break;
    }
}

#define PLAYER_NUMBER_OFFSET 300.0f

void PostgameInterface::renderFreeForAll(const vector<EndGameStat> &endGameStats)
{
    int placements = 0;
    for (EndGameStat s : endGameStats)
    {
        switch (s.hovercraft) {
            case HOVERCRAFT_PLAYER_1:
                renderImage(IMAGE_PLAYER, placements);
                renderImage(IMAGE_1_1, placements, PLAYER_NUMBER_OFFSET, 0.0f);
                break;
            case HOVERCRAFT_PLAYER_2:
                renderImage(IMAGE_PLAYER, placements);
                renderImage(IMAGE_1_1, placements, PLAYER_NUMBER_OFFSET, 0.0f);
                break;
            case HOVERCRAFT_PLAYER_3:
                renderImage(IMAGE_PLAYER, placements);
                renderImage(IMAGE_1_1, placements, PLAYER_NUMBER_OFFSET, 0.0f);
                break;
            case HOVERCRAFT_PLAYER_4:
                renderImage(IMAGE_PLAYER, placements);
                renderImage(IMAGE_1_1, placements, PLAYER_NUMBER_OFFSET, 0.0f);
                break;
            case HOVERCRAFT_BOT_1:
                renderImage(IMAGE_BOT_1, placements);
                break;
            case HOVERCRAFT_BOT_2:
                renderImage(IMAGE_BOT_2, placements);
                break;
            case HOVERCRAFT_BOT_3:
                renderImage(IMAGE_BOT_3, placements);
                break;
            case HOVERCRAFT_BOT_4:
                renderImage(IMAGE_BOT_4, placements);
                break;
        }
        renderScore(placements, s.afterAwardsScore);
        placements++;
    }
}

void PostgameInterface::renderTeamBots(const vector<EndGameStat> &endGameStats,
                                       bool renderBots)
{
    int teamBotScore = GAME_STATS->get(GameStats::eGlobalStat::SCORE_BOT_TEAM);
    // Apply awards
    for (EndGameStat s : endGameStats)
    {
        bool onBotTeam = GAME_STATS->isBot(s.hovercraft);
        if (onBotTeam)
        {
            for (Award a : s.awards)
            {
                teamBotScore += a.points;
            }
        }
    }
    bool teamBotPlaced = false;

    int placements = 0;
    for (EndGameStat s : endGameStats)
    {

        if (!teamBotPlaced && (teamBotScore > s.afterAwardsScore) && renderBots)
        {
            teamBotPlaced = true;
            renderImage(IMAGE_BOT_TEAM, placements);
            renderScore(placements, teamBotScore);
            placements++;
        }
        else if (GAME_STATS->isPlayer(s.hovercraft))
        {
            renderImage(IMAGE_PLAYER, placements);
            string playerNumber ;
            switch (s.hovercraft) {
                case HOVERCRAFT_PLAYER_1:
                    playerNumber = IMAGE_1_1;
                    break;
                case HOVERCRAFT_PLAYER_2:
                    playerNumber = IMAGE_2_1;
                    break;
                case HOVERCRAFT_PLAYER_3:
                    playerNumber = IMAGE_3_1;
                    break;
                case HOVERCRAFT_PLAYER_4:
                    playerNumber = IMAGE_4_1;
                    break;
                default:
                    playerNumber = IMAGE_1_1;
            }
            renderImage(playerNumber, placements, PLAYER_NUMBER_OFFSET, 0.0f);
            renderScore(placements, s.afterAwardsScore);
            placements++;
        }
    }
    if (!teamBotPlaced && renderBots)
    {
        renderImage(IMAGE_BOT_TEAM, placements);
        renderScore(placements, teamBotScore);
    }

}

void PostgameInterface::renderPlayersVsBots(const vector<EndGameStat> &endGameStats,
                                            bool renderBots)
{
    int placements = 0;
    int teamBotScore = GAME_STATS->get(GameStats::eGlobalStat::SCORE_BOT_TEAM);
    int teamPlayerScore = GAME_STATS->get(GameStats::eGlobalStat::SCORE_PLAYER_TEAM1);

    // update scores with awards
    for (EndGameStat s : endGameStats)
    {
        bool onPlayerTeam = GAME_STATS->isPlayer(s.hovercraft);
        if (onPlayerTeam)
        {
            for (Award a : s.awards)
            {
                teamPlayerScore += a.points;
            }
        }
        else
        {
            for (Award a : s.awards)
            {
                teamBotScore += a.points;
            }
        }
    }

    if (teamBotScore > teamPlayerScore) // Humans win by default on tie, get rekt bots
    {
        renderImage(IMAGE_BOT_TEAM, placements);
        renderScore(placements, teamBotScore);
        placements++;
        renderImage(IMAGE_PLAYER_TEAM, placements);
        renderScore(placements, teamPlayerScore);
    }
    else
    {
        renderImage(IMAGE_PLAYER_TEAM, placements);
        renderScore(placements, teamPlayerScore);
        placements++;
        if (renderBots)
        {
            renderImage(IMAGE_BOT_TEAM, placements);
            renderScore(placements, teamBotScore);
        }
    }


}

void PostgameInterface::renderTeamPlayers(const vector<EndGameStat>& endGameStats,
                                          bool renderBots)
{
    int placements = 0;
    int teamBotScore = GAME_STATS->get(GameStats::eGlobalStat::SCORE_BOT_TEAM);
    int teamPlayerScore = GAME_STATS->get(GameStats::eGlobalStat::SCORE_PLAYER_TEAM1);
    int team2PlayerScore = GAME_STATS->get(GameStats::eGlobalStat::SCORE_PLAYER_TEAM2);

    // update scores with awards
    for (EndGameStat s : endGameStats)
    {
        switch (s.hovercraft)
        {
        case HOVERCRAFT_PLAYER_1:
        case HOVERCRAFT_PLAYER_2:
            for (Award a : s.awards)
            {
                teamPlayerScore += a.points;
            }
            break;
        case HOVERCRAFT_PLAYER_3:
        case HOVERCRAFT_PLAYER_4:
            for (Award a : s.awards)
            {
                team2PlayerScore += a.points;
            }
            break;
        default:
            for (Award a : s.awards)
            {
                teamBotScore += a.points;
            }
        }
    }

    if (!renderBots)
    {
        teamBotScore = -1; // ensure bots are last place
    }



    vector<pair<int, string>> teamScores;
    teamScores.push_back(make_pair(teamBotScore, IMAGE_BOT_TEAM));
    teamScores.push_back(make_pair(teamPlayerScore, IMAGE_PLAYER_TEAM1));
    teamScores.push_back(make_pair(team2PlayerScore, IMAGE_PLAYER_TEAM2));

    if (teamScores[0].first < teamScores[1].first)
    {
        swap(teamScores[0], teamScores[1]);
    }
    if (teamScores[0].first < teamScores[2].first)
    {
        swap(teamScores[0], teamScores[2]);
    }
    if (teamScores[1].first < teamScores[2].first)
    {
        swap(teamScores[1], teamScores[2]);
    }
    // Sorted from highest score to less

    renderImage(teamScores[0].second, placements);
    renderScore(placements, teamScores[0].first);
    placements++;
    renderImage(teamScores[1].second, placements);
    renderScore(placements, teamScores[1].first);
    if (renderBots)
    {
        placements++;
        renderImage(teamScores[2].second, placements);
        renderScore(placements, teamScores[2].first);
    }

}


/*
    I fear no man...
    But this code...
    ... it scares me
*/
void PostgameInterface::renderScore(int placements, int score)
{
    int thousand = score / 1000;
    int hundred = score / 100 % 10;
    int ten = score / 10 % 10;
    int one = score % 10;

    string image;

    if (score > 999) {
        renderImage(digitToImage(thousand), 14 + placements * 5, 0.1f, 0.0f);
    }

    if (score > 99) {
        renderImage(digitToImage(hundred), 15 + placements * 5, 0.2f, 0.0f);
    }

    if (score > 9) {
        renderImage(digitToImage(ten), 16 + placements * 5, 0.3f, 0.0f);
    }

    renderImage(digitToImage(one), 17 + placements * 5, 0.4f, 0.0f);

    renderImage(IMAGE_POINT, 18 + placements * 5, 0.4f, 0.0f);
}


void PostgameInterface::renderOption()
{
    PostgameMenu *m = PostgameMenu::getInstance();
    string option = m->getCurrentPrompt();

    renderImage(option == AWARDS_OPTION ? IMAGE_AWARDS_2 : IMAGE_AWARDS_1, AWARDS);
    renderImage(option == BACK_TO_MAIN_MENU_OPTION ? IMAGE_MAIN_MENU_BUTTON_2 : IMAGE_MAIN_MENU_BUTTON_1, MAIN_MENU);
}

bool PostgameInterface::shouldRenderPlacement2(eGameMode gameMode,
                                               int hovercraftCount,
                                               int playerCount,
                                               int botCount)
{
    switch (gameMode)
    {
    case GAMEMODE_TEAMS_BOTS_VS_PLAYERS:
        return botCount > 0;
    case GAMEMODE_TEAMS_PLAYERS1_VS_PLAYERS2_VS_BOTS:
        // We force 4 players here so there are at least 2 player teams, guaranteed.
        return true;
    case GAMEMODE_TEAM_BOTS_VS_SOLO_PLAYERS:
        return (playerCount >= 2)
            || (playerCount >= 1 && botCount >= 1);
    default:
        return hovercraftCount >= 2;
    }
}

bool PostgameInterface::shouldRenderPlacement3(eGameMode gameMode,
                                               int hovercraftCount,
                                               int playerCount,
                                               int botCount)
{
    switch (gameMode)
    {
    case GAMEMODE_TEAMS_PLAYERS1_VS_PLAYERS2_VS_BOTS:
        // If we have team players, then the only placements are for player
        // team 1, player team 2, and the bot team.
        // Since player teams 1 and 2 are guaranteed every game, as we force
        // the player count to 4, we can guarantee the first 2 placements.
        // The only case in which there is no 3rd placement is if there is no
        // bot team.
        return botCount > 0;
    case GAMEMODE_TEAMS_BOTS_VS_PLAYERS:
        // There are only 2 placements at maximum, for the player team and bot team.
        return false;
    case GAMEMODE_TEAM_BOTS_VS_SOLO_PLAYERS:
        // If there 3 players, then each will take up a slot.
        // If there are 2 players, and at least a bot, then all three slots are filled.
        return (playerCount >= 3)
            || (playerCount >= 2 && botCount > 1);
    default:
        // In all other modes, as long as there are at least 3 hovercrafts,
        // there will be 3 placements.
        return hovercraftCount > 2;
    }
}
