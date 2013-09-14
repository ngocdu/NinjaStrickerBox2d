#include "MyContactListener.h"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"
using namespace CocosDenshion;
using namespace cocos2d;
MyContactListener::MyContactListener() : _contacts()
{
}

MyContactListener::~MyContactListener()
{
}

void MyContactListener::BeginContact(b2Contact* contact)
{
    // We need to copy out the data because the b2Contact passed in
    // is reused.
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    _contacts.push_back(myContact);
    CCLog("begin contact");
    GameManager::sharedGameManager()->setBeginContact(true);
    
    
    if (myContact.fixtureA->GetBody()->GetType() == b2_dynamicBody) {
        //            SimpleAudioEngine::sharedEngine()->playEffect("hit.caf");
        b2Body * bodyPlayer = myContact.fixtureA->GetBody();
        b2Body * bodyB = myContact.fixtureB->GetBody();
        if (bodyPlayer->GetPosition().y > bodyB->GetPosition().y) {
            GameManager::sharedGameManager()->setDirectionContact(1);
        }
    }
//    else if (contact.fixtureB == _player->getMpBody()->GetFixtureList()) {
//        b2Body * bodyPlayer = _player->getMpBody();
//        b2Body * bodyA = contact.fixtureA->GetBody();
//        if (bodyPlayer->GetPosition().y > bodyA->GetPosition().y) {
//            CCAnimation *anim=CCAnimation::create();
//            
//            anim->addSpriteFrameWithFileName("ninja_attack.png");
//            anim->addSpriteFrameWithFileName("ninja.png");
//            anim->setDelayPerUnit(2.8f / 4.0f);
//            anim->setRestoreOriginalFrame(true);
//            CCAnimate * animet=CCAnimate::create(anim);
//            CCRepeatForever * rep=CCRepeatForever::create(animet);
//            rep->setTag(123456);
//            _player->runAction(animet);
//        }
//    }

}

void MyContactListener::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(_contacts.begin(), _contacts.end(), myContact);
    if (pos != _contacts.end())
    {
        _contacts.erase(pos);
    }
    CCLog("end contact");
    GameManager::sharedGameManager()->setEndContact(true);
}

void MyContactListener::PreSolve(b2Contact* contact,
                                 const b2Manifold* oldManifold)
{
}

void MyContactListener::PostSolve(b2Contact* contact,
                                  const b2ContactImpulse* impulse)
{
}