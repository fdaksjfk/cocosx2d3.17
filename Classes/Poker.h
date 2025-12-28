// Poker.h - 卡牌类头文件
#pragma once
#ifndef __PUKE_SCENE_H__
#define __PUKE_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <vector>
#include <string>
#include <random>

USING_NS_CC;
using namespace ui;
using namespace std;

// 操作类型枚举
enum class CardActionType {
    MOVE,      // 移动卡片
    DESTROY,   // 销毁卡片
    COVER      // 覆盖卡片（底牌覆盖其他牌）
};

// 前置声明，避免循环包含
class BackGround;

// 单步操作记录（简化版，不在Poker.h中完整定义）
struct CardAction {
    CardActionType type;
    void* card;  // 使用void*避免具体类型
    cocos2d::Vec2 fromPosition;
    cocos2d::Vec2 toPosition;
    bool wasBottomCard;
    int cardData;
    void* targetCard;
};

class Puke : public Node
{
public:
    virtual bool init();
    void cardTouchLogic();
    std::vector<std::string> coreatPukeFaseVector();
    string randomNumb();  // 随机数字
    string randoSuits(string tmp);  // 随机花色
    int gatData();

    // 新增的比较相关函数
    bool compareWithOverlappedCards();  // 返回比较结果，true=可以移动，false=不能移动
    bool canBeat(Puke* otherCard);      // 判断是否能击败对方卡片
    void destroyCardWithEffect(Node* card);  // 销毁卡片（带特效）

    // 获取卡片信息
    int getRank() const { return _data % 13; }  // 点数 (0=A, 1=2, ..., 12=K)
    bool isRed() const { return _data >= 13; }  // 是否是红色牌
    bool isBlack() const { return _data < 13; } // 是否是黑色牌
    int getColorIndex() const { return (_data < 13) ? 0 : 1; }  // 颜色索引 0=黑, 1=红
    string getSuitString() const;  // 获取花色字符串
    string getRankString() const;  // 获取点数字符串
    string getCardString() const;  // 获取完整卡牌字符串
    int getNumb()const;
    // 设置卡片是否为底部卡片
    void setIsBottomCard(bool isBottom) { _isBottomCard = isBottom; }
    bool isBottomCard() const { return _isBottomCard; }

    // 设置重叠点
    static void setSharedOverlapPoint(const cocos2d::Vec2& point);
    static cocos2d::Vec2 getSharedOverlapPoint();

    // 回退相关方法
    void setOriginalPosition(const cocos2d::Vec2& pos) { _originalPosition = pos; }
    cocos2d::Vec2 getOriginalPosition() const { return _originalPosition; }
    int getCardData() const { return _data; }
    void setCardData(int data) { _data = data; }
    void resetCard();  // 重置卡片状态
    void restoreFromDestroy();  // 从销毁状态恢复
    void setDestroyed(bool destroyed) { _isDestroyed = destroyed; }
    bool isDestroyed() const { return _isDestroyed; }

    CREATE_FUNC(Puke);

private:
    int _data;            // 卡片数据 (0-12:黑色, 13-25:红色)
    int _numb;
    Sprite* _puke;
    Sprite* _suits;
    Sprite* _number;
    Vec2 _anchorPosition;
    Vec2 _originalPosition;  // 原始位置，用于回退
    float _scaleFactor;
    bool _isBottomCard;   // 是否为底部卡片（特殊规则）
    bool _isDestroyed;    // 是否已被销毁

    // 静态变量：共享重叠点
    static cocos2d::Vec2 _sharedOverlapPoint;
    static bool _isSharedOverlapPointSet;

    // 移动卡片到重叠点
    void moveToOverlapPoint();

    // 在原地抖动提示不能移动
    void shakeCard();

    // 底部卡片的特殊处理
    void handleBottomCard();

    // 获取最上面覆盖的卡片
    Puke* getTopCoveredCard();

    // 获取场景中的BackGround实例
    BackGround* getBackGround();
};

#endif // __PUKE_SCENE_H__