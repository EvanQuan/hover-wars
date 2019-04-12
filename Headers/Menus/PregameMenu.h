#pragma once

#include "Menus/PromptMenu.h"

class PregameMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PregameMenu() {}

    static Menu* getInstance();
    int getPlayerCount() const { return m_iPlayerCount; };
    int getBotCount() const { return m_iBotCount; };
    int getGameTime() const { return m_fGameTime; };
    eGameMode getGameMode() const { return static_cast<eGameMode>(m_eGameMode); };
    unsigned int getMapNumber() const { return m_iMapNumber; };
    eBotDifficulty getBotDifficulty() const { return static_cast<eBotDifficulty>(m_eBotDifficulty); }
    bool hasScoreLossEnabled() const { return m_bScoreLossEnabled; };

private:
    PregameMenu();

    void select(eFixedCommand command);

    void moveCursor(eFixedCommand command);

    void back();

    // @Override
    void enter();

    int m_iBotCount;
    int m_iPlayerCount;
    int m_fGameTime;
    unsigned int m_iMapNumber;
    bool m_bScoreLossEnabled;

    // These are actually enums, but are stored as ints to later be casted.
    // easier to work with ints
    unsigned int m_eBotDifficulty;
    unsigned int m_eGameMode;

    static PregameMenu* m_pInstance;
};
