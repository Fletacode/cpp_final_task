#include <gtest/gtest.h>
#include "TemporaryWallManager.hpp"
#include "GameMap.hpp"
#include <chrono>
#include <thread>

class TemporaryWallManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        map = std::make_unique<GameMap>(31, 31);
        manager = std::make_unique<TemporaryWallManager>(*map);
    }

    void TearDown() override {
        // unique_ptr이 자동으로 메모리 해제
    }

    std::unique_ptr<GameMap> map;
    std::unique_ptr<TemporaryWallManager> manager;
};

// TemporaryWallManager 생성 테스트
TEST_F(TemporaryWallManagerTest, ManagerCreationTest) {
    EXPECT_EQ(manager->getTemporaryWallCount(), 0);
}

// Temporary Wall 추가 테스트
TEST_F(TemporaryWallManagerTest, AddTemporaryWallTest) {
    Position pos(10, 15);
    auto lifetime = std::chrono::milliseconds(3000);
    
    manager->addTemporaryWall(pos, lifetime);
    
    EXPECT_EQ(manager->getTemporaryWallCount(), 1);
    EXPECT_TRUE(manager->hasTemporaryWallAt(pos));
}

// 여러 Temporary Wall 추가 테스트
TEST_F(TemporaryWallManagerTest, AddMultipleTemporaryWallsTest) {
    Position pos1(5, 5);
    Position pos2(10, 10);
    Position pos3(15, 15);
    auto lifetime = std::chrono::milliseconds(5000);
    
    manager->addTemporaryWall(pos1, lifetime);
    manager->addTemporaryWall(pos2, lifetime);
    manager->addTemporaryWall(pos3, lifetime);
    
    EXPECT_EQ(manager->getTemporaryWallCount(), 3);
    EXPECT_TRUE(manager->hasTemporaryWallAt(pos1));
    EXPECT_TRUE(manager->hasTemporaryWallAt(pos2));
    EXPECT_TRUE(manager->hasTemporaryWallAt(pos3));
}

// Temporary Wall 만료 및 제거 테스트
TEST_F(TemporaryWallManagerTest, TemporaryWallExpirationTest) {
    Position pos1(8, 8);
    Position pos2(12, 12);
    auto shortLifetime = std::chrono::milliseconds(100);
    auto longLifetime = std::chrono::milliseconds(5000);
    
    manager->addTemporaryWall(pos1, shortLifetime);
    manager->addTemporaryWall(pos2, longLifetime);
    
    EXPECT_EQ(manager->getTemporaryWallCount(), 2);
    
    // 150ms 대기 후 업데이트
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    manager->update();
    
    // 짧은 생존 시간의 벽은 제거되고, 긴 생존 시간의 벽은 남아있어야 함
    EXPECT_EQ(manager->getTemporaryWallCount(), 1);
    EXPECT_FALSE(manager->hasTemporaryWallAt(pos1));
    EXPECT_TRUE(manager->hasTemporaryWallAt(pos2));
}

// GameMap 업데이트 테스트
TEST_F(TemporaryWallManagerTest, UpdateMapTest) {
    Position pos(10, 10);
    auto lifetime = std::chrono::milliseconds(3000);
    
    // 초기에는 빈 공간
    EXPECT_EQ(map->getCellValue(pos.x, pos.y), 0);
    
    // Temporary Wall 추가
    manager->addTemporaryWall(pos, lifetime);
    manager->updateMap();
    
    // 맵에 Temporary Wall이 반영되어야 함
    EXPECT_EQ(map->getCellValue(pos.x, pos.y), 9);
}

// 모든 Temporary Wall 제거 테스트
TEST_F(TemporaryWallManagerTest, ClearAllTemporaryWallsTest) {
    Position pos1(5, 5);
    Position pos2(10, 10);
    Position pos3(15, 15);
    auto lifetime = std::chrono::milliseconds(5000);
    
    manager->addTemporaryWall(pos1, lifetime);
    manager->addTemporaryWall(pos2, lifetime);
    manager->addTemporaryWall(pos3, lifetime);
    
    EXPECT_EQ(manager->getTemporaryWallCount(), 3);
    
    manager->clear();
    
    EXPECT_EQ(manager->getTemporaryWallCount(), 0);
    EXPECT_FALSE(manager->hasTemporaryWallAt(pos1));
    EXPECT_FALSE(manager->hasTemporaryWallAt(pos2));
    EXPECT_FALSE(manager->hasTemporaryWallAt(pos3));
}

// 위치 중복 확인 테스트
TEST_F(TemporaryWallManagerTest, DuplicatePositionTest) {
    Position pos(12, 12);
    auto lifetime1 = std::chrono::milliseconds(2000);
    auto lifetime2 = std::chrono::milliseconds(3000);
    
    manager->addTemporaryWall(pos, lifetime1);
    EXPECT_EQ(manager->getTemporaryWallCount(), 1);
    
    // 같은 위치에 다시 추가 (기존 것을 대체해야 함)
    manager->addTemporaryWall(pos, lifetime2);
    EXPECT_EQ(manager->getTemporaryWallCount(), 1);
    EXPECT_TRUE(manager->hasTemporaryWallAt(pos));
}

// 맵 경계 밖 위치 테스트
TEST_F(TemporaryWallManagerTest, OutOfBoundsPositionTest) {
    Position invalidPos(-1, -1);
    auto lifetime = std::chrono::milliseconds(3000);
    
    // 유효하지 않은 위치에는 Temporary Wall을 추가하지 않아야 함
    manager->addTemporaryWall(invalidPos, lifetime);
    EXPECT_EQ(manager->getTemporaryWallCount(), 0);
    EXPECT_FALSE(manager->hasTemporaryWallAt(invalidPos));
} 