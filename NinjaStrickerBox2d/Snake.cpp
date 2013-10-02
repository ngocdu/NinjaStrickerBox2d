//
//  Snake.cpp
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/16.
//
//
#include "Snake.h"
Snake::Snake() {
    
}
void Snake::actionMoveToPoint(cocos2d::CCPoint p) {
    CCMoveTo * move = CCMoveTo::create(4, p);
    CCMoveTo * moveBack = CCMoveTo::create(4, this->getPosition());
    CCSequence *sq = CCSequence::create(move, moveBack, NULL);
    CCRepeatForever *rep = CCRepeatForever::create(sq);
    this->runAction(rep);
}