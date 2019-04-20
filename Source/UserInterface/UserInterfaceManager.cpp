#include "UserInterface/UserInterfaceManager.h"
#include "UserInterface/PromptInterface.h"
#include "ShaderManager.h"
#include "TextureManager.h"

UserInterfaceManager* UserInterfaceManager::m_pInstance = nullptr;

void UserInterfaceManager::updateWidthAndHeight(int iWidth, int iHeight)
{
    m_pPrompInterface->updateWidthAndHeight(iWidth, iHeight);
}

void UserInterfaceManager::render()
{
    m_pPrompInterface->render();
}

void UserInterfaceManager::update(float fFrameDeltaTime)
{
    m_pPrompInterface->update(fFrameDeltaTime);
}

UserInterfaceManager::UserInterfaceManager()
{
    TextureManager *textureManager = TEXTURE_MANAGER;


    // @NOTE bug. If both the outturn and tron directories are loaded then there are errors
    // for rendering text... but not images... even though text is not involved here?
    // It doesn't matter if both directories are used.
    // It doesn't matter which map (m_mTexturesTron or m_mTexturesOutrun) is being used.
    // It doesn't matter the number of times TRON_DIRECTORY or OUTRUN directory
    // is loaded or in which order.
    // Both directories load without errors individually.
    // m_mTexturesTron = textureManager->loadTextures(m_mImagePaths, TRON_DIRECTORY);

    m_mTexturesOutrun = textureManager->loadTextures(m_mImagePaths, OUTRUN_DIRECTORY);

    // default theme
    setTheme(THEME_OUTRUN);
}

UserInterfaceManager * UserInterfaceManager::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new UserInterfaceManager();
    }
    return m_pInstance;
}

UserInterfaceManager::~UserInterfaceManager()
{
    // User Interface
    for (UserInterface* ui : m_vInterfaceInstances) {
        delete ui;
    }
    m_vInterfaceInstances.clear();
}

void UserInterfaceManager::setTheme(eTheme theme)
{
    m_eCurrentTheme = theme;
    switch (theme)
    {
    case THEME_TRON:
        m_mTexturesCurrent = m_mTexturesTron;
        break;
    case THEME_OUTRUN:
    default:
        m_mTexturesCurrent = m_mTexturesOutrun;
    }
}
