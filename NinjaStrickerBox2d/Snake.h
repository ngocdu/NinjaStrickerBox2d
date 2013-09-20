//
//  Snake.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/16.
//
//

#ifndef NinjaStrickerBox2d_Snake_h
#define NinjaStrickerBox2d_Snake_h
#include "cocos2d.h"
using namespace cocos2d;
class Snake : public CCSprite
{
private:
    CC_SYNTHESIZE(int, _point, Point);
    CC_SYNTHESIZE(int, _direction, Direction);
    CC_SYNTHESIZE(CCPoint, _location, Location);
public:
    Snake();
    void actionMoveToPoint(CCPoint p);
};

#endif
