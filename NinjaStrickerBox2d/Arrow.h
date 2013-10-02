//
//  Arrow.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/16.
//
//

#ifndef NinjaStrickerBox2d_Arrow_h
#define NinjaStrickerBox2d_Arrow_h
#include "cocos2d.h"
using namespace cocos2d;
class Arrow : public CCSprite
{
private:
    
    CC_SYNTHESIZE(int, _direction, Direction);
public:
    Arrow();
    // 1 -> 8 tuong duong voi 8 huong cung chieu kim dong ho : 1 ->
    Arrow(int direction);
    void rotate();
};
#endif
