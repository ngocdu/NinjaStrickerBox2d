//
//  HelloWorldScene.cpp
//  NinjaStrickerBox2d
//
//  Created by MinhNT on 13/09/09.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define PTM_RATIO 32

enum {
    kTagParentNode = 1,
};

PhysicsSprite::PhysicsSprite()
: _mpBody(NULL)
{

}
//b2Body PhysicsSprite::getPhysicsBody() {
//    return *m_pBody;
//}
//void PhysicsSprite::setPhysicsBody(b2Body * body)
//{
//    m_pBody = body;
//}

// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool PhysicsSprite::isDirty(void)
{
    return true;
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform PhysicsSprite::nodeToParentTransform(void)
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

HelloWorld::HelloWorld()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    delta = 3.14f;
    setTouchEnabled( true );
    setAccelerometerEnabled( true );
    // init physics
    this->initPhysics();
    //---------------------------------
    _tileMap = new CCTMXTiledMap();
    _tileMap->initWithTMXFile("TileMap.tmx");
    _background = _tileMap->layerNamed("Background");
    _foreground = _tileMap->layerNamed("Foreground");
    _foreground->getTexture()->setAntiAliasTexParameters();
    _meta = _tileMap->layerNamed("Meta");
    _meta->setVisible(false);
    
    CCLOG("size tiled: %f , %f",_tileMap->getTileSize().width, _tileMap->getTileSize().height);
    withTileMap = _tileMap->getMapSize().width * _tileMap->getTileSize().width;
    CCLog("size map with %f", withTileMap);
    //    _tileMap->setPosition(ccp(size.width/2, size.height/2));
    this->addChild(_tileMap);
    
    CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("Objects");
    
    if(objectGroup == NULL){
        CCLog("tile map has no objects object layer");
//        return false;
    }
    
    CCDictionary *spawnPoint = objectGroup->objectNamed("SpawnPoint");
    
//    int x = ((CCString)*spawnPoint->valueForKey("x")).intValue();
//    int y = ((CCString)*spawnPoint->valueForKey("y")).intValue();
//    
//
//    _player->setPosition(ccp(x,y));
//    
//    this->addChild(_player);
    this->prepareLayers();
    
    addNewSpriteAtPosition(ccp(s.width/2, s.height/4));
    this->setViewPointCenter(_player->getPosition());
    
    //----------------------------------
    
    
    
    
    CCLabelTTF *label = CCLabelTTF::create("Tap screen", "Marker Felt", 32);
    addChild(label, 0);
    label->setColor(ccc3(0,0,255));
    label->setPosition(ccp( s.width/2, s.height-50));
    
    
    scheduleUpdate();
}

HelloWorld::~HelloWorld()
{
    delete world;
    world = NULL;
    
    //delete m_debugDraw;
}

void HelloWorld::initPhysics()
{

    CCSize s = CCDirector::sharedDirector()->getWinSize();

    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    world = new b2World(gravity);

    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);

    world->SetContinuousPhysics(true);

//     m_debugDraw = new GLESDebugDraw( PTM_RATIO );
//     world->SetDebugDraw(m_debugDraw);

    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    //m_debugDraw->SetFlags(flags);


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
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO));
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
    CCLOG("Add sprite %0.2f x %02.f",p.x,p.y);
   
    
    //We have a 64x64 sprite sheet with 4 different 32x32 images.  The following code is
    //just randomly picking one of the images
    int idx = (CCRANDOM_0_1() > .5 ? 0:1);
    int idy = (CCRANDOM_0_1() > .5 ? 0:1);
    _player = new PhysicsSprite();
    _player->initWithTexture(m_pSpriteTexture, CCRectMake(32 * idx,32 * idy,32,32));
    _player->autorelease();
    
    this->addChild(_player, 1000);
    
    _player->setPosition( CCPointMake( p.x, p.y) );
    
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
    
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(.5f, .5f);//These are mid points for our 1m box
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;    
    fixtureDef.density = 1.0f; // trong luong
    fixtureDef.friction = 0.9f; //ma sat
    body->CreateFixture(&fixtureDef);
    
    _player->setMpBody(body);
}


void HelloWorld::update(float dt)
{
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/
    
    if (_player->getMpBody()) {
        _player->setPositionX(_player->getMpBody()->GetPosition().x * PTM_RATIO);
        _player->setPositionY(_player->getMpBody()->GetPosition().y * PTM_RATIO);
        _player->getMpBody()->SetFixedRotation(false);
    }

    
    if (_player->getPosition().x != 480 || _player->getPosition().y != 320) {
        CCLog("_player x * %f", _player->getPosition().x);
        CCLog("_player y * %f", _player->getPosition().y);
    }
    
    this->setViewPointCenter(ccp(_player->getPosition().x ,
                                 _player->getPosition().y ));
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);
    
    //Iterate over the bodies in the physics world
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        if (b->GetUserData() != NULL) {
            //Synchronize the AtlasSprites position and rotation with the corresponding body
            CCSprite* myActor = (CCSprite*)b->GetUserData();
            myActor->setPosition( CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) );
            myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
        }    
    }
}
bool HelloWorld::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    CCPoint touchLocation = touch->getLocationInView();
    touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
    touchLocation = this->convertToNodeSpace(touchLocation);
    
    CCSprite * t = CCSprite::create("Icon-72.png");
    CCScaleBy *scale = CCScaleBy::create(2, 0);
    CCHide *hide = CCHide::create();
    CCSequence * sq = CCSequence::create(scale, hide, NULL);
    t->runAction(sq);
    t->setPosition(touchLocation);
    this->addChild(t, 10);
    
    this->touch(touchLocation);
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
        
//        addNewSpriteAtPosition( location );
        this->touch(location);
    }
}
void HelloWorld::touch( CCPoint location)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    b2Vec2 currentVelocity = _player->getMpBody()->GetLinearVelocity();
    b2Vec2 impulse(0.0f,0.0f);
    
//    // walk
//    if( location.y < (winSize.height * 0.5f) )
//    {
//        // apply impulse if x velocity is getting low
//        if( fabsf(currentVelocity.x) < 5.0f )
//        {
//            impulse.y = 0.0f;
//            impulse.x = 1.0f * delta;
//            if( location.x < (winSize.width * 0.5f) )
//                impulse.x = -impulse.x;
//            _player->getMpBody()->ApplyLinearImpulse(impulse, _player->getMpBody()->GetWorldCenter());
//        }
//    }
//    
//    // jump
//    else
    {
        // apply impulse
//        impulse.y = 1300.0f * delta;
//        impulse.x = 30.0f * delta;
//        impulse.y = 16.0f;
//        impulse.x = 10.0f;
//        CCLOG("location x : %f", location.x);
//        CCLOG("location y : %f", location.y);
//        CCLOG("point x : %f", _player->getMpBody()->GetPosition().x * PTM_RATIO);
//        CCLOG("point y : %f", _player->getMpBody()->GetPosition().y * PTM_RATIO);
        impulse.y = (location.y - _player->getMpBody()->GetPosition().y * PTM_RATIO)/20;
        impulse.x = (location.x - _player->getMpBody()->GetPosition().x * PTM_RATIO)/40;
        if( location.x < (winSize.width * 0.5f) )
//            impulse.x = -impulse.x;
        b2Vec2 point((location.x - _player->getPositionX())/10, (location.y - _player->getPositionY())/10);
        _player->getMpBody()->ApplyLinearImpulse(impulse, _player->getMpBody()->GetWorldCenter());
//        _player->getMpBody()->ApplyLinearImpulse(impulse, point);
    }
}
#pragma mark - create map 
void HelloWorld::prepareLayers()
{
//    CCObject *object;
//    CCARRAY_FOREACH(this->_tileMap->getChildren(), object)
//    {
//        // is this map child a tile layer?
//        CCTMXLayer* layer = dynamic_cast<CCTMXLayer*>(object);
//        if( layer != NULL )
//            this->createFixtures(layer);
//    }
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
                this->createRectangularFixture(layer, x, y, 1.1f, 1.1f);
        }
    }
}

void HelloWorld::createRectangularFixture(CCTMXLayer* layer, int x, int y,
                                     float width, float height)
{
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
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.0f;
//    fixtureDef.filter.categoryBits = kFilterCategoryLevel;
    fixtureDef.filter.maskBits = 0xffff;
    body->CreateFixture(&fixtureDef);
}
#pragma mark - handle touches

void HelloWorld::registerWithTouchDispatcher()
{
//    CCDirector* pDirector = CCDirector::sharedDirector();
//    pDirector->getTouchDispatcher()->addTargetedDelegate(this, -10, true);
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}
void HelloWorld::setViewPointCenter(CCPoint position)
{
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    int x = MAX(position.x, winSize.width/2);
    int y = MAX(position.y, winSize.height/2);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    CCPoint actualPosition = ccp(x, y);
    
    CCPoint centerOfView = ccp(winSize.width/2, winSize.height/2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    if (this->getPositionX() != 0 || this->getPositionY() != 160) {
        CCLog("this x: %f", this->getPositionX());
        CCLog("this v: %f", this->getPositionY());
    }
    
    this->setPosition(viewPoint);
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
