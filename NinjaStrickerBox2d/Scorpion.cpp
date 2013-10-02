//
//  Scorpion.cpp
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/16.
//
//
#include "Scorpion.h"
Scorpion::Scorpion() {
    
}
void Scorpion::fluctuating(float amplitude ) {
    CCMoveTo * movetop = CCMoveTo::create(1, CCPoint(this->getPosition().x,
                                        this->getPosition().y + amplitude));
    CCMoveTo * movebottom = CCMoveTo::create(2, CCPoint(this->getPosition().x,
                                        this->getPosition().y - amplitude));
    CCSequence *sq = CCSequence::create(movetop, movebottom, NULL);
    CCRepeatForever * rep = CCRepeatForever::create(sq);
    this->runAction(rep);
}
void Scorpion::fluctuatingAndMove(float amplitude, float distances) {
    CCMoveTo * movetop = CCMoveTo::create(1, CCPoint(this->getPosition().x + distances / 4,
                                                this->getPosition().y + amplitude));
    CCMoveTo * movebottom = CCMoveTo::create(2, CCPoint(this->getPosition().x + distances *2/ 4,
                                                this->getPosition().y - amplitude));
    CCMoveTo * movetop1 = CCMoveTo::create(1, CCPoint(this->getPosition().x + distances *3/ 4,
                                                this->getPosition().y + amplitude));
    CCMoveTo * movebottom1 = CCMoveTo::create(2, CCPoint(this->getPosition().x + distances ,
                                                this->getPosition().y - amplitude));
    CCMoveTo * movetop2 = CCMoveTo::create(1, CCPoint(this->getPosition().x + distances *3/ 4,
                                                this->getPosition().y + amplitude));
    CCMoveTo * movebottom2 = CCMoveTo::create(2, CCPoint(this->getPosition().x + distances *2/ 4,
                                                this->getPosition().y - amplitude));
    CCMoveTo * movetop3 = CCMoveTo::create(1, CCPoint(this->getPosition().x + distances / 4,
                                                this->getPosition().y + amplitude));
    CCMoveTo * movebottom3 = CCMoveTo::create(2, CCPoint(this->getPosition().x,
                                                this->getPosition().y - amplitude));
    CCSequence *sq = CCSequence::create(movetop, movebottom,movetop1, movebottom1,
                                    movetop2, movebottom2, movetop3, movebottom3, NULL);
    CCRepeatForever * rep = CCRepeatForever::create(sq);
    this->runAction(rep);
}