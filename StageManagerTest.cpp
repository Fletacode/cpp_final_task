#include <gtest/gtest.h>
#include "StageManager.hpp"
#include "GameMap.hpp"

class StageManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        stageManager = std::make_unique<StageManager>();
        gameMap = std::make_unique<GameMap>(31, 31);
    }

    std::unique_ptr<StageManager> stageManager;
    std::unique_ptr<GameMap> gameMap;
};

// StageManager 생성 테스트
TEST_F(StageManagerTest, StageManagerCreationTest) {
    EXPECT_TRUE(stageManager != nullptr);
    EXPECT_EQ(stageManager->getCurrentStageNumber(), 1);  // 첫 번째 스테이지부터 시작
    EXPECT_EQ(stageManager->getTotalStageCount(), 4);     // 4개 스테이지
    EXPECT_FALSE(stageManager->isLastStage());
}

// 현재 스테이지 가져오기 테스트
TEST_F(StageManagerTest, GetCurrentStageTest) {
    const Stage* currentStage = stageManager->getCurrentStage();
    EXPECT_TRUE(currentStage != nullptr);
    EXPECT_EQ(currentStage->getStageNumber(), 1);
    EXPECT_EQ(currentStage->getStageName(), "Basic Stage");
}

// 스테이지 전환 테스트
TEST_F(StageManagerTest, NextStageTest) {
    EXPECT_EQ(stageManager->getCurrentStageNumber(), 1);
    EXPECT_FALSE(stageManager->isLastStage());
    
    // 다음 스테이지로 이동
    bool success = stageManager->nextStage();
    EXPECT_TRUE(success);
    EXPECT_EQ(stageManager->getCurrentStageNumber(), 2);
    
    // 또 다음 스테이지로
    success = stageManager->nextStage();
    EXPECT_TRUE(success);
    EXPECT_EQ(stageManager->getCurrentStageNumber(), 3);
    
    // 마지막 스테이지로
    success = stageManager->nextStage();
    EXPECT_TRUE(success);
    EXPECT_EQ(stageManager->getCurrentStageNumber(), 4);
    EXPECT_TRUE(stageManager->isLastStage());
    
    // 마지막 스테이지에서 더 이상 진행 불가
    success = stageManager->nextStage();
    EXPECT_FALSE(success);
    EXPECT_EQ(stageManager->getCurrentStageNumber(), 4);
}

// 스테이지 맵 적용 테스트
TEST_F(StageManagerTest, ApplyCurrentStageToMapTest) {
    // 첫 번째 스테이지 (기본 맵)
    stageManager->applyCurrentStageToMap(*gameMap);
    
    // 기본 맵은 내부가 모두 빈 공간이어야 함
    EXPECT_EQ(gameMap->getCell(15, 15), 0);  // Empty
    EXPECT_EQ(gameMap->getCell(10, 10), 0);  // Empty
    
    // 두 번째 스테이지로 이동 (십자가 맵)
    stageManager->nextStage();
    stageManager->applyCurrentStageToMap(*gameMap);
    
    // 십자가 중앙에 벽이 있어야 함 (31x31 맵에서 중앙은 (15, 15))
    EXPECT_EQ(gameMap->getCell(15, 15), 1);  // Wall (십자가 중심)
}

// 미션 진행도 업데이트 테스트
TEST_F(StageManagerTest, UpdateMissionProgressTest) {
    const Stage* stage1 = stageManager->getCurrentStage();
    EXPECT_FALSE(stage1->allMissionsCompleted());
    
    // 첫 번째 스테이지 미션 진행 (목표: Growth Item 1개)
    // 아직 완료되지 않음
    EXPECT_FALSE(stage1->allMissionsCompleted());
    
    // 미션 완료
    stageManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
    
    EXPECT_TRUE(stage1->allMissionsCompleted());
}

// 스테이지 완료 확인 테스트
TEST_F(StageManagerTest, StageCompletionTest) {
    // 첫 번째 스테이지 미션 완료
    stageManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
    
    EXPECT_TRUE(stageManager->isCurrentStageCompleted());
    
    // 다음 스테이지로 이동
    stageManager->nextStage();
    EXPECT_FALSE(stageManager->isCurrentStageCompleted());  // 새 스테이지는 미완료
}

// 전체 게임 완료 테스트
TEST_F(StageManagerTest, GameCompletionTest) {
    EXPECT_FALSE(stageManager->isGameCompleted());
    
    // 모든 스테이지 완료
    for (int stage = 1; stage <= 4; stage++) {
        const Stage* currentStage = stageManager->getCurrentStage();
        
        // 각 스테이지의 미션을 완료
        if (stage == 1) {
            stageManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
        } else if (stage == 2) {
            stageManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
            stageManager->updateMissionProgress(MissionType::GATES, 1);
        } else if (stage == 3) {
            stageManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
            stageManager->updateMissionProgress(MissionType::GATES, 1);
        } else if (stage == 4) {
            stageManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
            stageManager->updateMissionProgress(MissionType::GATES, 1);
        }
        
        EXPECT_TRUE(currentStage->allMissionsCompleted());
        
        if (stage < 4) {
            stageManager->nextStage();
        }
    }
    
    EXPECT_TRUE(stageManager->isGameCompleted());
}

// 스테이지 리셋 테스트
TEST_F(StageManagerTest, ResetStageTest) {
    // 미션을 완료하여 진행도를 만듦
    stageManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 1);
    
    const Stage* stage = stageManager->getCurrentStage();
    EXPECT_GT(stage->getOverallProgress(), 0.0f);
    EXPECT_TRUE(stage->allMissionsCompleted());
    
    // 스테이지 리셋
    stageManager->resetCurrentStage();
    
    EXPECT_FLOAT_EQ(stage->getOverallProgress(), 0.0f);
    EXPECT_FALSE(stage->allMissionsCompleted());
}

// 전체 게임 리셋 테스트
TEST_F(StageManagerTest, ResetGameTest) {
    // 여러 스테이지 진행
    stageManager->nextStage();
    stageManager->nextStage();
    EXPECT_EQ(stageManager->getCurrentStageNumber(), 3);
    
    // 게임 리셋
    stageManager->resetGame();
    
    EXPECT_EQ(stageManager->getCurrentStageNumber(), 1);
    const Stage* stage = stageManager->getCurrentStage();
    EXPECT_FLOAT_EQ(stage->getOverallProgress(), 0.0f);
}

// 스테이지 정보 가져오기 테스트
TEST_F(StageManagerTest, GetStageInfoTest) {
    // 첫 번째 스테이지
    const Stage* stage1 = stageManager->getCurrentStage();
    EXPECT_EQ(stage1->getStageName(), "Basic Stage");
    
    // 두 번째 스테이지
    stageManager->nextStage();
    const Stage* stage2 = stageManager->getCurrentStage();
    EXPECT_EQ(stage2->getStageName(), "Cross Stage");
    
    // 세 번째 스테이지
    stageManager->nextStage();
    const Stage* stage3 = stageManager->getCurrentStage();
    EXPECT_EQ(stage3->getStageName(), "L-Shape Stage");
    
    // 네 번째 스테이지
    stageManager->nextStage();
    const Stage* stage4 = stageManager->getCurrentStage();
    EXPECT_EQ(stage4->getStageName(), "Box Stage");
} 