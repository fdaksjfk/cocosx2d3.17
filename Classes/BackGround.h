// BackGround.h - 场景类头文件
#pragma once
#ifndef __BACKGROUND_SCENE_H__
#define __BACKGROUND_SCENE_H__

#include "cocos2d.h"
#include <vector>
#include "Poker.h"

USING_NS_CC;
using namespace std;

class BackGround : public Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void cardGame();  // 初始化棋盘
    void cardPlay();  // 游戏逻辑处理
    void createBottomRowCards();  // 创建底部一行卡片

    // 回退功能
    void cardRebakeButton();  // 创建回退按钮
    void cardRebake();        // 执行回退
    void cardRebakeAll();     // 回退到初始状态

    // 添加获取和设置方法
    void addPoker(Puke* poker) { _pokers.push_back(poker); }
    vector<Puke*>& getPokers() { return _pokers; }
    void clearPokers();

    // 简单回退方法
    bool undoLastMove();  // 回退最后一步
    bool canUndo() const { return !_undoStack.empty(); }  // 是否可以回退
    void clearUndoStack();  // 清空回退栈

    // 辅助函数
    void updateCardVisuals();  // 更新卡片视觉效果
    void showMessage(const string& message, const Color3B& color, float duration = 2.0f);  // 显示消息

    CREATE_FUNC(BackGround);

private:
    Sprite* _backGround;
    vector<Puke*> _pokers;

    // 简单回退栈：存储卡片指针和原始位置
    struct UndoRecord {
        Puke* card;
        cocos2d::Vec2 originalPosition;
        bool wasBottomCard;
        bool wasDestroyed;
    };
    vector<UndoRecord> _undoStack;

    // 存储卡片被覆盖前的状态
    struct CoveredCardRecord {
        Puke* card;
        cocos2d::Vec2 position;
        bool isBottom;
    };
    vector<CoveredCardRecord> _coveredCards;

    // 记录卡片移动
    void recordMove(Puke* card);
    // 记录卡片被销毁
    void recordDestroy(Puke* card);

    // 初始状态备份
    void saveInitialState();
    // 恢复初始状态
    void restoreInitialState();
};

#endif // __BACKGROUND_SCENE_H__