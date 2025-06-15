#include <gtest/gtest.h>
#include "TemporaryWall.hpp"
#include <chrono>
#include <thread>

class TemporaryWallTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 테스트 설정
    }

    void TearDown() override {
        // 테스트 정리
    }
};

// TemporaryWall 생성 테스트
TEST_F(TemporaryWallTest, TemporaryWallCreationTest) {
    Position pos(10, 15);
    auto lifetime = std::chrono::milliseconds(3000);  // 3초
    
    TemporaryWall tempWall(pos, lifetime);
    
    EXPECT_EQ(tempWall.getPosition().x, 10);
    EXPECT_EQ(tempWall.getPosition().y, 15);
    EXPECT_FALSE(tempWall.isExpired());
}

// TemporaryWall 만료 테스트
TEST_F(TemporaryWallTest, TemporaryWallExpirationTest) {
    Position pos(5, 8);
    auto lifetime = std::chrono::milliseconds(100);  // 100ms
    
    TemporaryWall tempWall(pos, lifetime);
    
    // 새로 생성된 벽은 만료되지 않아야 함
    EXPECT_FALSE(tempWall.isExpired());
    
    // 150ms 대기 후 만료되어야 함
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_TRUE(tempWall.isExpired());
}

// TemporaryWall 위치 테스트
TEST_F(TemporaryWallTest, TemporaryWallPositionTest) {
    Position pos1(0, 0);
    Position pos2(20, 20);
    auto lifetime = std::chrono::milliseconds(5000);
    
    TemporaryWall tempWall1(pos1, lifetime);
    TemporaryWall tempWall2(pos2, lifetime);
    
    EXPECT_EQ(tempWall1.getPosition().x, 0);
    EXPECT_EQ(tempWall1.getPosition().y, 0);
    EXPECT_EQ(tempWall2.getPosition().x, 20);
    EXPECT_EQ(tempWall2.getPosition().y, 20);
}

// TemporaryWall 생성 시간 테스트
TEST_F(TemporaryWallTest, TemporaryWallCreationTimeTest) {
    auto beforeCreation = std::chrono::steady_clock::now();
    
    Position pos(10, 10);
    auto lifetime = std::chrono::milliseconds(1000);
    TemporaryWall tempWall(pos, lifetime);
    
    auto afterCreation = std::chrono::steady_clock::now();
    auto creationTime = tempWall.getCreationTime();
    
    EXPECT_GE(creationTime, beforeCreation);
    EXPECT_LE(creationTime, afterCreation);
}

// TemporaryWall 다양한 생존 시간 테스트
TEST_F(TemporaryWallTest, TemporaryWallVariousLifetimeTest) {
    Position pos(12, 12);
    
    // 짧은 생존 시간 (50ms)
    auto shortLifetime = std::chrono::milliseconds(50);
    TemporaryWall shortWall(pos, shortLifetime);
    
    // 긴 생존 시간 (10초)
    auto longLifetime = std::chrono::milliseconds(10000);
    TemporaryWall longWall(pos, longLifetime);
    
    // 초기에는 둘 다 만료되지 않음
    EXPECT_FALSE(shortWall.isExpired());
    EXPECT_FALSE(longWall.isExpired());
    
    // 100ms 대기 후
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 짧은 벽은 만료, 긴 벽은 아직 유효
    EXPECT_TRUE(shortWall.isExpired());
    EXPECT_FALSE(longWall.isExpired());
} 