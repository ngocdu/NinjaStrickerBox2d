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
    
    CC_SYNTHESIZE(bool, _beginContactBoss, BeginContactBoss);
    CC_SYNTHESIZE(bool, _endContactBoss, EndContactBoss);
    
    // 1 - top, 2 bottom, 3 left, 4 right
    CC_SYNTHESIZE(int, _directionContact, DirectionContact);
    CC_SYNTHESIZE(int, _numberActionPlayer, NumberActionPlayer);
    //Constructor
    GameManager();

    //Instance of the singleton
    static GameManager* m_mySingleton;
    CC_SYNTHESIZE(float, _SCALE_N_X, SCALE_N_X);
    CC_SYNTHESIZE(float, _SCALE_N_Y, SCALE_N_Y);
public:
    //Get instance of singleton
    static GameManager* sharedGameManager();
    
};

#endif