//
//  HelloWorldScene.cpp
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/09.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GB2ShapeCache-x.h"
#include <math.h>
using namespace cocos2d;
using namespace CocosDenshion;

#define PTM_RATIO 32

enum {
    kTagParentNode = 1,
};

HelloWorld::HelloWorld()
{
    // load physics shapes
    GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("ninjaS.plist");
    
    s = CCDirector::sharedDirector()->getWinSize();
    delta = 3.14f;
    touchBack = true;
    setTouchEnabled( true );
    setAccelerometerEnabled( true );
    // init physics
    this->initPhysics();
    GameManager::sharedGameManager()->setNumberActionPlayer(2);
    //---------------------------------
    _tileMap = new CCTMXTiledMap();
    _tileMap->initWithTMXFile("TileMap.tmx");
    _background = _tileMap->layerNamed("Background");
    _coin = _tileMap->layerNamed("Coin");
    _snake = _tileMap->layerNamed("Snake");
    _scorpion = _tileMap->layerNamed("Scorpion");
    _arrow = _tileMap->layerNamed("Arrow");
    _meta = _tileMap->layerNamed("Meta");
    _coin->setVisible(false);
    _snake->setVisible(false);
    _scorpion->setVisible(false);
    _meta->setVisible(false);
    _arrow->setVisible(false);
    
    CCLOG("size tiled: %f , %f",_tileMap->getTileSize().width, _tileMap->getTileSize().height);
    withTileMap = _tileMap->getMapSize().width * _tileMap->getTileSize().width;
    heightTileMap = _tileMap->getMapSize().height * _tileMap->getTileSize().height;
    CCLog("size map with %f", withTileMap);

    this->addChild(_tileMap);
    
    CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("Objects");
    
    if(objectGroup == NULL){
        CCLog("tile map has no objects object layer");
    }    
    addNewSpriteAtPosition(convertPoitMapToPixel(CCPoint(3, 19)));    
    //----------------------------------
    _arrayCoin = new CCArray();
    _arraySnake = new CCArray();
    _arrayScorpion = new CCArray();
    _arrayArrow = new CCArray();
    _arrayRemoveCoin = new CCArray();
    _arrayRemoveSnake = new CCArray();
    _arrayRemoveScorpion = new CCArray();
    _arrayBlood = new CCArray();
    _arrayBloodRemove = new CCArray();
    this->addCoins();
    this->addSnakes();
    this->addScorpions();
    this->addArrows();    
    this->addWalls();
    
    this->addGroupGear();
    
    //----------point ,life-----------------------
    _scores = 0;
    _lifes = 3;
    _layerBg = CCLayer::create();
    _layerBg->setPosition(CCPoint(s.width/2, s.height/2));
    _layerBg->setContentSize(CCSize(s.width, s.height));
    this->addChild(_layerBg, 1000);
    _lbScores = CCLabelTTF::create("0", "", 20);
    _lbScores->setPosition(CCPoint(s.width/2 - 100, s.height/2 - 80));
    _layerBg->addChild(_lbScores, 100);
    _lbLifes = CCLabelTTF::create("X  X  X", "", 40);
    _lbLifes->setPosition(CCPoint(s.width/2 - 100, s.height/2 - 40));
    _layerBg->addChild(_lbLifes, 100);
    
    _lbWin = CCLabelTTF::create("YOU WIN", "", 40);
    _lbWin->setPosition(CCPoint(0, -40));
    _lbWin->setVisible(false);
    _lbWin->setColor(ccc3(0, 0, 255));
    _layerBg->addChild(_lbWin, 1000);
    
    _lbLost = CCLabelTTF::create("YOU LOST", "", 40);
    _lbLost->setPosition(CCPoint(0, -40));
    _lbLost->setVisible(false);
    _lbLost->setColor(ccc3(0, 0, 255));
    _layerBg->addChild(_lbLost, 1000);
    
    btpause = CCMenuItemFont::create("PAUSE", this, menu_selector(HelloWorld::click_pause));
    btpause->setPosition(CCPoint( - s.width/2, -50));
    
    btquit = CCMenuItemFont::create("QUIT", this, menu_selector(HelloWorld::click_quit));
    btquit->setPosition(CCPoint(- s.width/2, - s.height/2 * 0.275f));
    btquit->setVisible(false);
    
    btcontinue = CCMenuItemFont::create("CONTINUE", this, menu_selector(HelloWorld::click_continue));
    btcontinue->setPosition(CCPoint(- s.width/2, - s.height/4));
    btcontinue->setVisible(false);
    
    btreset = CCMenuItemFont::create("RESET", this, menu_selector(HelloWorld::click_reset));
    btreset->setPosition(CCPoint(- s.width/2, - s.height/4));
    btreset->setVisible(false);
    
    menu = CCMenu::create(btpause, btquit, btcontinue, btreset, NULL);
    menu->setPosition(CCPoint( s.width/2 ,  s.height/2 ));
    _layerBg->addChild(menu, 10);
    
    CCLog(" bt pause  x: %f, y : %f", btpause->getPosition().x, btpause->getPosition().y);
    
    delayPlayer = 25;
    spriteTaget = NULL;
    this->setViewPointCenter(_player->getPosition());
    
    this->schedule(schedule_selector(HelloWorld::updateLocation_Direction), 0.1f);
    this->schedule(schedule_selector(HelloWorld::updateCheckStop), 0.05f);
    this->schedule(schedule_selector(HelloWorld::updatePhantom), 0.02f);
    this->schedule(schedule_selector(HelloWorld::updateScoreLife), 0.2f);
    this->schedule(schedule_selector(HelloWorld::updateEtfect), 0.1f);
    scheduleUpdate();
}

HelloWorld::~HelloWorld()
{
    delete world;
    world = NULL;    
    delete m_debugDraw;
}

void HelloWorld::initPhysics()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    b2Vec2 gravity;
    gravity.Set(0.0f, -15.0f);
    world = new b2World(gravity);
    _contactListener = new MyContactListener();
    _contactListener->setNumberBegin(0);
    world->SetContactListener(_contactListener);
    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);
    world->SetContinuousPhysics(true);
    m_debugDraw = new GLESDebugDraw( PTM_RATIO );
    world->SetDebugDraw(m_debugDraw);
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    m_debugDraw->SetFlags(flags);
    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0); // bottom-left corner
    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    b2Body* groundBody = world->CreateBody(&groundBodyDef);
    // Define the ground box shape.
    b2EdgeShape groundBox;
    
    // bottom
    groundBox.Set(b2Vec2(0,0), b2Vec2(withTileMap/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
    // top
    groundBox.Set(b2Vec2(0, heightTileMap/PTM_RATIO), b2Vec2(withTileMap/PTM_RATIO, heightTileMap/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);
    // left
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0 ,0));
    groundBody->CreateFixture(&groundBox,0);
    // right
    groundBox.Set(b2Vec2(withTileMap/PTM_RATIO, s.height/PTM_RATIO), b2Vec2(withTileMap/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
}

void HelloWorld::draw()
{
    //
    // IMPORTANT:
    // This is only for debug purposes
    // It is recommend to disable it
    //
    CCLayer::draw();
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
    kmGLPushMatrix();
    world->DrawDebugData();
    kmGLPopMatrix();
}

void HelloWorld::addNewSpriteAtPosition(CCPoint p)
{
    CCSprite *sp = CCSprite::create("ninja.png");
    _player = new Ninja();
    _player->setAttack(4);
    _player->init();
    this->addChild(_player, 1000);
    _player->addChild(sp);
    _player->setImage(sp);
    
    _player->setPosition( CCPointMake( p.x, p.y) );
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
    bodyDef.userData = _player;
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    
//    b2Vec2 vertices[4];
//    vertices[0].Set(-1.2f, -1.2f);    
//    vertices[1].Set(1.2f, -1.2f);    
//    vertices[2].Set(1.2f, 1.2f);
//    vertices[3].Set(-1.2f, 1.2f);
//    
//    int32 count = 4;
//
//    b2PolygonShape polygon;
//    
//    dynamicBox.Set(vertices, count);
    
    b2CircleShape circle;
    
    float _radius = _player->getImage()->getContentSize().width / 2;
    circle.m_radius = _radius / PTM_RATIO;


    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
//    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 4.0f; // trong luong
    fixtureDef.friction = 1.0f; //ma sat
    fixtureDef.restitution = 0;
    fixtureDef.filter.groupIndex = -10;
    body->CreateFixture(&fixtureDef);
    
    _player->setMpBody(body);
    
    //------------- use GB2ShapeCache - note: no remove ---------
//    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
//    sc->addFixturesToBody(body, "ninja");
//    _player->setAnchorPoint(sc->anchorPointForShape("ninja"));
//    _player->setScale(0.5f);
    
    
//     _player = new PhysicsSprite();
//    _player->initWithFile("ninja_attack.png");
//    
//    _player->setPosition(p);
//    
//    this->addChild(_player);
//    
//	b2BodyDef bodyDef;
//	bodyDef.type = b2_dynamicBody;
//    
//	bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
//	bodyDef.userData = _player;
//	b2Body *body = world->CreateBody(&bodyDef);
//    
//    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
//    sc->addFixturesToBody(body, "ninja_attack");
//    _player->setAnchorPoint(sc->anchorPointForShape("ninja_attack"));
//    body->GetFixtureList()->SetDensity(5.0f);
//    body->GetFixtureList()->SetFriction(1.0f);
//    _player->setMpBody(body);
//    //khong co tac dung gi 
//    _player->setScale(0.1f);
//    CCScaleTo *scale = CCScaleTo::create(0, 0.5f);
//    _player->runAction(scale);
    
//    CCAnimation *anim=CCAnimation::create();
//    for (int i = 1; i <= 5; i++) {
//        char strname[20] = {0};
//        sprintf(strname, "%i.png", i);
//        string file_name = strname;
//        anim->addSpriteFrameWithFileName(file_name.c_str());
//    }
//    anim->setDelayPerUnit(2.8f / 19.0f);
//    anim->setRestoreOriginalFrame(true);
//    CCAnimate * animet=CCAnimate::create(anim);
//    CCRepeatForever * rep=CCRepeatForever::create(animet);
//    rep->setTag(123456);
////    _player->runAction(rep);
//    
//    CCSprite * attack = CCSprite::create("ninja_attack.png");
//    CCRotateTo * rotete = CCRotateTo::create(1, 180);
//    CCRotateTo * rotete2 = CCRotateTo::create(1, 360);
//    CCSequence * sq = CCSequence::create(rotete, rotete2, NULL);
//    CCRepeatForever * rep2=CCRepeatForever::create(sq);
//    attack->runAction(rep2);
////    _player->addChild(attack);
}
void HelloWorld::addBlood(cocos2d::CCPoint p, b2Vec2 impluse) {
    Blood * blood = new Blood();
    blood->initWithFile("fire.png");
    blood->setColor(ccc3(255, 0, 0));
    this->addChild(blood, 1000);
    blood->setContentSize(CCSize(5, 5));
    blood->setScale(0.2f);
    blood->setPosition( CCPointMake( p.x, p.y) );
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
    bodyDef.userData = blood;
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    b2CircleShape circle;
    
    float _radius = blood->getContentSize().width / 4;
    circle.m_radius = _radius / PTM_RATIO;
    
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = .1f; // trong luong
    fixtureDef.friction = 1.0f; //ma sat
    fixtureDef.restitution = 0;
    fixtureDef.filter.groupIndex = -10;
    body->CreateFixture(&fixtureDef);
    body->GetFixtureList()->SetSensor(true);
    body->SetLinearVelocity(impluse);
    blood->setMpBody(body);
    _arrayBlood->addObject(blood);
}
void HelloWorld::addGroupBlood(cocos2d::CCPoint p, int numberBoold) {
    for (int i = 1; i <= numberBoold; i++) {
        int x = (rand() % 3 - 1) * 40 ;
        int y = (rand() % 3 - 1) * 40 ;
        this->addBlood(p, b2Vec2(x, y));
    } 
}
void HelloWorld::addGear(cocos2d::CCPoint p, int direction, int r) {
    Gear * gear = new Gear();
    gear->setDirectionRotation(direction);
    gear->setScale(0.5f);
    gear->initWithFile("fire.png");
//    gear->setColor(ccc3(255, 0, 0));
    this->addChild(gear, 1000);
//    gear->setContentSize(CCSize(5, 5));
//    gear->setScale(0.2f);
    gear->setPosition( CCPointMake( p.x, p.y) );
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
    bodyDef.userData = gear;
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    b2CircleShape circle;
    
    float _radius = gear->getContentSize().width / r;
    circle.m_radius = _radius / PTM_RATIO;
    
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 10.1f; // trong luong
    fixtureDef.friction = 1.0f; //ma sat
    fixtureDef.restitution = 0;
//    fixtureDef.filter.groupIndex = -10;
    body->CreateFixture(&fixtureDef);
//    body->GetFixtureList()->SetSensor(true);
    body->SetAngularVelocity(10000 * direction);
    gear->setMpBody(body);
}
void HelloWorld::addGroupGear() {
    for (int i = 113; i <= 129 ; i++) {
        CCPoint p1 = convertPoitMapToPixelReverseY(CCPoint(i, 5));
        if (i == 113 || i == 129) {
            this->addGear(p1, 1, 4);
        }else
        this->addGear(p1, 1, 2);
    }
    for (int i = 113; i <= 129 ; i++) {
        CCPoint p1 = convertPoitMapToPixelReverseY(CCPoint(i, 8));
        if (i == 113 || i == 129) {
            this->addGear(p1, 1, 4);
        }else
        this->addGear(p1, 1, 2);
    }
}
#pragma mark - update
void HelloWorld::update(float dt)
{
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/
    
    if (_player->getMpBody()) {
        _player->setPositionX(_player->getMpBody()->GetPosition().x * PTM_RATIO);
        _player->setPositionY(_player->getMpBody()->GetPosition().y * PTM_RATIO);
        //true thi _player khong xoay false thi xoay
        _player->getMpBody()->SetFixedRotation(true);
//        _player->getMpBody()->SetFixedRotation(false);
        
        if (_player->getMpBody()->GetPosition().y * PTM_RATIO > touchLocation.y &&
            giamVanToc == true && isTouchTop == true && _player->getAttack() == 1) {
            _player->setAttack(2);
            b2Vec2 v = _player->getMpBody()->GetLinearVelocity();
            CCLog("velocity x: %f", v.x);
            CCLog("velocity y: %f", v.y);
            float diffx = abs(_player->getMpBody()->GetPosition().x * PTM_RATIO - touchLocation.x);
            float diffy = abs(_player->getMpBody()->GetPosition().y * PTM_RATIO - touchLocation.y);
            CCLog("diff y / diff x: %f", v.y / v.x);
            if (abs(v.y / v.x) > 1 && abs(v.x) > 40) {
                if (abs(v.x) > 20) {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/15 + 1), v.y  /(diffy/50 + 1)));
                }else if (abs(v.y) > 12) {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/20 + 1), v.y  /(diffy/30 + 1)));
                }else {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/20 + 1), v.y  /(diffy/50 + 1)));
                }

            }else if (abs(v.y / v.x) < 1 && abs(v.x) < 20) {
                if (abs(v.x) > 20) {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/150 + 1), v.y  /(diffy/50 + 1)));
                }else if (abs(v.y) > 12) {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/200 + 1), v.y  /(diffy/30 + 1)));
                }else {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/200 + 1), v.y  /(diffy/50 + 1)));
                }
            }else if (abs(v.y / v.x) > 1  || abs(v.x) > 20){
                if (abs(v.x) > 20) {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/50 + 1), v.y  /(diffy/50 + 1)));
                }else if (abs(v.y) > 12) {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/100 + 1), v.y  /(diffy/30 + 1)));
                }else {
                    _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x  /(diffx/100 + 1), v.y  /(diffy/50 + 1)));
                }
            }
            //dung yen khong quay ************ cam xoa doan nay 
//            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
//            _player->getMpBody()->SetGravityScale(0);
//            
//            _player->getMpBody()->SetAngularVelocity(0);
//            //giam soc
//            _player->getMpBody()->SetLinearDamping(0);
//            _player->getMpBody()->se 
            giamVanToc = false;
        }else if (_player->getMpBody()->GetPosition().y * PTM_RATIO < touchLocation.y &&
                  giamVanToc == true && isTouchTop == false && _player->getAttack() == 1) {
            _player->setAttack(2);
            b2Vec2 v = _player->getMpBody()->GetLinearVelocity();
            CCLog("velocity x: %f", v.x);
            CCLog("velocity y: %f", v.y);
            float diffx = abs(_player->getMpBody()->GetPosition().x * PTM_RATIO - touchLocation.x);
            float diffy = abs(_player->getMpBody()->GetPosition().y * PTM_RATIO - touchLocation.y);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x /(diffx/100 + 1), v.y /(diffy/30 + 1)));
            giamVanToc = false;
        }
        
    }
    
    this->setViewPointCenter(ccp(_player->getPosition().x ,
                                 _player->getPosition().y ));
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);

    if (_player->getAttack() == 1 && isTouchRight == true) {
        //do something
    }else if (_player->getAttack() == 1 && isTouchRight == false) {
        //do something
    }else {
        _player->getMpBody()->SetAngularVelocity(0);
        _player->getMpBody()->SetTransform(_player->getMpBody()->GetPosition(), 0);
    }
    
    // Collision Detection ----------- contact wall ----------------------------
    //begin contact
    if (GameManager::sharedGameManager()->getBeginContact() == true) {
        if (_player->getAttack() != 0) {
            _player->setAttack(0);
        }
        _player->getMpBody()->SetAngularVelocity(0);
        CCAnimation *anim=CCAnimation::create();
        //top
        if (GameManager::sharedGameManager()->getDirectionContact() == 1) {
            anim->addSpriteFrameWithFileName("ninja_attack.png");
            anim->addSpriteFrameWithFileName("ninja.png");
            _player->getMpBody()->SetGravityScale(1);
        //bottom
        }else if (GameManager::sharedGameManager()->getDirectionContact() == 2) {
            anim->addSpriteFrameWithFileName("ninja_attack.png");
            anim->addSpriteFrameWithFileName("ninja.png");
            anim->addSpriteFrameWithFileName("ninja_attack.png");
            anim->addSpriteFrameWithFileName("ninja.png");
            _player->setFlipY(true);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
            _player->getMpBody()->SetGravityScale(0);
            _player->getMpBody()->SetAngularVelocity(0);
            //giam soc
            _player->getMpBody()->SetLinearDamping(0);
            
        //left
        }else if (GameManager::sharedGameManager()->getDirectionContact() == 3) {
            anim->addSpriteFrameWithFileName("ninja2_bam_tuong.png");
            _player->getImage()->setFlipX(true);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
            _player->getMpBody()->SetGravityScale(0);
            _player->getMpBody()->SetAngularVelocity(0);
            //giam soc
            _player->getMpBody()->SetLinearDamping(0);
        //right
        }else if (GameManager::sharedGameManager()->getDirectionContact() == 4) {
            anim->addSpriteFrameWithFileName("ninja2_bam_tuong.png");
//            _player->setFlipX(true);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
            _player->getMpBody()->SetGravityScale(0);
            _player->getMpBody()->SetAngularVelocity(0);
            //giam soc
            _player->getMpBody()->SetLinearDamping(0);
        }

        anim->setDelayPerUnit(2.8f / 4.0f);
        anim->setRestoreOriginalFrame(true);
        CCAnimate * animet=CCAnimate::create(anim);
        CCRepeatForever * rep=CCRepeatForever::create(animet);
        rep->setTag(123456);
        _player->getImage()->runAction(animet);
        GameManager::sharedGameManager()->setBeginContact(false);
    }else if (GameManager::sharedGameManager()->getBeginContactKinamic() == true) {
        GameManager::sharedGameManager()->setBeginContactKinamic(false);
        if (_player->getAttack() != 0) {
            _player->setAttack(0);
        }
        _player->getMpBody()->SetAngularVelocity(0);
        CCAnimation *anim=CCAnimation::create();
        //top
        if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 1) {
            anim->addSpriteFrameWithFileName("ninja_attack.png");
            anim->addSpriteFrameWithFileName("ninja.png");
            //bottom
        }else if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 2) {
            anim->addSpriteFrameWithFileName("ninja_attack.png");
            anim->addSpriteFrameWithFileName("ninja.png");
            anim->addSpriteFrameWithFileName("ninja_attack.png");
            anim->addSpriteFrameWithFileName("ninja.png");
            _player->setFlipY(true);
//            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
            _player->getMpBody()->SetGravityScale(-1);
            _player->getMpBody()->SetAngularVelocity(0);
            //giam soc
            _player->getMpBody()->SetLinearDamping(0);
            
            //left
        }else if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 3) {
            anim->addSpriteFrameWithFileName("ninja2_bam_tuong.png");
            _player->getImage()->setFlipX(true);
//            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
            _player->getMpBody()->SetGravityScale(0);
            _player->getMpBody()->SetAngularVelocity(0);
            //giam soc
            _player->getMpBody()->SetLinearDamping(0);
            //right
        }else if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 4) {
            anim->addSpriteFrameWithFileName("ninja2_bam_tuong.png");
            //            _player->setFlipX(true);
//            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
            _player->getMpBody()->SetGravityScale(0);
            _player->getMpBody()->SetAngularVelocity(0);
            //giam soc
            _player->getMpBody()->SetLinearDamping(0);
        }
        
        anim->setDelayPerUnit(2.8f / 4.0f);
        anim->setRestoreOriginalFrame(true);
        CCAnimate * animet=CCAnimate::create(anim);
        CCRepeatForever * rep=CCRepeatForever::create(animet);
        rep->setTag(123456);
        _player->getImage()->runAction(animet);
        GameManager::sharedGameManager()->setBeginContact(false);
    }
    //end contact
    if (GameManager::sharedGameManager()->getEndContact() == true) {
        _player->getImage()->stopActionByTag(123456);
        _player->getMpBody()->SetGravityScale(1);
        GameManager::sharedGameManager()->setEndContact(false);
        CCAnimation *anim=CCAnimation::create();
        anim->addSpriteFrameWithFileName("ninja.png");
        anim->setDelayPerUnit(2.8f / 6.0f);
        anim->setRestoreOriginalFrame(true);
        CCAnimate * animet=CCAnimate::create(anim);
        _player->getImage()->runAction(animet);
//        _player->getImage()->setFlipX(false);
        _player->getImage()->setFlipY(false);
        if (_player->getAttack() == 3) {
            this->touch1(touchLocation);
        }else {
            this->touch2(touchLocation);
        }
    }else if (GameManager::sharedGameManager()->getEndContactKinamic() == true) {
        if (GameManager::sharedGameManager()->getDirectionContactKinamic() != 2) {
            _player->getMpBody()->SetGravityScale(1);
        }
        GameManager::sharedGameManager()->setEndContactKinamic(false);
    }
    //---------------------change direction ------------------------------------
    CCObject * i1;
    CCARRAY_FOREACH(_arraySnake, i1) {
        Snake *snake = (Snake*)i1;
        if (snake->getPosition().x > snake->getLocation().x) {
            snake->setDirection(1);
            snake->setFlipX(true);
        }else if (snake->getPosition().x < snake->getLocation().x){
            snake->setDirection(0);
            snake->setFlipX(false);
        }
    }
    
    CCObject * i2;
    CCARRAY_FOREACH(_arrayScorpion, i2) {
        Scorpion *scorpion = (Scorpion*)i2;
        if (scorpion->getPosition().x > scorpion->getLocation().x) {
            scorpion->setDirection(1);
            scorpion->setFlipX(true);
        }else if (scorpion->getPosition().x < scorpion->getLocation().x){
            scorpion->setDirection(0);
            scorpion->setFlipX(false);
        }

    }
    //--------------------contact with coins------------------------------------
    CCObject *i3 ;
    CCARRAY_FOREACH(_arrayCoin, i3) {
        Coin *coin = (Coin*)i3;
        float kc1 = _player->getImage()->getContentSize().width/2 + coin->getContentSize().width/2;
        float kc2 = ccpDistance(_player->getPosition(), coin->getPosition());
        if (kc2 < kc1) {
            _arrayRemoveCoin->addObject(coin);
        }
    }
    //--------------------contact with snake------------------------------------
    CCObject *i4 ;
    CCARRAY_FOREACH(_arraySnake, i4) {
        Coin *snake = (Coin*)i4;
        float kc1 = _player->getImage()->getContentSize().width/2 + snake->getContentSize().width/2;
        float kc2 = ccpDistance(_player->getPosition(), snake->getPosition());
        if (kc2 < kc1 && _player->getAttack() == 0 && delayPlayer <= 0) {
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 5));
            _player->setAttack(4);
            _lifes --;
            delayPlayer = 15;
        }
        else if (kc2 < kc1 && _player->getAttack() == 2 && delayPlayer <= 0) {
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 5));
            _player->setAttack(4);
            _lifes --;
            delayPlayer = 15;
        }else if (kc2 <= kc1 && _player->getAttack() == 1) {
            CCSprite *attack = CCSprite::create("attack.png");
            attack->setPosition(snake->getPosition());
            this->addChild(attack, 11);
            CCDelayTime * delay = CCDelayTime::create(0.3f);
            CCHide * hide = CCHide::create();
            CCSequence *sq = CCSequence::create(delay, hide, NULL);
            attack->runAction(sq);
            snake->stopAllActions();
            _arrayRemoveSnake->addObject(snake);
            _player->setAttack(4);
            _player->actionAttack();
//            this->addGroupBlood(snake->getPosition(), 10);
        }else if (kc2 <= kc1 && _player->getAttack() == 3) {
            CCSprite *attack = CCSprite::create("attack.png");
            CCSprite *attack2 = CCSprite::create("attack.png");
            CCRotateTo *rotate = CCRotateTo::create(0, 90);
            attack->setPosition(snake->getPosition());
            attack2->setPosition(snake->getPosition());
            this->addChild(attack, 11);
            this->addChild(attack2, 11);
            CCDelayTime * delay = CCDelayTime::create(0.3f);
            CCHide * hide = CCHide::create();
            CCSequence *sq = CCSequence::create(delay, hide, NULL);
            CCSequence *sq2 = CCSequence::create(rotate, delay, hide, NULL);
            attack->runAction(sq);
            attack2->runAction(sq2);
            snake->stopAllActions();
            _arrayRemoveSnake->addObject(snake);
            if (isTouchTop) {
                _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 10));
            }
            _player->actionAttack2();
            _player->setAttack(4);
//            this->addGroupBlood(snake->getPosition(), 10);
        }
    }
    //--------------------contact with Scorpion---------------------------------
    CCObject *i5 ;
    CCARRAY_FOREACH(_arrayScorpion, i5) {
        Scorpion *scorpion = (Scorpion*)i5;
        float kc1 = _player->getImage()->getContentSize().width/2 + scorpion->getContentSize().width/2;
        float kc2 = ccpDistance(_player->getPosition(), scorpion->getPosition());
        if (kc2 < kc1 && _player->getAttack() == 0 && delayPlayer <= 0) {
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 5));
            _player->setAttack(4);
            delayPlayer = 15;
            _lifes --;
        }
        else if (kc2 < kc1 && _player->getAttack() == 2 && delayPlayer <= 0) {
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 5));
            _player->setAttack(4);
            _lifes --;
            delayPlayer = 15;
        }else if (kc2 <= kc1 && _player->getAttack() == 1) {
            CCSprite *attack = CCSprite::create("attack.png");
            attack->setPosition(scorpion->getPosition());
            this->addChild(attack, 11);
            CCDelayTime * delay = CCDelayTime::create(0.3f);
            CCHide * hide = CCHide::create();
            CCSequence *sq = CCSequence::create(delay, hide, NULL);
            attack->runAction(sq);
            scorpion->stopAllActions();
            _arrayRemoveScorpion->addObject(scorpion);
            _player->setAttack(4);
            _player->actionAttack();
//            this->addGroupBlood(scorpion->getPosition(), 10);
        }else if (kc2 <= kc1 && _player->getAttack() == 3) {
            CCSprite *attack = CCSprite::create("attack.png");
            CCSprite *attack2 = CCSprite::create("attack.png");
            CCRotateTo *rotate = CCRotateTo::create(0, 90);
            attack->setPosition(scorpion->getPosition());
            attack2->setPosition(scorpion->getPosition());
            this->addChild(attack, 11);
            this->addChild(attack2, 11);
            CCDelayTime * delay = CCDelayTime::create(0.3f);
            CCHide * hide = CCHide::create();
            CCSequence *sq = CCSequence::create(delay, hide, NULL);
            CCSequence *sq2 = CCSequence::create(rotate, delay, hide, NULL);
            attack->runAction(sq);
            attack2->runAction(sq2);

            scorpion->stopAllActions();
            _arrayRemoveScorpion->addObject(scorpion);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 10));
            _player->setAttack(4);
            _player->actionAttack2();
//            this->addGroupBlood(scorpion->getPosition(), 10);
        }
    }
    //--------------------contact with arrow -----------------------------------
    CCObject *i8;
    CCARRAY_FOREACH(_arrayArrow, i8) {
        Arrow *arrow = (Arrow*)i8;
        float kc1 = _player->getImage()->getContentSize().width/2 + arrow->getContentSize().width/2;
        float kc2 = ccpDistance(arrow->getPosition(), _player->getPosition());
        if (kc2 <= kc1) {
            GameManager::sharedGameManager()->setNumberActionPlayer(1);
            this->arrowAttack(arrow->getDirection());
        }
    }        
    //---------------remove object when ninja attack ---------------------------
    CCObject * i6;
    CCARRAY_FOREACH(_arrayRemoveSnake, i6) {
        Snake * sp = (Snake*)i6;
        sp->resumeSchedulerAndActions();
        _scores = _scores + sp->getPoint();
        CCDelayTime * delay = CCDelayTime::create(0.5f);
        CCFadeIn *fo = CCFadeIn::create(0.5f);
        CCCallFuncN *remove = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::removeSnake));
        CCSequence * sq = CCSequence::create(delay, fo, remove, NULL);
        sp->runAction(sq);
        _player->effectsAddPoint(sp->getPoint());
//        _player->effectsAddPractics(sp->getLocation());
        this->addGroupBlood(sp->getPosition(), 1);
    }
    
    CCObject * i7;
    CCARRAY_FOREACH(_arrayRemoveScorpion, i7) {
        Scorpion * sp = (Scorpion*)i7;
        sp->resumeSchedulerAndActions();
        _scores = _scores + sp->getPoint();
        CCDelayTime * delay = CCDelayTime::create(0.5f);
        CCFadeIn *fo = CCFadeIn::create(0.5f);
        CCCallFuncN *remove = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::removeScorpion));
        CCSequence * sq = CCSequence::create(delay, fo, remove, NULL);
        sp->runAction(sq);
        _player->effectsAddPoint(sp->getPoint());
//        _player->effectsAddPractics(sp->getLocation());
        this->addGroupBlood(sp->getPosition(), 1);
    }
    
    CCObject * i9;
    CCARRAY_FOREACH(_arrayRemoveCoin, i9) {
        Coin * sp = (Coin*)i9;
        _scores = _scores + sp->getPoint();
        CCMoveTo * moveTop = CCMoveTo::create(0.3f, CCPoint(sp->getPosition().x ,
                                                    sp->getPosition().y + 10));
        CCCallFuncN *remove = CCCallFuncN::create(this,callfuncN_selector(HelloWorld::removeCoin));
        CCSequence * sq = CCSequence::create(moveTop, remove, NULL);
        sp->runAction(sq);
        _player->effectsAddPoint(sp->getPoint());
    }
    _arrayRemoveCoin->removeAllObjects();
    //----------------------End game ------------Lost-------------
    if ( _player->getPosition().y < - 5000) {
        this->unscheduleAllSelectors();
        this->unscheduleUpdate();
        _lbLost->setVisible(true);
        btquit->setVisible(true);
        btreset->setVisible(true);
        btpause->setVisible(false);
    }
    //--------------------- Win ---------------------------------
    if (_arraySnake->count() <= 0 && _arrayScorpion->count() <= 0) {
        this->unscheduleAllSelectors();
        this->unscheduleUpdate();
        _lbWin->setVisible(true);
        btquit->setVisible(true);
        btreset->setVisible(true);
        btpause->setVisible(false);
    }
}
void HelloWorld::updatePhantom(float dt) {
    //-----------------phantom--------------------------------------------------
    if (_player->getStop() == false) {
        if (_player->getAttack() == 3 ) {
            CCSprite * sprite = CCSprite::create("ninja.png");
            sprite->setPosition(_player->getPosition());
            CCFadeOut *fo = CCFadeOut::create(0.4f);
            CCCallFuncN *remove = CCCallFuncN::create(sprite,
                                callfuncN_selector(HelloWorld::removeSprite));
            CCSequence * sq = CCSequence::create(fo, remove, NULL);
            sprite->runAction(sq);
            this->addChild(sprite, 1000);
        }else if (_player->getAttack() == 1 ) {
            CCSprite * sprite = CCSprite::create("ninja_attack.png");
            sprite->setPosition(_player->getPosition());
            int i = rand() % 4 + 1;
            CCRotateTo *rotate = CCRotateTo::create(0, 360 / i);
            CCFadeOut *fo = CCFadeOut::create(0.4f);
            CCCallFuncN *remove = CCCallFuncN::create(sprite,
                                callfuncN_selector(HelloWorld::removeSprite));
            CCSequence * sq = CCSequence::create(fo, remove, NULL);
            sprite->runAction(sq);
            sprite->runAction(rotate);
            this->addChild(sprite, 1000);
        }
    }else {
        _player->getMpBody()->SetAngularVelocity(0);
        _player->getMpBody()->SetTransform(_player->getMpBody()->GetPosition(), 0);
    }    
}
void HelloWorld::updateCheckStop(float dt) {
    CCPoint p = _player->getPosition();
    CCPoint p2 = _player->getPoint();
    if (p.x == p2.x && p.y == p2.y) {
        _player->setStop(true);
    }else _player->setStop(false);    
}
void HelloWorld::updateLocation_Direction(float dt) {
    if (delayPlayer > 0) {
        delayPlayer --;
    }
    CCObject * i1;
    CCARRAY_FOREACH(_arraySnake, i1) {
        Snake *snake = (Snake*)i1;
        snake->setLocation(snake->getPosition());
    }
    CCObject * i2;
    CCARRAY_FOREACH(_arrayScorpion, i2) {
        Scorpion *scorpion = (Scorpion*)i2;
        scorpion->setLocation(scorpion->getPosition());
    }
    CCPoint pPlay = convertMetterToPixel(CCPoint(_player->getMpBody()->GetPosition().x,
                                                _player->getMpBody()->GetPosition().y));
    if (pPlay.x < _player->getPoint().x) {
        _player->getImage()->setFlipX(true);
        _player->setFlipX(true);
    }else if (pPlay.x > _player->getPoint().x){
        _player->getImage()->setFlipX(false);
        _player->setFlipX(false);
    }
    _player->setPoint(pPlay);
}
void HelloWorld::updateScoreLife(float dt) {
    char scoreStr[20] = {0};
    sprintf(scoreStr, "Score: %i", _scores);
    _lbScores->setString(scoreStr);    
    char strlife[20] = {0};
    sprintf(strlife, "Lifes: %i-X", _lifes);
    _lbLifes->setString(strlife);
}
void HelloWorld::updateEtfect(float dt) {
    CCObject *i;
    CCARRAY_FOREACH(_arrayBlood, i) {
        Blood * bl = (Blood*)i;
        CCPoint p1 = convertMetterToPixel(ccp(_player->getMpBody()->GetPosition().x,
                                              _player->getMpBody()->GetPosition().y));
        CCPoint p2 = convertMetterToPixel(ccp(bl->getMpBody()->GetPosition().x,
                                              bl->getMpBody()->GetPosition().y));
        float kc1 = _player->getImage()->getContentSize().width/2;
        float kc2 = ccpDistance(p1, p2);
        if (kc2 <= kc1) {
            _arrayBloodRemove->addObject(bl);
        }
        bl->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
        b2Vec2 imple = b2Vec2((_player->getMpBody()->GetPosition().x - bl->getMpBody()->GetPosition().x) *5,
                              (_player->getMpBody()->GetPosition().y - bl->getMpBody()->GetPosition().y) *5);
        bl->getMpBody()->SetLinearVelocity(imple);
    }
    CCObject *j;
    CCARRAY_FOREACH(_arrayBloodRemove, j) {
        Blood * bl = (Blood*)j;
        world->DestroyBody(bl->getMpBody());
        _arrayBlood->removeObject(bl);
        this->removeChild(bl);
    }
    _arrayBloodRemove->removeAllObjects();
}

bool HelloWorld::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    if (GameManager::sharedGameManager()->getNumberActionPlayer() > 0)
    {
        touchBack = true;
        _contactting = false;
        giamVanToc = true;
        touchLocation = touch->getLocationInView();
        touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
        touchLocation = this->convertToNodeSpace(touchLocation);
        if (_player->getMpBody()->GetPosition().y * PTM_RATIO < touchLocation.y )
            isTouchTop = true;
        else {
            isTouchTop = false;
            _player->getMpBody()->SetGravityScale(1);
        }
        if (_player->getMpBody()->GetPosition().x * PTM_RATIO < touchLocation.x ) {
            isTouchRight = true;
        }else isTouchRight = false;
        this->checkTouchPoint(touchLocation);
    }
    return true;
}
void HelloWorld::ccTouchEnded(CCSet* touches, CCEvent* event)
{
    //Add a new body/atlas sprite at the touched location
    CCSetIterator it;
    CCTouch* touch;    
    for( it = touches->begin(); it != touches->end(); it++) 
    {
        touch = (CCTouch*)(*it);
        if(!touch)
            break;
        CCPoint location = touch->getLocationInView();
        location = CCDirector::sharedDirector()->convertToGL(location);
        this->touch(location);
    }
}
void HelloWorld::checkTouchPoint(cocos2d::CCPoint p) {
    CCObject *i1;
    CCARRAY_FOREACH(_arraySnake, i1) {
        Snake * snake = (Snake*)i1;
        CCRect rect = snake->boundingBox();
        if (rect.containsPoint(p)) {
            spriteTaget = (CCSprite*)snake;
            CCSprite * taget = CCSprite::create("muctieu.png");
            taget->setPosition(CCPoint(snake->getContentSize().width/2, snake->getContentSize().height/2));
            snake->addChild(taget);
            CCFadeOut * fo = CCFadeOut::create(1.5f);
            CCHide * hide = CCHide::create();
            CCSequence * sq = CCSequence::create(fo, hide, NULL);
            taget->runAction(sq);
            _player->setAttack(3);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
            this->touch1(p);
            GameManager::sharedGameManager()->setNumberActionPlayer(
                    GameManager::sharedGameManager()->getNumberActionPlayer() - 1);
            return;
        }
    }   
    CCObject *i2;
    CCARRAY_FOREACH(_arrayScorpion, i2) {
        Scorpion * scorpion = (Scorpion*)i2;
        CCRect rect = scorpion->boundingBox();
        if (rect.containsPoint(p)) {
            spriteTaget = (CCSprite*)scorpion;
            CCSprite * taget = CCSprite::create("muctieu.png");
            taget->setPosition(CCPoint(scorpion->getContentSize().width/2, scorpion->getContentSize().height/2));
            scorpion->addChild(taget);
            CCFadeOut * fo = CCFadeOut::create(1.5f);
            CCHide * hide = CCHide::create();
            CCSequence * sq = CCSequence::create(fo, hide, NULL);
            taget->runAction(sq);
            _player->setAttack(3);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
            this->touch1(p);
            GameManager::sharedGameManager()->setNumberActionPlayer(
                    GameManager::sharedGameManager()->getNumberActionPlayer() - 1);
            return;
        }
    }
    CCSprite * t = CCSprite::create("Icon-72.png");
    CCScaleBy *scale = CCScaleBy::create(2, 0);
    CCHide *hide = CCHide::create();
    CCSequence * sq = CCSequence::create(scale, hide, NULL);
    t->runAction(sq);
    t->setPosition(p);
    this->addChild(t, 10);
    _player->setAttack(1);
    _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
    this->touch(p);    
    GameManager::sharedGameManager()->setNumberActionPlayer(
                GameManager::sharedGameManager()->getNumberActionPlayer() - 1);
}
void HelloWorld::touch( CCPoint location)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    b2Vec2 currentVelocity = _player->getMpBody()->GetLinearVelocity();
    b2Vec2 impulse(0.0f,0.0f);
    
    {
        impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) * 1.5f + 10;
        impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) * 1.5f;
        if (abs(impulse.y / impulse.x) < 0.9f) {
            if (impulse.LengthSquared() > 100000 && impulse.LengthSquared() < 350000) {
                impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) * 1.8f + 150 * (1-abs(impulse.y / impulse.x));
                impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) * 1.5f;
            }else if (impulse.LengthSquared() < 100000) {
                impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) * 2.8f + 150 * (1-abs(impulse.y / impulse.x));
                impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) * 2.0f;
            }else if (impulse.LengthSquared() > 350000) {
                impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) * 1.2f + 150 * (1-abs(impulse.y / impulse.x));
                impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) * 1.2f;
            }
        }else {
            if (impulse.LengthSquared() > 100000 && impulse.LengthSquared() < 350000) {
                impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) * 1.5f + 30;
                impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) * 1.5f;
            }else if (impulse.LengthSquared() < 100000) {
                impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) * 2.8f + 30;
                impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) * 2.0f;
            }else if (impulse.LengthSquared() > 350000) {
                impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) * 1.2f + 10;
                impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) * 1.2f;
            }
        }
        if( location.x < (winSize.width * 0.5f) )
        b2Vec2 point((location.x - _player->getPositionX())/10, (location.y - _player->getPositionY())/10);
        _player->getMpBody()->ApplyLinearImpulse(impulse, _player->getMpBody()->GetWorldCenter());
        if (isTouchRight == true) {
            _player->getMpBody()->SetAngularVelocity(-10000);
        }else if ( isTouchRight == false) {
            _player->getMpBody()->SetAngularVelocity(10000);        }
    }
}
void HelloWorld::touch1(cocos2d::CCPoint location) {
    b2Vec2 impulse(0.0f,0.0f);
    impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) / 7 ;
    impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) /7 ;
    _player->getMpBody()->SetLinearVelocity(impulse);
    if (isTouchRight == true) {
        _player->getMpBody()->SetAngularVelocity(-100);
    }else if ( isTouchRight == false) {
        _player->getMpBody()->SetAngularVelocity(100);
    }
}
void HelloWorld::touch2(cocos2d::CCPoint location) {
    b2Vec2 impulse(0.0f,0.0f);
    impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO) / 18 ;
    impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO) / 18 ;
    if (touchBack == true)
    {
        _player->getMpBody()->SetLinearVelocity(impulse);
        if (isTouchRight == true) {
            _player->getMpBody()->SetAngularVelocity(-100);
        }else if ( isTouchRight == false) {
            _player->getMpBody()->SetAngularVelocity(100);
        }
        if (GameManager::sharedGameManager()->getDirectionContact() == 3) {
            touchBack = false;
        }      
    }
}

void HelloWorld::attackTaget(cocos2d::CCPoint p) {
    CCPoint point = convertPixelToMetter(p);
    b2Vec2  p1 = b2Vec2(point.x, point.y);
    b2Vec2  p2 = _player->getMpBody()->GetPosition();
    b2Vec2 impulse(0.0f, 0.0f) ;
    impulse.y = (- p2.y + p1.y) * 20 ;
    impulse.x = (- p2.x + p1.x) * 20  ;
    _player->getMpBody()->SetLinearVelocity(impulse);
}
#pragma mark - create map
void HelloWorld::prepareLayers()
{
    this->createFixtures(_meta);
}
void HelloWorld::createFixtures(CCTMXLayer* layer)
{
    // create all the rectangular fixtures for each tile in the level
    CCSize layerSize = layer->getLayerSize();
    for( int y=0; y < layerSize.height; y++ )
    {
        for( int x=0; x < layerSize.width; x++ )
        {
            // create a fixture if this tile has a sprite
            CCSprite* tileSprite = layer->tileAt(ccp(x, y));
            if( tileSprite )
                this->createRectangularFixture(layer, x, y, 1.0f, 1.0f);
        }
    }
}
void HelloWorld::createRectangularFixture(CCTMXLayer* layer, int x, int y,
                                     float width, float height) {
    // get position & size
    CCPoint p = layer->positionAt(ccp(x,y));
    CCSize tileSize = this->_tileMap->getTileSize();
    const float pixelsPerMeter = 32.0f;    
    // create the body
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set((p.x + (tileSize.width / 2.0f)) / pixelsPerMeter,
                         (p.y + (tileSize.height / 2.0f)) / pixelsPerMeter);
    b2Body* body = world->CreateBody(&bodyDef);
    // define the shape
    b2PolygonShape shape;
    shape.SetAsBox((tileSize.width / pixelsPerMeter) * 0.5f * width,
                   (tileSize.width / pixelsPerMeter) * 0.5f * height);    
    // create the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 0.0f;
    fixtureDef.filter.maskBits = 0xffff;
    body->CreateFixture(&fixtureDef);
}
void HelloWorld::createRectangularFixtureWithPoint(cocos2d::CCPoint p1, cocos2d::CCPoint p2) {
    const float pixelsPerMeter = 32.0f;
    float with = 0;
    float height = 0;
    float sizeTileWith = _tileMap->getTileSize().width;
    float sizeTileHeight = _tileMap->getTileSize().height;
    CCPoint pointWord1 = convertPoitMapToPixelReverseY(p1);
    CCPoint pointWord2 = convertPoitMapToPixelReverseY(p2);
    CCLog("pointWord 1 : %f   -  %f", pointWord1.x, pointWord1.y);
    CCLog("pointWord 2 : %f   -  %f", pointWord2.x, pointWord2.y);
    CCPoint pw1 = CCPoint(pointWord1.x - sizeTileWith/2, pointWord1.y + sizeTileHeight/2);
    CCPoint pw2 = CCPoint(pointWord2.x + sizeTileWith/2, pointWord2.y - sizeTileHeight/2);
    CCPoint pCenter = CCPoint((pw2.x - pw1.x)/2 + pw1.x, abs(pw1.y - pw2.y)/2 + pw2.y );
    CCLog("pw1 1 : %f   -  %f", pw1.x, pw1.y);
    CCLog("pw1 2 : %f   -  %f", pw2.x, pw2.y);
    CCLog("pCenter 2 : %f   -  %f", pCenter.x, pCenter.y);
    with = pw2.x - pw1.x;
    height = abs(pw1.y - pw2.y);
    // create the body
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(pCenter.x / pixelsPerMeter, pCenter.y / pixelsPerMeter);
    b2Body* body = world->CreateBody(&bodyDef);
    // define the shape
    b2PolygonShape shape;
    CCLog("with : %f, height : %f", with, height);
    shape.SetAsBox((with / 2 /pixelsPerMeter), (height / 2/ pixelsPerMeter));
    // create the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1000.0f;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 0.0f;
    fixtureDef.filter.maskBits = 0xffff;
    body->CreateFixture(&fixtureDef);
}
void HelloWorld::addWalls() {
    this->createRectangularFixtureWithPoint(CCPoint(0, 21), CCPoint(16, 23));
    this->createRectangularFixtureWithPoint(CCPoint(21, 14), CCPoint(24, 23));
    this->createRectangularFixtureWithPoint(CCPoint(25, 18), CCPoint(38, 23));
    this->createRectangularFixtureWithPoint(CCPoint(39, 21), CCPoint(57, 23));
    this->createRectangularFixtureWithPoint(CCPoint(44, 18), CCPoint(52, 20));
    this->createRectangularFixtureWithPoint(CCPoint(47, 16), CCPoint(49, 17));
    this->createRectangularFixtureWithPoint(CCPoint(64, 20), CCPoint(75, 23));
    this->createRectangularFixtureWithPoint(CCPoint(85, 20), CCPoint(107, 23));
    
    this->createRectangularFixtureWithPoint(CCPoint(70, 12), CCPoint(79, 13));
    this->createRectangularFixtureWithPoint(CCPoint(49, 9), CCPoint(58, 10));
    
    this->createRectangularFixtureWithPoint(CCPoint(108, 15), CCPoint(127, 23));
    this->createRectangularFixtureWithPoint(CCPoint(128, 20), CCPoint(199, 23));
    this->createRectangularFixtureWithPoint(CCPoint(199, 0), CCPoint(199, 19));
}
#pragma mark - handle touches
void HelloWorld::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}
void HelloWorld::setViewPointCenter(CCPoint position) {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();    
    int x = MAX(position.x, winSize.width/2);
    int y = MAX(position.y, winSize.height/2);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    CCPoint actualPosition = ccp(x, y);
    CCPoint centerOfView = ccp(winSize.width/2, winSize.height/2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    this->setPosition(viewPoint);
    if (position.y < winSize.height / 2) {
        _layerBg->setPosition(CCPoint(s.width/2 - viewPoint.x, s.height/2 + viewPoint.y));
    }else _layerBg->setPosition(CCPoint(s.width/2 - viewPoint.x, s.height/2 - viewPoint.y));
    
}
CCPoint HelloWorld::tileCoordForPosition(CCPoint position)
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return ccp(x, y);
}
CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    // add layer as a child to scene
    CCLayer* layer = new HelloWorld();
    scene->addChild(layer);
    layer->release();
    return scene;
}
#pragma mark - add object 
void HelloWorld::addCoins() {
    CCSize layerSize = _coin->getLayerSize();
    for( int y=0; y < layerSize.height; y++ )
    {
        for( int x=0; x < layerSize.width; x++ )
        {
            // create a fixture if this tile has a sprite
            CCSprite* tileSprite = _coin->tileAt(ccp(x, y));
            if( tileSprite ) {
                CCPoint p = convertPoitMapToPixel(ccp(tileSprite->getPosition().x / PTM_RATIO,
                                                      tileSprite->getPosition().y / PTM_RATIO));
                Coin * coin =  new Coin();
                coin->setPoint(10);
                coin->initWithFile("coin.png");
                coin->setPosition(p);
                coin->action();
                _arrayCoin->addObject(coin);
                this->addChild(coin, 10000);
            }                
        }
    }
}
void HelloWorld::addSnakes() {
    CCSize layerSize = _snake->getLayerSize();
    for( int y=0; y < layerSize.height; y++ )
    {
        for( int x=0; x < layerSize.width; x++ )
        {
            // create a fixture if this tile has a sprite
            CCSprite* tileSprite = _snake->tileAt(ccp(x, y));
            if( tileSprite ) {
                CCPoint p = convertPoitMapToPixel(ccp(tileSprite->getPosition().x / PTM_RATIO,
                                                      tileSprite->getPosition().y / PTM_RATIO));
                Snake * snake =  new Snake();
                snake->setPoint(20);
                snake->initWithFile("monter01.png");
                snake->setPosition(p);
                snake->actionMoveToPoint(CCPoint(p.x + 200, p.y));
                _arraySnake->addObject(snake);
                this->addChild(snake, 10000);
            }    
        }
    }
}
void HelloWorld::addScorpions() {
    CCSize layerSize = _scorpion->getLayerSize();
    for( int y=0; y < layerSize.height; y++ )
    {
        for( int x=0; x < layerSize.width; x++ )
        {
            // create a fixture if this tile has a sprite
            CCSprite* tileSprite = _scorpion->tileAt(ccp(x, y));
            if( tileSprite ) {
                CCPoint p = convertPoitMapToPixel(ccp(tileSprite->getPosition().x / PTM_RATIO,
                                                      tileSprite->getPosition().y / PTM_RATIO));
                Scorpion * scorpion =  new Scorpion();
                scorpion->setPoint(40);
                scorpion->initWithFile("monter02.png");
                scorpion->setPosition(p);
                int i = rand() % 2;
                if (i == 0) {
                    scorpion->fluctuating(30);
                }
                if(i == 1)
                    scorpion->fluctuatingAndMove(30, 200);
                
                _arrayScorpion->addObject(scorpion);
                this->addChild(scorpion, 10000);
            }   
        }
    }
}
void HelloWorld::addArrows() {
    CCSize layerSize = _arrow->getLayerSize();
    for( int y=0; y < layerSize.height; y++ )
    {
        for( int x=0; x < layerSize.width; x++ )
        {
            // create a fixture if this tile has a sprite
            CCSprite* tileSprite = _arrow->tileAt(ccp(x, y));
            CCPoint tileCoord = this->tileCoordForPosition(convertPoitMapToPixel(ccp(x, y)));
            int tileGid = _arrow->tileGIDAt(ccp(x, y));
            if (tileGid) {
                CCDictionary *properties = _tileMap->propertiesForGID(tileGid);
                if (properties) {
                    CCPoint p = convertPoitMapToPixel(ccp(tileSprite->getPosition().x / PTM_RATIO,
                                                          tileSprite->getPosition().y / PTM_RATIO));
                    Arrow * arrow =  new Arrow();
                    arrow->initWithFile("arrow.png");
                    arrow->setPosition(p);
                    _arrayArrow->addObject(arrow);
                    this->addChild(arrow, 100);
                    //right
                    CCString *collisionRight = new CCString();
                    *collisionRight = *properties->valueForKey("right");
                    if (collisionRight && (collisionRight->compare("true") == 0)) {
                        int i =  1;
                        arrow->setDirection(i);
                        CCRotateTo *rotate = CCRotateTo::create(0, (i-1) * 45);                            
                        arrow->runAction(rotate);
                    }
                    //right down
                    CCString *collisionRightDown = new CCString();
                    *collisionRightDown = *properties->valueForKey("right_down");
                    if (collisionRightDown && (collisionRightDown->compare("true") == 0)) {
                        int i =  2;
                        arrow->setDirection(i);
                        CCRotateTo *rotate = CCRotateTo::create(0, (i-1) * 45);
                        arrow->runAction(rotate);
                    }  
                    // down
                    CCString *collisionDown = new CCString();
                    *collisionDown = *properties->valueForKey("down");
                    if (collisionDown && (collisionDown->compare("true") == 0)) {
                        int i =  3;
                        arrow->setDirection(i);
                        CCRotateTo *rotate = CCRotateTo::create(0, (i-1) * 45);
                        arrow->runAction(rotate);
                    }
                    
                    //left down
                    CCString *collisionLeftDown = new CCString();
                    *collisionLeftDown = *properties->valueForKey("left_down");
                    if (collisionLeftDown && (collisionLeftDown->compare("true") == 0)) {
                        int i =  4;
                        arrow->setDirection(i);
                        CCRotateTo *rotate = CCRotateTo::create(0, (i-1) * 45);
                        arrow->runAction(rotate);
                    }

                    //left
                    CCString *collisionLeft = new CCString();
                    *collisionLeft = *properties->valueForKey("left");
                    if (collisionLeft && (collisionLeft->compare("true") == 0)) {
                        int i = 5;
                        arrow->setDirection(i);
                        CCRotateTo *rotate = CCRotateTo::create(0, (i-1) * 45);
                        arrow->runAction(rotate);
                    }
                    
                    //left up
                    CCString *collisionLeftUp = new CCString();
                    *collisionLeftUp = *properties->valueForKey("left_up");
                    if (collisionLeftUp && (collisionLeftUp->compare("true") == 0)) {
                        int i = 6;
                        arrow->setDirection(i);
                        CCRotateTo *rotate = CCRotateTo::create(0, (i-1) * 45);
                        arrow->runAction(rotate);
                    }

                    // up
                    CCString *collisionUp = new CCString();
                    *collisionUp = *properties->valueForKey("up");
                    if (collisionUp && (collisionUp->compare("true") == 0)) {
                        int i = 7;
                        arrow->setDirection(i);
                        CCRotateTo *rotate = CCRotateTo::create(0, (i-1) * 45);
                        arrow->runAction(rotate);
                    }
                    
                    //right up
                    CCString *collisionRightUp = new CCString();
                    *collisionRightUp = *properties->valueForKey("right_up");
                    if (collisionRightUp && (collisionRightUp->compare("true") == 0)) {
                        int i = 8;
                        arrow->setDirection(i);
                        CCRotateTo *rotate = CCRotateTo::create(0, (i-1) * 45);
                        arrow->runAction(rotate);
                    }
                }
            }
        }
    }
}
#pragma mark - convert point
CCPoint HelloWorld::convertMetterToPixel(CCPoint p) {
    return CCPoint(p.x * PTM_RATIO, p.y * PTM_RATIO);
}
CCPoint HelloWorld::convertPixelToMetter(cocos2d::CCPoint p) {
    return CCPoint(p.x / PTM_RATIO, p.y / PTM_RATIO);
}
CCPoint HelloWorld::convertPoitMapToPixel(cocos2d::CCPoint pointMap) {
    float H = CCDirector::sharedDirector()->getWinSize().height;
    float Y = _tileMap->getMapSize().height;
    float x = _tileMap->getTileSize().width;
    float y = _tileMap->getTileSize().height;
    return CCPoint(x * 0.5f + x * pointMap.x,H - ((Y - pointMap.y) * y - y * 0.5f));
}
CCPoint HelloWorld::convertPoitMapToPixelReverseY(cocos2d::CCPoint pointMap) {
    float Y = _tileMap->getMapSize().height;
    float x = _tileMap->getTileSize().width;
    float y = _tileMap->getTileSize().height;
    return CCPoint(x * 0.5f + x * pointMap.x,((Y - pointMap.y) * y - y * 0.5f));
}
void HelloWorld::removeSprite(cocos2d::CCNode *node) {
    CCSprite * sp = (CCSprite*)node;
    sp->removeFromParentAndCleanup(true);
}
void HelloWorld::removeCoin(cocos2d::CCNode *node) {
    Coin * coin = (Coin*)node;
    _arrayCoin->removeObject(coin);
    _arrayRemoveCoin->removeObject(coin);
    this->removeChild(coin);
}
void HelloWorld::removeSnake(cocos2d::CCNode *node) {
    Snake * snake = (Snake*)node;
    snake->removeAllChildren();
    _arraySnake->removeObject(snake);
    _arrayRemoveSnake->removeObject(snake);
    this->removeChild(snake);
}
void HelloWorld::removeScorpion(cocos2d::CCNode *node) {
    Scorpion * scorpion = (Scorpion*)node;
    scorpion->removeAllChildren();
    _arrayScorpion->removeObject(scorpion);
    _arrayRemoveScorpion->removeObject(scorpion);
    this->removeChild(scorpion);
}

void HelloWorld::arrowAttack(int direction) {
    _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
    _player->setAttack(1);
    int x = 50;
    int y = 30;
    switch (direction) {
        case 1:
            _player->getMpBody()->SetGravityScale(1);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(x + 50, 0));
            break;
        case 2:
            _player->getMpBody()->SetGravityScale(1);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(x/2, -y/2));
            break;
        case 3:
            _player->getMpBody()->SetGravityScale(1);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, -y));
            break;
        case 4:
            _player->getMpBody()->SetGravityScale(1);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(-x/2, -y/2));
            break;
        case 5:
            _player->getMpBody()->SetGravityScale(1);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(-x - 50, 0));
            break;
        case 6:
            _player->getMpBody()->SetGravityScale(1);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(-x/2, y/2));
            break;
        case 7:
            _player->getMpBody()->SetGravityScale(1);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, y));
            break;
        case 8:
            _player->getMpBody()->SetGravityScale(1);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(x/2, y/2));
            break;
        default:
            break;
    }
}
#pragma mark - click menu item
void HelloWorld::click_continue(cocos2d::CCObject *pSender) {
    btquit->setVisible(false);
    btcontinue->setVisible(false);
    btpause->setVisible(true);
    this->resumeSchedulerAndActions();
}
void HelloWorld::click_pause(cocos2d::CCObject *pSender) {
    btquit->setVisible(true);
    btcontinue->setVisible(true);
    btpause->setVisible(false);
    this->pauseSchedulerAndActions();
}
void HelloWorld::click_quit(cocos2d::CCObject *pSender) {
    this->unscheduleUpdate();
    this->unscheduleAllSelectors();
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.7f, GameMenu::scene()));
}
void HelloWorld::click_reset(cocos2d::CCObject *pSender) {
    this->unscheduleUpdate();
    this->unscheduleAllSelectors();
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.7f, HelloWorld::scene()));
}