#ifndef GAME_HPP
#define GAME_HPP

#include "GameMap.hpp"
#include "Snake.hpp"
#include "ItemManager.hpp"
#include "GateManager.hpp"
#include "TemporaryWallManager.hpp"
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
    TemporaryWallManager& getTemporaryWallManager() { return temporaryWallManager; }
    const TemporaryWallManager& getTemporaryWallManager() const { return temporaryWallManager; }
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
    
    // Temporary Wall 관련
    void createTemporaryWallAroundSnake();
    void createRandomTemporaryWalls();
    
    // 자동 생성 타이머 관련
    std::chrono::steady_clock::time_point getLastTemporaryWallCreation() const { return lastTemporaryWallCreation; }
    void setLastTemporaryWallCreation(std::chrono::steady_clock::time_point time) { lastTemporaryWallCreation = time; }
    int getTemporaryWallCreationInterval() const { return temporaryWallCreationInterval.count(); }

private:
    GameMap map;
    Snake snake;
    ItemManager itemManager;
    GateManager gateManager;
    TemporaryWallManager temporaryWallManager;
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
    
    // 자동 Temporary Wall 생성 관련
    std::chrono::steady_clock::time_point lastTemporaryWallCreation;
    std::chrono::milliseconds temporaryWallCreationInterval;

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
    
    // Temporary Wall 관련 (private)
    void checkTemporaryWallCreation();
};

#endif // GAME_HPP 