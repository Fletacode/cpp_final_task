#include <gtest/gtest.h>
#include "ItemManager.hpp"
#include "GameMap.hpp"
#include "Snake.hpp"
#include <memory>
#include <chrono>
#include <thread>

class ItemManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        gameMap = std::make_unique<GameMap>(31, 31);  // 기본 맵 크기
        snake = std::make_unique<Snake>(10, 10);
        itemManager = std::make_unique<ItemManager>(*gameMap);
    }

    void TearDown() override {
        // 테스트 정리
    }

    std::unique_ptr<GameMap> gameMap;
    std::unique_ptr<Snake> snake;
    std::unique_ptr<ItemManager> itemManager;
};

// ItemManager 생성 테스트
TEST_F(ItemManagerTest, ItemManagerCreationTest) {
    EXPECT_EQ(itemManager->getItemCount(), 0);
    EXPECT_TRUE(itemManager->getItems().empty());
}

// 아이템 생성 테스트
TEST_F(ItemManagerTest, ItemGenerationTest) {
    // 아이템 생성
    itemManager->generateItems(*snake);
    
    // 최대 3개까지 생성되어야 함
    EXPECT_LE(itemManager->getItemCount(), 3);
    EXPECT_GE(itemManager->getItemCount(), 0);
}

// 최대 아이템 수 제한 테스트
TEST_F(ItemManagerTest, MaxItemLimitTest) {
    // 여러 번 생성 시도
    for (int i = 0; i < 10; ++i) {
        itemManager->generateItems(*snake);
    }
    
    // 최대 3개를 초과하지 않아야 함
    EXPECT_LE(itemManager->getItemCount(), 3);
}

// 아이템 만료 처리 테스트
TEST_F(ItemManagerTest, ItemExpirationTest) {
    // 매우 짧은 지속시간으로 아이템 생성
    itemManager->addItem(5, 5, ItemType::GROWTH, std::chrono::milliseconds(50));
    EXPECT_EQ(itemManager->getItemCount(), 1);
    
    // 만료 시간 대기
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 만료된 아이템 제거
    itemManager->removeExpiredItems();
    EXPECT_EQ(itemManager->getItemCount(), 0);
}

// 아이템 충돌 감지 테스트
TEST_F(ItemManagerTest, ItemCollisionTest) {
    // 아이템 생성
    itemManager->addItem(10, 10, ItemType::GROWTH);
    EXPECT_EQ(itemManager->getItemCount(), 1);
    
    // Snake를 아이템 위치로 이동
    snake->teleportTo(Position(10, 10));
    
    // 충돌 감지
    auto collectedItem = itemManager->checkCollision(*snake);
    EXPECT_TRUE(collectedItem.has_value());
    EXPECT_EQ(collectedItem->getType(), ItemType::GROWTH);
    
    // 아이템이 제거되었는지 확인
    EXPECT_EQ(itemManager->getItemCount(), 0);
}

// 아이템 충돌 없음 테스트
TEST_F(ItemManagerTest, NoItemCollisionTest) {
    // Snake 머리와 다른 위치에 아이템 생성
    Position headPos = snake->getHead();
    itemManager->addItem(headPos.x + 5, headPos.y + 5, ItemType::POISON);
    
    EXPECT_EQ(itemManager->getItemCount(), 1);
    
    // 충돌 감지 (충돌 없음)
    auto collectedItem = itemManager->checkCollision(*snake);
    EXPECT_FALSE(collectedItem.has_value());
    EXPECT_EQ(itemManager->getItemCount(), 1);
}

// 맵 업데이트 테스트
TEST_F(ItemManagerTest, MapUpdateTest) {
    // 아이템 생성
    itemManager->addItem(5, 5, ItemType::GROWTH);
    itemManager->addItem(10, 10, ItemType::POISON);
    
    // 맵 업데이트
    itemManager->updateMap();
    
    // 맵에서 아이템 위치 확인
    EXPECT_EQ(gameMap->getCell(5, 5), 5);  // Growth Item
    EXPECT_EQ(gameMap->getCell(10, 10), 6);  // Poison Item
}

// 빈 공간 찾기 테스트
TEST_F(ItemManagerTest, FindEmptySpaceTest) {
    // 빈 공간이 있는지 확인
    auto emptyPos = itemManager->findEmptyPosition(*snake);
    EXPECT_TRUE(emptyPos.has_value());
    
    // 빈 공간이면 값이 0이어야 함
    if (emptyPos.has_value()) {
        EXPECT_EQ(gameMap->getCell(emptyPos->x, emptyPos->y), 0);
    }
}

// SPEED 아이템 생성 테스트
TEST_F(ItemManagerTest, SpeedItemGenerationTest) {
    // SPEED 아이템 직접 추가
    itemManager->addItem(5, 5, ItemType::SPEED);
    EXPECT_EQ(itemManager->getItemCount(), 1);
    
    const auto& items = itemManager->getItems();
    EXPECT_EQ(items[0].getType(), ItemType::SPEED);
    EXPECT_EQ(items[0].getX(), 5);
    EXPECT_EQ(items[0].getY(), 5);
}

// SPEED 아이템 맵 업데이트 테스트
TEST_F(ItemManagerTest, SpeedItemMapUpdateTest) {
    // SPEED 아이템 추가
    itemManager->addItem(10, 10, ItemType::SPEED);
    
    // 맵 업데이트
    itemManager->updateMap();
    
    // 맵에서 SPEED 아이템이 값 8로 표시되는지 확인
    EXPECT_EQ(gameMap->getCellValue(10, 10), 8);
}

// SPEED 아이템 충돌 감지 테스트
TEST_F(ItemManagerTest, SpeedItemCollisionTest) {
    // SPEED 아이템 생성
    itemManager->addItem(15, 15, ItemType::SPEED);
    EXPECT_EQ(itemManager->getItemCount(), 1);
    
    // Snake를 SPEED 아이템 위치로 이동
    snake->teleportTo(Position(15, 15));
    
    // 충돌 감지
    auto collectedItem = itemManager->checkCollision(*snake);
    EXPECT_TRUE(collectedItem.has_value());
    EXPECT_EQ(collectedItem->getType(), ItemType::SPEED);
    
    // 아이템이 제거되었는지 확인
    EXPECT_EQ(itemManager->getItemCount(), 0);
}

// 랜덤 아이템 타입에 SPEED 포함 테스트
TEST_F(ItemManagerTest, RandomItemTypeIncludesSpeedTest) {
    // 여러 번 랜덤 타입을 생성하여 SPEED가 포함되는지 확인
    bool speedFound = false;
    for (int i = 0; i < 100; ++i) {
        ItemType type = itemManager->getRandomItemType();
        if (type == ItemType::SPEED) {
            speedFound = true;
            break;
        }
    }
    EXPECT_TRUE(speedFound);
} 