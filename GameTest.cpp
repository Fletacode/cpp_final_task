#include <gtest/gtest.h>
#include "Game.hpp"

class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = new Game(21, 21);
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