//
//  GamePlayWithBoss.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/23.
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
#include "GameMenu.h"
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
    void updateBoss(float dt);
    void updateTime(float dt);
    void updatePhantom(float dt);
    void updateCheckStop(float dt);
    void updateLocation_Direction(float dt);
    void registerWithTouchDispatcher();
    void setViewPointCenter(CCPoint position);
    void addNewSpriteAtPosition(cocos2d::CCPoint p);
    void addNewBossAtPosition(cocos2d::CCPoint p);
    void touch( CCPoint location);
    void touchBoss( CCPoint location);
    
    void bossAttack();
    void attackBoss();
    
    CCPoint convertPoitMapToPixel(CCPoint pointMap);
    CCPoint convertPoitMapToPixelReverseY(CCPoint pointMap);
    CCPoint convertMetterToPixel(CCPoint p);
    
    CCPoint convertPixelToMetter(CCPoint p);
    void createRectangularFixtureWithPoint(CCPoint p1, CCPoint p2);
    void addWalls();
    void checkTouchPoint(CCPoint p);
    
    void removeSprite(CCNode * node);
    
    void click_pause(CCObject* pSender);
    void click_quit(CCObject* pSender);
    void click_continue(CCObject* pSender);
    void click_reset(CCObject* pSender);
private:
    CCSize size;
    b2World* world;
    GLESDebugDraw *m_debugDraw;
    CCTMXTiledMap *_tileMap;
    
    CCTMXLayer *_background;
    Ninja * _player;
    Boss * _boss;
    CCArray * array_taget;
    float maxHeightJumpBoss;
    float maxWithJumpBoss;
    bool bossContacting;
    
    int timeDelayContactBoss;
    int timeDelayAttackBoss;
    
    MyContactListener *_contactListener;
    bool _contactting;
    
    bool isTouchRight;
    CCPoint touchLocation;
    bool giamVanToc;
    bool isTouchTop;
    
    bool ninjaAttack;
    bool isBossLeftPlayer;
    
    float withTileMap;
    float heightTileMap;
    
    //-------------score -----------
    float _bloodBoss;
    CCProgressTimer *_progressBloodBoss;
    int _lifes;
    CCLabelTTF *_lbLifes;
    
    CCMenu * menu;
    CCMenuItemFont * btpause;
    CCMenuItemFont * btquit;
    CCMenuItemFont * btcontinue;
    
    //--------------end game -------
    CCMenuItemFont * btreset;
    CCLabelTTF *_lbWin;
    CCLabelTTF *_lbLost;
};
#endif