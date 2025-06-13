#include <gtest/gtest.h>
#include "Gate.hpp"
#include <chrono>
#include <thread>

class GateTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 테스트 설정
    }

    void TearDown() override {
        // 테스트 정리
    }
};

// Gate 생성 테스트
TEST_F(GateTest, GateCreationTest) {
    Gate gate(5, 10, GateType::ENTRANCE, WallType::OUTER, 1, 1);
    
    EXPECT_EQ(gate.getX(), 5);
    EXPECT_EQ(gate.getY(), 10);
    EXPECT_EQ(gate.getType(), GateType::ENTRANCE);
    EXPECT_EQ(gate.getWallType(), WallType::OUTER);
    EXPECT_EQ(gate.getPairId(), 1);
    EXPECT_EQ(gate.getOriginalWallValue(), 1);
}

// Gate 위치 테스트
TEST_F(GateTest, GatePositionTest) {
    Gate gate1(0, 0, GateType::ENTRANCE, WallType::OUTER);
    Gate gate2(20, 20, GateType::EXIT, WallType::INNER);
    
    EXPECT_EQ(gate1.getPosition().x, 0);
    EXPECT_EQ(gate1.getPosition().y, 0);
    EXPECT_EQ(gate2.getPosition().x, 20);
    EXPECT_EQ(gate2.getPosition().y, 20);
}

// Gate 타입 테스트
TEST_F(GateTest, GateTypeTest) {
    Gate entrance(5, 5, GateType::ENTRANCE, WallType::OUTER, 1, 1);
    Gate exit(10, 10, GateType::EXIT, WallType::INNER, 1, 2);
    
    EXPECT_EQ(entrance.getType(), GateType::ENTRANCE);
    EXPECT_EQ(exit.getType(), GateType::EXIT);
    EXPECT_TRUE(entrance.isEntrance());
    EXPECT_TRUE(exit.isExit());
    EXPECT_FALSE(entrance.isExit());
    EXPECT_FALSE(exit.isEntrance());
}

// Wall 타입 테스트
TEST_F(GateTest, WallTypeTest) {
    Gate outerGate(1, 1, GateType::ENTRANCE, WallType::OUTER);
    Gate innerGate(5, 5, GateType::EXIT, WallType::INNER);
    
    EXPECT_EQ(outerGate.getWallType(), WallType::OUTER);
    EXPECT_EQ(innerGate.getWallType(), WallType::INNER);
    EXPECT_TRUE(outerGate.isOuterWall());
    EXPECT_TRUE(innerGate.isInnerWall());
    EXPECT_FALSE(outerGate.isInnerWall());
    EXPECT_FALSE(innerGate.isOuterWall());
}

// Gate 만료 시간 테스트
TEST_F(GateTest, GateExpirationTest) {
    Gate gate(1, 1, GateType::ENTRANCE, WallType::OUTER, 1, 1);
    
    // 새로 생성된 Gate는 만료되지 않아야 함
    EXPECT_FALSE(gate.isExpired());
    
    // 생성 시간 확인
    auto creationTime = gate.getCreationTime();
    auto now = std::chrono::steady_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::seconds>(now - creationTime).count();
    
    // 생성 시간이 현재 시간과 거의 같아야 함 (1초 이내)
    EXPECT_LE(timeDiff, 1);
}

// Gate 생성 시간 테스트
TEST_F(GateTest, GateCreationTimeTest) {
    auto beforeCreation = std::chrono::steady_clock::now();
    Gate gate(5, 5, GateType::ENTRANCE, WallType::OUTER);
    auto afterCreation = std::chrono::steady_clock::now();
    
    auto creationTime = gate.getCreationTime();
    EXPECT_GE(creationTime, beforeCreation);
    EXPECT_LE(creationTime, afterCreation);
}

// Gate getter 메서드 테스트
TEST_F(GateTest, GateGetterTest) {
    Gate gate(3, 7, GateType::EXIT, WallType::INNER, 2, 2);
    
    Position pos = gate.getPosition();
    EXPECT_EQ(pos.x, 3);
    EXPECT_EQ(pos.y, 7);
    
    EXPECT_TRUE(gate.isExit());
    EXPECT_FALSE(gate.isEntrance());
    EXPECT_TRUE(gate.isInnerWall());
    EXPECT_FALSE(gate.isOuterWall());
    EXPECT_EQ(gate.getPairId(), 2);
    EXPECT_EQ(gate.getOriginalWallValue(), 2);
}

// Gate 벽 타입 테스트
TEST_F(GateTest, GateWallTypeTest) {
    Gate outerGate(2, 2, GateType::ENTRANCE, WallType::OUTER, 1, 1);
    Gate innerGate(3, 3, GateType::EXIT, WallType::INNER, 1, 2);
    
    EXPECT_EQ(outerGate.getWallType(), WallType::OUTER);
    EXPECT_EQ(innerGate.getWallType(), WallType::INNER);
    
    EXPECT_TRUE(outerGate.isOuterWall());
    EXPECT_FALSE(outerGate.isInnerWall());
    
    EXPECT_FALSE(innerGate.isOuterWall());
    EXPECT_TRUE(innerGate.isInnerWall());
}

// Gate 쌍 ID 테스트
TEST_F(GateTest, GatePairIdTest) {
    Gate gate1(0, 0, GateType::ENTRANCE, WallType::OUTER, 5, 1);
    Gate gate2(1, 1, GateType::EXIT, WallType::INNER, 5, 2);
    
    EXPECT_EQ(gate1.getPairId(), 5);
    EXPECT_EQ(gate2.getPairId(), 5);
    EXPECT_EQ(gate1.getPairId(), gate2.getPairId());
} 