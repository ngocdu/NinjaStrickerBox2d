//
//  Scorpion.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/16.
//
//

#ifndef NinjaStrickerBox2d_Scorpion_h
#define NinjaStrickerBox2d_Scorpion_h
#include "cocos2d.h"
using namespace cocos2d;
class Scorpion : public CCSprite
{
private:
    CC_SYNTHESIZE(int, _direction, Direction);
    CC_SYNTHESIZE(CCPoint, _location, Location);
    CC_SYNTHESIZE(int, _point, Point);
public:
    Scorpion();
    void fluctuating(float amplitude);
    void fluctuatingAndMove(float amplitude, float distances);
};
#endif
