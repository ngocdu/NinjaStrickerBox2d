//
//  HelloWorldScene.h
//  NinjaStrickerBox2d
//
//  Created by MinhNT on 13/09/09.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#ifndef __HELLO_WORLD_H__
#define __HELLO_WORLD_H__

// When you import this file, you import all the cocos2d classes
#include "cocos2d.h"
#include "Box2D.h"
#include "GB2ShapeCache-x.h"
#include "GLES-Render.h"
#include "MyContactListener.h"
using namespace cocos2d;
class PhysicsSprite : public cocos2d::CCSprite
{
public:
    PhysicsSprite();
//    void setPhysicsBody(b2Body * body);
//    b2Body getPhysicsBody();
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
//    b2Body* m_pBody;    // strong ref
    CC_SYNTHESIZE(b2Body *, _mpBody, MpBody);
};

class HelloWorld : public cocos2d::CCLayer {
public:
    ~HelloWorld();
    HelloWorld();
    
    // returns a Scene that contains the HelloWorld as the only child
    static cocos2d::CCScene* scene();
    
    void initPhysics();
    // adds a new sprite at a given coordinate
    void addNewSpriteAtPosition(cocos2d::CCPoint p);

    virtual void draw();
    bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void update(float dt);
    void touch( CCPoint location);
    
    
    void setViewPointCenter(CCPoint position);
    
    void registerWithTouchDispatcher();
    
    void setPlayerPosition(CCPoint position);
    CCPoint tileCoordForPosition(CCPoint position);
    
    void prepareLayers();
    void createFixtures(CCTMXLayer* layer);
    void createRectangularFixture(CCTMXLayer* layer, int x, int y,
                                  float width, float height);
    
//    void addCharacter();
private:
    b2World* world;
    cocos2d::CCTexture2D* m_pSpriteTexture; // weak ref
    PhysicsSprite * _player;
    float delta;
    
    CCTMXTiledMap *_tileMap;
    
    CCTMXLayer *_background;
    
    CCTMXLayer *_meta;
    
//    CCTMXLayer *_foreground;
    float withTileMap;
    float heightTileMap;
    GLESDebugDraw *m_debugDraw;
//    
//    CCArray * _array_money;
//    CCArray * _array_
    
    CCPoint touchLocation;
    bool giamVanToc;
    bool isTouchTop;
    
    MyContactListener *_contactListener;
    bool _contactting;
};

#endif // __HELLO_WORLD_H__
