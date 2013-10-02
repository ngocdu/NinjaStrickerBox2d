//
//  Blood.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/10/02.
//
//

#ifndef NinjaStrickerBox2d_Blood_h
#define NinjaStrickerBox2d_Blood_h
#include "cocos2d.h"
#include "Box2D.h"
using namespace cocos2d;
class Blood : public CCSprite
{
public:
    Blood();
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
    CC_SYNTHESIZE(b2Body *, _mpBody, MpBody);
};

#endif
