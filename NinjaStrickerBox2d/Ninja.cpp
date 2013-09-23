//
//  Ninja.cpp
//  NinjaStrickerBox2d
//
//  Created by MinhNT on 13/09/17.
//
//
#define PTM_RATIO 32
#include "Ninja.h"
Ninja::Ninja()
: _mpBody(NULL)
{
    
}
// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool Ninja::isDirty(void)
{
    return true;
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform Ninja::nodeToParentTransform(void)
{
    b2Vec2 pos  = _mpBody->GetPosition();
    
    float x = pos.x * PTM_RATIO;
    float y = pos.y * PTM_RATIO;
    
    if ( isIgnoreAnchorPointForPosition() ) {
        x += m_obAnchorPointInPoints.x;
        y += m_obAnchorPointInPoints.y;
    }
    
    // Make matrix
    float radians = _mpBody->GetAngle();
    float c = cosf(radians);
    float s = sinf(radians);
    
    if( ! m_obAnchorPointInPoints.equals(CCPointZero) ){
        x += c*-m_obAnchorPointInPoints.x + -s*-m_obAnchorPointInPoints.y;
        y += s*-m_obAnchorPointInPoints.x + c*-m_obAnchorPointInPoints.y;
    }
    
    // Rot, Translate Matrix
    m_sTransform = CCAffineTransformMake( c,  s,
                                         -s,    c,
                                         x,    y );
    
    return m_sTransform;
}
void Ninja::actionAttack() {
    CCAnimation *anim=CCAnimation::create();
    anim->addSpriteFrameWithFileName("ninja_attack.png");
    anim->addSpriteFrameWithFileName("ninja_attack3.png");
    anim->addSpriteFrameWithFileName("ninja_attack2.png");
    anim->setDelayPerUnit(2.8f / 24.0f);
    anim->setRestoreOriginalFrame(true);
    CCAnimate * animet=CCAnimate::create(anim);
    this->getImage()->runAction(animet);
}
void Ninja::actionAttack2() {
    CCAnimation *anim=CCAnimation::create();
    anim->addSpriteFrameWithFileName("ninja_attack.png");
    anim->addSpriteFrameWithFileName("ninja_attack3.png");
    anim->addSpriteFrameWithFileName("ninja_attack2.png");
    anim->addSpriteFrameWithFileName("ninja_attack.png");
    anim->addSpriteFrameWithFileName("ninja_attack3.png");
    anim->addSpriteFrameWithFileName("ninja_attack2.png");
    anim->setDelayPerUnit(2.8f / 24.0f);
    anim->setRestoreOriginalFrame(true);
    CCAnimate * animet=CCAnimate::create(anim);
    this->getImage()->runAction(animet);
}
void Ninja::effectsAddPoint(int point) {
    char scoreStr[20] = {0};
    sprintf(scoreStr, "%i", point);
    CCLabelTTF *lable = CCLabelTTF::create(scoreStr, "", 25);
    lable->setPosition(this->getPosition());
    int k = rand() % (int)(this->getImage()->getContentSize().width * 2)  -   this->getImage()->getContentSize().width;
    CCMoveTo * moveTop = CCMoveTo::create(2, CCPoint(lable->getPosition().x + k, lable->getPosition().y + 70));
    CCHide * hide = CCHide::create();
    CCSequence * sq = CCSequence::create(moveTop, hide, NULL);
    lable->runAction(sq);
    this->getParent()->addChild(lable);
}
