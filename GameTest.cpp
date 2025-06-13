#include <gtest/gtest.h>
#include "Game.hpp"

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