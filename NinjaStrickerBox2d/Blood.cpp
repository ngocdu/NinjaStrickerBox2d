//
//  Blood.cpp
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/10/02.
//
//
#define PTM_RATIO 32
#include "Blood.h"
Blood::Blood()
: _mpBody(NULL)
{
    
}
// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool Blood::isDirty(void)
{
    return true;
}
// returns the transform matrix according the Chipmunk Body values
CCAffineTransform Blood::nodeToParentTransform(void)
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
