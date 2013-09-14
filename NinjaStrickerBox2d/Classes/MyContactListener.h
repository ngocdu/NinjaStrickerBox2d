#ifndef __Sample_Box2D__MyContactListener__
#define __Sample_Box2D__MyContactListener__

#include <iostream>
#include <Box2D/Box2D.h>
#include <vector>
#include "GameManager.h"
struct MyContact {
    b2Fixture *fixtureA;
    b2Fixture *fixtureB;
    bool operator==(const MyContact& other) const
    {
        return (fixtureA == other.fixtureA) && (fixtureB == other.fixtureB);
    }
    };
    
    
    class MyContactListener : public b2ContactListener {
    public:
        std::vector<MyContact> _contacts;
        
        MyContactListener();
        ~MyContactListener();
        
        virtual void BeginContact(b2Contact* contact);
        virtual void EndContact(b2Contact* contact);
        virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
        virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
        
    private:
    };

#endif /* defined(__AirHockey__MyContactListener__) */
