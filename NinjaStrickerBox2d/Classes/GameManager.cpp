#include "GameManager.h" 

GameManager* GameManager::m_mySingleton = NULL;

GameManager::GameManager() {
   
}

GameManager* GameManager::sharedGameManager() {
    if(NULL == m_mySingleton) {
        m_mySingleton = new GameManager();
    }
    return m_mySingleton;
}
