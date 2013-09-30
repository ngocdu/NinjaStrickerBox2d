//
//  GamePlayWithBoss.cpp
//  NinjaStrickerBox2d
//
//  Created by MinhNT on 13/09/23.
//
//
#include "SimpleAudioEngine.h"
#include "GB2ShapeCache-x.h"
#include <math.h>
using namespace cocos2d;
using namespace CocosDenshion;

#define PTM_RATIO 32
#include "GamePlayWithBoss.h"
#include "GameManager.h"
GamePlayWithBoss::~GamePlayWithBoss() {
    delete world;
    world = NULL;
    
    delete m_debugDraw;
}
GamePlayWithBoss::GamePlayWithBoss() {
    size = CCDirector::sharedDirector()->getWinSize();
    array_taget = new CCArray();
    // init physics
    this->initPhysics();
    GameManager::sharedGameManager()->setNumberActionPlayer(2);
    setTouchEnabled( true );
    setAccelerometerEnabled( true );
    //---------------------------------
    _tileMap = new CCTMXTiledMap();
    _tileMap->initWithTMXFile("mapBoss.tmx");
//    _tileMap->setScaleX(GameManager::sharedGameManager()->getSCALE_N_X());
//    _tileMap->setScaleY(GameManager::sharedGameManager()->getSCALE_N_Y());
    this->addChild(_tileMap);
    withTileMap = _tileMap->getMapSize().width * _tileMap->getTileSize().width;
    heightTileMap = _tileMap->getMapSize().height * _tileMap->getTileSize().height;
    
    
    addNewSpriteAtPosition(convertPoitMapToPixel(CCPoint(10, 17)));
    addNewBossAtPosition(convertPoitMapToPixel(CCPoint(28, 17)));
    this->addWalls();
    timeDelayAttackBoss = 2;
    timeDelayContactBoss = 2;
    ninjaAttack = false;
    
    _lifes = 3;
    _bloodBoss = 100;
    
    _lbLifes = CCLabelTTF::create("X  X  X", "", 40);
    _lbLifes->setPosition(CCPoint(size.width - 150, size.height - 50));
    _lbLifes->setColor(ccc3(150, 150, 255));
    this->addChild(_lbLifes, 100);
    
    _lbWin = CCLabelTTF::create("YOU WIN", "", 40);
    _lbWin->setPosition(CCPoint(size.width/2 , size.height/2));
    _lbWin->setVisible(false);
    _lbWin->setColor(ccc3(0, 0, 255));
    this->addChild(_lbWin, 100);
    
    _lbLost = CCLabelTTF::create("YOU LOST", "", 40);
    _lbLost->setPosition(CCPoint(size.width/2 , size.height/2));
    _lbLost->setVisible(false);
    _lbLost->setColor(ccc3(0, 0, 255));
    this->addChild(_lbLost, 100);

    
    //---------menu-------
    btpause = CCMenuItemFont::create("PAUSE", this, menu_selector(GamePlayWithBoss::click_pause));
    btpause->setPosition(CCPoint(  size.width/2, size.height - 50));
    
    btquit = CCMenuItemFont::create("QUIT", this, menu_selector(GamePlayWithBoss::click_quit));
    btquit->setPosition(CCPoint( size.width/2, size.height - size.height * 0.2f));
    btquit->setVisible(false);
    btcontinue = CCMenuItemFont::create("CONTINUE", this, menu_selector(GamePlayWithBoss::click_continue));
    btcontinue->setPosition(CCPoint( size.width/2, size.height - size.height/3));
    btcontinue->setVisible(false);
    
    btreset = CCMenuItemFont::create("RESET", this, menu_selector(GamePlayWithBoss::click_reset));
    btreset->setPosition(CCPoint(size.width/2, size.height - size.height/3));
    btreset->setVisible(false);
    
    menu = CCMenu::create(btpause, btquit, btcontinue, btreset, NULL);
    menu->setPosition(CCPoint( 0 ,  0 ));
    this->addChild(menu, 10);
    
    CCSprite * spprogre = CCSprite::create("progressBar.png");
    spprogre->setAnchorPoint(ccp(0, 0));
    _progressBloodBoss = CCProgressTimer::create(spprogre);
    _progressBloodBoss->setType(kCCProgressTimerTypeBar);
    _progressBloodBoss->setPercentage(100);
    _progressBloodBoss->setPosition(CCPoint(size.width/2, size.height/6));
//    _progressBloodBoss->setAnchorPoint(ccp(0, 0));
    _progressBloodBoss->setMidpoint(ccp(0,0));
    _progressBloodBoss->setBarChangeRate(ccp(1, 0));
    this->addChild(_progressBloodBoss, 100);
    
    scheduleUpdate();
    this->schedule(schedule_selector(GamePlayWithBoss::updateLocation_Direction), 0.1f);
    this->schedule(schedule_selector(GamePlayWithBoss::updateCheckStop), 0.05f);
    this->schedule(schedule_selector(GamePlayWithBoss::updatePhantom), 0.01f);
    this->schedule(schedule_selector(GamePlayWithBoss::updateTime), 0.4f);
    this->schedule(schedule_selector(GamePlayWithBoss::updateBoss));
}
void GamePlayWithBoss::initPhysics()
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
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
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
    
    groundBox.Set(b2Vec2(0,0), b2Vec2(s.width/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
    
    // top
    groundBox.Set(b2Vec2(0, s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO, s.height/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);
    
    // left
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0 ,0));
    groundBody->CreateFixture(&groundBox,0);
    
    // right
    
    groundBox.Set(b2Vec2(s.width/PTM_RATIO, s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
    
    
    
//    CCSize s = CCDirector::sharedDirector()->getWinSize();
//    
//    b2Vec2 gravity;
//    gravity.Set(0.0f, -10.0f);
//    world = new b2World(gravity);
//    _contactListener = new MyContactListener();
//    _contactListener->setNumberBegin(0);
//    world->SetContactListener(_contactListener);
//    // Do we want to let bodies sleep?
//    world->SetAllowSleeping(true);
//    
//    world->SetContinuousPhysics(true);
//    
//    
//    m_debugDraw = new GLESDebugDraw( PTM_RATIO );
//    world->SetDebugDraw(m_debugDraw);
//    
//    uint32 flags = 0;
//    flags += b2Draw::e_shapeBit;
//    //        flags += b2Draw::e_jointBit;
//    //        flags += b2Draw::e_aabbBit;
//    //        flags += b2Draw::e_pairBit;
//    //        flags += b2Draw::e_centerOfMassBit;
//    m_debugDraw->SetFlags(flags);
//    
//    
//    // Define the ground body.
//    b2BodyDef groundBodyDef;
//    groundBodyDef.position.Set(0, 0); // bottom-left corner
//    
//    // Call the body factory which allocates memory for the ground body
//    // from a pool and creates the ground box shape (also from a pool).
//    // The body is also added to the world.
//    b2Body* groundBody = world->CreateBody(&groundBodyDef);
//    
//    // Define the ground box shape.
//    b2EdgeShape groundBox;
//    
//    // bottom
//    
//    groundBox.Set(b2Vec2(0,0), b2Vec2(withTileMap/PTM_RATIO,0));
//    groundBody->CreateFixture(&groundBox,0);
//    
//    // top
//    groundBox.Set(b2Vec2(0, heightTileMap/PTM_RATIO), b2Vec2(withTileMap/PTM_RATIO, heightTileMap/PTM_RATIO));
//    groundBody->CreateFixture(&groundBox,0);
//    
//    // left
//    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0 ,0));
//    groundBody->CreateFixture(&groundBox,0);
//    
//    // right
//    
//    groundBox.Set(b2Vec2(withTileMap/PTM_RATIO, s.height/PTM_RATIO), b2Vec2(withTileMap/PTM_RATIO,0));
//    groundBody->CreateFixture(&groundBox,0);
}

void GamePlayWithBoss::draw()
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
void GamePlayWithBoss::addNewSpriteAtPosition(CCPoint p)
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
    
    b2CircleShape circle;
    
    float _radius = _player->getImage()->getContentSize().width / 2;
    circle.m_radius = _radius / PTM_RATIO;
    
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    //    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 0.1f; // trong luong
    fixtureDef.friction = 1.0f; //ma sat
    fixtureDef.restitution = 0;
    fixtureDef.filter.groupIndex = -10;
    body->CreateFixture(&fixtureDef);
    
    _player->setMpBody(body);
}
void GamePlayWithBoss::addNewBossAtPosition(CCPoint p)
{
    CCSprite *sp = CCSprite::create("ninja2d.png");
    _boss = new Boss();
    
    _boss->setAttack(4);
    _boss->init();
    this->addChild(_boss, 1000);
    _boss->addChild(sp);
    _boss->setImage(sp);
    
    _boss->setPosition( CCPointMake( p.x, p.y) );
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
    bodyDef.userData = _boss;
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    
    b2CircleShape circle;
    
    float _radius = _boss->getImage()->getContentSize().width / 2;
    circle.m_radius = _radius / PTM_RATIO;
    
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    //    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 14.0f; // trong luong
    fixtureDef.friction = 1.0f; //ma sat
    fixtureDef.restitution = 0.0f;
    fixtureDef.filter.groupIndex = -10;
    body->CreateFixture(&fixtureDef);
    
    _boss->setMpBody(body);
    
}

void GamePlayWithBoss::setViewPointCenter(CCPoint position)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    int x = MAX(position.x, winSize.width/2);
    int y = MAX(position.y, winSize.height/2);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    CCPoint actualPosition = ccp(x, y);
    
    CCPoint centerOfView = ccp(winSize.width/2, winSize.height/2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    this->setPosition(viewPoint);
}

#pragma mark - Touch
bool GamePlayWithBoss::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    if (_player->getAttack() == 3) {
        _player->setAttack(0);
        for (b2Fixture* f = _player->getMpBody()->GetFixtureList(); f; f = f->GetNext())
        {
            //do something with the fixture 'f'
            b2Filter  fi = b2Filter();
            fi.groupIndex = - 10;
            f->SetFilterData(fi);
        }
    }
//    if (GameManager::sharedGameManager()->getNumberActionPlayer() > 0)
    {
        _contactting = false;
        giamVanToc = true;
        touchLocation = touch->getLocationInView();
        touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
        touchLocation = this->convertToNodeSpace(touchLocation);
        
        
        
        if (_player->getMpBody()->GetPosition().y * PTM_RATIO < touchLocation.y )
            isTouchTop = true;
        else isTouchTop = false;
        if (_player->getMpBody()->GetPosition().x * PTM_RATIO < touchLocation.x ) {
            isTouchRight = true;
        }else isTouchRight = false;
        this->checkTouchPoint(touchLocation);     
    }
    return true;
}

void GamePlayWithBoss::ccTouchEnded(CCSet* touches, CCEvent* event)
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
void GamePlayWithBoss::checkTouchPoint(cocos2d::CCPoint p) {
    
    float kc1 = _boss->getImage()->getContentSize().width/2;
    float kc2 = ccpDistance(convertMetterToPixel(CCPoint(_boss->getMpBody()->GetPosition().x,
                                        _boss->getMpBody()->GetPosition().y)), p);
    if (kc2 <= kc1 && _player->getAttack() != 3) {
        ninjaAttack = true;
        CCSprite * taget = CCSprite::create("muctieu.png");
        _boss->addChild(taget);
        CCFadeOut * fo = CCFadeOut::create(1.5f);
        CCHide * hide = CCHide::create();
        CCSequence * sq = CCSequence::create(fo, hide, NULL);
        taget->runAction(sq);
        _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
        _player->setAttack(3);
    }else  {
        CCSprite * t = CCSprite::create("Icon-72.png");
        CCScaleBy *scale = CCScaleBy::create(2, 0);
        CCHide *hide = CCHide::create();
        CCSequence * sq = CCSequence::create(scale, hide, NULL);
        t->runAction(sq);
        t->setPosition(p);
        this->addChild(t, 10);
        if (_player->getAttack() != 3) {
            _player->setAttack(1);
        }else _player->setAttack(0);
        
        ninjaAttack = false;
        _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
        this->touch(p);
    }
    
    GameManager::sharedGameManager()->setNumberActionPlayer(
                GameManager::sharedGameManager()->getNumberActionPlayer() - 1);
}
void GamePlayWithBoss::touch( CCPoint location)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    b2Vec2 impulse(0.0f,0.0f);
    if (ninjaAttack == false)
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
        impulse.x = impulse.x / 43;
        impulse.y = impulse.y / 43;
                _player->getMpBody()->ApplyLinearImpulse(impulse, _player->getMpBody()->GetWorldCenter());
        
        if (isTouchRight == true) {
            _player->getMpBody()->SetAngularVelocity(-10000);
        }else if ( isTouchRight == false) {
            _player->getMpBody()->SetAngularVelocity(10000);
        }
    }
}
void GamePlayWithBoss::touchBoss(cocos2d::CCPoint location) {
    b2Vec2 impulse(0.0f,0.0f);
    impulse.y = (location.y - _boss->getMpBody()->GetPosition().y * PTM_RATIO) / 10 ;
    if (isBossLeftPlayer == false) {
        impulse.x = - abs(location.x - _boss->getMpBody()->GetPosition().x * PTM_RATIO) / 35 ;
    }else
        impulse.x =  abs(location.x - _boss->getMpBody()->GetPosition().x * PTM_RATIO) / 65 ;
    
    _boss->getMpBody()->SetLinearVelocity(impulse);
}
void GamePlayWithBoss::bossAttack() {
//    CCPoint p = _player->getPosition();
//    CCSprite * taget = CCSprite::create("phitieu.png");
//    taget->setPosition(convertMetterToPixel(ccp(_boss->getMpBody()->GetPosition().x,
//                                                _boss->getMpBody()->GetPosition().y)));
//    this->addChild(taget, 10);
//    taget->setTag(10);
//    CCPoint p1 = convertMetterToPixel(CCPoint(_boss->getMpBody()->GetPosition().x,
//                                              _boss->getMpBody()->GetPosition().y));
//    CCPoint p2 = convertMetterToPixel(CCPoint(_player->getMpBody()->GetPosition().x,
//                                              _player->getMpBody()->GetPosition().y));
//    float s = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) *  (p1.y - p2.y));
//    CCMoveTo * move = CCMoveTo::create(s/600, p);
//    CCCallFuncN *remove = CCCallFuncN::create(this,
//                                callfuncN_selector(GamePlayWithBoss::removeSprite));
//    CCSequence * sq = CCSequence::create(move, remove, NULL);
//    taget->runAction(sq);
//    if (this->getChildByTag(10)) {
//        this->removeChildByTag(10);
//    }
    b2Vec2 p = _boss->getMpBody()->GetPosition();
    CCSprite *taget = CCSprite::create("phitieu.png");
    array_taget->addObject(taget);
    this->addChild(taget, 1000);
    taget->setTag(10);
    taget->setPosition( convertMetterToPixel(ccp( p.x, p.y)) );
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x, p.y);
    bodyDef.userData = taget;
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    
    b2CircleShape circle;
    
    float _radius = taget->getContentSize().width / 2;
    circle.m_radius = _radius / PTM_RATIO;
    
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    //    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 14.0f; // trong luong
    fixtureDef.friction = 1.0f; //ma sat
    fixtureDef.restitution = 0.0f;
    fixtureDef.filter.groupIndex = -10;
    body->CreateFixture(&fixtureDef);
    
    body->SetGravityScale(0);
    body->GetFixtureList()->SetSensor(true);
    
    b2Vec2  p1 = _boss->getMpBody()->GetPosition();
    b2Vec2  p2 = _player->getMpBody()->GetPosition();
    float lenght = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    
    b2Vec2 impulse(0.0f, 0.0f) ;
    impulse.y = (( p2.y - p1.y) / lenght) * 20  ;
    impulse.x = (( p2.x - p1.x) / lenght) * 20  ;
    body->SetLinearVelocity(impulse);
}
void GamePlayWithBoss::attackBoss() {
    b2Vec2  p1 = _boss->getMpBody()->GetPosition();
    b2Vec2  p2 = _player->getMpBody()->GetPosition();
    b2Vec2 impulse(0.0f, 0.0f) ;
    impulse.y = (- p2.y + p1.y) * 6 ;
    impulse.x = (- p2.x + p1.x) * 6  ;
    
    _player->getMpBody()->SetLinearVelocity(impulse);
//    _player->getMpBody()->ApplyForce(impulse, p1);
//    _player->setPosition(_boss->getPoint());
}
#pragma mark - update
void GamePlayWithBoss::update(float dt) {
    //Iterate over the bodies in the physics world
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL)
		{
			CCSprite *myActor = (CCSprite*)b->GetUserData();
			myActor->setPosition(ccp(b->GetPosition().x * PTM_RATIO,
                                           b->GetPosition().y * PTM_RATIO));
//            CCRect rect = this->boundingBox();
//            if (myActor->getPosition().y < 0 && myActor->getTag() == 10) {
//                this->removeChildByTag(10);
//                world->DestroyBody(b);
//            }
		}
	}
    if (_player->getMpBody()) {
        _player->setPositionX(_player->getMpBody()->GetPosition().x * PTM_RATIO);
        _player->setPositionY(_player->getMpBody()->GetPosition().y * PTM_RATIO);
        //true thi _player khong xoay false thi xoay
        _player->getMpBody()->SetFixedRotation(true);
        _boss->getMpBody()->SetFixedRotation(true);
        int velocityIterations = 8;
        int positionIterations = 1;
        
        // Instruct the world to perform a single step of simulation. It is
        // generally best to keep the time step and iterations fixed.
        world->Step(dt, velocityIterations, positionIterations);
//        this->setPosition(_player->getPosition());
        //        _player->getMpBody()->SetFixedRotation(false);
//        
        if (_player->getMpBody()->GetPosition().y * PTM_RATIO > touchLocation.y &&
            giamVanToc == true && isTouchTop == true && _player->getAttack() == 1) {
//            _player->setAttack(2);
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
            giamVanToc = false;
        }else if (_player->getMpBody()->GetPosition().y * PTM_RATIO < touchLocation.y &&
                  giamVanToc == true && isTouchTop == false && _player->getAttack() == 1) {
//            _player->setAttack(2);
            b2Vec2 v = _player->getMpBody()->GetLinearVelocity();
            CCLog("velocity x: %f", v.x);
            CCLog("velocity y: %f", v.y);
            float diffx = abs(_player->getMpBody()->GetPosition().x * PTM_RATIO - touchLocation.x);
            float diffy = abs(_player->getMpBody()->GetPosition().y * PTM_RATIO - touchLocation.y);
            _player->getMpBody()->SetLinearVelocity(b2Vec2(v.x /(diffx/100 + 1), v.y /(diffy/30 + 1)));
            giamVanToc = false;
        }  
    }
    
    
    // Collision Detection ----------- contact wall ----------------------------
    //begin contact
    if (GameManager::sharedGameManager()->getBeginContact() == true) {
        if (_player->getAttack() != 0) {
//            _player->setAttack(0);
        }
        _player->getMpBody()->SetAngularVelocity(0);
        CCAnimation *anim=CCAnimation::create();
        //top
        if (GameManager::sharedGameManager()->getDirectionContact() == 1) {
            anim->addSpriteFrameWithFileName("ninja_attack.png");
            anim->addSpriteFrameWithFileName("ninja.png");
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
//            _player->getImage()->setFlipX(true);
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
//        anim->setRestoreOriginalFrame(true);
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
//        anim->setRestoreOriginalFrame(true);
        CCAnimate * animet=CCAnimate::create(anim);
        _player->getImage()->runAction(animet);
        //        _player->getImage()->setFlipX(false);
        _player->getImage()->setFlipY(false);
    }
  
}
void GamePlayWithBoss::updateCheckStop(float dt) {
    CCPoint p = _player->getPosition();
    CCPoint p2 = _player->getPoint();
    if (p.x == p2.x && p.y == p2.y) {
        _player->setStop(true);
    }else _player->setStop(false);
}
void GamePlayWithBoss::updateLocation_Direction(float dt) {
    CCPoint pBoss = convertMetterToPixel(CCPoint(_boss->getMpBody()->GetPosition().x, _boss->getMpBody()->GetPosition().y));
    CCPoint pPlay = convertMetterToPixel(CCPoint(_player->getMpBody()->GetPosition().x, _player->getMpBody()->GetPosition().y));
    
    if (pPlay.x < _player->getPoint().x) {
        _player->getImage()->setFlipX(true);
        _player->setFlipX(true);
    }else if (pPlay.x > _player->getPoint().x){
        _player->getImage()->setFlipX(false);
        _player->setFlipX(false);
    }
    
    if (pBoss.x < _boss->getPoint().x) {
        _boss->getImage()->setFlipX(true);
        _boss->setFlipX(true);
    }else if (pBoss.x > _boss->getPoint().x){
        _boss->getImage()->setFlipX(false);
        _boss->setFlipX(false);
    }
    _boss->setPoint(pBoss);
    _player->setPoint(pPlay);
}
void GamePlayWithBoss::updatePhantom(float dt) {
    //-----------------phantom--------------------------------------------------
    if (_player->getStop() == false) {
        if (_player->getAttack() == 3 ) {
            CCSprite * sprite = CCSprite::create("ninja.png");
            sprite->setPosition(_player->getPosition());
            CCFadeOut *fo = CCFadeOut::create(0.4f);
            CCCallFuncN *remove = CCCallFuncN::create(sprite,callfuncN_selector(GamePlayWithBoss::removeSprite));
            CCSequence * sq = CCSequence::create(fo, remove, NULL);
            sprite->runAction(sq);
            this->addChild(sprite, 1000);
        }else if (_player->getAttack() == 1 ) {
            CCSprite * sprite = CCSprite::create("ninja_attack.png");
            sprite->setPosition(_player->getPosition());
            int i = rand() % 4 + 1;
            CCRotateTo *rotate = CCRotateTo::create(0, 360 / i);
            CCFadeOut *fo = CCFadeOut::create(0.4f);
            CCCallFuncN *remove = CCCallFuncN::create(sprite,callfuncN_selector(GamePlayWithBoss::removeSprite));
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
void GamePlayWithBoss::updateTime(float dt) {
    if (timeDelayAttackBoss > 0) {
        timeDelayAttackBoss --;
    }
    if (timeDelayContactBoss > 0) {
        timeDelayContactBoss --;
    }
    
    if (timeDelayAttackBoss == 0) {
        _boss->getMpBody()->SetGravityScale(1);
        timeDelayAttackBoss = -1;
        if (isBossLeftPlayer == false) {
            _boss->getMpBody()->SetLinearVelocity(b2Vec2(-6, -4));
        }else _boss->getMpBody()->SetLinearVelocity(b2Vec2(6, -4));
        
    }
    if (timeDelayContactBoss == 0) {
        timeDelayContactBoss = -1;
        maxHeightJumpBoss = size.height * 2.0f/3 + rand() % (int)(size.height * 1.0f/6);
        maxWithJumpBoss = size.width * 1.0f/3 + rand() % (int)(size.width * 1.0f/6);
        if (_boss->getPositionX() > _player->getPositionX()) {
            CCPoint p = CCPoint(abs(_boss->getPosition().x - maxWithJumpBoss/2), maxHeightJumpBoss);
            touchBoss(p);
        }else {
            CCPoint p = CCPoint(abs(_boss->getPosition().x + maxWithJumpBoss/2), maxHeightJumpBoss);
            touchBoss(p);
        }
        
    }
}
void GamePlayWithBoss::updateBoss(float dt) {
    char strlife[20] = {0};
    sprintf(strlife, "%i-X", _lifes);
    _lbLifes->setString(strlife);
    //-------------Lost------------------------
    if (_lifes <= 0) {
        this->unscheduleUpdate();
        this->unscheduleAllSelectors();
        _lbLost->setVisible(true);
        btquit->setVisible(true);
        btreset->setVisible(true);
        btpause->setVisible(false);
    }
    if (_bloodBoss <= 0) {
        this->unscheduleUpdate();
        this->unscheduleAllSelectors();
        _lbWin->setVisible(true);
        btquit->setVisible(true);
        btreset->setVisible(true);
        btpause->setVisible(false);
    }
    if (_player->getAttack() != 3 && GameManager::sharedGameManager()->getBeginContact() == false) {
        for (b2Fixture* f = _player->getMpBody()->GetFixtureList(); f; f = f->GetNext())
        {
            //do something with the fixture 'f'
            b2Filter  fi = b2Filter();
            fi.groupIndex = - 10;
            f->SetFilterData(fi);
        }
    }else {
        for (b2Fixture* f = _player->getMpBody()->GetFixtureList(); f; f = f->GetNext())
        {
            //do something with the fixture 'f'
            b2Filter  fi = b2Filter();
            fi.groupIndex =  10;
            f->SetFilterData(fi);
        }
    }
    if (GameManager::sharedGameManager()->getBeginContactBoss() == true) {
        timeDelayContactBoss = 2;
        GameManager::sharedGameManager()->setBeginContactBoss(false);
        if (_boss->getMpBody()->GetPosition().x >= _player->getMpBody()->GetPosition().x) {
            isBossLeftPlayer = false;
        }else isBossLeftPlayer = true;
    }
    if (_boss->getMpBody()->GetPosition().y * 32 > maxHeightJumpBoss) {
        maxHeightJumpBoss = size.height + 100;
        timeDelayAttackBoss = 2;
        _boss->getMpBody()->SetLinearVelocity(b2Vec2(0, 0));
        _boss->getMpBody()->SetGravityScale(0);
        _boss->getMpBody()->SetAngularVelocity(0);
        //giam soc
        _boss->getMpBody()->SetLinearDamping(0);
        this->bossAttack();
    }
    
    // contact boss with taget
    CCSprite * sprite = (CCSprite*)array_taget->lastObject();
    if (sprite) {
        float kc1 = _boss->getImage()->getContentSize().width/2 + sprite->getContentSize().width/2;
        float kc2 = ccpDistance(_player->getPosition(), sprite->getPosition());
        if (kc2 <= kc1 ) {
            _player->getMpBody()->SetLinearVelocity(b2Vec2(0, 4));
            _player->getMpBody()->SetGravityScale(1);
            _lifes --;
            ninjaAttack = false;
            _player->setAttack(2);
//            this->removeChild(sprite);
            array_taget->removeLastObject();
        }
    }
    
    // ninja attack boss
    if (ninjaAttack == true) {
        this->attackBoss();
//        _player->getMpBody()->GetFixtureList()->SetSensor(false);
//        fixtureDef.filter.groupIndex = -10;
    }else {
//        _player->getMpBody()->GetFixtureList()->SetSensor(false);
    }
    
    //----------ninja contact with boss------------
    float kc1 = _player->getImage()->getContentSize().width/2 + _boss->getImage()->getContentSize().width/2;
    float kc2 = ccpDistance(_player->getPoint(), _boss->getPoint());
    if (kc2 < kc1) {
        
        if (_player->getAttack() == 1) {
            _bloodBoss = _bloodBoss - 0.2;
            _player->actionAttack();
        }else if(_player->getAttack() == 3) {
            _bloodBoss = _bloodBoss - 0.05f;
            _player->actionAttack2();
        }
        
    }
    _progressBloodBoss->setPercentage(_bloodBoss);
    
}
void GamePlayWithBoss::removeSprite(cocos2d::CCNode *node) {
    CCSprite * sp = (CCSprite*)node;
    sp->removeFromParentAndCleanup(true);
}
void GamePlayWithBoss::registerWithTouchDispatcher()
{
    //    CCDirector* pDirector = CCDirector::sharedDirector();
    //    pDirector->getTouchDispatcher()->addTargetedDelegate(this, -10, true);
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}
CCScene* GamePlayWithBoss::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    CCLayer* layer = new GamePlayWithBoss();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}
#pragma mark - add wall
void GamePlayWithBoss::createRectangularFixtureWithPoint(cocos2d::CCPoint p1, cocos2d::CCPoint p2) {
    const float pixelsPerMeter = 32.0f;
    float with = 0;
    float height = 0;
    float sizeTileWith = _tileMap->getTileSize().width;
    float sizeTileHeight = _tileMap->getTileSize().height;
    CCPoint pointWord1 = convertPoitMapToPixelReverseY(p1);
    CCPoint pointWord2 = convertPoitMapToPixelReverseY(p2);
   
    CCPoint pw1 = CCPoint(pointWord1.x - sizeTileWith/2, pointWord1.y + sizeTileHeight/2);
    CCPoint pw2 = CCPoint(pointWord2.x + sizeTileWith/2, pointWord2.y - sizeTileHeight/2);
    CCPoint pCenter = CCPoint((pw2.x - pw1.x)/2 + pw1.x, abs(pw1.y - pw2.y)/2 + pw2.y );

    with = pw2.x - pw1.x;
    height = abs(pw1.y - pw2.y);
    
    // create the body
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(pCenter.x / pixelsPerMeter, pCenter.y / pixelsPerMeter);
    //    bodyDef.position.Set(pCenter.x , pCenter.y );
    b2Body* body = world->CreateBody(&bodyDef);
    // define the shape
    b2PolygonShape shape;
    CCLog("with : %f, height : %f", with, height);
    shape.SetAsBox((with / 2 /pixelsPerMeter), (height / 2/ pixelsPerMeter));
//    shape.SetAsBox((with / 2 /pixelsPerMeter) * GameManager::sharedGameManager()->getSCALE_N_X(),
//                   (height / 2/ pixelsPerMeter) * GameManager::sharedGameManager()->getSCALE_N_Y());
    
    // create the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1000.0f;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 0.0f;
    //    fixtureDef.filter.categoryBits = 1;
    fixtureDef.filter.maskBits = 0xffff;
    body->CreateFixture(&fixtureDef);
}
void GamePlayWithBoss::addWalls() {
    this->createRectangularFixtureWithPoint(CCPoint(0, 19), CCPoint(31, 19));
    this->createRectangularFixtureWithPoint(CCPoint(0, 0), CCPoint(9, 1));
    this->createRectangularFixtureWithPoint(CCPoint(20, 0), CCPoint(31, 1));
}

#pragma mark - convert point
CCPoint GamePlayWithBoss::convertMetterToPixel(CCPoint p) {
    return CCPoint(p.x * PTM_RATIO, p.y * PTM_RATIO);
}
CCPoint GamePlayWithBoss::convertPixelToMetter(cocos2d::CCPoint p) {
    return CCPoint(p.x / PTM_RATIO, p.y / PTM_RATIO);
}
CCPoint GamePlayWithBoss::convertPoitMapToPixel(cocos2d::CCPoint pointMap) {
    float H = CCDirector::sharedDirector()->getWinSize().height;
    float Y = _tileMap->getMapSize().height;
    float x = _tileMap->getTileSize().width;
    float y = _tileMap->getTileSize().height;
    return CCPoint(x * 0.5f + x * pointMap.x,H - ((Y - pointMap.y) * y - y * 0.5f));
}
CCPoint GamePlayWithBoss::convertPoitMapToPixelReverseY(cocos2d::CCPoint pointMap) {
    float Y = _tileMap->getMapSize().height;
    float x = _tileMap->getTileSize().width;
    float y = _tileMap->getTileSize().height;
    return CCPoint(x * 0.5f + x * pointMap.x,((Y - pointMap.y) * y - y * 0.5f));
}
#pragma mark - click menu item
void GamePlayWithBoss::click_continue(cocos2d::CCObject *pSender) {
    btquit->setVisible(false);
    btcontinue->setVisible(false);
    btpause->setVisible(true);
    this->resumeSchedulerAndActions();
}
void GamePlayWithBoss::click_pause(cocos2d::CCObject *pSender) {
    btquit->setVisible(true);
    btcontinue->setVisible(true);
    btpause->setVisible(false);
    this->pauseSchedulerAndActions();
}
void GamePlayWithBoss::click_quit(cocos2d::CCObject *pSender) {
    this->unscheduleUpdate();
    this->unscheduleAllSelectors();
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.7f, GameMenu::scene()));
}
void GamePlayWithBoss::click_reset(cocos2d::CCObject *pSender) {
    this->unscheduleUpdate();
    this->unscheduleAllSelectors();
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.7f, GamePlayWithBoss::scene()));
}