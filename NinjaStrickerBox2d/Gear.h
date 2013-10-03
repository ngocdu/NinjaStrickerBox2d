//
//  Gear.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/10/03.
//
//

#ifndef NinjaStrickerBox2d_Gear_h
#define NinjaStrickerBox2d_Gear_h
#include "cocos2d.h"
#include "Box2D.h"
using namespace cocos2d;
class Gear : public CCSprite
{
public:
    Gear();
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
    CC_SYNTHESIZE(b2Body *, _mpBody, MpBody);
    // 1 - coincides with clockwise rotation, -1 - contrast with clockwise rotation
    CC_SYNTHESIZE(int , _directionRotation, DirectionRotation);
};

#endif
