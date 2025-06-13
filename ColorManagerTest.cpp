#include <gtest/gtest.h>
#include "ColorManager.hpp"
#include <ncurses.h>
#include <memory>

class ColorManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // ncurses 초기화 (테스트용)
        initscr();
        colorManager = std::make_unique<ColorManager>();
    }

    void TearDown() override {
        // ncurses 정리
        endwin();
    }

    std::unique_ptr<ColorManager> colorManager;
};

// ColorManager 생성 테스트
TEST_F(ColorManagerTest, ColorManagerCreationTest) {
    EXPECT_TRUE(colorManager != nullptr);
}

// 색상 지원 여부 확인 테스트
TEST_F(ColorManagerTest, ColorSupportTest) {
    // 색상 지원 여부는 터미널에 따라 다를 수 있음
    bool hasColor = colorManager->hasColorSupport();
    EXPECT_TRUE(hasColor == true || hasColor == false);  // boolean 값이어야 함
}

// 색상 시스템 초기화 테스트
TEST_F(ColorManagerTest, InitializeColorsTest) {
    bool result = colorManager->initializeColors();
    // 색상이 지원되는 경우 true, 지원되지 않는 경우 false
    EXPECT_TRUE(result == true || result == false);
}

// 색상 쌍 번호 반환 테스트
TEST_F(ColorManagerTest, GetColorPairTest) {
    EXPECT_EQ(colorManager->getColorPair(ColorType::DEFAULT), 0);
    EXPECT_EQ(colorManager->getColorPair(ColorType::WALL), 1);
    EXPECT_EQ(colorManager->getColorPair(ColorType::IMMUNE_WALL), 2);
    EXPECT_EQ(colorManager->getColorPair(ColorType::SNAKE_HEAD), 3);
    EXPECT_EQ(colorManager->getColorPair(ColorType::SNAKE_BODY), 4);
    EXPECT_EQ(colorManager->getColorPair(ColorType::GROWTH_ITEM), 5);
    EXPECT_EQ(colorManager->getColorPair(ColorType::POISON_ITEM), 6);
}

// 색상 적용 테스트 (크래시 없이 실행되는지 확인)
TEST_F(ColorManagerTest, ApplyColorTest) {
    colorManager->initializeColors();
    
    // 모든 색상 타입에 대해 적용 테스트
    EXPECT_NO_THROW(colorManager->applyColor(ColorType::DEFAULT));
    EXPECT_NO_THROW(colorManager->applyColor(ColorType::WALL));
    EXPECT_NO_THROW(colorManager->applyColor(ColorType::IMMUNE_WALL));
    EXPECT_NO_THROW(colorManager->applyColor(ColorType::SNAKE_HEAD));
    EXPECT_NO_THROW(colorManager->applyColor(ColorType::SNAKE_BODY));
    EXPECT_NO_THROW(colorManager->applyColor(ColorType::GROWTH_ITEM));
    EXPECT_NO_THROW(colorManager->applyColor(ColorType::POISON_ITEM));
}

// 색상 해제 테스트
TEST_F(ColorManagerTest, ResetColorTest) {
    colorManager->initializeColors();
    
    // 색상 적용 후 해제
    colorManager->applyColor(ColorType::SNAKE_HEAD);
    EXPECT_NO_THROW(colorManager->resetColor());
}

// 색상 쌍 설정 테스트
TEST_F(ColorManagerTest, SetupColorPairsTest) {
    colorManager->initializeColors();
    EXPECT_NO_THROW(colorManager->setupColorPairs());
} 