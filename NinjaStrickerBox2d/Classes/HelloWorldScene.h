//
//  HelloWorldScene.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/09.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#ifndef __HELLO_WORLD_H__
#define __HELLO_WORLD_H__

// When you import this file, you import all the cocos2d classes
#include "cocos2d.h"
#include "Box2D.h"
#include "GB2ShapeCache-x.h"
#include "GLES-Render.h"
#include "GameManager.h"
#include "MyContactListener.h"
#include "Coin.h"
#include "Snake.h"
#include "Scorpion.h"
#include "Arrow.h"
#include "Ninja.h"
#include "GameMenu.h"
using namespace cocos2d;
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
    void updatePhantom(float dt);
    void updateCheckStop(float dt);
    void updateScoreLife(float dt);
    void touch( CCPoint location);
    void touch1( CCPoint location);
    
    
    void setViewPointCenter(CCPoint position);
    
    void registerWithTouchDispatcher();
    
    void setPlayerPosition(CCPoint position);
    CCPoint tileCoordForPosition(CCPoint position);
    
    void prepareLayers();
    void createFixtures(CCTMXLayer* layer);
    void createRectangularFixture(CCTMXLayer* layer, int x, int y,
                                  float width, float height);
    void createRectangularFixtureWithPoint(CCPoint p1, CCPoint p2);
    CCPoint convertPoitMapToPixel(CCPoint pointMap);
    CCPoint convertPoitMapToPixelReverseY(CCPoint pointMap);
    CCPoint convertMetterToPixel(CCPoint p);
    
    CCPoint convertPixelToMetter(CCPoint p);
    
    void addCoins();
    void addSnakes();
    void addScorpions();
    void addArrows();
    
    void addWalls();
    
    void updateLocation_Direction(float dt);
    
    void removeSprite(CCNode * node);
    void removeCoin(CCNode * node);
    void removeSnake(CCNode * node);
    void removeScorpion(CCNode * node);
    
    void checkTouchPoint(CCPoint p);
    void arrowAttack(int direction);
    void attackTaget(CCPoint p);
    
    void click_pause(CCObject* pSender);
    void click_quit(CCObject* pSender);
    void click_continue(CCObject* pSender);
private:
    CCSize s;
    CCLayer * _layerBg;
    b2World* world;
    cocos2d::CCTexture2D* m_pSpriteTexture; // weak ref
    Ninja * _player;
    float delta;
    int delayPlayer;
    CCTMXTiledMap *_tileMap;
    
    CCTMXLayer *_background;
    
    CCTMXLayer *_meta;
    
    CCTMXLayer *_coin;
    
    CCTMXLayer *_snake;
    
    CCTMXLayer *_scorpion;
    
    CCTMXLayer *_arrow;
    
    float withTileMap;
    float heightTileMap;
    GLESDebugDraw *m_debugDraw;

    
    CCPoint touchLocation;
    bool giamVanToc;
    bool isTouchTop;
    bool isTouchRight;
    
    MyContactListener *_contactListener;
    bool _contactting;
    
    // ------------ array------------
    CCArray * _arrayCoin;
    CCArray * _arraySnake;
    CCArray * _arrayScorpion;
    CCArray * _arrayArrow;
    CCArray * _arrayRemoveSnake;
    CCArray * _arrayRemoveScorpion;
    CCArray * _arrayRemoveCoin;
    
    //--------------score-------------
    int _scores;
    CCLabelTTF *_lbScores;
    
    int _lifes;
    CCLabelTTF *_lbLifes;
    
    CCSprite * spriteTaget;
    
    CCMenu * menu;
    CCMenuItemFont * btpause;
    CCMenuItemFont * btquit;
    CCMenuItemFont * btcontinue;
};

#endif // __HELLO_WORLD_H__
