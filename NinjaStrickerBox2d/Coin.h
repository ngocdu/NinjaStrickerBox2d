//
//  Coin.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/16.
//
//

#ifndef NinjaStrickerBox2d_Coin_h
#define NinjaStrickerBox2d_Coin_h
#include "cocos2d.h"
using namespace cocos2d;
class Coin : public CCSprite
{
public:
    //Constructor
    Coin();
    void action();
    void actionMoveTop();
private:
    CC_SYNTHESIZE(int, _point, Point);
};

#endif
