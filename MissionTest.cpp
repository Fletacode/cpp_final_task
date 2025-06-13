#include <gtest/gtest.h>
#include "Mission.hpp"

class MissionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 각 테스트마다 새로운 미션 생성
    }
};

// Mission 생성 테스트
TEST_F(MissionTest, MissionCreationTest) {
    Mission lengthMission(MissionType::LENGTH, 10, "Reach length 10");
    
    EXPECT_EQ(lengthMission.getType(), MissionType::LENGTH);
    EXPECT_EQ(lengthMission.getTargetValue(), 10);
    EXPECT_EQ(lengthMission.getCurrentValue(), 0);
    EXPECT_EQ(lengthMission.getDescription(), "Reach length 10");
    EXPECT_FALSE(lengthMission.isCompleted());
    EXPECT_FLOAT_EQ(lengthMission.getProgress(), 0.0f);
}

// 미션 진행도 업데이트 테스트
TEST_F(MissionTest, MissionProgressUpdateTest) {
    Mission growthMission(MissionType::GROWTH_ITEMS, 5, "Collect 5 growth items");
    
    // 진행도 업데이트
    growthMission.updateProgress(2);
    EXPECT_EQ(growthMission.getCurrentValue(), 2);
    EXPECT_FLOAT_EQ(growthMission.getProgress(), 0.4f);  // 2/5 = 0.4
    EXPECT_FALSE(growthMission.isCompleted());
    
    // 추가 진행
    growthMission.updateProgress(3);
    EXPECT_EQ(growthMission.getCurrentValue(), 3);
    EXPECT_FLOAT_EQ(growthMission.getProgress(), 0.6f);  // 3/5 = 0.6
    EXPECT_FALSE(growthMission.isCompleted());
}

// 미션 완료 테스트
TEST_F(MissionTest, MissionCompletionTest) {
    Mission poisonMission(MissionType::POISON_ITEMS, 3, "Collect 3 poison items");
    
    // 목표 달성
    poisonMission.updateProgress(3);
    EXPECT_EQ(poisonMission.getCurrentValue(), 3);
    EXPECT_FLOAT_EQ(poisonMission.getProgress(), 1.0f);  // 3/3 = 1.0
    EXPECT_TRUE(poisonMission.isCompleted());
    
    // 목표 초과
    poisonMission.updateProgress(5);
    EXPECT_EQ(poisonMission.getCurrentValue(), 5);
    EXPECT_FLOAT_EQ(poisonMission.getProgress(), 1.0f);  // 최대 1.0으로 제한
    EXPECT_TRUE(poisonMission.isCompleted());
}

// Gate 미션 테스트
TEST_F(MissionTest, GateMissionTest) {
    Mission gateMission(MissionType::GATES, 2, "Use gates 2 times");
    
    EXPECT_EQ(gateMission.getType(), MissionType::GATES);
    EXPECT_EQ(gateMission.getTargetValue(), 2);
    
    gateMission.updateProgress(1);
    EXPECT_FALSE(gateMission.isCompleted());
    
    gateMission.updateProgress(2);
    EXPECT_TRUE(gateMission.isCompleted());
}

// 미션 리셋 테스트
TEST_F(MissionTest, MissionResetTest) {
    Mission mission(MissionType::LENGTH, 15, "Reach length 15");
    
    // 진행도 설정
    mission.updateProgress(10);
    EXPECT_EQ(mission.getCurrentValue(), 10);
    EXPECT_FALSE(mission.isCompleted());
    
    // 리셋
    mission.reset();
    EXPECT_EQ(mission.getCurrentValue(), 0);
    EXPECT_FLOAT_EQ(mission.getProgress(), 0.0f);
    EXPECT_FALSE(mission.isCompleted());
}

// 미션 타입별 테스트
TEST_F(MissionTest, MissionTypesTest) {
    Mission lengthMission(MissionType::LENGTH, 8, "Length mission");
    Mission growthMission(MissionType::GROWTH_ITEMS, 4, "Growth mission");
    Mission poisonMission(MissionType::POISON_ITEMS, 2, "Poison mission");
    Mission gateMission(MissionType::GATES, 3, "Gate mission");
    
    EXPECT_EQ(lengthMission.getType(), MissionType::LENGTH);
    EXPECT_EQ(growthMission.getType(), MissionType::GROWTH_ITEMS);
    EXPECT_EQ(poisonMission.getType(), MissionType::POISON_ITEMS);
    EXPECT_EQ(gateMission.getType(), MissionType::GATES);
}

// 진행도 계산 정확성 테스트
TEST_F(MissionTest, ProgressCalculationTest) {
    Mission mission(MissionType::LENGTH, 20, "Reach length 20");
    
    mission.updateProgress(5);
    EXPECT_FLOAT_EQ(mission.getProgress(), 0.25f);  // 5/20 = 0.25
    
    mission.updateProgress(10);
    EXPECT_FLOAT_EQ(mission.getProgress(), 0.5f);   // 10/20 = 0.5
    
    mission.updateProgress(15);
    EXPECT_FLOAT_EQ(mission.getProgress(), 0.75f);  // 15/20 = 0.75
    
    mission.updateProgress(20);
    EXPECT_FLOAT_EQ(mission.getProgress(), 1.0f);   // 20/20 = 1.0
} 