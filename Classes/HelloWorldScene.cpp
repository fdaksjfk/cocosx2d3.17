#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include"BackGround.h"
#include"Poker.h"
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Scene::init() )
    {
        return false;
    }


    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    //添加一个装饰卡片
    Puke* puke = Puke::create();
    puke->setPosition(Vec2(origin.x+ visibleSize.width/2, 150));
    this->addChild(puke);

    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    //添加场景
    auto play = MenuItemImage::create(
                                        "play.png",
                                        "play.png",
                                         CC_CALLBACK_1(HelloWorld::menuPlay, this));

    if (play == nullptr)
    {
        return false;
    }
    else
    {               
        //让它居中偏下一点
        float x = visibleSize.width / 2 - play->getContentSize().width / 2;
        float y = visibleSize.height / 2 - play->getContentSize().height / 2;
        play->setPosition(Vec2(x, y - 200));
    }
    auto play_menu = Menu::create(play, NULL);
    play_menu->setPosition(0, 0);
    this->addChild(play_menu);

    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
    return true;
}
void HelloWorld::menuPlay(Ref* pSender)
{
    //需要切换场景
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    float x = visibleSize.width / 2;
    float y = visibleSize.height / 2;
    Director::getInstance()->replaceScene(BackGround::createScene());

}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
