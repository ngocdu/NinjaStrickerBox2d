//
//  Coin.cpp
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/16.
//
//

#include "Coin.h"
Coin::Coin() {
    
}
void Coin::action() {
//    CCRotateTo* actionTo = CCRotateTo::create(2, 37.2f, -37.2f);
//    CCRotateTo* actionToBack = CCRotateTo::create(2, 0, 0);
    CCRotateBy* actionBy = CCRotateBy::create(1.5f, 0.0f, -90.0f);
//    CCRotateBy* actionBy2 = CCRotateBy::create(2, 45.0f, 45.0f);
//    CCRotateBy* actionByBack = (CCRotateBy*)actionBy->reverse();
    
    
    CCSequence * sq = CCSequence::create(actionBy, NULL);
    CCRepeatForever * rep1 = CCRepeatForever::create(sq);
    this->runAction(rep1);
}