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
    GameManager::sharedGameManager()->setNumberActionPlayer(2);
    GameManager::sharedGameManager()->setBeginContact(true);
    this->setNumberBegin(this->getNumberBegin() + 1);
//    if (this->getNumberBegin() == 2)
    {
        if (myContact.fixtureA->GetBody()->GetType() == b2_dynamicBody) {
            //            SimpleAudioEngine::sharedEngine()->playEffect("hit.caf");
            myContact.fixtureA->GetBody()->SetAngularVelocity(0);
//            myContact.fixtureA->GetBody()->SetAngularDamping(0);
            b2Body * bodyPlayer = myContact.fixtureA->GetBody();
            b2Body * bodyB = myContact.fixtureB->GetBody();
            if (bodyPlayer->GetPosition().y >= bodyB->GetPosition().y + 0.5f) {
                GameManager::sharedGameManager()->setDirectionContact(1);//top
            }else if (bodyPlayer->GetPosition().y <= bodyB->GetPosition().y - 0.5f){
                GameManager::sharedGameManager()->setDirectionContact(2);//bottom
            }else if (bodyPlayer->GetPosition().x >= bodyB->GetPosition().x + 0.5f) {
                GameManager::sharedGameManager()->setDirectionContact(4);//right
            }else if (bodyPlayer->GetPosition().x <= bodyB->GetPosition().x - 0.5f) {
                GameManager::sharedGameManager()->setDirectionContact(3);//left
            }
        }else if (myContact.fixtureB->GetBody()->GetType() == b2_dynamicBody) {
            //            SimpleAudioEngine::sharedEngine()->playEffect("hit.caf");
             myContact.fixtureB->GetBody()->SetAngularVelocity(0);
//            myContact.fixtureB->GetBody()->SetAngularDamping(0);
            b2Body * bodyPlayer = myContact.fixtureB->GetBody();
            b2Body * bodyB = myContact.fixtureA->GetBody();
            if (bodyPlayer->GetPosition().y >= bodyB->GetPosition().y + 0.5f) {
                GameManager::sharedGameManager()->setDirectionContact(1);
            }else if (bodyPlayer->GetPosition().y <= bodyB->GetPosition().y - 0.5f){
                GameManager::sharedGameManager()->setDirectionContact(2);
            }else if (bodyPlayer->GetPosition().x >= bodyB->GetPosition().x + 0.5f) {
                GameManager::sharedGameManager()->setDirectionContact(4);
            }else if (bodyPlayer->GetPosition().x <= bodyB->GetPosition().x - 0.5f) {
                GameManager::sharedGameManager()->setDirectionContact(3);
            }
        }
    }
//    else if(this->getNumberBegin() == 4) {
//        this->setNumberBegin(0);
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
    GameManager::sharedGameManager()->setNumberActionPlayer(1);
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