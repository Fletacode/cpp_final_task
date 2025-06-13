#include <gtest/gtest.h>
#include "Snake.hpp"

class SnakeTest : public ::testing::Test {
protected:
    void SetUp() override {
        snake = new Snake(10, 10);  // 초기 위치 (10, 10)
    }

    void TearDown() override {
        delete snake;
    }

    Snake* snake;
};

// Snake 초기화 테스트
TEST_F(SnakeTest, InitializationTest) {
    EXPECT_EQ(snake->getHeadX(), 10);
    EXPECT_EQ(snake->getHeadY(), 10);
    EXPECT_EQ(snake->getLength(), 3);  // 초기 길이 3
    EXPECT_EQ(snake->getDirection(), Direction::RIGHT);  // 초기 방향 오른쪽
}

// Snake 이동 테스트
TEST_F(SnakeTest, MoveTest) {
    snake->move();
    EXPECT_EQ(snake->getHeadX(), 11);  // 오른쪽으로 이동
    EXPECT_EQ(snake->getHeadY(), 10);
}

// 방향 변경 테스트
TEST_F(SnakeTest, DirectionChangeTest) {
    snake->setDirection(Direction::UP);
    EXPECT_EQ(snake->getDirection(), Direction::UP);
    
    snake->move();
    EXPECT_EQ(snake->getHeadX(), 10);
    EXPECT_EQ(snake->getHeadY(), 9);  // 위로 이동
}

// 반대 방향 이동 제한 테스트
TEST_F(SnakeTest, OppositeDirectionRestrictionTest) {
    // 현재 방향이 RIGHT일 때 LEFT로 변경 시도
    snake->setDirection(Direction::LEFT);
    EXPECT_EQ(snake->getDirection(), Direction::RIGHT);  // 변경되지 않음
    
    // UP으로 변경 후 DOWN으로 변경 시도
    snake->setDirection(Direction::UP);
    EXPECT_EQ(snake->getDirection(), Direction::UP);
    snake->setDirection(Direction::DOWN);
    EXPECT_EQ(snake->getDirection(), Direction::UP);  // 변경되지 않음
}

// 자기 자신과의 충돌 테스트
TEST_F(SnakeTest, SelfCollisionTest) {
    // 뱀을 충분히 길게 만들기
    for (int i = 0; i < 10; i++) {
        snake->grow();
    }
    
    // 간단한 U턴으로 자기 자신과 충돌
    snake->setDirection(Direction::UP);
    snake->move();  // 위로 이동
    snake->setDirection(Direction::LEFT);
    snake->move();  // 왼쪽으로 이동
    snake->setDirection(Direction::DOWN);
    snake->move();  // 아래로 이동 - 이제 몸통과 충돌해야 함
    
    EXPECT_TRUE(snake->checkSelfCollision());
}

// Snake 성장 테스트
TEST_F(SnakeTest, GrowthTest) {
    int initialLength = snake->getLength();
    snake->grow();
    EXPECT_EQ(snake->getLength(), initialLength + 1);
}

// Snake 몸통 위치 테스트
TEST_F(SnakeTest, BodyPositionTest) {
    auto body = snake->getBody();
    EXPECT_EQ(body.size(), 3);  // 초기 길이 3
    
    // 초기 위치: 머리(10,10), 몸통(9,10), 꼬리(8,10)
    EXPECT_EQ(body[0].x, 10);
    EXPECT_EQ(body[0].y, 10);
    EXPECT_EQ(body[1].x, 9);
    EXPECT_EQ(body[1].y, 10);
    EXPECT_EQ(body[2].x, 8);
    EXPECT_EQ(body[2].y, 10);
}

// Snake 리셋 테스트
TEST_F(SnakeTest, ResetTest) {
    // 뱀을 변경: 성장, 이동, 방향 변경
    snake->grow();
    snake->grow();
    EXPECT_EQ(snake->getLength(), 5);  // 길이 증가
    
    snake->setDirection(Direction::UP);
    snake->move();
    snake->move();
    EXPECT_EQ(snake->getDirection(), Direction::UP);
    EXPECT_EQ(snake->getHeadX(), 10);
    EXPECT_EQ(snake->getHeadY(), 8);  // 위로 2칸 이동
    
    // 새로운 위치로 리셋
    snake->reset(5, 5);
    
    // 리셋 후 상태 확인
    EXPECT_EQ(snake->getLength(), 3);  // 초기 길이로 복원
    EXPECT_EQ(snake->getDirection(), Direction::RIGHT);  // 초기 방향으로 복원
    EXPECT_EQ(snake->getHeadX(), 5);
    EXPECT_EQ(snake->getHeadY(), 5);
    
    // 몸통 위치도 올바르게 설정되었는지 확인
    auto body = snake->getBody();
    EXPECT_EQ(body[0].x, 5);  // 머리
    EXPECT_EQ(body[0].y, 5);
    EXPECT_EQ(body[1].x, 4);  // 몸통
    EXPECT_EQ(body[1].y, 5);
    EXPECT_EQ(body[2].x, 3);  // 꼬리
    EXPECT_EQ(body[2].y, 5);
} 