#include <gtest/gtest.h>
#include "MissionManager.hpp"

class MissionManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        missionManager = std::make_unique<MissionManager>();
    }

    std::unique_ptr<MissionManager> missionManager;
};

// MissionManager 생성 테스트
TEST_F(MissionManagerTest, MissionManagerCreationTest) {
    EXPECT_TRUE(missionManager != nullptr);
    EXPECT_EQ(missionManager->getMissionCount(), 0);
    EXPECT_TRUE(missionManager->allMissionsCompleted());  // 미션이 없으면 완료로 간주
}

// 미션 추가 테스트
TEST_F(MissionManagerTest, AddMissionTest) {
    missionManager->addMission(MissionType::LENGTH, 10, "Reach length 10");
    EXPECT_EQ(missionManager->getMissionCount(), 1);
    EXPECT_FALSE(missionManager->allMissionsCompleted());
    
    missionManager->addMission(MissionType::GROWTH_ITEMS, 5, "Collect 5 growth items");
    EXPECT_EQ(missionManager->getMissionCount(), 2);
    EXPECT_FALSE(missionManager->allMissionsCompleted());
}

// 미션 진행도 업데이트 테스트
TEST_F(MissionManagerTest, UpdateMissionProgressTest) {
    missionManager->addMission(MissionType::LENGTH, 8, "Reach length 8");
    missionManager->addMission(MissionType::GROWTH_ITEMS, 3, "Collect 3 growth items");
    
    // LENGTH 미션 업데이트
    missionManager->updateMissionProgress(MissionType::LENGTH, 5);
    const Mission* lengthMission = missionManager->getMission(0);
    EXPECT_EQ(lengthMission->getCurrentValue(), 5);
    EXPECT_FALSE(lengthMission->isCompleted());
    
    // GROWTH_ITEMS 미션 업데이트
    missionManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 2);
    const Mission* growthMission = missionManager->getMission(1);
    EXPECT_EQ(growthMission->getCurrentValue(), 2);
    EXPECT_FALSE(growthMission->isCompleted());
}

// 미션 완료 테스트
TEST_F(MissionManagerTest, MissionCompletionTest) {
    missionManager->addMission(MissionType::POISON_ITEMS, 2, "Collect 2 poison items");
    missionManager->addMission(MissionType::GATES, 1, "Use gate 1 time");
    
    EXPECT_FALSE(missionManager->allMissionsCompleted());
    
    // 첫 번째 미션 완료
    missionManager->updateMissionProgress(MissionType::POISON_ITEMS, 2);
    EXPECT_FALSE(missionManager->allMissionsCompleted());  // 아직 하나 더 남음
    
    // 두 번째 미션 완료
    missionManager->updateMissionProgress(MissionType::GATES, 1);
    EXPECT_TRUE(missionManager->allMissionsCompleted());  // 모든 미션 완료
}

// 특정 미션 가져오기 테스트
TEST_F(MissionManagerTest, GetMissionTest) {
    missionManager->addMission(MissionType::LENGTH, 15, "Reach length 15");
    missionManager->addMission(MissionType::GROWTH_ITEMS, 7, "Collect 7 growth items");
    
    const Mission* mission0 = missionManager->getMission(0);
    EXPECT_TRUE(mission0 != nullptr);
    EXPECT_EQ(mission0->getType(), MissionType::LENGTH);
    EXPECT_EQ(mission0->getTargetValue(), 15);
    
    const Mission* mission1 = missionManager->getMission(1);
    EXPECT_TRUE(mission1 != nullptr);
    EXPECT_EQ(mission1->getType(), MissionType::GROWTH_ITEMS);
    EXPECT_EQ(mission1->getTargetValue(), 7);
    
    // 범위 밖 인덱스
    const Mission* invalidMission = missionManager->getMission(2);
    EXPECT_TRUE(invalidMission == nullptr);
}

// 완료된 미션 수 테스트
TEST_F(MissionManagerTest, CompletedMissionCountTest) {
    missionManager->addMission(MissionType::LENGTH, 5, "Reach length 5");
    missionManager->addMission(MissionType::GROWTH_ITEMS, 3, "Collect 3 growth items");
    missionManager->addMission(MissionType::POISON_ITEMS, 1, "Collect 1 poison item");
    
    EXPECT_EQ(missionManager->getCompletedMissionCount(), 0);
    
    // 첫 번째 미션 완료
    missionManager->updateMissionProgress(MissionType::LENGTH, 5);
    EXPECT_EQ(missionManager->getCompletedMissionCount(), 1);
    
    // 두 번째 미션 완료
    missionManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 3);
    EXPECT_EQ(missionManager->getCompletedMissionCount(), 2);
    
    // 세 번째 미션 완료
    missionManager->updateMissionProgress(MissionType::POISON_ITEMS, 1);
    EXPECT_EQ(missionManager->getCompletedMissionCount(), 3);
}

// 미션 리셋 테스트
TEST_F(MissionManagerTest, ResetMissionsTest) {
    missionManager->addMission(MissionType::LENGTH, 10, "Reach length 10");
    missionManager->addMission(MissionType::GATES, 2, "Use gates 2 times");
    
    // 진행도 설정
    missionManager->updateMissionProgress(MissionType::LENGTH, 7);
    missionManager->updateMissionProgress(MissionType::GATES, 1);
    
    EXPECT_EQ(missionManager->getMission(0)->getCurrentValue(), 7);
    EXPECT_EQ(missionManager->getMission(1)->getCurrentValue(), 1);
    
    // 리셋
    missionManager->resetAllMissions();
    
    EXPECT_EQ(missionManager->getMission(0)->getCurrentValue(), 0);
    EXPECT_EQ(missionManager->getMission(1)->getCurrentValue(), 0);
    EXPECT_FALSE(missionManager->allMissionsCompleted());
}

// 전체 진행도 테스트
TEST_F(MissionManagerTest, OverallProgressTest) {
    missionManager->addMission(MissionType::LENGTH, 10, "Reach length 10");
    missionManager->addMission(MissionType::GROWTH_ITEMS, 4, "Collect 4 growth items");
    
    // 초기 진행도는 0
    EXPECT_FLOAT_EQ(missionManager->getOverallProgress(), 0.0f);
    
    // 첫 번째 미션 50% 완료
    missionManager->updateMissionProgress(MissionType::LENGTH, 5);
    EXPECT_FLOAT_EQ(missionManager->getOverallProgress(), 0.25f);  // (0.5 + 0) / 2 = 0.25
    
    // 두 번째 미션 100% 완료
    missionManager->updateMissionProgress(MissionType::GROWTH_ITEMS, 4);
    EXPECT_FLOAT_EQ(missionManager->getOverallProgress(), 0.75f);  // (0.5 + 1.0) / 2 = 0.75
    
    // 첫 번째 미션도 100% 완료
    missionManager->updateMissionProgress(MissionType::LENGTH, 10);
    EXPECT_FLOAT_EQ(missionManager->getOverallProgress(), 1.0f);   // (1.0 + 1.0) / 2 = 1.0
}

// 미션 클리어 테스트
TEST_F(MissionManagerTest, ClearMissionsTest) {
    missionManager->addMission(MissionType::LENGTH, 5, "Reach length 5");
    missionManager->addMission(MissionType::GROWTH_ITEMS, 2, "Collect 2 growth items");
    
    EXPECT_EQ(missionManager->getMissionCount(), 2);
    
    missionManager->clearAllMissions();
    
    EXPECT_EQ(missionManager->getMissionCount(), 0);
    EXPECT_TRUE(missionManager->allMissionsCompleted());
} 