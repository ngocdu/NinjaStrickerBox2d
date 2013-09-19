//
//  Ninja.h
//  NinjaStrickerBox2d
//
//  Created by MinhNT on 13/09/17.
//
//

#ifndef NinjaStrickerBox2d_Ninja_h
#define NinjaStrickerBox2d_Ninja_h
#include "cocos2d.h"
#include "Box2D.h"
using namespace cocos2d;
class Ninja : public CCSprite
{
public:
    Ninja();
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
    void actionAttack();
    void actionAttack2();
private:
    CC_SYNTHESIZE(b2Body *, _mpBody, MpBody);
    // 0 nothing 1 - attack  2 - was attack 3 - attack to taget
    CC_SYNTHESIZE(int, _attack, Attack);
    CC_SYNTHESIZE(CCSprite *, _image, Image);
    CC_SYNTHESIZE(CCPoint , _point, Point);
    CC_SYNTHESIZE(bool , _stop, Stop);
};




#endif
