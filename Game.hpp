#ifndef GAME_HPP
#define GAME_HPP

#include "GameMap.hpp"
#include "Snake.hpp"
#include "ItemManager.hpp"
#include "GateManager.hpp"
#include "ColorManager.hpp"
#include "ScoreManager.hpp"
#include "StageManager.hpp"
#include <ncurses.h>
#include <memory>

class Game {
public:
    Game(int width, int height);
    ~Game();

    // 게임 상태
    bool isGameOver() const { return gameOver; }
    
    // 게임 객체 접근
    Snake& getSnake() { return snake; }
    const Snake& getSnake() const { return snake; }
    GameMap& getMap() { return map; }
    ScoreManager& getScoreManager() { return scoreManager; }
    ItemManager& getItemManager() { return itemManager; }
    const ItemManager& getItemManager() const { return itemManager; }
    GateManager& getGateManager() { return gateManager; }
    const GateManager& getGateManager() const { return gateManager; }
    StageManager& getStageManager() { return stageManager; }
    const StageManager& getStageManager() const { return stageManager; }

    // 속도 관리
    int getCurrentTickDuration() const { return currentTickDuration; }
    int getBaseTickDuration() const { return baseTickDuration; }
    int getSpeedBoostCount() const { return speedBoostCount; }
    void applySpeedBoost();
    void resetSpeed();

    // 게임 로직
    void update();
    void handleInput(int key);
    void draw();

    // 게임 루프
    void run();

private:
    GameMap map;
    Snake snake;
    ItemManager itemManager;
    GateManager gateManager;
    std::shared_ptr<ColorManager> colorManager;
    ScoreManager scoreManager;
    StageManager stageManager;
    bool gameOver;
    bool gameCompleted;

    // 속도 관리
    static const int baseTickDuration = 200;  // 기본 틱 지속시간 (ms)
    static const int minTickDuration = 50;    // 최소 틱 지속시간 (ms)
    int currentTickDuration;                  // 현재 틱 지속시간 (ms)
    int speedBoostCount;                      // 속도 부스트 횟수

    // 충돌 감지
    bool checkWallCollision() const;
    void updateMap();
    
    // 아이템 관련
    void handleItemCollision();
    
    // Gate 관련
    void handleGateCollision();
    
    // 점수 표시
    void drawScoreBoard();
    
    // 스테이지 관련
    void checkStageCompletion();
    void drawMissionInfo();
};

#endif // GAME_HPP 