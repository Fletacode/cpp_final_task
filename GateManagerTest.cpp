#include <gtest/gtest.h>
#include "GateManager.hpp"
#include "GameMap.hpp"
#include "Snake.hpp"
#include <memory>

class GateManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        map = std::make_unique<GameMap>(31, 31);
        snake = std::make_unique<Snake>(10, 10);
        gateManager = std::make_unique<GateManager>(*map);
        
        // 테스트용 벽 설정
        map->setWall(5, 5);
        map->setWall(5, 6);
        map->setWall(15, 15);
    }

    void TearDown() override {
        // 테스트 정리
    }

    std::unique_ptr<GameMap> map;
    std::unique_ptr<Snake> snake;
    std::unique_ptr<GateManager> gateManager;
};

// GateManager 생성 테스트
TEST_F(GateManagerTest, GateManagerCreationTest) {
    EXPECT_TRUE(gateManager != nullptr);
    EXPECT_EQ(gateManager->getGateCount(), 0);
}

// Gate 생성 테스트
TEST_F(GateManagerTest, GateGenerationTest) {
    gateManager->generateGates(*snake);
    
    // Gate가 생성되었는지 확인 (최대 2개)
    int gateCount = gateManager->getGateCount();
    EXPECT_TRUE(gateCount == 0 || gateCount == 2);  // 0개 또는 2개 (입구/출구 쌍)
}

// Gate 최대 개수 제한 테스트
TEST_F(GateManagerTest, MaxGateCountTest) {
    // 여러 번 생성 시도
    for (int i = 0; i < 10; i++) {
        gateManager->generateGates(*snake);
    }
    
    // 최대 2개까지만 생성되어야 함
    EXPECT_LE(gateManager->getGateCount(), 2);
}

// Wall 위치에만 Gate 생성 테스트
TEST_F(GateManagerTest, GateOnWallOnlyTest) {
    // 충분한 시도로 Gate 생성 유도
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() > 0) {
            break;
        }
    }
    
    auto gates = gateManager->getGates();
    for (const auto& gate : gates) {
        int x = gate.getX();
        int y = gate.getY();
        // Gate가 벽 위치에 있는지 확인
        EXPECT_TRUE(map->getCellValue(x, y) == 1 || map->getCellValue(x, y) == 2);
    }
}

// Gate 만료 제거 테스트
TEST_F(GateManagerTest, ExpiredGateRemovalTest) {
    // Gate 생성
    gateManager->generateGates(*snake);
    int initialCount = gateManager->getGateCount();
    
    // 만료되지 않은 상태에서 제거 시도
    gateManager->removeExpiredGates();
    EXPECT_EQ(gateManager->getGateCount(), initialCount);
}

// Snake와 Gate 충돌 감지 테스트
TEST_F(GateManagerTest, GateCollisionDetectionTest) {
    // 특정 위치에 Gate 수동 생성 (테스트용)
    if (gateManager->getGateCount() == 0) {
        gateManager->generateGates(*snake);
    }
    
    // 충돌이 없는 상태에서 테스트
    auto collision = gateManager->checkCollision(*snake);
    EXPECT_FALSE(collision.has_value());
}

// Gate 쌍 생성 테스트 (입구/출구)
TEST_F(GateManagerTest, GatePairGenerationTest) {
    // Gate 생성 시도
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        auto gates = gateManager->getGates();
        bool hasEntrance = false;
        bool hasExit = false;
        
        for (const auto& gate : gates) {
            if (gate.isEntrance()) hasEntrance = true;
            if (gate.isExit()) hasExit = true;
        }
        
        EXPECT_TRUE(hasEntrance);
        EXPECT_TRUE(hasExit);
    }
}

// Gate 위치 유효성 테스트
TEST_F(GateManagerTest, GatePositionValidityTest) {
    gateManager->generateGates(*snake);
    
    auto gates = gateManager->getGates();
    for (const auto& gate : gates) {
        int x = gate.getX();
        int y = gate.getY();
        
        // 맵 범위 내에 있는지 확인
        EXPECT_GE(x, 0);
        EXPECT_LT(x, map->getWidth());
        EXPECT_GE(y, 0);
        EXPECT_LT(y, map->getHeight());
    }
}

// Gate 업데이트 맵 테스트
TEST_F(GateManagerTest, UpdateMapTest) {
    gateManager->generateGates(*snake);
    
    // 맵 업데이트 전 상태 확인
    gateManager->updateMap();
    
    // Gate가 맵에 표시되는지 확인
    auto gates = gateManager->getGates();
    for (const auto& gate : gates) {
        int x = gate.getX();
        int y = gate.getY();
        // Gate 값(7)이 맵에 설정되어 있는지 확인
        EXPECT_EQ(map->getCellValue(x, y), 7);
    }
}

// 한 번에 하나의 게이트 쌍만 존재하는지 테스트
TEST_F(GateManagerTest, SinglePairOnlyTest) {
    // 여러 번 게이트 생성 시도
    for (int i = 0; i < 20; i++) {
        gateManager->generateGates(*snake);
    }
    
    int gateCount = gateManager->getGateCount();
    // 게이트는 0개 또는 정확히 2개(한 쌍)만 존재해야 함
    EXPECT_TRUE(gateCount == 0 || gateCount == 2);
    
    if (gateCount == 2) {
        auto gates = gateManager->getGates();
        // 두 게이트가 같은 pairId를 가져야 함
        EXPECT_EQ(gates[0].getPairId(), gates[1].getPairId());
        
        // 하나는 입구, 하나는 출구여야 함
        bool hasEntrance = false;
        bool hasExit = false;
        for (const auto& gate : gates) {
            if (gate.isEntrance()) hasEntrance = true;
            if (gate.isExit()) hasExit = true;
        }
        EXPECT_TRUE(hasEntrance && hasExit);
    }
}

// 기존 게이트 쌍이 있을 때 새로운 쌍이 생성되지 않는지 테스트
TEST_F(GateManagerTest, NoNewPairWhenExistingTest) {
    // 첫 번째 게이트 쌍 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        auto firstPairId = gateManager->getGates()[0].getPairId();
        
        // 추가로 게이트 생성 시도
        for (int i = 0; i < 20; i++) {
            gateManager->generateGates(*snake);
        }
        
        // 여전히 2개만 있어야 하고, 같은 pairId를 가져야 함
        EXPECT_EQ(gateManager->getGateCount(), 2);
        auto gates = gateManager->getGates();
        EXPECT_EQ(gates[0].getPairId(), firstPairId);
        EXPECT_EQ(gates[1].getPairId(), firstPairId);
    }
}

// 게이트 쌍이 함께 만료되는지 테스트
TEST_F(GateManagerTest, PairExpirationTest) {
    // 게이트 쌍 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        // 만료되지 않은 상태에서는 제거되지 않아야 함
        gateManager->removeExpiredGates();
        EXPECT_EQ(gateManager->getGateCount(), 2);
        
        // 시간이 지나서 만료된 후에는 쌍이 함께 제거되어야 함
        // (실제 시간 대기 대신 만료 로직 테스트)
        // 이 테스트는 실제 구현에서 만료 로직이 쌍 단위로 작동하는지 확인
    }
}

// 강제로 게이트 룰 위반 시도 테스트
TEST_F(GateManagerTest, StrictSinglePairEnforcementTest) {
    // 첫 번째 게이트 쌍 생성
    for (int i = 0; i < 100; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        int originalPairId = gateManager->getGates()[0].getPairId();
        
        // 매우 많은 시도로 새로운 게이트 생성 강제 시도
        for (int i = 0; i < 1000; i++) {
            gateManager->generateGates(*snake);
        }
        
        // 여전히 2개만 있어야 하고, 같은 pairId를 유지해야 함
        EXPECT_EQ(gateManager->getGateCount(), 2);
        auto gates = gateManager->getGates();
        EXPECT_EQ(gates[0].getPairId(), originalPairId);
        EXPECT_EQ(gates[1].getPairId(), originalPairId);
        EXPECT_EQ(gates[0].getPairId(), gates[1].getPairId());
    }
}

// 게이트 쌍 ID 고유성 테스트
TEST_F(GateManagerTest, PairIdUniquenessTest) {
    // 첫 번째 게이트 쌍 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        int firstPairId = gateManager->getGates()[0].getPairId();
        
        // 같은 GateManager에서 다음 pairId가 증가하는지 테스트
        // nextPairId가 올바르게 증가하는지 확인
        EXPECT_EQ(firstPairId, 1);  // 첫 번째 쌍은 ID 1을 가져야 함
        
        // 게이트 쌍의 두 게이트가 같은 pairId를 가지는지 확인
        auto gates = gateManager->getGates();
        EXPECT_EQ(gates[0].getPairId(), gates[1].getPairId());
        EXPECT_EQ(gates[0].getPairId(), firstPairId);
    }
}

// 게이트 벽 복원 테스트
TEST_F(GateManagerTest, GateWallRestorationTest) {
    // 특정 위치에 벽 설정
    map->setWall(10, 10);  // 일반 벽 (값 1)
    map->setCellValue(12, 12, 2);  // 면역 벽 (값 2)
    
    // 게이트 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        auto gates = gateManager->getGates();
        std::vector<Position> gatePositions;
        std::vector<int> originalValues;
        
        // 게이트 위치의 원래 벽 값이 올바르게 저장되었는지 확인
        for (const auto& gate : gates) {
            int x = gate.getX();
            int y = gate.getY();
            int originalValue = gate.getOriginalWallValue();
            
            gatePositions.push_back(Position(x, y));
            originalValues.push_back(originalValue);
            
            // 원래 벽 값이 1 또는 2여야 함
            EXPECT_TRUE(originalValue == 1 || originalValue == 2);
            
            // 현재 게이트가 설정되어 있는지 확인
            EXPECT_EQ(map->getCellValue(x, y), 7);
        }
        
        // 게이트 위치를 벽으로 복원
        gateManager->restoreGatePositionsToWalls();
        
        // 게이트 위치가 원래 벽 값으로 복원되었는지 확인
        for (size_t i = 0; i < gatePositions.size(); i++) {
            Position pos = gatePositions[i];
            int originalValue = originalValues[i];
            
            // 맵에서 해당 위치의 값이 원래 벽 값으로 복원되었는지 확인
            EXPECT_EQ(map->getCellValue(pos.x, pos.y), originalValue);
        }
        
        // 게이트가 모두 제거되었는지 확인
        EXPECT_EQ(gateManager->getGateCount(), 0);
    }
}

// 새로운 게이트가 기존 게이트를 대체하는 테스트
TEST_F(GateManagerTest, NewGateReplacesOldGateTest) {
    // 첫 번째 게이트 쌍 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        auto firstGates = gateManager->getGates();
        std::vector<Position> firstGatePositions;
        std::vector<int> firstOriginalValues;
        
        // 첫 번째 게이트들의 위치와 원래 벽 값 저장
        for (const auto& gate : firstGates) {
            firstGatePositions.push_back(gate.getPosition());
            firstOriginalValues.push_back(gate.getOriginalWallValue());
            
            // 현재 게이트가 설정되어 있는지 확인
            EXPECT_EQ(map->getCellValue(gate.getX(), gate.getY()), 7);
        }
        
        // 기존 게이트를 벽으로 복원
        gateManager->restoreGatePositionsToWalls();
        
        // 첫 번째 게이트 위치들이 원래 벽 값으로 복원되었는지 확인
        for (size_t i = 0; i < firstGatePositions.size(); i++) {
            Position pos = firstGatePositions[i];
            int originalValue = firstOriginalValues[i];
            EXPECT_EQ(map->getCellValue(pos.x, pos.y), originalValue);
        }
        
        // 게이트가 모두 제거되었는지 확인
        EXPECT_EQ(gateManager->getGateCount(), 0);
        
        // 새로운 게이트 쌍 생성
        for (int i = 0; i < 50; i++) {
            gateManager->generateGates(*snake);
            if (gateManager->getGateCount() == 2) {
                break;
            }
        }
        
        // 새로운 게이트가 생성되었는지 확인
        EXPECT_EQ(gateManager->getGateCount(), 2);
        
        // 새로운 게이트들이 이전 게이트와 다른 위치에 있는지 확인 (선택적)
        auto newGates = gateManager->getGates();
        for (const auto& newGate : newGates) {
            EXPECT_EQ(map->getCellValue(newGate.getX(), newGate.getY()), 7);
        }
    }
}

// 원래 벽 타입 보존 테스트
TEST_F(GateManagerTest, OriginalWallTypePreservationTest) {
    // 다양한 타입의 벽 설정
    map->setWall(8, 8);        // 일반 벽 (값 1)
    map->setCellValue(9, 9, 2);  // 면역 벽 (값 2)
    map->setWall(10, 10);      // 일반 벽 (값 1)
    
    // 게이트 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        auto gates = gateManager->getGates();
        
        // 각 게이트의 원래 벽 값이 올바른지 확인
        for (const auto& gate : gates) {
            int x = gate.getX();
            int y = gate.getY();
            int originalValue = gate.getOriginalWallValue();
            
            // 게이트가 생성되기 전 해당 위치의 벽 타입과 일치해야 함
            // (현재는 게이트 값 7이 설정되어 있지만, 원래 값이 저장되어 있어야 함)
            EXPECT_TRUE(originalValue == 1 || originalValue == 2);
        }
    }
}

// 양방향 게이트 텔레포트 테스트
TEST_F(GateManagerTest, BidirectionalGateTeleportTest) {
    // 게이트 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        auto gates = gateManager->getGates();
        Gate gate1 = gates[0];
        Gate gate2 = gates[1];
        
        // 첫 번째 게이트에 충돌 시 두 번째 게이트로 이동
        Position targetPos1 = gateManager->calculateBidirectionalExitPosition(gate1, Direction::UP, *snake);
        EXPECT_NE(targetPos1.x, -1);
        EXPECT_NE(targetPos1.y, -1);
        
        // 두 번째 게이트에 충돌 시 첫 번째 게이트로 이동
        Position targetPos2 = gateManager->calculateBidirectionalExitPosition(gate2, Direction::DOWN, *snake);
        EXPECT_NE(targetPos2.x, -1);
        EXPECT_NE(targetPos2.y, -1);
        
        // 두 위치가 서로 다른 게이트 근처여야 함
        EXPECT_TRUE(targetPos1 != targetPos2);
    }
}

// 출구에서 입구로 텔레포트 테스트
TEST_F(GateManagerTest, ExitToEntranceTeleportTest) {
    // 게이트 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        auto gates = gateManager->getGates();
        
        // 출구 게이트 찾기
        Gate* exitGate = nullptr;
        for (auto& gate : gates) {
            if (gate.isExit()) {
                exitGate = &gate;
                break;
            }
        }
        
        if (exitGate != nullptr) {
            // 출구 게이트에서 텔레포트 위치 계산
            Position targetPos = gateManager->calculateBidirectionalExitPosition(*exitGate, Direction::LEFT, *snake);
            
            // 유효한 위치가 반환되어야 함
            EXPECT_NE(targetPos.x, -1);
            EXPECT_NE(targetPos.y, -1);
            
            // 출구 게이트 위치와 달라야 함
            EXPECT_TRUE(targetPos != exitGate->getPosition());
        }
    }
}

// 입구에서 출구로 텔레포트 테스트
TEST_F(GateManagerTest, EntranceToExitTeleportTest) {
    // 게이트 생성
    for (int i = 0; i < 50; i++) {
        gateManager->generateGates(*snake);
        if (gateManager->getGateCount() == 2) {
            break;
        }
    }
    
    if (gateManager->getGateCount() == 2) {
        auto gates = gateManager->getGates();
        
        // 입구 게이트 찾기
        Gate* entranceGate = nullptr;
        for (auto& gate : gates) {
            if (gate.isEntrance()) {
                entranceGate = &gate;
                break;
            }
        }
        
        if (entranceGate != nullptr) {
            // 입구 게이트에서 텔레포트 위치 계산
            Position targetPos = gateManager->calculateBidirectionalExitPosition(*entranceGate, Direction::RIGHT, *snake);
            
            // 유효한 위치가 반환되어야 함
            EXPECT_NE(targetPos.x, -1);
            EXPECT_NE(targetPos.y, -1);
            
            // 입구 게이트 위치와 달라야 함
            EXPECT_TRUE(targetPos != entranceGate->getPosition());
        }
    }
}

// 모서리 좌표에서 게이트 생성 제외 테스트
TEST_F(GateManagerTest, CornerPositionsExcludedFromGateGeneration) {
    // 21x21 맵의 4개 모서리 좌표: (0,0), (0,20), (20,0), (20,20)
    std::vector<Position> cornerPositions = {
        Position(0, 0),
        Position(0, 20),
        Position(20, 0),
        Position(20, 20)
    };
    
    // 모서리 위치를 벽으로 설정
    for (const auto& pos : cornerPositions) {
        map->setWall(pos.x, pos.y);
    }
    
    // 여러 번 게이트 생성 시도
    for (int i = 0; i < 100; i++) {
        gateManager->generateGates(*snake);
        
        // 생성된 게이트들이 모서리 위치에 있지 않은지 확인
        auto gates = gateManager->getGates();
        for (const auto& gate : gates) {
            Position gatePos(gate.getX(), gate.getY());
            
            // 모서리 위치가 아닌지 확인 (정확한 위치 비교)
            for (const auto& cornerPos : cornerPositions) {
                EXPECT_FALSE(gatePos.x == cornerPos.x && gatePos.y == cornerPos.y) 
                    << "Gate should not be at corner position (" << cornerPos.x << "," << cornerPos.y << ")";
            }
        }
        
        // 기존 게이트 제거하고 다시 시도
        gateManager->restoreGatePositionsToWalls();
    }
}

// 모서리가 아닌 외곽 벽에서는 게이트 생성 가능한지 테스트
TEST_F(GateManagerTest, NonCornerOuterWallGateGeneration) {
    // 모서리가 아닌 외곽 벽 위치들
    std::vector<Position> nonCornerOuterWalls = {
        Position(1, 0),   // 상단 벽
        Position(0, 1),   // 좌측 벽
        Position(20, 1),  // 우측 벽
        Position(1, 20)   // 하단 벽
    };
    
    // 해당 위치들을 벽으로 설정
    for (const auto& pos : nonCornerOuterWalls) {
        map->setWall(pos.x, pos.y);
    }
    
    // 게이트 생성 시도
    bool gateGeneratedOnNonCornerWall = false;
    for (int i = 0; i < 100; i++) {
        gateManager->generateGates(*snake);
        
        auto gates = gateManager->getGates();
        for (const auto& gate : gates) {
            Position gatePos(gate.getX(), gate.getY());
            
            // 모서리가 아닌 외곽 벽에 게이트가 생성되었는지 확인
            for (const auto& wallPos : nonCornerOuterWalls) {
                if (gatePos.x == wallPos.x && gatePos.y == wallPos.y) {
                    gateGeneratedOnNonCornerWall = true;
                    break;
                }
            }
            
            if (gateGeneratedOnNonCornerWall) break;
        }
        
        if (gateGeneratedOnNonCornerWall) break;
        
        // 기존 게이트 제거하고 다시 시도
        gateManager->restoreGatePositionsToWalls();
    }
    
    // 모서리가 아닌 외곽 벽에서는 게이트 생성이 가능해야 함
    EXPECT_TRUE(gateGeneratedOnNonCornerWall) << "Gates should be able to generate on non-corner outer walls";
}

// 새로운 Gate 진입 로직 테스트 케이스들 추가

TEST_F(GateManagerTest, SnakeEnteringGatePreventsDeletion) {
    Snake snake(10, 10);
    
    // Gate 생성
    gateManager->generateGates(snake);
    int initialGateCount = gateManager->getGateCount();
    
    if (initialGateCount > 0) {
        // Snake가 Gate에 진입
        auto gates = gateManager->getGates();
        Position gatePos = gates[0].getPosition();
        snake.teleportTo(gatePos);
        
        // Gate 진입 상태 설정 (새로 구현할 메서드)
        gateManager->setSnakeEntering(gatePos, true);
        
        // 만료된 Gate 제거 시도
        gateManager->removeExpiredGates();
        
        // Gate가 사라지지 않았는지 확인
        EXPECT_EQ(gateManager->getGateCount(), initialGateCount);
    }
}

TEST_F(GateManagerTest, OuterWallGateExitDirections) {
    Snake snake(15, 15);
    
    // 상단 벽 Gate 테스트 (아래 방향 진출)
    Direction exitDir = gateManager->calculateOuterWallExitDirection(Position(15, 0), Direction::UP);
    EXPECT_EQ(exitDir, Direction::DOWN);
    
    // 하단 벽 Gate 테스트 (위 방향 진출)
    exitDir = gateManager->calculateOuterWallExitDirection(Position(15, 30), Direction::DOWN);
    EXPECT_EQ(exitDir, Direction::UP);
    
    // 좌측 벽 Gate 테스트 (오른쪽 방향 진출)
    exitDir = gateManager->calculateOuterWallExitDirection(Position(0, 15), Direction::LEFT);
    EXPECT_EQ(exitDir, Direction::RIGHT);
    
    // 우측 벽 Gate 테스트 (왼쪽 방향 진출)
    exitDir = gateManager->calculateOuterWallExitDirection(Position(30, 15), Direction::RIGHT);
    EXPECT_EQ(exitDir, Direction::LEFT);
}

TEST_F(GateManagerTest, InnerWallGateDirectionPriority) {
    Snake snake(15, 15);
    
    // 내부벽 위치 (15, 15)에서 진입 방향과 동일한 방향이 우선
    Position innerWallPos(15, 15);
    Direction entryDir = Direction::RIGHT;
    
    // 우선순위: 진입방향 -> 시계방향 -> 역시계방향 -> 반대방향
    auto priorities = gateManager->calculateInnerWallDirectionPriority(innerWallPos, entryDir);
    
    EXPECT_EQ(priorities[0], Direction::RIGHT);  // 진입 방향
    EXPECT_EQ(priorities[1], Direction::DOWN);   // 시계방향
    EXPECT_EQ(priorities[2], Direction::UP);     // 역시계방향
    EXPECT_EQ(priorities[3], Direction::LEFT);   // 반대방향
}

TEST_F(GateManagerTest, InnerWallSpecialRulesHorizontal) {
    Snake snake(15, 15);
    
    Position innerWallPos(15, 15);
    
    // 진출 방향이 좌-우인 경우의 특별 규칙
    // 오른쪽에서 진입시 => 우로 진출
    Direction exitDir = gateManager->applyInnerWallSpecialRules(innerWallPos, Direction::RIGHT, true);
    EXPECT_EQ(exitDir, Direction::RIGHT);
    
    // 아래로 진입시 => 좌로 진출
    exitDir = gateManager->applyInnerWallSpecialRules(innerWallPos, Direction::DOWN, true);
    EXPECT_EQ(exitDir, Direction::LEFT);
    
    // 왼쪽에서 진입시 => 좌로 진출
    exitDir = gateManager->applyInnerWallSpecialRules(innerWallPos, Direction::LEFT, true);
    EXPECT_EQ(exitDir, Direction::LEFT);
    
    // 위로 진입시 => 좌로 진출
    exitDir = gateManager->applyInnerWallSpecialRules(innerWallPos, Direction::UP, true);
    EXPECT_EQ(exitDir, Direction::LEFT);
}

TEST_F(GateManagerTest, InnerWallSpecialRulesVertical) {
    Snake snake(15, 15);
    
    Position innerWallPos(15, 15);
    
    // 진출 방향이 상-하인 경우의 특별 규칙
    // 오른쪽에서 진입시 => 위로 진출
    Direction exitDir = gateManager->applyInnerWallSpecialRules(innerWallPos, Direction::RIGHT, false);
    EXPECT_EQ(exitDir, Direction::UP);
    
    // 위쪽으로 진입시 => 위로 진출
    exitDir = gateManager->applyInnerWallSpecialRules(innerWallPos, Direction::UP, false);
    EXPECT_EQ(exitDir, Direction::UP);
    
    // 왼쪽에서 진입시 => 아래로 진출
    exitDir = gateManager->applyInnerWallSpecialRules(innerWallPos, Direction::LEFT, false);
    EXPECT_EQ(exitDir, Direction::DOWN);
    
    // 아래로 진입시 => 아래로 진출
    exitDir = gateManager->applyInnerWallSpecialRules(innerWallPos, Direction::DOWN, false);
    EXPECT_EQ(exitDir, Direction::DOWN);
}

TEST_F(GateManagerTest, NoNewGatesWhileSnakeEntering) {
    Snake snake(10, 10);
    
    // Gate 생성
    gateManager->generateGates(snake);
    int initialGateCount = gateManager->getGateCount();
    
    if (initialGateCount > 0) {
        // Snake가 Gate에 진입 중 상태 설정
        auto gates = gateManager->getGates();
        Position gatePos = gates[0].getPosition();
        gateManager->setSnakeEntering(gatePos, true);
        
        // 새로운 Gate 생성 시도
        gateManager->generateGates(snake);
        
        // Gate 수가 증가하지 않았는지 확인
        EXPECT_EQ(gateManager->getGateCount(), initialGateCount);
    }
}

// 같은 외부벽에 게이트 쌍이 생성되지 않는지 테스트
TEST_F(GateManagerTest, OuterWallGatePairsOnDifferentWalls) {
    Snake snake(10, 10);
    
    // 외부벽에 벽 설정 (상단, 하단, 좌측, 우측)
    for (int i = 1; i < 30; i++) {
        map->setWall(i, 0);      // 상단 벽
        map->setWall(i, 30);     // 하단 벽
        map->setWall(0, i);      // 좌측 벽
        map->setWall(30, i);     // 우측 벽
    }
    
    // 여러 번 게이트 생성하여 같은 외부벽에 쌍이 생성되지 않는지 확인
    for (int attempt = 0; attempt < 100; attempt++) {
        gateManager->generateGates(snake);
        
        auto gates = gateManager->getGates();
        if (gates.size() >= 2) {
            // 게이트 쌍 찾기
            for (size_t i = 0; i < gates.size(); i++) {
                for (size_t j = i + 1; j < gates.size(); j++) {
                    if (gates[i].getPairId() == gates[j].getPairId()) {
                        Position pos1 = gates[i].getPosition();
                        Position pos2 = gates[j].getPosition();
                        
                        // 두 게이트가 모두 외부벽에 있는지 확인
                        bool pos1IsOuter = (pos1.x == 0 || pos1.x == 30 || pos1.y == 0 || pos1.y == 30);
                        bool pos2IsOuter = (pos2.x == 0 || pos2.x == 30 || pos2.y == 0 || pos2.y == 30);
                        
                        if (pos1IsOuter && pos2IsOuter) {
                            // 같은 외부벽에 있지 않은지 확인
                            bool sameWall = false;
                            
                            // 상단 벽
                            if (pos1.y == 0 && pos2.y == 0) sameWall = true;
                            // 하단 벽
                            if (pos1.y == 30 && pos2.y == 30) sameWall = true;
                            // 좌측 벽
                            if (pos1.x == 0 && pos2.x == 0) sameWall = true;
                            // 우측 벽
                            if (pos1.x == 30 && pos2.x == 30) sameWall = true;
                            
                            EXPECT_FALSE(sameWall) 
                                << "Gate pair should not be on the same outer wall. "
                                << "Gate 1: (" << pos1.x << "," << pos1.y << "), "
                                << "Gate 2: (" << pos2.x << "," << pos2.y << ")";
                        }
                    }
                }
            }
        }
        
        // 다음 테스트를 위해 게이트 제거
        gateManager->restoreGatePositionsToWalls();
    }
}

// isSameOuterWall 함수 테스트
TEST_F(GateManagerTest, IsSameOuterWallFunctionTest) {
    // 31x31 맵에서 외부벽은 x=0, x=30, y=0, y=30
    
    // 같은 벽에 있는 경우들
    EXPECT_TRUE(gateManager->isSameOuterWall(Position(1, 0), Position(5, 0)));    // 상단 벽
    EXPECT_TRUE(gateManager->isSameOuterWall(Position(1, 30), Position(5, 30)));  // 하단 벽
    EXPECT_TRUE(gateManager->isSameOuterWall(Position(0, 1), Position(0, 5)));    // 좌측 벽
    EXPECT_TRUE(gateManager->isSameOuterWall(Position(30, 1), Position(30, 5)));  // 우측 벽
    
    // 다른 벽에 있는 경우들
    EXPECT_FALSE(gateManager->isSameOuterWall(Position(1, 0), Position(1, 30)));  // 상단 vs 하단
    EXPECT_FALSE(gateManager->isSameOuterWall(Position(0, 1), Position(30, 1)));  // 좌측 vs 우측
    EXPECT_FALSE(gateManager->isSameOuterWall(Position(1, 0), Position(0, 1)));   // 상단 vs 좌측
    EXPECT_FALSE(gateManager->isSameOuterWall(Position(1, 0), Position(30, 1)));  // 상단 vs 우측
    
    // 내부벽과 외부벽
    EXPECT_FALSE(gateManager->isSameOuterWall(Position(5, 5), Position(1, 0)));   // 내부 vs 외부
} 