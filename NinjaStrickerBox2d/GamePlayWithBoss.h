//
//  GamePlayWithBoss.h
//  NinjaStrickerBox2d
//
//  Created by MinhNT on 13/09/23.
//
//

#ifndef NinjaStrickerBox2d_GamePlayWithBoss_h
#define NinjaStrickerBox2d_GamePlayWithBoss_h
#include "cocos2d.h"
#include "Box2D.h"
#include "GB2ShapeCache-x.h"
#include "GLES-Render.h"
#include "MyContactListener.h"
#include "Ninja.h"
#include "Boss.h"
using namespace cocos2d;
class GamePlayWithBoss : public CCLayer
{
public:
    ~GamePlayWithBoss();
    GamePlayWithBoss();
    static cocos2d::CCScene* scene();
    void initPhysics();
    virtual void draw();
    bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void update(float dt);
    void updatePhantom(float dt);
    void updateCheckStop(float dt);
    void updateLocation_Direction(float dt);
    void registerWithTouchDispatcher();
    void setViewPointCenter(CCPoint position);
    void addNewSpriteAtPosition(cocos2d::CCPoint p);
    void addNewBossAtPosition(cocos2d::CCPoint p);
    void touch( CCPoint location);
    
    CCPoint convertPoitMapToPixel(CCPoint pointMap);
    CCPoint convertPoitMapToPixelReverseY(CCPoint pointMap);
    CCPoint convertMetterToPixel(CCPoint p);
    
    CCPoint convertPixelToMetter(CCPoint p);
    void createRectangularFixtureWithPoint(CCPoint p1, CCPoint p2);
    void addWalls();
    void checkTouchPoint(CCPoint p);
    
    void removeSprite(CCNode * node);
private:
    b2World* world;
    GLESDebugDraw *m_debugDraw;
    CCTMXTiledMap *_tileMap;
    
    CCTMXLayer *_background;
    Ninja * _player;
    Boss * _boss;
    
    MyContactListener *_contactListener;
    bool _contactting;
    
    bool isTouchRight;
    CCPoint touchLocation;
    bool giamVanToc;
    bool isTouchTop;
    
    float withTileMap;
    float heightTileMap;
};


#endif
