#include "MyContactListener.h"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"
using namespace CocosDenshion;
using namespace cocos2d;
MyContactListener::MyContactListener() : _contacts() {
}
MyContactListener::~MyContactListener() {
}
void MyContactListener::BeginContact(b2Contact* contact) {
    // We need to copy out the data because the b2Contact passed in
    // is reused.
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    _contacts.push_back(myContact);
    CCLog("begin contact");  
//    if (this->getNumberBegin() == 2)
    {
        
        if (myContact.fixtureA->GetBody()->GetType() == b2_dynamicBody) {
            if (myContact.fixtureB->GetBody()->GetType() == b2_staticBody) {
                if (myContact.fixtureA->GetDensity() == 14) {
                    GameManager::sharedGameManager()->setBeginContactBoss(true);
                    GameManager::sharedGameManager()->setEndContactBoss(false);
                }else {
                    GameManager::sharedGameManager()->setNumberActionPlayer(2);
                    GameManager::sharedGameManager()->setBeginContact(true);
                    this->setNumberBegin(this->getNumberBegin() + 1);
                    myContact.fixtureA->GetBody()->SetAngularVelocity(0);
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
                }
            }else if (myContact.fixtureB->GetBody()->GetType() == b2_kinematicBody) {
//                myContact.fixtureA->GetBody()->SetAngularVelocity(0);
//                myContact.fixtureA->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
                GameManager::sharedGameManager()->setNumberActionPlayer(2);
                GameManager::sharedGameManager()->setBeginContactKinamic(true);
                this->setNumberBegin(this->getNumberBegin() + 1);
                myContact.fixtureA->GetBody()->SetAngularVelocity(0);
                b2Body * bodyPlayer = myContact.fixtureA->GetBody();
                b2Body * bodyB = myContact.fixtureB->GetBody();
                if (bodyPlayer->GetPosition().y >= bodyB->GetPosition().y + 0.5f) {
                    GameManager::sharedGameManager()->setDirectionContactKinamic(1);//top
                }else if (bodyPlayer->GetPosition().y <= bodyB->GetPosition().y - 0.5f){
                    GameManager::sharedGameManager()->setDirectionContactKinamic(2);//bottom
                }else if (bodyPlayer->GetPosition().x >= bodyB->GetPosition().x + 0.5f) {
                    GameManager::sharedGameManager()->setDirectionContactKinamic(4);//right
                    if (myContact.fixtureB->GetBody()->GetAngularVelocity() > 0) {
                        myContact.fixtureA->GetBody()->SetLinearVelocity(b2Vec2(-14, -10));
                    }
                }else if (bodyPlayer->GetPosition().x <= bodyB->GetPosition().x - 0.5f) {
                    GameManager::sharedGameManager()->setDirectionContactKinamic(3);//left
                    if (myContact.fixtureB->GetBody()->GetAngularVelocity() < 0) {
                        myContact.fixtureA->GetBody()->SetLinearVelocity(b2Vec2(14, 10));
                    }
                }

            }
        }else if (myContact.fixtureB->GetBody()->GetType() == b2_dynamicBody) {
            if (myContact.fixtureA->GetBody()->GetType() == b2_staticBody) {
                if (myContact.fixtureB->GetDensity() == 14) {
                    GameManager::sharedGameManager()->setBeginContactBoss(true);
                    GameManager::sharedGameManager()->setEndContactBoss(false);
                }else {
                    GameManager::sharedGameManager()->setNumberActionPlayer(2);
                    GameManager::sharedGameManager()->setBeginContact(true);
                    this->setNumberBegin(this->getNumberBegin() + 1);
                    myContact.fixtureB->GetBody()->SetAngularVelocity(0);
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
            }else if (myContact.fixtureA->GetBody()->GetType() == b2_kinematicBody) {
//                myContact.fixtureB->GetBody()->SetAngularVelocity(0);
//                myContact.fixtureB->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
                GameManager::sharedGameManager()->setNumberActionPlayer(2);
                GameManager::sharedGameManager()->setBeginContactKinamic(true);
                this->setNumberBegin(this->getNumberBegin() + 1);
                myContact.fixtureB->GetBody()->SetAngularVelocity(0);
                b2Body * bodyPlayer = myContact.fixtureB->GetBody();
                b2Body * bodyB = myContact.fixtureA->GetBody();
                if (bodyPlayer->GetPosition().y >= bodyB->GetPosition().y + 0.5f) {
                    GameManager::sharedGameManager()->setDirectionContactKinamic(1);
                }else if (bodyPlayer->GetPosition().y <= bodyB->GetPosition().y - 0.5f){
                    GameManager::sharedGameManager()->setDirectionContactKinamic(2);
                }else if (bodyPlayer->GetPosition().x >= bodyB->GetPosition().x + 0.5f) {
                    GameManager::sharedGameManager()->setDirectionContactKinamic(4);//right
                    if (myContact.fixtureA->GetBody()->GetAngularVelocity() > 0) {
                        myContact.fixtureB->GetBody()->SetLinearVelocity(b2Vec2(-14, -10));
                    }
                }else if (bodyPlayer->GetPosition().x <= bodyB->GetPosition().x - 0.5f) {
                    GameManager::sharedGameManager()->setDirectionContactKinamic(3);//left
                    if (myContact.fixtureA->GetBody()->GetAngularVelocity() < 0) {
                        myContact.fixtureB->GetBody()->SetLinearVelocity(b2Vec2(14, 10));
                    }
                }
            }
        }
    }
}

void MyContactListener::EndContact(b2Contact* contact) {
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(_contacts.begin(), _contacts.end(), myContact);
    if (pos != _contacts.end())
    {
        _contacts.erase(pos);
    }
    CCLog("end contact");
    if (myContact.fixtureB->GetBody()->GetType() == b2_dynamicBody) {
        if (myContact.fixtureA->GetBody()->GetType() == b2_staticBody)
        {
            if (myContact.fixtureB->GetDensity() == 14) {
                GameManager::sharedGameManager()->setEndContactBoss(true);
                GameManager::sharedGameManager()->setBeginContactBoss(false);
            }else {
                GameManager::sharedGameManager()->setNumberActionPlayer(1);
                GameManager::sharedGameManager()->setEndContact(true);
            }
        }
        else if (myContact.fixtureA->GetBody()->GetType() == b2_kinematicBody) {
            GameManager::sharedGameManager()->setEndContactKinamic(true);
            if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 1 ||
                GameManager::sharedGameManager()->getDirectionContactKinamic() == 2) {
                myContact.fixtureB->GetBody()->SetAngularVelocity(0);
                myContact.fixtureB->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
            }else if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 4) {
                myContact.fixtureB->GetBody()->SetGravityScale(1);
//                myContact.fixtureB->GetBody()->SetAngularVelocity(0);
//                myContact.fixtureB->GetBody()->SetLinearVelocity(b2Vec2(-10, 10));
            }else if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 3) {
//                myContact.fixtureB->GetBody()->SetAngularVelocity(0);
//                myContact.fixtureB->GetBody()->SetLinearVelocity(b2Vec2(10, -10));
            }
        }
    }else if (myContact.fixtureA->GetBody()->GetType() == b2_dynamicBody) {
        if (myContact.fixtureB->GetBody()->GetType() == b2_staticBody) {
            if (myContact.fixtureA->GetDensity() == 14) {
                GameManager::sharedGameManager()->setEndContactBoss(true);
                GameManager::sharedGameManager()->setBeginContactBoss(false);
            }else {
                GameManager::sharedGameManager()->setNumberActionPlayer(1);
                GameManager::sharedGameManager()->setEndContact(true);
            }
        }else if (myContact.fixtureB->GetBody()->GetType() == b2_kinematicBody) {
            GameManager::sharedGameManager()->setEndContactKinamic(true);
            if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 1 ||
                GameManager::sharedGameManager()->getDirectionContactKinamic() == 2) {
                myContact.fixtureA->GetBody()->SetAngularVelocity(0);
                myContact.fixtureA->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
            }else if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 4) {
                myContact.fixtureB->GetBody()->SetGravityScale(1);
//                myContact.fixtureA->GetBody()->SetAngularVelocity(0);
//                myContact.fixtureA->GetBody()->SetLinearVelocity(b2Vec2(-10, 10));
            }else if (GameManager::sharedGameManager()->getDirectionContactKinamic() == 3) {
//                myContact.fixtureA->GetBody()->SetAngularVelocity(0);
//                myContact.fixtureA->GetBody()->SetLinearVelocity(b2Vec2(10, -10));
            }
        }
    }
}
void MyContactListener::PreSolve(b2Contact* contact,
                                 const b2Manifold* oldManifold) {
}
void MyContactListener::PostSolve(b2Contact* contact,
                                  const b2ContactImpulse* impulse) {
}