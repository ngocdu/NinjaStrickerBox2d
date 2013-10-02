//
//  Arrow.cpp
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/16.
//
//
#include "Arrow.h"
Arrow::Arrow() {
    
}
Arrow::Arrow(int d) {
    this->setDirection(d);
}
void Arrow::rotate() {
    CCRotateTo * ro;
    switch (this->getDirection()) {
        case 1:
            
            break;
        case 2:
            ro = CCRotateTo::create(0, -45);
            break;
        case 3:
            ro = CCRotateTo::create(0, -90);
            break;
        case 4:
            ro = CCRotateTo::create(0, -135);
            break;
        case 5:
            ro = CCRotateTo::create(0, -180);
            break;
        case 6:
            ro = CCRotateTo::create(0, -225);
            break;
        case 7:
            ro = CCRotateTo::create(0, -270);
            break;
        case 8:
            ro = CCRotateTo::create(0, -315);
            break;
        default:
            break;
    }
    this->runAction(ro);
}