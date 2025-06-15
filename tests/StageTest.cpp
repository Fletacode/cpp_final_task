#include <gtest/gtest.h>
#include "Stage.hpp"
#include "GameMap.hpp"

class StageTest : public ::testing::Test {
protected:
    void SetUp() override {
        gameMap = std::make_unique<GameMap>(31, 31);
    }

    std::unique_ptr<GameMap> gameMap;
};

// Stage 생성 테스트
TEST_F(StageTest, StageCreationTest) {
    Stage stage(1, "Basic Stage");
    
    EXPECT_EQ(stage.getStageNumber(), 1);
    EXPECT_EQ(stage.getStageName(), "Basic Stage");
    EXPECT_EQ(stage.getMissionCount(), 0);
    EXPECT_TRUE(stage.allMissionsCompleted());
}

// 미션 추가 테스트
TEST_F(StageTest, AddMissionTest) {
    Stage stage(2, "Challenge Stage");
    
    stage.addMission(MissionType::LENGTH, 10, "Reach length 10");
    EXPECT_EQ(stage.getMissionCount(), 1);
    EXPECT_FALSE(stage.allMissionsCompleted());
    
    stage.addMission(MissionType::GROWTH_ITEMS, 5, "Collect 5 growth items");
    EXPECT_EQ(stage.getMissionCount(), 2);
    EXPECT_FALSE(stage.allMissionsCompleted());
}

// 벽 레이아웃 설정 테스트
TEST_F(StageTest, WallLayoutTest) {
    Stage stage(1, "Test Stage");
    
    // 십자가 모양 벽 설정
    std::vector<std::pair<int, int>> walls = {
        {10, 5}, {10, 6}, {10, 7}, {10, 8}, {10, 9},  // 세로줄
        {8, 7}, {9, 7}, {11, 7}, {12, 7}              // 가로줄
    };
    
    stage.setWallLayout(walls);
    
    // GameMap에 적용
    stage.applyToMap(*gameMap);
    
    // 벽이 올바르게 설정되었는지 확인
    EXPECT_EQ(gameMap->getCell(10, 5), 1);  // Wall
    EXPECT_EQ(gameMap->getCell(10, 6), 1);  // Wall
    EXPECT_EQ(gameMap->getCell(8, 7), 1);   // Wall
    EXPECT_EQ(gameMap->getCell(9, 7), 1);   // Wall
    
    // 빈 공간 확인
    EXPECT_EQ(gameMap->getCell(5, 5), 0);   // Empty
    EXPECT_EQ(gameMap->getCell(15, 15), 0); // Empty
}

// 미션 진행도 업데이트 테스트
TEST_F(StageTest, MissionProgressTest) {
    Stage stage(3, "Progress Stage");
    
    stage.addMission(MissionType::LENGTH, 8, "Reach length 8");
    stage.addMission(MissionType::POISON_ITEMS, 2, "Collect 2 poison items");
    
    // 미션 진행도 업데이트
    stage.updateMissionProgress(MissionType::LENGTH, 5);
    stage.updateMissionProgress(MissionType::POISON_ITEMS, 1);
    
    EXPECT_FALSE(stage.allMissionsCompleted());
    
    // 모든 미션 완료
    stage.updateMissionProgress(MissionType::LENGTH, 8);
    stage.updateMissionProgress(MissionType::POISON_ITEMS, 2);
    
    EXPECT_TRUE(stage.allMissionsCompleted());
}

// 스테이지 완료 조건 테스트
TEST_F(StageTest, StageCompletionTest) {
    Stage stage(4, "Final Stage");
    
    stage.addMission(MissionType::LENGTH, 15, "Reach length 15");
    stage.addMission(MissionType::GROWTH_ITEMS, 8, "Collect 8 growth items");
    stage.addMission(MissionType::GATES, 3, "Use gates 3 times");
    
    EXPECT_FALSE(stage.allMissionsCompleted());
    EXPECT_EQ(stage.getCompletedMissionCount(), 0);
    
    // 첫 번째 미션 완료
    stage.updateMissionProgress(MissionType::LENGTH, 15);
    EXPECT_EQ(stage.getCompletedMissionCount(), 1);
    EXPECT_FALSE(stage.allMissionsCompleted());
    
    // 두 번째 미션 완료
    stage.updateMissionProgress(MissionType::GROWTH_ITEMS, 8);
    EXPECT_EQ(stage.getCompletedMissionCount(), 2);
    EXPECT_FALSE(stage.allMissionsCompleted());
    
    // 세 번째 미션 완료
    stage.updateMissionProgress(MissionType::GATES, 3);
    EXPECT_EQ(stage.getCompletedMissionCount(), 3);
    EXPECT_TRUE(stage.allMissionsCompleted());
}

// 특정 미션 가져오기 테스트
TEST_F(StageTest, GetMissionTest) {
    Stage stage(2, "Mission Stage");
    
    stage.addMission(MissionType::LENGTH, 12, "Reach length 12");
    stage.addMission(MissionType::GROWTH_ITEMS, 6, "Collect 6 growth items");
    
    const Mission* mission0 = stage.getMission(0);
    EXPECT_TRUE(mission0 != nullptr);
    EXPECT_EQ(mission0->getType(), MissionType::LENGTH);
    EXPECT_EQ(mission0->getTargetValue(), 12);
    
    const Mission* mission1 = stage.getMission(1);
    EXPECT_TRUE(mission1 != nullptr);
    EXPECT_EQ(mission1->getType(), MissionType::GROWTH_ITEMS);
    EXPECT_EQ(mission1->getTargetValue(), 6);
    
    // 범위 밖 인덱스
    const Mission* invalidMission = stage.getMission(2);
    EXPECT_TRUE(invalidMission == nullptr);
}

// 전체 진행도 테스트
TEST_F(StageTest, OverallProgressTest) {
    Stage stage(1, "Progress Test Stage");
    
    stage.addMission(MissionType::LENGTH, 10, "Reach length 10");
    stage.addMission(MissionType::GROWTH_ITEMS, 4, "Collect 4 growth items");
    
    // 초기 진행도
    EXPECT_FLOAT_EQ(stage.getOverallProgress(), 0.0f);
    
    // 첫 번째 미션 50% 완료
    stage.updateMissionProgress(MissionType::LENGTH, 5);
    EXPECT_FLOAT_EQ(stage.getOverallProgress(), 0.25f);  // (0.5 + 0) / 2
    
    // 두 번째 미션 75% 완료
    stage.updateMissionProgress(MissionType::GROWTH_ITEMS, 3);
    EXPECT_FLOAT_EQ(stage.getOverallProgress(), 0.625f); // (0.5 + 0.75) / 2
    
    // 모든 미션 완료
    stage.updateMissionProgress(MissionType::LENGTH, 10);
    stage.updateMissionProgress(MissionType::GROWTH_ITEMS, 4);
    EXPECT_FLOAT_EQ(stage.getOverallProgress(), 1.0f);   // (1.0 + 1.0) / 2
} 