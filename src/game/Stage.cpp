#include "Stage.hpp"
#include "GameMap.hpp"

Stage::Stage(int number, const std::string& name)
    : stageNumber(number), stageName(name) {
}

int Stage::getStageNumber() const {
    return stageNumber;
}

std::string Stage::getStageName() const {
    return stageName;
}

void Stage::setWallLayout(const std::vector<std::pair<int, int>>& walls) {
    wallLayout = walls;
}

void Stage::applyToMap(GameMap& map) const {
    // 맵을 기본 상태로 초기화 (테두리는 Immune Wall로 유지)
    for (int i = 1; i < 20; i++) {
        for (int j = 1; j < 20; j++) {
            map.setCell(i, j, 0);  // Empty
        }
    }
    
    // 벽 레이아웃 적용
    for (const auto& wall : wallLayout) {
        int x = wall.first;
        int y = wall.second;
        if (x >= 1 && x < 20 && y >= 1 && y < 20) {
            map.setCell(x, y, 1);  // Wall
        }
    }
}

void Stage::addMission(MissionType type, int targetValue, const std::string& description) {
    missionManager.addMission(type, targetValue, description);
}

void Stage::updateMissionProgress(MissionType type, int currentValue) {
    missionManager.updateMissionProgress(type, currentValue);
}

bool Stage::allMissionsCompleted() const {
    return missionManager.allMissionsCompleted();
}

int Stage::getMissionCount() const {
    return missionManager.getMissionCount();
}

int Stage::getCompletedMissionCount() const {
    return missionManager.getCompletedMissionCount();
}

float Stage::getOverallProgress() const {
    return missionManager.getOverallProgress();
}

const Mission* Stage::getMission(int index) const {
    return missionManager.getMission(index);
}

void Stage::resetMissions() {
    missionManager.resetAllMissions();
} 