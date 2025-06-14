#include <gtest/gtest.h>
#include "ScoreManager.hpp"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>

class ScoreManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        scoreManager = std::make_unique<ScoreManager>();
        testFilename = "test_scores.txt";
    }

    void TearDown() override {
        // 테스트 파일 정리
        if (std::filesystem::exists(testFilename)) {
            std::filesystem::remove(testFilename);
        }
    }

    std::unique_ptr<ScoreManager> scoreManager;
    std::string testFilename;
};

// ScoreManager 생성 테스트
TEST_F(ScoreManagerTest, ScoreManagerCreationTest) {
    EXPECT_TRUE(scoreManager != nullptr);
    EXPECT_EQ(scoreManager->getCurrentLength(), 3);  // 초기 Snake 길이
    EXPECT_EQ(scoreManager->getMaxLength(), 3);
    EXPECT_EQ(scoreManager->getGrowthItemsCollected(), 0);
    EXPECT_EQ(scoreManager->getPoisonItemsCollected(), 0);
    EXPECT_EQ(scoreManager->getGatesUsed(), 0);
    EXPECT_EQ(scoreManager->getTotalScore(), 30);  // 초기 길이 3 * 10점
}

// Snake 길이 업데이트 테스트
TEST_F(ScoreManagerTest, SnakeLengthUpdateTest) {
    scoreManager->updateSnakeLength(5);
    EXPECT_EQ(scoreManager->getCurrentLength(), 5);
    EXPECT_EQ(scoreManager->getMaxLength(), 5);
    
    scoreManager->updateSnakeLength(4);  // 길이 감소
    EXPECT_EQ(scoreManager->getCurrentLength(), 4);
    EXPECT_EQ(scoreManager->getMaxLength(), 5);  // 최대값은 유지
    
    scoreManager->updateSnakeLength(7);  // 새로운 최대값
    EXPECT_EQ(scoreManager->getCurrentLength(), 7);
    EXPECT_EQ(scoreManager->getMaxLength(), 7);
}

// Growth Item 수집 테스트
TEST_F(ScoreManagerTest, GrowthItemCollectionTest) {
    EXPECT_EQ(scoreManager->getGrowthItemsCollected(), 0);
    
    scoreManager->incrementGrowthItems();
    EXPECT_EQ(scoreManager->getGrowthItemsCollected(), 1);
    
    scoreManager->incrementGrowthItems();
    scoreManager->incrementGrowthItems();
    EXPECT_EQ(scoreManager->getGrowthItemsCollected(), 3);
}

// Poison Item 수집 테스트
TEST_F(ScoreManagerTest, PoisonItemCollectionTest) {
    EXPECT_EQ(scoreManager->getPoisonItemsCollected(), 0);
    
    scoreManager->incrementPoisonItems();
    EXPECT_EQ(scoreManager->getPoisonItemsCollected(), 1);
    
    scoreManager->incrementPoisonItems();
    EXPECT_EQ(scoreManager->getPoisonItemsCollected(), 2);
}

// Gate 사용 테스트
TEST_F(ScoreManagerTest, GateUsageTest) {
    EXPECT_EQ(scoreManager->getGatesUsed(), 0);
    
    scoreManager->incrementGatesUsed();
    EXPECT_EQ(scoreManager->getGatesUsed(), 1);
    
    scoreManager->incrementGatesUsed();
    scoreManager->incrementGatesUsed();
    EXPECT_EQ(scoreManager->getGatesUsed(), 3);
}

// 점수 계산 테스트
TEST_F(ScoreManagerTest, ScoreCalculationTest) {
    scoreManager->incrementGrowthItems();
    scoreManager->incrementPoisonItems();
    scoreManager->incrementGatesUsed();
    
    // 초기 길이 3 * 10점 + Growth(50) + Poison(30) + Gate(100)
    int expectedScore = 3 * 10 + 1 * 50 + 1 * 30 + 1 * 100;
    EXPECT_EQ(scoreManager->getTotalScore(), expectedScore);
}

// 스테이지별 카운터 초기화 테스트
TEST_F(ScoreManagerTest, ResetStageSpecificCountersTest) {
    // 모든 값들을 증가시킴
    scoreManager->updateSnakeLength(10);
    scoreManager->incrementGrowthItems();
    scoreManager->incrementGrowthItems();
    scoreManager->incrementPoisonItems();
    scoreManager->incrementGatesUsed();
    
    // 초기화 전 값 확인
    EXPECT_EQ(scoreManager->getCurrentLength(), 10);
    EXPECT_EQ(scoreManager->getMaxLength(), 10);
    EXPECT_EQ(scoreManager->getGrowthItemsCollected(), 2);
    EXPECT_EQ(scoreManager->getPoisonItemsCollected(), 1);
    EXPECT_EQ(scoreManager->getGatesUsed(), 1);
    
    // 스테이지별 카운터만 초기화
    scoreManager->resetStageSpecificCounters();
    
    // Growth Items와 Gates만 초기화되고 나머지는 유지
    EXPECT_EQ(scoreManager->getCurrentLength(), 10);  // 유지
    EXPECT_EQ(scoreManager->getMaxLength(), 10);      // 유지
    EXPECT_EQ(scoreManager->getGrowthItemsCollected(), 0);  // 초기화
    EXPECT_EQ(scoreManager->getPoisonItemsCollected(), 1);  // 유지
    EXPECT_EQ(scoreManager->getGatesUsed(), 0);       // 초기화
}

// 파일 저장 테스트
TEST_F(ScoreManagerTest, SaveToFileTest) {
    // 점수 데이터 설정
    scoreManager->updateSnakeLength(8);
    scoreManager->incrementGrowthItems();
    scoreManager->incrementGrowthItems();
    scoreManager->incrementPoisonItems();
    scoreManager->incrementGatesUsed();
    
    // 파일 저장
    scoreManager->saveToFile(testFilename);
    
    // 파일이 생성되었는지 확인
    EXPECT_TRUE(std::filesystem::exists(testFilename));
    
    // 파일 내용 확인
    std::ifstream file(testFilename);
    EXPECT_TRUE(file.is_open());
    
    std::string line;
    bool foundCurrentLength = false;
    bool foundMaxLength = false;
    bool foundGrowthItems = false;
    
    while (std::getline(file, line)) {
        if (line.find("current_length=8") != std::string::npos) foundCurrentLength = true;
        if (line.find("max_length=8") != std::string::npos) foundMaxLength = true;
        if (line.find("growth_items=2") != std::string::npos) foundGrowthItems = true;
    }
    
    EXPECT_TRUE(foundCurrentLength);
    EXPECT_TRUE(foundMaxLength);
    EXPECT_TRUE(foundGrowthItems);
}

// 파일 로드 테스트
TEST_F(ScoreManagerTest, LoadFromFileTest) {
    // 테스트 파일 생성
    std::ofstream file(testFilename);
    file << "current_length=10\n";
    file << "max_length=12\n";
    file << "growth_items=5\n";
    file << "poison_items=2\n";
    file << "gates_used=3\n";
    file.close();
    
    // 파일에서 로드
    scoreManager->loadFromFile(testFilename);
    
    // 로드된 데이터 확인
    EXPECT_EQ(scoreManager->getCurrentLength(), 10);
    EXPECT_EQ(scoreManager->getMaxLength(), 12);
    EXPECT_EQ(scoreManager->getGrowthItemsCollected(), 5);
    EXPECT_EQ(scoreManager->getPoisonItemsCollected(), 2);
    EXPECT_EQ(scoreManager->getGatesUsed(), 3);
}

// 존재하지 않는 파일 로드 테스트
TEST_F(ScoreManagerTest, LoadNonExistentFileTest) {
    // 존재하지 않는 파일 로드 시도
    scoreManager->loadFromFile("nonexistent_file.txt");
    
    // 기본값 유지 확인
    EXPECT_EQ(scoreManager->getCurrentLength(), 3);
    EXPECT_EQ(scoreManager->getMaxLength(), 3);
    EXPECT_EQ(scoreManager->getGrowthItemsCollected(), 0);
    EXPECT_EQ(scoreManager->getPoisonItemsCollected(), 0);
    EXPECT_EQ(scoreManager->getGatesUsed(), 0);
}

// 생존시간 설정 테스트
TEST_F(ScoreManagerTest, SurvivalTimeSetupTest) {
    // 게임 시작 시간 설정
    scoreManager->setGameStartTime();
    
    // 초기 생존시간은 0초여야 함
    EXPECT_EQ(scoreManager->getSurvivalTimeSeconds(), 0);
    
    // 포맷된 시간도 00:00이어야 함
    EXPECT_EQ(scoreManager->getFormattedSurvivalTime(), "00:00");
}

// 생존시간 계산 테스트
TEST_F(ScoreManagerTest, SurvivalTimeCalculationTest) {
    // 게임 시작 시간을 과거로 설정 (2초 전)
    auto pastTime = std::chrono::steady_clock::now() - std::chrono::seconds(2);
    scoreManager->setGameStartTime(pastTime);
    
    // 생존시간이 2초 정도여야 함 (약간의 오차 허용)
    int survivalTime = scoreManager->getSurvivalTimeSeconds();
    EXPECT_GE(survivalTime, 1);  // 최소 1초
    EXPECT_LE(survivalTime, 3);  // 최대 3초 (오차 고려)
}

// 생존시간 포맷팅 테스트
TEST_F(ScoreManagerTest, SurvivalTimeFormattingTest) {
    // 65초 전으로 설정 (1분 5초)
    auto pastTime = std::chrono::steady_clock::now() - std::chrono::seconds(65);
    scoreManager->setGameStartTime(pastTime);
    
    std::string formatted = scoreManager->getFormattedSurvivalTime();
    
    // 01:05 형식이어야 함 (약간의 오차 허용)
    EXPECT_TRUE(formatted.find("01:0") == 0);  // 1분대여야 함
}

// 생존시간 리셋 테스트
TEST_F(ScoreManagerTest, SurvivalTimeResetTest) {
    // 시작 시간 설정 후 잠시 대기 (1초로 증가)
    scoreManager->setGameStartTime();
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    
    // 생존시간이 0보다 커야 함
    EXPECT_GT(scoreManager->getSurvivalTimeSeconds(), 0);
    
    // 리셋 후 다시 시작
    scoreManager->reset();
    
    // 리셋 후 생존시간은 다시 0이어야 함 (reset에서 자동으로 시작시간 설정됨)
    EXPECT_EQ(scoreManager->getSurvivalTimeSeconds(), 0);
}

// 생존시간 파일 저장/로드 테스트
TEST_F(ScoreManagerTest, SurvivalTimeFileIOTest) {
    // 시작 시간을 과거로 설정
    auto pastTime = std::chrono::steady_clock::now() - std::chrono::seconds(30);
    scoreManager->setGameStartTime(pastTime);
    
    // 파일 저장
    scoreManager->saveToFile(testFilename);
    
    // 새로운 ScoreManager로 로드
    auto newScoreManager = std::make_unique<ScoreManager>();
    newScoreManager->loadFromFile(testFilename);
    
    // 생존시간이 복원되어야 함 (약간의 오차 허용)
    int originalTime = scoreManager->getSurvivalTimeSeconds();
    int loadedTime = newScoreManager->getSurvivalTimeSeconds();
    
    // 로드된 시간이 원래 시간과 비슷해야 함 (±2초 오차 허용)
    EXPECT_GE(loadedTime, originalTime - 2);
    EXPECT_LE(loadedTime, originalTime + 2);
} 