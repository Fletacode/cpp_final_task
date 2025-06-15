#include <gtest/gtest.h>
#include "Game.hpp"
#include <iostream>

class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = new Game(31, 31);
    }

    void TearDown() override {
        delete game;
    }

    Game* game;
};

// 게임 초기화 테스트
TEST_F(GameTest, InitializationTest) {
    EXPECT_FALSE(game->isGameOver());
    EXPECT_EQ(game->getSnake().getLength(), 3);
    
    // ScoreManager 초기화 확인
    EXPECT_EQ(game->getScoreManager().getCurrentLength(), 3);
    EXPECT_EQ(game->getScoreManager().getMaxLength(), 3);
    EXPECT_EQ(game->getScoreManager().getGrowthItemsCollected(), 0);
    EXPECT_EQ(game->getScoreManager().getPoisonItemsCollected(), 0);
    EXPECT_EQ(game->getScoreManager().getGatesUsed(), 0);
}

// 벽 충돌 테스트
TEST_F(GameTest, WallCollisionTest) {
    // Snake를 맵 경계로 이동시켜 충돌 테스트
    Snake& snake = game->getSnake();
    
    // 왼쪽 벽으로 이동
    snake.setDirection(Direction::LEFT);
    for (int i = 0; i < 15; i++) {  // 충분히 이동하여 벽에 충돌
        game->update();
        if (game->isGameOver()) break;
    }
    EXPECT_TRUE(game->isGameOver());
}

// 키 입력 처리 테스트
TEST_F(GameTest, KeyInputTest) {
    Snake& snake = game->getSnake();
    Direction initialDirection = snake.getDirection();
    
    // 위쪽 키 입력
    game->handleInput(KEY_UP);
    EXPECT_EQ(snake.getDirection(), Direction::UP);
    
    // 아래쪽 키 입력 (반대 방향이므로 무시되어야 함)
    game->handleInput(KEY_DOWN);
    EXPECT_EQ(snake.getDirection(), Direction::UP);
    
    // 오른쪽 키 입력
    game->handleInput(KEY_RIGHT);
    EXPECT_EQ(snake.getDirection(), Direction::RIGHT);
}

// 게임 업데이트 테스트
TEST_F(GameTest, UpdateTest) {
    Snake& snake = game->getSnake();
    int initialX = snake.getHeadX();
    int initialY = snake.getHeadY();
    
    game->update();
    
    // Snake가 이동했는지 확인
    EXPECT_TRUE(snake.getHeadX() != initialX || snake.getHeadY() != initialY);
}

// 자기 자신과의 충돌로 인한 게임 오버 테스트
TEST_F(GameTest, SelfCollisionGameOverTest) {
    Snake& snake = game->getSnake();
    
    // 뱀을 길게 만들기
    snake.grow();
    snake.grow();
    snake.grow();
    
    // 자기 자신과 충돌하도록 이동
    snake.setDirection(Direction::UP);
    game->update();
    snake.setDirection(Direction::LEFT);
    game->update();
    snake.setDirection(Direction::DOWN);
    game->update();
    snake.setDirection(Direction::RIGHT);
    game->update();
    
    EXPECT_TRUE(game->isGameOver());
}

// ScoreManager 통합 테스트
TEST_F(GameTest, ScoreManagerIntegrationTest) {
    // 초기 점수 확인
    EXPECT_EQ(game->getScoreManager().getCurrentLength(), 3);
    EXPECT_EQ(game->getScoreManager().getGrowthItemsCollected(), 0);
    EXPECT_EQ(game->getScoreManager().getPoisonItemsCollected(), 0);
    EXPECT_EQ(game->getScoreManager().getGatesUsed(), 0);
    
    // 게임 업데이트 후에도 ScoreManager가 정상 작동하는지 확인
    game->update();
    EXPECT_GE(game->getScoreManager().getCurrentLength(), 3);
}

// 출구 게이트 충돌 시 게임 종료되지 않는 테스트
TEST_F(GameTest, NoGameOverOnExitGateCollisionTest) {
    // 게이트 생성을 위해 여러 번 업데이트
    for (int i = 0; i < 50; i++) {
        game->update();
        if (game->getGateManager().getGateCount() == 2) {
            break;
        }
    }
    
    if (game->getGateManager().getGateCount() == 2) {
        auto gates = game->getGateManager().getGates();
        
        // 출구 게이트 찾기
        Gate* exitGate = nullptr;
        for (auto& gate : gates) {
            if (gate.isExit()) {
                exitGate = &gate;
                break;
            }
        }
        
        if (exitGate != nullptr) {
            // 뱀을 출구 게이트 위치로 이동
            game->getSnake().teleportTo(exitGate->getPosition());
            
            // 게임이 종료되지 않았는지 확인
            EXPECT_FALSE(game->isGameOver());
            
            // 게임 업데이트 (충돌 처리)
            game->update();
            
            // 여전히 게임이 종료되지 않았는지 확인
            EXPECT_FALSE(game->isGameOver());
        }
    }
}

// 양방향 게이트 텔레포트 통합 테스트
TEST_F(GameTest, BidirectionalGateTeleportIntegrationTest) {
    // 게이트 생성을 위해 여러 번 업데이트
    for (int i = 0; i < 50; i++) {
        game->update();
        if (game->getGateManager().getGateCount() == 2) {
            break;
        }
    }
    
    if (game->getGateManager().getGateCount() == 2) {
        auto gates = game->getGateManager().getGates();
        Position originalSnakePos = game->getSnake().getHead();
        
        // 첫 번째 게이트로 뱀 이동
        game->getSnake().teleportTo(gates[0].getPosition());
        Position gate1Pos = game->getSnake().getHead();
        
        // 게임 업데이트 (텔레포트 처리)
        game->update();
        
        // 뱀이 다른 위치로 이동했는지 확인
        Position newSnakePos = game->getSnake().getHead();
        EXPECT_TRUE(newSnakePos != gate1Pos);
        
        // 게이트 사용 횟수가 증가했는지 확인
        EXPECT_GT(game->getScoreManager().getGatesUsed(), 0);
    }
}

// 스테이지 전환 시 뱀 초기화 테스트
TEST_F(GameTest, SnakeResetOnStageTransitionTest) {
    // 뱀을 변경: 성장, 이동, 방향 변경
    Snake& snake = game->getSnake();
    snake.grow();
    snake.grow();
    EXPECT_EQ(snake.getLength(), 5);  // 길이 증가
    
    snake.setDirection(Direction::UP);
    game->update();
    game->update();
    EXPECT_EQ(snake.getDirection(), Direction::UP);
    
    Position originalPos = snake.getHead();
    
    // 첫 번째 스테이지 미션 완료 (Growth Item 1개)
    game->getStageManager().updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
    
    // 스테이지 완료 확인
    EXPECT_TRUE(game->getStageManager().isCurrentStageCompleted());
    EXPECT_EQ(game->getStageManager().getCurrentStageNumber(), 1);
    
    // 게임 업데이트 (스테이지 전환 처리)
    game->update();
    
    // 다음 스테이지로 이동했는지 확인
    EXPECT_EQ(game->getStageManager().getCurrentStageNumber(), 2);
    
    // 뱀이 초기화되었는지 확인
    EXPECT_EQ(snake.getLength(), 3);  // 초기 길이로 복원
    EXPECT_EQ(snake.getDirection(), Direction::RIGHT);  // 초기 방향으로 복원
    
    // 뱀이 안전한 위치로 이동했는지 확인
    Position newPos = snake.getHead();
    EXPECT_TRUE(newPos != originalPos);  // 위치가 변경됨
    
    // 뱀의 새 위치가 맵 경계 내에 있는지 확인
    EXPECT_GE(newPos.x, 3);  // 최소 x 좌표 (뱀 길이 3 고려)
    EXPECT_LT(newPos.x, game->getMap().getWidth() - 1);
    EXPECT_GE(newPos.y, 1);  // 최소 y 좌표
    EXPECT_LT(newPos.y, game->getMap().getHeight() - 1);
}

// 게임 속도 관리 테스트
TEST_F(GameTest, GameSpeedManagementTest) {
    // 초기 속도 확인 (200ms)
    EXPECT_EQ(game->getCurrentTickDuration(), 200);
    
    // 기본 속도 확인
    EXPECT_EQ(game->getBaseTickDuration(), 200);
    
    // 속도 부스트 카운터 초기값 확인
    EXPECT_EQ(game->getSpeedBoostCount(), 0);
}

// SPEED 아이템 습득 시 속도 증가 테스트
TEST_F(GameTest, SpeedItemBoostTest) {
    // 초기 속도 확인
    EXPECT_EQ(game->getCurrentTickDuration(), 200);
    EXPECT_EQ(game->getSpeedBoostCount(), 0);
    
    // SPEED 아이템 1개 습득 시뮬레이션
    game->applySpeedBoost();
    
    // 속도가 증가했는지 확인 (200ms / 1.2 = 166ms)
    EXPECT_EQ(game->getCurrentTickDuration(), 166);
    EXPECT_EQ(game->getSpeedBoostCount(), 1);
    
    // SPEED 아이템 2개 습득 시뮬레이션
    game->applySpeedBoost();
    
    // 속도가 더 증가했는지 확인 (200ms / 1.4 = 142ms)
    EXPECT_EQ(game->getCurrentTickDuration(), 142);
    EXPECT_EQ(game->getSpeedBoostCount(), 2);
}

// 최대 속도 제한 테스트
TEST_F(GameTest, MaxSpeedLimitTest) {
    // 여러 번 속도 부스트 적용
    for (int i = 0; i < 10; ++i) {
        game->applySpeedBoost();
    }
    
    // 최소 틱 지속시간 50ms 이하로 내려가지 않는지 확인
    EXPECT_GE(game->getCurrentTickDuration(), 50);
}

// 스테이지 전환 시 속도 초기화 테스트
TEST_F(GameTest, SpeedResetOnStageTransitionTest) {
    // 속도 부스트 적용
    game->applySpeedBoost();
    game->applySpeedBoost();
    EXPECT_EQ(game->getCurrentTickDuration(), 142);
    EXPECT_EQ(game->getSpeedBoostCount(), 2);
    
    // 스테이지 전환 시뮬레이션
    game->resetSpeed();
    
    // 속도가 초기화되었는지 확인
    EXPECT_EQ(game->getCurrentTickDuration(), 200);
    EXPECT_EQ(game->getSpeedBoostCount(), 0);
}

// SPEED 아이템 습득 통합 테스트
TEST_F(GameTest, SpeedItemCollectionIntegrationTest) {
    // 초기 속도 확인
    EXPECT_EQ(game->getCurrentTickDuration(), 200);
    EXPECT_EQ(game->getSpeedBoostCount(), 0);
    
    // 뱀의 현재 위치와 방향 확인
    Position headPos = game->getSnake().getHead();
    Direction direction = game->getSnake().getDirection();
    std::cout << "Snake head position: (" << headPos.x << ", " << headPos.y << ")" << std::endl;
    std::cout << "Snake direction: " << static_cast<int>(direction) << std::endl;
    
    // 뱀이 이동할 다음 위치에 SPEED 아이템 배치
    Position nextPos = headPos;
    switch (direction) {
        case Direction::RIGHT:
            nextPos.x += 1;
            break;
        case Direction::LEFT:
            nextPos.x -= 1;
            break;
        case Direction::UP:
            nextPos.y -= 1;
            break;
        case Direction::DOWN:
            nextPos.y += 1;
            break;
    }
    
    std::cout << "Placing item at next position: (" << nextPos.x << ", " << nextPos.y << ")" << std::endl;
    
    game->getItemManager().addItem(nextPos.x, nextPos.y, ItemType::SPEED);
    std::cout << "Item count after adding: " << game->getItemManager().getItemCount() << std::endl;
    
    // 맵 업데이트 (아이템이 맵에 반영되도록)
    game->getItemManager().updateMap();
    
    // 게임 업데이트 (뱀 이동 및 아이템 충돌 처리)
    game->update();
    
    std::cout << "Item count after update: " << game->getItemManager().getItemCount() << std::endl;
    std::cout << "Speed boost count: " << game->getSpeedBoostCount() << std::endl;
    std::cout << "Current tick duration: " << game->getCurrentTickDuration() << std::endl;
    
    // 속도가 증가했는지 확인
    EXPECT_EQ(game->getCurrentTickDuration(), 166);  // 200ms / 1.2 = 166ms
    EXPECT_EQ(game->getSpeedBoostCount(), 1);
    
    // 아이템이 제거되었는지 확인 (새로 생성된 아이템 제외하고)
    EXPECT_LE(game->getItemManager().getItemCount(), 3);  // 최대 3개까지 가능
}

// 여러 SPEED 아이템 연속 습득 테스트
TEST_F(GameTest, MultipleSpeedItemsTest) {
    // 초기 상태 확인
    EXPECT_EQ(game->getCurrentTickDuration(), 200);
    
    // 첫 번째 SPEED 아이템 습득
    Position headPos = game->getSnake().getHead();
    Direction direction = game->getSnake().getDirection();
    
    // 뱀이 이동할 다음 위치에 아이템 배치
    Position nextPos = headPos;
    switch (direction) {
        case Direction::RIGHT: nextPos.x += 1; break;
        case Direction::LEFT: nextPos.x -= 1; break;
        case Direction::UP: nextPos.y -= 1; break;
        case Direction::DOWN: nextPos.y += 1; break;
    }
    
    game->getItemManager().addItem(nextPos.x, nextPos.y, ItemType::SPEED);
    game->getItemManager().updateMap();
    game->update();
    EXPECT_EQ(game->getCurrentTickDuration(), 166);
    
    // 두 번째 SPEED 아이템 습득
    headPos = game->getSnake().getHead();
    direction = game->getSnake().getDirection();
    nextPos = headPos;
    switch (direction) {
        case Direction::RIGHT: nextPos.x += 1; break;
        case Direction::LEFT: nextPos.x -= 1; break;
        case Direction::UP: nextPos.y -= 1; break;
        case Direction::DOWN: nextPos.y += 1; break;
    }
    
    game->getItemManager().addItem(nextPos.x, nextPos.y, ItemType::SPEED);
    game->getItemManager().updateMap();
    game->update();
    EXPECT_EQ(game->getCurrentTickDuration(), 142);  // 200ms / 1.4 = 142ms
    
    // 세 번째 SPEED 아이템 습득
    headPos = game->getSnake().getHead();
    direction = game->getSnake().getDirection();
    nextPos = headPos;
    switch (direction) {
        case Direction::RIGHT: nextPos.x += 1; break;
        case Direction::LEFT: nextPos.x -= 1; break;
        case Direction::UP: nextPos.y -= 1; break;
        case Direction::DOWN: nextPos.y += 1; break;
    }
    
    game->getItemManager().addItem(nextPos.x, nextPos.y, ItemType::SPEED);
    game->getItemManager().updateMap();
    game->update();
    EXPECT_EQ(game->getCurrentTickDuration(), 125);  // 200ms / 1.6 = 125ms
}

// TemporaryWallManager 통합 테스트
TEST_F(GameTest, TemporaryWallManagerIntegrationTest) {
    // Game 클래스에 TemporaryWallManager가 통합되어 있는지 확인
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 0);
    
    // Temporary Wall 추가
    Position pos(10, 10);
    auto lifetime = std::chrono::milliseconds(3000);
    game->getTemporaryWallManager().addTemporaryWall(pos, lifetime);
    
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 1);
    EXPECT_TRUE(game->getTemporaryWallManager().hasTemporaryWallAt(pos));
}

// Temporary Wall 충돌 테스트
TEST_F(GameTest, TemporaryWallCollisionTest) {
    // 뱀의 현재 위치 확인
    Position headPos = game->getSnake().getHead();
    Direction direction = game->getSnake().getDirection();
    
    // 뱀이 이동할 다음 위치에 Temporary Wall 배치
    Position nextPos = headPos;
    switch (direction) {
        case Direction::RIGHT: nextPos.x += 1; break;
        case Direction::LEFT: nextPos.x -= 1; break;
        case Direction::UP: nextPos.y -= 1; break;
        case Direction::DOWN: nextPos.y += 1; break;
    }
    
    // Temporary Wall 추가
    auto lifetime = std::chrono::milliseconds(5000);
    game->getTemporaryWallManager().addTemporaryWall(nextPos, lifetime);
    game->getTemporaryWallManager().updateMap();
    
    // 맵에 Temporary Wall이 반영되었는지 확인
    EXPECT_EQ(game->getMap().getCellValue(nextPos.x, nextPos.y), 9);
    
    // 게임 업데이트 (뱀이 Temporary Wall과 충돌해야 함)
    game->update();
    
    // 게임 오버 상태 확인
    EXPECT_TRUE(game->isGameOver());
}

// Temporary Wall 만료 테스트
TEST_F(GameTest, TemporaryWallExpirationIntegrationTest) {
    // 자동 생성 타이머를 미래로 설정하여 자동 생성 방지
    game->setLastTemporaryWallCreation(std::chrono::steady_clock::now());
    
    // 짧은 생존 시간의 Temporary Wall 추가
    Position pos(15, 15);
    auto shortLifetime = std::chrono::milliseconds(100);
    game->getTemporaryWallManager().addTemporaryWall(pos, shortLifetime);
    
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 1);
    
    // 150ms 대기 후 게임 업데이트
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    game->update();
    
    // Temporary Wall이 만료되어 제거되었는지 확인
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 0);
    EXPECT_FALSE(game->getTemporaryWallManager().hasTemporaryWallAt(pos));
}

// 스테이지 전환 시 Temporary Wall 초기화 테스트
TEST_F(GameTest, TemporaryWallResetOnStageTransitionTest) {
    // Temporary Wall 추가
    Position pos1(8, 8);
    Position pos2(12, 12);
    auto lifetime = std::chrono::milliseconds(10000);  // 긴 생존 시간
    
    game->getTemporaryWallManager().addTemporaryWall(pos1, lifetime);
    game->getTemporaryWallManager().addTemporaryWall(pos2, lifetime);
    
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 2);
    
    // 첫 번째 스테이지 미션 완료
    game->getStageManager().updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
    
    // 게임 업데이트 (스테이지 전환 처리)
    game->update();
    
    // 스테이지 전환 후 Temporary Wall이 초기화되었는지 확인
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 0);
    EXPECT_FALSE(game->getTemporaryWallManager().hasTemporaryWallAt(pos1));
    EXPECT_FALSE(game->getTemporaryWallManager().hasTemporaryWallAt(pos2));
}

// 자동 Temporary Wall 생성 타이머 테스트
TEST_F(GameTest, AutoTemporaryWallCreationTimerTest) {
    // 초기 상태 확인
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 0);
    
    // 타이머 설정 확인 (20초 간격)
    EXPECT_EQ(game->getTemporaryWallCreationInterval(), 20000);
    
    // 마지막 생성 시간이 초기화되어 있는지 확인
    auto now = std::chrono::steady_clock::now();
    auto lastCreation = game->getLastTemporaryWallCreation();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCreation);
    
    // 게임 시작 시점에서는 충분한 시간이 지났다고 가정 (즉시 생성 가능)
    EXPECT_GE(elapsed.count(), 20000);
}

// 자동 Temporary Wall 생성 기능 테스트
TEST_F(GameTest, AutoTemporaryWallCreationTest) {
    // 초기 상태 확인
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 0);
    
    // 강제로 자동 생성 트리거
    game->createRandomTemporaryWalls();
    
    // Temporary Wall이 생성되었는지 확인 (2개 생성 예상)
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 2);
}

// 자동 생성된 Temporary Wall 위치 검증 테스트
TEST_F(GameTest, AutoTemporaryWallPositionValidationTest) {
    Position snakeHead = game->getSnake().getHead();
    
    // 자동 생성 실행
    game->createRandomTemporaryWalls();
    
    // 맵 업데이트 (Temporary Wall이 맵에 반영되도록)
    game->getTemporaryWallManager().updateMap();
    
    // 생성된 모든 Temporary Wall이 뱀과 최소 거리를 유지하는지 확인
    const auto& tempWalls = game->getTemporaryWallManager().getTemporaryWalls();
    for (const auto& wall : tempWalls) {
        Position wallPos = wall.getPosition();
        
        // 뱀 머리와의 거리 계산 (맨하탄 거리)
        int distance = abs(wallPos.x - snakeHead.x) + abs(wallPos.y - snakeHead.y);
        EXPECT_GE(distance, 3);  // 최소 3칸 이상 떨어져 있어야 함
        
        // 유효한 위치인지 확인
        EXPECT_TRUE(game->getMap().isValidPosition(wallPos.x, wallPos.y));
        
        // 맵에 Temporary Wall이 반영되었는지 확인
        EXPECT_EQ(game->getMap().getCellValue(wallPos.x, wallPos.y), 9);
    }
}

// 주기적 자동 생성 통합 테스트
TEST_F(GameTest, PeriodicAutoCreationIntegrationTest) {
    // 초기 상태
    EXPECT_EQ(game->getTemporaryWallManager().getTemporaryWallCount(), 0);
    
    // 타이머를 과거로 설정하여 즉시 생성 조건 만족
    game->setLastTemporaryWallCreation(
        std::chrono::steady_clock::now() - std::chrono::milliseconds(21000)
    );
    
    // 게임 업데이트 (자동 생성 트리거)
    game->update();
    
    // Temporary Wall이 자동 생성되었는지 확인
    EXPECT_GT(game->getTemporaryWallManager().getTemporaryWallCount(), 0);
    
    // 마지막 생성 시간이 업데이트되었는지 확인
    auto now = std::chrono::steady_clock::now();
    auto lastCreation = game->getLastTemporaryWallCreation();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCreation);
    
    // 방금 생성되었으므로 경과 시간이 짧아야 함
    EXPECT_LT(elapsed.count(), 1000);  // 1초 미만
}

// 맵에 T가 실제로 표시되는지 확인하는 테스트
TEST_F(GameTest, TemporaryWallMapDisplayTest) {
    // 자동 생성 타이머를 과거로 설정
    game->setLastTemporaryWallCreation(
        std::chrono::steady_clock::now() - std::chrono::milliseconds(21000)
    );
    
    // 게임 업데이트 (자동 생성 + 맵 업데이트)
    game->update();
    
    // Temporary Wall이 생성되었는지 확인
    int tempWallCount = game->getTemporaryWallManager().getTemporaryWallCount();
    EXPECT_GT(tempWallCount, 0);
    
    // 맵에서 값 9(Temporary Wall)를 가진 셀이 있는지 확인
    int foundTempWalls = 0;
    for (int y = 1; y < game->getMap().getHeight() - 1; y++) {
        for (int x = 1; x < game->getMap().getWidth() - 1; x++) {
            if (game->getMap().getCellValue(x, y) == 9) {
                foundTempWalls++;
            }
        }
    }
    
    // 생성된 Temporary Wall 개수와 맵에 표시된 개수가 일치해야 함
    EXPECT_EQ(foundTempWalls, tempWallCount);
    EXPECT_GT(foundTempWalls, 0);
}

// 생존시간 초기화 테스트
TEST_F(GameTest, SurvivalTimeInitializationTest) {
    // 게임 생성 직후 생존시간은 0초여야 함
    EXPECT_EQ(game->getScoreManager().getSurvivalTimeSeconds(), 0);
    
    // 포맷된 시간도 00:00이어야 함
    EXPECT_EQ(game->getScoreManager().getFormattedSurvivalTime(), "00:00");
}

// 생존시간 진행 테스트
TEST_F(GameTest, SurvivalTimeProgressTest) {
    // 게임 시작 시간을 과거로 설정 (3초 전)
    auto pastTime = std::chrono::steady_clock::now() - std::chrono::seconds(3);
    game->getScoreManager().setGameStartTime(pastTime);
    
    // 생존시간이 3초 정도여야 함 (약간의 오차 허용)
    int survivalTime = game->getScoreManager().getSurvivalTimeSeconds();
    EXPECT_GE(survivalTime, 2);  // 최소 2초
    EXPECT_LE(survivalTime, 4);  // 최대 4초 (오차 고려)
    
    // 포맷된 시간 확인
    std::string formatted = game->getScoreManager().getFormattedSurvivalTime();
    EXPECT_TRUE(formatted.find("00:0") == 0);  // 00:0X 형식이어야 함
}

// 스테이지 전환 시 생존시간 유지 테스트
TEST_F(GameTest, SurvivalTimeStageTransitionTest) {
    // 게임 시작 시간을 과거로 설정
    auto pastTime = std::chrono::steady_clock::now() - std::chrono::seconds(5);
    game->getScoreManager().setGameStartTime(pastTime);
    
    // 초기 생존시간 확인
    int initialSurvivalTime = game->getScoreManager().getSurvivalTimeSeconds();
    EXPECT_GT(initialSurvivalTime, 0);
    
    // 스테이지별 카운터 리셋 (생존시간은 유지되어야 함)
    game->getScoreManager().resetStageSpecificCounters();
    
    // 생존시간이 유지되는지 확인 (약간의 시간 경과 허용)
    int afterResetSurvivalTime = game->getScoreManager().getSurvivalTimeSeconds();
    EXPECT_GE(afterResetSurvivalTime, initialSurvivalTime);
    EXPECT_LE(afterResetSurvivalTime, initialSurvivalTime + 2);  // 최대 2초 차이 허용
} 