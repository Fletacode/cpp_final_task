#include <gtest/gtest.h>
#include "Item.hpp"
#include "Snake.hpp"  // Position 구조체를 위해 필요
#include <chrono>
#include <thread>

class ItemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 테스트 설정
    }

    void TearDown() override {
        // 테스트 정리
    }
};

// Item 생성 테스트
TEST_F(ItemTest, ItemCreationTest) {
    Item growthItem(5, 10, ItemType::GROWTH);
    EXPECT_EQ(growthItem.getX(), 5);
    EXPECT_EQ(growthItem.getY(), 10);
    EXPECT_EQ(growthItem.getType(), ItemType::GROWTH);
    
    Item poisonItem(15, 20, ItemType::POISON);
    EXPECT_EQ(poisonItem.getX(), 15);
    EXPECT_EQ(poisonItem.getY(), 20);
    EXPECT_EQ(poisonItem.getType(), ItemType::POISON);
}

// Item 위치 테스트
TEST_F(ItemTest, ItemPositionTest) {
    Item item(7, 8, ItemType::GROWTH);
    Position pos = item.getPosition();
    EXPECT_EQ(pos.x, 7);
    EXPECT_EQ(pos.y, 8);
}

// Item 만료 시간 테스트
TEST_F(ItemTest, ItemExpirationTest) {
    Item item(5, 5, ItemType::GROWTH);
    
    // 생성 직후에는 만료되지 않음
    EXPECT_FALSE(item.isExpired());
    
    // 짧은 시간 대기 후에도 만료되지 않음
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(item.isExpired());
}

// Item 만료 시간 설정 테스트
TEST_F(ItemTest, ItemCustomExpirationTest) {
    // 매우 짧은 만료 시간으로 아이템 생성 (테스트용)
    Item item(5, 5, ItemType::GROWTH, std::chrono::milliseconds(50));
    
    // 생성 직후에는 만료되지 않음
    EXPECT_FALSE(item.isExpired());
    
    // 만료 시간보다 긴 시간 대기
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(item.isExpired());
}

// Item 타입별 테스트
TEST_F(ItemTest, ItemTypeTest) {
    Item growthItem(0, 0, ItemType::GROWTH);
    Item poisonItem(0, 0, ItemType::POISON);
    
    EXPECT_TRUE(growthItem.getType() == ItemType::GROWTH);
    EXPECT_TRUE(poisonItem.getType() == ItemType::POISON);
    EXPECT_FALSE(growthItem.getType() == ItemType::POISON);
    EXPECT_FALSE(poisonItem.getType() == ItemType::GROWTH);
} 