//
//  GameMenu.cpp
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/20.
//
//

#include "GameMenu.h"
CCScene* GameMenu::scene() {
    CCScene *scene = CCScene::create();
    GameMenu *layer1 = GameMenu::create();
    scene->addChild(layer1);
    return scene;
}

bool GameMenu::init() {
    size = CCDirector::sharedDirector()->getWinSize();
    CCMenuItemFont * menuPlay = CCMenuItemFont::create("PLAY", this, menu_selector(GameMenu::click_play));
    menuPlay->setPosition(CCPoint(size.width/2, size.height/2));
    
    CCMenuItemFont * menuExit = CCMenuItemFont::create("EXIT", this, menu_selector(GameMenu::click_exit));
    menuExit->setPosition(CCPoint(size.width/2, size.height/3));
    
    menu = CCMenu::create(menuPlay, menuExit, NULL);
    menu->setPosition(CCPoint(0, 0));
    this->addChild(menu, 10);
    this->addSpriteBg();
    return true;
}
void GameMenu::click_play(cocos2d::CCObject *pSender) {
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.7f, HelloWorld::scene()));
}
void GameMenu::click_exit(cocos2d::CCObject *pSender) {
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
CCPoint GameMenu::convertPoitMapToPixel(cocos2d::CCPoint pointMap) {
    float H = CCDirector::sharedDirector()->getWinSize().height;
    float Y = mapBg->getMapSize().height;
    float x = mapBg->getTileSize().width;
    float y = mapBg->getTileSize().height;
    return CCPoint(x * 0.5f + x * pointMap.x,H - ((Y - pointMap.y) * y - y * 0.5f));
}
void GameMenu::addSpriteBg() {
    mapBg = new CCTMXTiledMap();
    mapBg->initWithTMXFile("Map.tmx");
    mapBg->setPosition(CCPoint(0, 0));
    this->addChild(mapBg);
    CCTMXLayer *_meta = mapBg->layerNamed("meta");
    CCSize layerSize = _meta->getLayerSize();
    for( int y=0; y < layerSize.height; y++ )
    {
        for( int x=0; x < layerSize.width; x++ )
        {
            CCSprite* tileSprite = _meta->tileAt(ccp(x, y));
            if( tileSprite ) {
                CCPoint p = convertPoitMapToPixel(CCPoint(x, y));
                CCSprite *sp = CCSprite::create("ninja.png");
                sp->setPosition(p);
                this->addChild(sp, 2);
                CCAnimation *anim=CCAnimation::create();
                anim->addSpriteFrameWithFileName("ninja.png");
                anim->addSpriteFrameWithFileName("ninja_attack.png");
                anim->addSpriteFrameWithFileName("ninja_attack2.png");
                anim->addSpriteFrameWithFileName("ninja_attack3.png");
                anim->setDelayPerUnit(2.8f / 7.0f);
                anim->setRestoreOriginalFrame(true);
                CCAnimate * animet=CCAnimate::create(anim);
                CCRepeatForever * rep=CCRepeatForever::create(animet);
                sp->runAction(rep);
                
                CCMoveTo *move = CCMoveTo::create(4, CCPoint(p.x - 100, p.y));
                CCMoveTo *moveback = CCMoveTo::create(4, p);
                CCFlipX *fx = CCFlipX::create(true);
                CCFlipX *fx2 = CCFlipX::create(false);
                CCSequence * sq = CCSequence::create(fx, move, fx2, moveback, NULL);
                CCRepeatForever * rep2 =CCRepeatForever::create(sq);
                sp->runAction(rep2);
            }
        }
    }

}