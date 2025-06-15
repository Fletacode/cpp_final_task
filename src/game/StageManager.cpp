#include "StageManager.hpp"
#include "GameMap.hpp"

StageManager::StageManager() : currentStageIndex(0) {
    initializeStages();
}

void StageManager::initializeStages() {
    // Stage 1: Basic Stage (빈 맵)
    auto stage1 = std::make_unique<Stage>(1, "Basic Stage");
    stage1->addMission(MissionType::GROWTH_ITEMS, 1, "Collect 1 growth item");
    // 벽 레이아웃 없음 (빈 맵)
    stages.push_back(std::move(stage1));

    // Stage 2: Cross Stage (간단한 십자가 맵)
    auto stage2 = std::make_unique<Stage>(2, "Cross Stage");
    stage2->addMission(MissionType::GROWTH_ITEMS, 1, "Collect 1 growth item");
    stage2->addMission(MissionType::GATES, 1, "Use gates 1 time");
    
    // 간단한 십자가 모양 벽 레이아웃 (31x31 맵 중앙)
    std::vector<std::pair<int, int>> crossWalls = {
        // 세로줄 (중앙)
        {15, 8}, {15, 9}, {15, 10}, {15, 11}, {15, 12}, {15, 13}, 
        {15, 14}, {15, 15}, {15, 16}, {15, 17}, {15, 18}, {15, 19}, {15, 20}, {15, 21}, {15, 22},
        // 가로줄 (중앙)
        {8, 15}, {9, 15}, {10, 15}, {11, 15}, {12, 15}, {13, 15}, {14, 15},
        {16, 15}, {17, 15}, {18, 15}, {19, 15}, {20, 15}, {21, 15}, {22, 15}
    };
    stage2->setWallLayout(crossWalls);
    stages.push_back(std::move(stage2));

    // Stage 3: L-Shape Stage (간단한 L자 모양)
    auto stage3 = std::make_unique<Stage>(3, "L-Shape Stage");
    stage3->addMission(MissionType::GROWTH_ITEMS, 1, "Collect 1 growth item");
    stage3->addMission(MissionType::GATES, 1, "Use gates 1 time");
    
    // 간단한 L자 모양 벽 레이아웃
    std::vector<std::pair<int, int>> lShapeWalls = {
        // 세로줄
        {10, 8}, {10, 9}, {10, 10}, {10, 11}, {10, 12}, {10, 13}, {10, 14}, {10, 15},
        // 가로줄
        {10, 15}, {11, 15}, {12, 15}, {13, 15}, {14, 15}, {15, 15}, {16, 15}, {17, 15}
    };
    stage3->setWallLayout(lShapeWalls);
    stages.push_back(std::move(stage3));

    // Stage 4: Box Stage (간단한 사각형 구조)
    auto stage4 = std::make_unique<Stage>(4, "Box Stage");
    stage4->addMission(MissionType::GROWTH_ITEMS, 1, "Collect 1 growth item");
    stage4->addMission(MissionType::GATES, 1, "Use gates 1 time");
    
    // 간단한 사각형 벽 레이아웃
    std::vector<std::pair<int, int>> boxWalls = {
        // 외부 사각형
        {8, 8}, {9, 8}, {10, 8}, {11, 8}, {12, 8}, {13, 8}, {14, 8}, {15, 8}, {16, 8}, {17, 8}, {18, 8}, {19, 8}, {20, 8}, {21, 8}, {22, 8},
        {8, 22}, {9, 22}, {10, 22}, {11, 22}, {12, 22}, {13, 22}, {14, 22}, {15, 22}, {16, 22}, {17, 22}, {18, 22}, {19, 22}, {20, 22}, {21, 22}, {22, 22},
        {8, 9}, {8, 10}, {8, 11}, {8, 12}, {8, 13}, {8, 14}, {8, 15}, {8, 16}, {8, 17}, {8, 18}, {8, 19}, {8, 20}, {8, 21},
        {22, 9}, {22, 10}, {22, 11}, {22, 12}, {22, 13}, {22, 14}, {22, 15}, {22, 16}, {22, 17}, {22, 18}, {22, 19}, {22, 20}, {22, 21},
        
        // 내부 작은 사각형 (중앙에 구멍)
        {13, 13}, {14, 13}, {15, 13}, {16, 13}, {17, 13},
        {13, 17}, {14, 17}, {15, 17}, {16, 17}, {17, 17},
        {13, 14}, {13, 15}, {13, 16},
        {17, 14}, {17, 15}, {17, 16}
    };
    stage4->setWallLayout(boxWalls);
    stages.push_back(std::move(stage4));
}

int StageManager::getCurrentStageNumber() const {
    return currentStageIndex + 1;
}

int StageManager::getTotalStageCount() const {
    return static_cast<int>(stages.size());
}

const Stage* StageManager::getCurrentStage() const {
    if (currentStageIndex >= 0 && currentStageIndex < static_cast<int>(stages.size())) {
        return stages[currentStageIndex].get();
    }
    return nullptr;
}

bool StageManager::isLastStage() const {
    return currentStageIndex == static_cast<int>(stages.size()) - 1;
}

bool StageManager::nextStage() {
    if (currentStageIndex < static_cast<int>(stages.size()) - 1) {
        currentStageIndex++;
        return true;
    }
    return false;
}

void StageManager::resetGame() {
    currentStageIndex = 0;
    for (auto& stage : stages) {
        stage->resetMissions();
    }
}

void StageManager::resetCurrentStage() {
    if (currentStageIndex >= 0 && currentStageIndex < static_cast<int>(stages.size())) {
        stages[currentStageIndex]->resetMissions();
    }
}

void StageManager::applyCurrentStageToMap(GameMap& map) const {
    const Stage* currentStage = getCurrentStage();
    if (currentStage) {
        currentStage->applyToMap(map);
    }
}

void StageManager::updateMissionProgress(MissionType type, int currentValue) {
    Stage* currentStage = stages[currentStageIndex].get();
    if (currentStage) {
        currentStage->updateMissionProgress(type, currentValue);
    }
}

bool StageManager::isCurrentStageCompleted() const {
    const Stage* currentStage = getCurrentStage();
    return currentStage ? currentStage->allMissionsCompleted() : false;
}

bool StageManager::isGameCompleted() const {
    return isLastStage() && isCurrentStageCompleted();
} 