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
    eAIType getAIType() const { return m_eAIType; };
    unsigned int getMapNumber() const { return m_iMapNumber; };

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

    eAIType m_eAIType;

    static PregameMenu* m_pInstance;
};
