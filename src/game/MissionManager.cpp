#include "MissionManager.hpp"

MissionManager::MissionManager() {
}

void MissionManager::addMission(MissionType type, int targetValue, const std::string& description) {
    missions.push_back(std::make_unique<Mission>(type, targetValue, description));
}

void MissionManager::clearAllMissions() {
    missions.clear();
}

int MissionManager::getMissionCount() const {
    return static_cast<int>(missions.size());
}

const Mission* MissionManager::getMission(int index) const {
    if (index >= 0 && index < static_cast<int>(missions.size())) {
        return missions[index].get();
    }
    return nullptr;
}

void MissionManager::updateMissionProgress(MissionType type, int currentValue) {
    for (auto& mission : missions) {
        if (mission->getType() == type) {
            mission->updateProgress(currentValue);
        }
    }
}

void MissionManager::resetAllMissions() {
    for (auto& mission : missions) {
        mission->reset();
    }
}

bool MissionManager::allMissionsCompleted() const {
    if (missions.empty()) {
        return true;  // 미션이 없으면 완료로 간주
    }
    
    for (const auto& mission : missions) {
        if (!mission->isCompleted()) {
            return false;
        }
    }
    return true;
}

int MissionManager::getCompletedMissionCount() const {
    int count = 0;
    for (const auto& mission : missions) {
        if (mission->isCompleted()) {
            count++;
        }
    }
    return count;
}

float MissionManager::getOverallProgress() const {
    if (missions.empty()) {
        return 1.0f;  // 미션이 없으면 100% 완료
    }
    
    float totalProgress = 0.0f;
    for (const auto& mission : missions) {
        totalProgress += mission->getProgress();
    }
    
    return totalProgress / static_cast<float>(missions.size());
} 