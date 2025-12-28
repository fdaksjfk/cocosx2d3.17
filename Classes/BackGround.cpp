// BackGround.cpp - 场景类实现
#include "BackGround.h"
#include "Poker.h"

USING_NS_CC;

// 重新生成一份背景，用在进入游戏后
Scene* BackGround::createScene() {
    return BackGround::create();
}

bool BackGround::init()
{
    if (!Scene::init()) {
        return false;
    }

    _backGround = Sprite::create("baunk.jpg");
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 检查背景是否加载成功
    if (!_backGround) {
        // 如果背景图片不存在，创建纯色背景
        _backGround = Sprite::create();
        auto drawNode = DrawNode::create();
        drawNode->drawSolidRect(origin,
            Vec2(origin.x + visibleSize.width, origin.y + visibleSize.height),
            Color4F(0.2f, 0.3f, 0.4f, 1.0f));
        _backGround->addChild(drawNode);
    }
    else {
        Size bgSize = _backGround->getContentSize();
        // 计算缩放比例，保持宽高比
        float scaleX = visibleSize.width / bgSize.width;
        float scaleY = visibleSize.height / bgSize.height;

        _backGround->setScaleX(scaleX);
        _backGround->setScaleY(scaleY);
    }

    _backGround->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2));

    this->addChild(_backGround, -1);

    // 创建回退按钮
    cardRebakeButton();

    // 初始化棋盘
    cardGame();

    // 游戏逻辑处理
    cardPlay();

    return true;
}

// 创建回退按钮
void BackGround::cardRebakeButton() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 创建回退按钮
    auto button = ui::Button::create();
    button->setTitleText(u8"回退");
    button->setTitleFontSize(24);
    button->setTitleColor(Color3B::WHITE);
    button->setContentSize(Size(100, 50));
    button->setColor(Color3B(0, 0, 0));

    // 添加边框
    auto border = DrawNode::create();
    border->drawRect(Vec2(-50, -25), Vec2(50, 25), Color4F(1.0f, 1.0f, 1.0f, 0.5f));
    border->setLineWidth(2.0f);
    button->addChild(border);

    button->setPosition(Vec2(visibleSize.width / 2, 150));

    // 添加点击事件
    button->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        auto btn = dynamic_cast<ui::Button*>(sender);
        if (!btn) return;

        switch (type) {
        case ui::Widget::TouchEventType::BEGAN:
            btn->setScale(0.95f);
            break;
        case ui::Widget::TouchEventType::ENDED:
            btn->setScale(1.0f);
            cardRebake();  // 执行回退
            break;
 
        default:
            break;
        }
        });

    // 创建"全部回退"按钮
    auto resetButton = ui::Button::create();
    resetButton->setTitleText(u8"重置");
    resetButton->setTitleFontSize(20);
    resetButton->setTitleColor(Color3B::WHITE);
    resetButton->setContentSize(Size(80, 40));
    resetButton->setColor(Color3B(0, 0, 0));
    resetButton->setPosition(Vec2(visibleSize.width / 2, 100));

    resetButton->addClickEventListener([this](Ref* sender) {
        cardRebakeAll();  // 回退到初始状态
        });

    // 添加按钮说明
    auto tipLabel = Label::createWithSystemFont(u8"点击撤销上一步", "Arial", 14);
    tipLabel->setColor(Color3B::YELLOW);
    tipLabel->setPosition(Vec2(visibleSize.width / 2, 180));

    auto resetTipLabel = Label::createWithSystemFont(u8"重置所有卡片", "Arial", 12);
    resetTipLabel->setColor(Color3B::YELLOW);
    resetTipLabel->setPosition(Vec2(visibleSize.width / 2, 130));

    this->addChild(button, 100);
    this->addChild(resetButton, 100);
    this->addChild(tipLabel, 100);
    this->addChild(resetTipLabel, 100);
}

// 执行回退
void BackGround::cardRebake() {

    if (!canUndo()) {
        showMessage(u8"没有可回退的操作", Color3B::RED);
        return;
    }

    bool success = undoLastMove();
    if (success) {
        showMessage(u8"回退成功", Color3B::GREEN);
        updateCardVisuals();
    }
    else {
        showMessage(u8"回退失败", Color3B::RED);
    }
}

// 回退到初始状态
void BackGround::cardRebakeAll() {

    // 清空回退栈
    clearUndoStack();

    // 恢复所有卡片到原始位置
    for (auto poker : _pokers) {
        if (poker) {
            poker->resetCard();
            poker->restoreFromDestroy();
            poker->setVisible(true);
        }
    }

    showMessage(u8"已重置到初始状态", Color3B::GREEN);
    updateCardVisuals();
}

// 清空卡片
void BackGround::clearPokers() {
    for (auto poker : _pokers) {
        if (poker) {
            poker->removeFromParentAndCleanup(true);
        }
    }
    _pokers.clear();
    clearUndoStack();
}

// 初始化棋盘
void BackGround::cardGame() {
    // 先清空现有的卡片
    clearPokers();

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 定义列数
    const int COLS = 3;
    // 每列卡片数量
    const int cardsPerColumn = 5;

    // 卡片尺寸（原始尺寸）
    const float CARD_WIDTH = 50.0f;
    const float CARD_HEIGHT = 70.0f;

    // 实际显示尺寸（考虑缩放0.5）
    const float DISPLAY_WIDTH = CARD_WIDTH * 0.5f;
    const float DISPLAY_HEIGHT = CARD_HEIGHT * 0.5f;

    float columnWidth = visibleSize.width / (COLS + 1);

    // 计算垂直位置
    float topMargin = visibleSize.height * 0.4f;
    float bottomMargin = visibleSize.height * 0.4f;
    float availableHeight = visibleSize.height - topMargin - bottomMargin;
    float verticalSpacing = (availableHeight - cardsPerColumn * DISPLAY_HEIGHT) / (cardsPerColumn + 1);

    for (int col = 0; col < COLS; col++) {
        float xPos = origin.x + (col + 1) * columnWidth;
        float startY = visibleSize.height - topMargin - verticalSpacing - DISPLAY_HEIGHT / 2;

        // 创建该列的所有卡片
        for (int i = 0; i < cardsPerColumn; i++) {
            // 计算卡片位置
            float yPos = startY - i * (DISPLAY_HEIGHT + verticalSpacing);

            Puke* card = Puke::create();
            if (card) {
                // 设置卡片位置
                card->setPosition(Vec2(xPos, yPos));

                // 记录原始位置
                card->setOriginalPosition(Vec2(xPos, yPos));

                // 设置卡片大小
                card->setScale(0.5);

                // 标记是否为底部卡片（这里不标记，只有底部的牌才标记）
                card->setIsBottomCard(false);

                // 添加到当前场景
                this->addChild(card);
                _pokers.push_back(card);

                // 设置ZOrder
                card->setLocalZOrder(1);
            }
        }
    }

    // 创建最下面一行特殊卡片
    createBottomRowCards();

    CCLOG("Card game initialized with %zu cards", _pokers.size());
}

void BackGround::createBottomRowCards() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 底部卡片的数量
    const int BOTTOM_ROW_CARD_COUNT = 7;

    // 卡片尺寸（考虑缩放0.5）
    const float CARD_WIDTH = 50.0f * 0.5f;
    const float CARD_HEIGHT = 70.0f * 0.5f;

    // 底部边距
    const float BOTTOM_MARGIN = 50.0f;

    // 水平边距
    const float HORIZONTAL_MARGIN = 30.0f;

    // 计算每张卡片的水平间距
    float availableWidth = visibleSize.width - 2 * HORIZONTAL_MARGIN - 300;
    float totalCardsWidth = BOTTOM_ROW_CARD_COUNT * CARD_WIDTH;
    float spacing = 0;

    if (BOTTOM_ROW_CARD_COUNT > 1) {
        spacing = (availableWidth - totalCardsWidth) / (BOTTOM_ROW_CARD_COUNT - 1);
    }

    // 计算起始X坐标（从左到右均匀分布）
    float startX = origin.x + HORIZONTAL_MARGIN + CARD_WIDTH / 2;

    // 计算Y坐标（屏幕底部）
    float bottomY = origin.y + BOTTOM_MARGIN + CARD_HEIGHT / 2;

    // 创建底部卡片
    for (int i = 0; i < BOTTOM_ROW_CARD_COUNT; i++) {
        // 计算卡片位置
        float xPos = startX + i * (CARD_WIDTH + spacing);

        // 创建卡片
        Puke* card = Puke::create();
        if (card) {
            // 设置卡片位置
            card->setPosition(Vec2(xPos, bottomY));

            // 记录原始位置
            card->setOriginalPosition(Vec2(xPos, bottomY));

            // 设置卡片大小
            card->setScale(0.5);

            // 标记为底部卡片（特殊规则）
            card->setIsBottomCard(true);

            // 添加金色边框标识
            auto border = DrawNode::create();
            border->drawRect(
                Vec2(-CARD_WIDTH / 2, -CARD_HEIGHT / 2),
                Vec2(CARD_WIDTH / 2, CARD_HEIGHT / 2),
                Color4F(1.0f, 0.84f, 0.0f, 0.8f)
            );
            border->setLineWidth(3.0f);
            card->addChild(border, 10);

            // 添加"底牌"文字标签
            auto label = Label::createWithSystemFont("底牌", "Arial", 12);
            label->setPosition(Vec2(0, -CARD_HEIGHT / 2 - 15));
            label->setColor(Color3B::YELLOW);
            card->addChild(label, 11);

            // 添加到当前场景
            this->addChild(card);

            // 添加到卡片列表
            _pokers.push_back(card);

            // 设置ZOrder（底部卡片应该在上方显示）
            card->setLocalZOrder(2);
        }
    }

    CCLOG("Created %d special bottom row cards", BOTTOM_ROW_CARD_COUNT);
}

// 游戏逻辑处理
void BackGround::cardPlay() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) {
        CCLOG("Screen touched at: (%.1f, %.1f)",
            touch->getLocation().x,
            touch->getLocation().y);
        return true;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 记录卡片移动
void BackGround::recordMove(Puke* card) {
    if (!card) return;

    UndoRecord record;
    record.card = card;
    record.originalPosition = card->getOriginalPosition();
    record.wasBottomCard = card->isBottomCard();
    record.wasDestroyed = false;

    _undoStack.push_back(record);
    CCLOG("记录卡片移动: %s", card->getCardString().c_str());
}

// 记录卡片被销毁
void BackGround::recordDestroy(Puke* card) {
    if (!card) return;

    UndoRecord record;
    record.card = card;
    record.originalPosition = card->getOriginalPosition();
    record.wasBottomCard = card->isBottomCard();
    record.wasDestroyed = true;

    _undoStack.push_back(record);
    CCLOG("记录卡片被销毁: %s", card->getCardString().c_str());
}

// 回退最后一步
bool BackGround::undoLastMove() {
    if (_undoStack.empty()) {
        CCLOG("回退栈为空，无法回退");
        return false;
    }

    UndoRecord record = _undoStack.back();
    _undoStack.pop_back();

    if (!record.card) {
        CCLOG("卡片指针为空，无法回退");
        return false;
    }

    if (record.wasDestroyed) {
        // 恢复被销毁的卡片
        CCLOG("恢复被销毁的卡片: %s", record.card->getCardString().c_str());
        record.card->restoreFromDestroy();
        record.card->setPosition(record.originalPosition);
        record.card->setScale(0.5f);
        record.card->setLocalZOrder(record.wasBottomCard ? 2 : 1);

        // 确保卡片在场景中
        if (record.card->getParent() == nullptr) {
            this->addChild(record.card);
        }
    }
    else {
        // 回退移动的卡片
        CCLOG("回退移动的卡片: %s", record.card->getCardString().c_str());
        record.card->resetCard();
    }

    CCLOG("回退完成，剩余回退记录: %zu", _undoStack.size());
    return true;
}

// 清空回退栈
void BackGround::clearUndoStack() {
    _undoStack.clear();
    CCLOG("回退栈已清空");
}

// 更新卡片视觉效果
void BackGround::updateCardVisuals() {
    // 更新所有卡片的Z轴顺序
    int normalZ = 1;
    int bottomZ = 2;
    int overlapZ = 100;

    Vec2 overlapPoint = Puke::getSharedOverlapPoint();

    for (auto poker : _pokers) {
        if (poker && poker->isVisible()) {
            // 计算与重叠点的距离
            float distance = poker->getPosition().distance(overlapPoint);

            if (poker->isBottomCard()) {
                poker->setLocalZOrder(bottomZ++);
            }
            else if (distance < 30.0f) {
                poker->setLocalZOrder(overlapZ++);
            }
            else {
                poker->setLocalZOrder(normalZ++);
            }
        }
    }

    CCLOG("更新了卡片视觉效果");
}

// 显示消息
void BackGround::showMessage(const string& message, const Color3B& color, float duration) {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithSystemFont(message, "Arial", 24);
    label->setColor(color);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2));
    this->addChild(label, 200);

    // 淡入淡出效果
    auto fadeIn = FadeIn::create(0.2f);
    auto delay = DelayTime::create(duration - 0.4f);
    auto fadeOut = FadeOut::create(0.2f);
    auto remove = RemoveSelf::create();

    label->runAction(Sequence::create(fadeIn, delay, fadeOut, remove, nullptr));
}