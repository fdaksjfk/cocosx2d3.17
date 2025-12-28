// Poker.cpp - 卡牌类实现
#include "Poker.h"
#include <algorithm>

// 初始化静态变量
cocos2d::Vec2 Puke::_sharedOverlapPoint = cocos2d::Vec2::ZERO;
bool Puke::_isSharedOverlapPointSet = false;

// 获取场景中的BackGround实例的辅助函数
BackGround* Puke::getBackGround() {
    auto scene = Director::getInstance()->getRunningScene();
    // 这里使用dynamic_cast，但在Poker.cpp中需要包含BackGround.h
    // 由于循环包含问题，我们暂时不使用回退功能
    // 在后续版本中修复
    return nullptr;
}

int randomInt(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

int Puke::gatData() {
    return _data;
}

std::vector<std::string> Puke::coreatPukeFaseVector() {
    std::vector<std::string> ret;

    // 黑色牌 (0-12)
    ret.push_back("big_black_A.png");
    ret.push_back("big_black_2.png");
    ret.push_back("big_black_3.png");
    ret.push_back("big_black_4.png");
    ret.push_back("big_black_5.png");
    ret.push_back("big_black_6.png");
    ret.push_back("big_black_7.png");
    ret.push_back("big_black_8.png");
    ret.push_back("big_black_9.png");
    ret.push_back("big_black_10.png");
    ret.push_back("big_black_J.png");
    ret.push_back("big_black_Q.png");
    ret.push_back("big_black_K.png"); // 索引 12

    // 红色牌 (13-25)
    ret.push_back("big_red_A.png");
    ret.push_back("big_red_2.png");
    ret.push_back("big_red_3.png");
    ret.push_back("big_red_4.png");
    ret.push_back("big_red_5.png");
    ret.push_back("big_red_6.png");
    ret.push_back("big_red_7.png");
    ret.push_back("big_red_8.png");
    ret.push_back("big_red_9.png");
    ret.push_back("big_red_10.png");
    ret.push_back("big_red_J.png");
    ret.push_back("big_red_Q.png");
    ret.push_back("big_red_K.png"); // 索引 25

    // 花色 (26-29)
    ret.push_back("club.png");     // 梅花 - 黑色，索引26
    ret.push_back("spade.png");    // 黑桃 - 黑色，索引27
    ret.push_back("diamond.png");  // 方块 - 红色，索引28
    ret.push_back("heart.png");    // 红心 - 红色，索引29

    return ret;
}

// 随机函数
string Puke::randomNumb() {
    int tmp = randomInt(1, 2);
    vector<string> makecurd = coreatPukeFaseVector();
    if (tmp == 1) {
        // 当为1就为黑牌
        tmp = randomInt(0, 12);
        string ret = makecurd[tmp];
        _data = tmp;  // 0-12
        _numb = tmp;
        return ret;
    }
    else {
        tmp = randomInt(13, 25);
        string ret = makecurd[tmp];
        _data = tmp - 13;  // 转换为0-12
        _numb = tmp - 13;
        return ret;
    }
}

string Puke::randoSuits(string tmp) {
    vector<string> makecurd = coreatPukeFaseVector();
    int find;
    string ret;
    // 寻找到底是黑牌还是红牌
    for (find = 0; find < makecurd.size(); find++) {
        if (makecurd[find] == tmp) {
            break;
        }
    }
    if (find < 13) {
        // 黑牌，选择黑色花色（梅花或黑桃）
        int suitIndex = randomInt(26, 27);
        _data += (suitIndex == 26 ? 0 : 1) * 100;  // 用百位存储花色信息
        return makecurd[suitIndex];
    }
    else {
        // 红牌，选择红色花色（方块或红心）
        int suitIndex = randomInt(28, 29);
        _data += (suitIndex == 28 ? 2 : 3) * 100;  // 用百位存储花色信息
        return makecurd[suitIndex];
    }
    return ret;
}

bool Puke::init()
{
    if (!Node::init()) {
        return false;
    }

    // 初始化成员变量
    _isBottomCard = false;
    _isDestroyed = false;

    // 创建扑克牌精灵
    _puke = Sprite::create("card_general.png");
    this->addChild(_puke);
    Size textureSize = _puke->getContentSize();
    this->setContentSize(textureSize); // 将Puke节点的内容大小设置为图片大小

    // 添加花色和数字
    string numb = randomNumb();
    string su = randoSuits(numb);
    _suits = Sprite::create(su);
    _puke->addChild(_suits);
    _suits->setPosition(Vec2(25, 230));
    _number = Sprite::create(numb);
    _puke->addChild(_number);
    _number->setPosition(Vec2(85, 150));

    // 记录初始位置
    _originalPosition = Vec2::ZERO;

    // 为整个_puke添加触摸事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        // 如果卡片已被销毁，不响应触摸
        if (_isDestroyed) return false;

        // 获取触摸点在_puke的局部坐标
        Vec2 locationInNode = _puke->convertToNodeSpace(touch->getLocation());
        Size s = _puke->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);

        // 检测是否点击在扑克牌范围内
        if (rect.containsPoint(locationInNode)) {
            // 可以添加按下效果
            _puke->setScale(0.95f);
            return true;  // 吞噬触摸事件
        }
        return false;
        };

    listener->onTouchMoved = [](Touch* touch, Event* event) {
        // 移动时的处理
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        // 恢复原始大小
        _puke->setScale(1.0f);

        // 执行触摸逻辑
        this->cardTouchLogic();

        CCLOG("Puke clicked! Card: %s, IsBottom: %s",
            getCardString().c_str(),
            _isBottomCard ? "Yes" : "No");
        };

    listener->onTouchCancelled = [this](Touch* touch, Event* event) {
        // 触摸取消时恢复原始大小
        _puke->setScale(1.0f);
        };

    // 添加事件监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _puke);

    // 如果没有设置共享重叠点，设置为屏幕右下角
    if (!_isSharedOverlapPointSet) {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        float overlapX = origin.x + visibleSize.width - 60;
        float overlapY = origin.y + 60;
        setSharedOverlapPoint(Vec2(overlapX, overlapY));
    }

    return true;
}

void Puke::cardTouchLogic() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 设置右下角重叠点
    float overlapX = origin.x + visibleSize.width - 60;  // 距离右边60像素
    float overlapY = origin.y + 60;                      // 距离底部60像素

    cocos2d::Vec2 overlapPoint(overlapX, overlapY);

    // 设置锚点位置
    _anchorPosition = overlapPoint;

    // 创建一个静态计数器，记录当前最高的Z轴顺序
    static int highestZOrder = 1000;  // 从1000开始，确保在普通UI之上

    // 提高当前卡片的Z轴顺序，使其显示在最上面
    highestZOrder++;
    this->setLocalZOrder(highestZOrder);

    // 检查是否为底部卡片
    if (_isBottomCard) {
        // 底部卡片特殊处理：直接覆盖
        handleBottomCard();
    }
    else {
        // 普通卡片：先进行比较逻辑，返回是否允许移动
        bool canMove = compareWithOverlappedCards();

        if (canMove) {
            // 如果可以击败或者没有卡片，移动到重叠点
            moveToOverlapPoint();
        }
        else {
            // 如果不能击败，在原地抖动提示
            shakeCard();

            // 恢复原来的Z轴顺序（因为我们在点击时提高了Z轴顺序）
            this->setLocalZOrder(highestZOrder - 1);
        }
    }
}

// 底部卡片的特殊处理：直接覆盖
void Puke::handleBottomCard() {
    CCLOG("Bottom card clicked: %s. Will directly cover.", getCardString().c_str());

    // 获取重叠点上的最上面卡片
    Puke* topCard = getTopCoveredCard();

    // 如果有卡片在重叠点上，销毁它
    if (topCard) {
        destroyCardWithEffect(topCard);
    }

    // 移动到底部
    moveToOverlapPoint();
}

// 获取最上面覆盖的卡片
Puke* Puke::getTopCoveredCard() {
    auto parent = this->getParent();
    if (!parent) return nullptr;

    Vec2 overlapPoint = getSharedOverlapPoint();
    Puke* topCard = nullptr;
    int highestZ = -1;

    if (parent) {
        auto& children = parent->getChildren();
        Vec2 overlapPoint = getSharedOverlapPoint();

        for (auto child : children) {
            Puke* otherCard = dynamic_cast<Puke*>(child);
            if (otherCard && otherCard != this) {
                Vec2 otherPos = otherCard->getPosition();

                // 检查是否在重叠点附近（30像素内）
                if (otherPos.distance(overlapPoint) < 30.0f) {
                    if (otherCard->getLocalZOrder() > highestZ) {
                        highestZ = otherCard->getLocalZOrder();
                        topCard = otherCard;
                    }
                }
            }
        }
    }

    return topCard;
}

// 移动到重叠点
void Puke::moveToOverlapPoint() {
    // 停止所有正在运行的动作
    this->stopAllActions();

    // 创建移动动画
    auto move = cocos2d::MoveTo::create(0.5f, _anchorPosition);
    auto easeOut = cocos2d::EaseBackOut::create(move);

    // 同时旋转到0度，确保所有卡片方向一致
    auto rotate = cocos2d::RotateTo::create(0.5f, 0);

    // 确保所有卡片缩放一致
    auto scaleTo = cocos2d::ScaleTo::create(0.5f, 0.5f);

    // 对于底部卡片，可以添加一些特殊效果
    if (_isBottomCard) {
        // 底部卡片移动时闪烁金色光芒
        auto tint1 = cocos2d::TintTo::create(0.1f, 255, 215, 0);  // 金色
        auto tint2 = cocos2d::TintTo::create(0.1f, 255, 255, 255); // 恢复白色
        auto tintSequence = cocos2d::Sequence::create(tint1, tint2, tint1, tint2, nullptr);

        // 同时放大缩小，突出效果
        auto scaleUp = cocos2d::ScaleTo::create(0.2f, 0.55f);
        auto scaleDown = cocos2d::ScaleTo::create(0.2f, 0.5f);
        auto scaleSequence = cocos2d::Sequence::create(scaleUp, scaleDown, nullptr);

        // 组合动作
        auto spawn = cocos2d::Spawn::create(easeOut, rotate, scaleSequence, tintSequence, nullptr);
        this->runAction(spawn);

        CCLOG("Bottom card %s moving to overlap point with special effects.",
            getCardString().c_str());
    }
    else {
        // 普通卡片的移动
        auto spawn = cocos2d::Spawn::create(easeOut, rotate, scaleTo, nullptr);
        this->runAction(spawn);

        CCLOG("Card %s moving to overlap point", getCardString().c_str());
    }
}

// 在原地抖动提示不能移动
void Puke::shakeCard() {
    // 停止所有正在运行的动作
    this->stopAllActions();

    // 获取当前位置
    Vec2 originalPos = this->getPosition();

    // 创建抖动动画：左-右-左-回到原位
    auto moveLeft = MoveBy::create(0.05f, Vec2(-10, 0));
    auto moveRight = MoveBy::create(0.05f, Vec2(20, 0));
    auto moveLeft2 = MoveBy::create(0.05f, Vec2(-20, 0));
    auto moveRight2 = MoveBy::create(0.05f, Vec2(10, 0));

    // 添加颜色变化提示
    auto tintRed = TintTo::create(0.1f, 255, 100, 100);
    auto tintNormal = TintTo::create(0.1f, 255, 255, 255);

    // 组合动画
    auto moveSequence = Sequence::create(moveLeft, moveRight, moveLeft2, moveRight2, nullptr);
    auto colorSequence = Sequence::create(tintRed, tintNormal, nullptr);
    auto spawn = Spawn::create(moveSequence, colorSequence, nullptr);

    this->runAction(spawn);

}

// 设置共享的覆盖点
void Puke::setSharedOverlapPoint(const cocos2d::Vec2& point) {
    _sharedOverlapPoint = point;
    _isSharedOverlapPointSet = true;
}

// 获取共享的覆盖点
cocos2d::Vec2 Puke::getSharedOverlapPoint() {
    if (!_isSharedOverlapPointSet) {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        _sharedOverlapPoint = Vec2(origin.x + visibleSize.width - 60, origin.y + 60);
        _isSharedOverlapPointSet = true;
    }
    return _sharedOverlapPoint;
}

// 获取花色字符串
string Puke::getSuitString() const {
    int suitType = (_data / 100) % 4;  // 从百位提取花色信息

    switch (suitType) {
    case 0: return "Club";
    case 1: return "Spade";
    case 2: return "Diamond";
    case 3: return "Heart";
    default: return "Unknown";
    }
}

// 获取点数字符串
string Puke::getRankString() const {
    int rank = _data % 13;  // 取模13得到点数

    switch (rank) {
    case 0: return "A";
    case 1: return "2";
    case 2: return "3";
    case 3: return "4";
    case 4: return "5";
    case 5: return "6";
    case 6: return "7";
    case 7: return "8";
    case 8: return "9";
    case 9: return "10";
    case 10: return "J";
    case 11: return "Q";
    case 12: return "K";
    default: return "?";
    }
}

// 获取完整卡牌字符串
string Puke::getCardString() const {
    return getRankString() + " of " + getSuitString();
}
int Puke::getNumb()const {
    return _numb;
}

// 判断是否能击败对方卡片
bool Puke::canBeat(Puke* otherCard) {
    if (!otherCard) return false;

  
    if (_isBottomCard) {
        return true;
    }
    int myRank = _numb;  // 当前卡片的点数 (0-12)
    int otherRank = otherCard->getNumb();  // 对方卡片的点数

    // 点数差值为±1
    if (myRank + 1 == otherRank || myRank - 1 == otherRank) {
        return true;
    }
       

    return false;
}

// 比较与重叠卡片的逻辑，返回是否允许移动
bool Puke::compareWithOverlappedCards() {
    auto parent = this->getParent();
    if (!parent) return false;

    // 获取重叠点
    Vec2 overlapPoint = getSharedOverlapPoint();

    // 查找已经在重叠点的卡片
    vector<Puke*> overlappedCards;

    auto& children = parent->getChildren();
    for (auto child : children) {
        Puke* otherCard = dynamic_cast<Puke*>(child);
        if (otherCard && otherCard != this) {
            Vec2 otherPos = otherCard->getPosition();

            // 检查是否在重叠点附近（30像素内）
            if (otherPos.distance(overlapPoint) < 30.0f) {
                overlappedCards.push_back(otherCard);
            }
        }
    }

    // 如果没有重叠卡片，直接返回true（允许移动）
    if (overlappedCards.empty()) {
        CCLOG("No cards at overlap point. Can move.");
        return true;
    }

    // 获取最上面的卡片（ZOrder最高的）
    Puke* topCard = nullptr;
    int highestZ = -1;

    for (auto card : overlappedCards) {
        if (card->getLocalZOrder() > highestZ) {
            highestZ = card->getLocalZOrder();
            topCard = card;
        }
    }

    if (topCard) {
        // 比较当前卡片与最上面卡片
        CCLOG("Comparing: %s vs %s",
            getCardString().c_str(),
            topCard->getCardString().c_str());

        // 使用规则判断是否可以击败
        if (this->canBeat(topCard)) {
            CCLOG("%s beats %s! Can move.",
                getCardString().c_str(),
                topCard->getCardString().c_str());

            // 销毁下面的卡片
            destroyCardWithEffect(topCard);

            return true;  // 可以移动
        }
        else {
            CCLOG("%s cannot beat %s. Cannot move.",
                getCardString().c_str(),
                topCard->getCardString().c_str());

            return false;  // 不能移动
        }
    }

    return false;  // 默认不能移动
}

// 销毁卡片（带特效）
void Puke::destroyCardWithEffect(Node* card) {
    if (!card) return;

    Puke* pukeCard = dynamic_cast<Puke*>(card);
    if (pukeCard) {
        // 标记为已销毁
        pukeCard->_isDestroyed = true;

        // 创建销毁特效
        auto scaleUp = ScaleTo::create(0.1f, 1.2f);
        auto scaleDown = ScaleTo::create(0.2f, 0.1f);
        auto fadeOut = FadeOut::create(0.3f);

        auto spawn = Spawn::create(scaleDown, fadeOut, nullptr);
        auto hide = CallFunc::create([pukeCard]() {
            pukeCard->setVisible(false);
            });

        auto sequence = Sequence::create(scaleUp, spawn, hide, nullptr);
        card->runAction(sequence);
    }
}

// 从销毁状态恢复
void Puke::restoreFromDestroy() {
    if (_isDestroyed) {
        _isDestroyed = false;
        this->setVisible(true);
        this->setScale(0.5f);  // 恢复原始大小
        this->setOpacity(255);  // 恢复不透明度
        this->setRotation(0);   // 恢复旋转角度

        // 重新显示精灵
        if (_puke) _puke->setVisible(true);
        if (_suits) _suits->setVisible(true);
        if (_number) _number->setVisible(true);
    }
}

// 重置卡片状态
void Puke::resetCard() {
    this->stopAllActions();  // 停止所有动作
    this->setPosition(_originalPosition);
    this->setScale(0.5f);
    this->setRotation(0);
    this->setLocalZOrder(1);  // 恢复原始ZOrder

    // 恢复颜色
    if (_puke) {
        _puke->setColor(Color3B::WHITE);
    }
}