#ifndef _GameManager_H_
#define _GameManager_H_

#include "cocos2d.h"

using namespace std;
using namespace cocos2d;

class GameManager : public cocos2d::CCObject
{
private:
    
    CC_SYNTHESIZE(int, _numberAction, NumberAction);
    CC_SYNTHESIZE(bool, _beginContact, BeginContact);
    CC_SYNTHESIZE(bool, _endContact, EndContact);
    // 1 - top, 2 bottom, 3 left, 4 right
    CC_SYNTHESIZE(int, _directionContact, DirectionContact);
    //Constructor
    GameManager();

    //Instance of the singleton
    static GameManager* m_mySingleton;

public:
    //Get instance of singleton
    static GameManager* sharedGameManager();
    
};

#endif