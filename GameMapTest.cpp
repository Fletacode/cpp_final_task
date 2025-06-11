#include <gtest/gtest.h>
#include "GameMap.hpp"

class GameMapTest : public ::testing::Test {
protected:
    void SetUp() override {
        map = new GameMap(21, 21);  // 최소 크기 21x21
    }

    void TearDown() override {
        delete map;
    }

    GameMap* map;
};

// 맵 크기 테스트
TEST_F(GameMapTest, MapSizeTest) {
    EXPECT_EQ(map->getWidth(), 21);
    EXPECT_EQ(map->getHeight(), 21);
}

// 맵 초기화 테스트
TEST_F(GameMapTest, MapInitializationTest) {
    // 모든 테두리가 Immune Wall(2)인지 확인
    for (int i = 0; i < 21; i++) {
        EXPECT_EQ(map->getCellValue(i, 0), 2);  // 상단 벽
        EXPECT_EQ(map->getCellValue(i, 20), 2); // 하단 벽
        EXPECT_EQ(map->getCellValue(0, i), 2);  // 좌측 벽
        EXPECT_EQ(map->getCellValue(20, i), 2); // 우측 벽
    }
}

// Wall 설정 테스트
TEST_F(GameMapTest, WallSettingTest) {
    map->setWall(10, 10);
    EXPECT_EQ(map->getCellValue(10, 10), 1);
}

// Snake 위치 설정 테스트
TEST_F(GameMapTest, SnakePositionTest) {
    // Snake 머리 설정
    map->setSnakeHead(5, 5);
    EXPECT_EQ(map->getCellValue(5, 5), 3);

    // Snake 몸통 설정
    map->setSnakeBody(6, 5);
    EXPECT_EQ(map->getCellValue(6, 5), 4);
}

// 맵 경계 검사 테스트
TEST_F(GameMapTest, BoundaryCheckTest) {
    EXPECT_FALSE(map->isValidPosition(-1, 5));
    EXPECT_FALSE(map->isValidPosition(5, -1));
    EXPECT_FALSE(map->isValidPosition(21, 5));
    EXPECT_FALSE(map->isValidPosition(5, 21));
    EXPECT_TRUE(map->isValidPosition(5, 5));
} 