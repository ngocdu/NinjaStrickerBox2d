//
//  GameMenu.h
//  NinjaStrickerBox2d
//
//  Created by NgocDu on 13/09/20.
//
//

#ifndef NinjaStrickerBox2d_GameMenu_h
#define NinjaStrickerBox2d_GameMenu_h
#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "GamePlayWithBoss.h"
using namespace cocos2d;
class GameMenu : public CCLayer
{
public:
    // returns a Scene that contains the HelloWorld as the only child
    static cocos2d::CCScene* scene();
    virtual bool init();
    CREATE_FUNC(GameMenu);

    void click_play_boss(CCObject* pSender);
    void click_play(CCObject* pSender);
    void click_exit(CCObject* pSender);
    void addSpriteBg();
    CCPoint convertPoitMapToPixel(CCPoint pointMap);
private:
    CCSize size;
    CCMenu * menu;
    CCTMXTiledMap *mapBg;
};
#endif
