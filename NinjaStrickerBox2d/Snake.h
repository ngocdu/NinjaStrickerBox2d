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
    
public:
    Snake();
    void actionMoveToPoint(CCPoint p);
};

#endif
