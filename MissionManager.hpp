#ifndef MISSIONMANAGER_HPP
#define MISSIONMANAGER_HPP

#include "Mission.hpp"
#include <vector>
#include <memory>

class MissionManager {
private:
    std::vector<std::unique_ptr<Mission>> missions;

public:
    // 생성자
    MissionManager();

    // 미션 관리
    void addMission(MissionType type, int targetValue, const std::string& description);
    void clearAllMissions();
    int getMissionCount() const;

    // 미션 접근
    const Mission* getMission(int index) const;

    // 진행도 업데이트
    void updateMissionProgress(MissionType type, int currentValue);
    void resetAllMissions();

    // 완료 상태 확인
    bool allMissionsCompleted() const;
    int getCompletedMissionCount() const;
    float getOverallProgress() const;
};

#endif // MISSIONMANAGER_HPP 