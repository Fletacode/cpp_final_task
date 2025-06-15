#include <gtest/gtest.h>
#include "GameMap.hpp"

class GameMapTest : public ::testing::Test {
protected:
    void SetUp() override {
        map = new GameMap(31, 31);  // 맵 크기 31x31
    }

    void TearDown() override {
        delete map;
    }

    GameMap* map;
};

// 맵 크기 테스트
TEST_F(GameMapTest, MapSizeTest) {
    EXPECT_EQ(map->getWidth(), 31);
    EXPECT_EQ(map->getHeight(), 31);
}

// 맵 초기화 테스트
TEST_F(GameMapTest, MapInitializationTest) {
    // 모든 테두리가 Immune Wall(2)인지 확인
    for (int i = 0; i < 31; i++) {
        EXPECT_EQ(map->getCellValue(i, 0), 2);  // 상단 벽
        EXPECT_EQ(map->getCellValue(i, 30), 2); // 하단 벽
        EXPECT_EQ(map->getCellValue(0, i), 2);  // 좌측 벽
        EXPECT_EQ(map->getCellValue(30, i), 2); // 우측 벽
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
    EXPECT_FALSE(map->isValidPosition(31, 5));
    EXPECT_FALSE(map->isValidPosition(5, 31));
    EXPECT_TRUE(map->isValidPosition(5, 5));
}

// 안전한 위치 찾기 테스트
TEST_F(GameMapTest, FindSafePositionTest) {
    // 빈 맵에서 안전한 위치 찾기
    auto safePos = map->findSafePosition();
    EXPECT_TRUE(safePos.has_value());
    
    if (safePos.has_value()) {
        int x = safePos.value().first;
        int y = safePos.value().second;
        
        // 안전한 위치는 경계에서 적절히 떨어져 있어야 함 (뱀 길이 3 고려)
        EXPECT_GE(x, 3);
        EXPECT_LE(x, 29);  // 31 - 1 - 1
        EXPECT_GE(y, 1);
        EXPECT_LE(y, 29);  // 31 - 1 - 1
        
        // 해당 위치와 오른쪽으로 2칸이 모두 빈 공간이어야 함
        EXPECT_EQ(map->getCellValue(x, y), 0);      // 머리 위치
        EXPECT_EQ(map->getCellValue(x-1, y), 0);    // 몸통 위치
        EXPECT_EQ(map->getCellValue(x-2, y), 0);    // 꼬리 위치
    }
}

// 벽이 있는 맵에서 안전한 위치 찾기 테스트
TEST_F(GameMapTest, FindSafePositionWithWallsTest) {
    // 맵 중앙에 벽 설치
    for (int i = 8; i <= 12; i++) {
        map->setWall(i, 10);
        map->setWall(10, i);
    }
    
    auto safePos = map->findSafePosition();
    EXPECT_TRUE(safePos.has_value());
    
    if (safePos.has_value()) {
        int x = safePos.value().first;
        int y = safePos.value().second;
        
        // 안전한 위치 조건 확인
        EXPECT_EQ(map->getCellValue(x, y), 0);      // 머리 위치
        EXPECT_EQ(map->getCellValue(x-1, y), 0);    // 몸통 위치
        EXPECT_EQ(map->getCellValue(x-2, y), 0);    // 꼬리 위치
    }
}

// 안전한 위치가 없는 경우 테스트
TEST_F(GameMapTest, NoSafePositionTest) {
    // 맵을 거의 모든 곳에 벽으로 채움 (안전한 위치가 없도록)
    for (int x = 1; x < 30; x++) {
        for (int y = 1; y < 30; y++) {
            map->setWall(x, y);
        }
    }
    
    auto safePos = map->findSafePosition();
    EXPECT_FALSE(safePos.has_value());
}

// SPEED 아이템 표시 테스트
TEST_F(GameMapTest, SpeedItemDisplayTest) {
    // SPEED 아이템은 맵 값 8로 설정되어야 함
    map->setCellValue(10, 10, 8);
    EXPECT_EQ(map->getCellValue(10, 10), 8);
    
    // 다른 아이템들과 구분되는지 확인
    map->setCellValue(5, 5, 5);   // Growth Item
    map->setCellValue(6, 6, 6);   // Poison Item
    map->setCellValue(7, 7, 7);   // Gate
    map->setCellValue(8, 8, 8);   // Speed Item
    
    EXPECT_EQ(map->getCellValue(5, 5), 5);
    EXPECT_EQ(map->getCellValue(6, 6), 6);
    EXPECT_EQ(map->getCellValue(7, 7), 7);
    EXPECT_EQ(map->getCellValue(8, 8), 8);
}

// Temporary Wall 표시 테스트
TEST_F(GameMapTest, TemporaryWallDisplayTest) {
    // Temporary Wall은 맵 값 9로 설정되어야 함
    map->setCellValue(12, 12, 9);
    EXPECT_EQ(map->getCellValue(12, 12), 9);
    
    // 다른 벽 타입들과 구분되는지 확인
    map->setCellValue(10, 10, 1);   // Wall
    map->setCellValue(11, 11, 2);   // Immune Wall
    map->setCellValue(12, 12, 9);   // Temporary Wall
    
    EXPECT_EQ(map->getCellValue(10, 10), 1);
    EXPECT_EQ(map->getCellValue(11, 11), 2);
    EXPECT_EQ(map->getCellValue(12, 12), 9);
}

// Temporary Wall 설정 메서드 테스트
TEST_F(GameMapTest, TemporaryWallSetterTest) {
    // setTemporaryWall 메서드가 있어야 함
    map->setTemporaryWall(15, 15);
    EXPECT_EQ(map->getCellValue(15, 15), 9);
    
    // 여러 위치에 Temporary Wall 설정
    map->setTemporaryWall(5, 10);
    map->setTemporaryWall(20, 25);
    
    EXPECT_EQ(map->getCellValue(5, 10), 9);
    EXPECT_EQ(map->getCellValue(20, 25), 9);
} 