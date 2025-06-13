#ifndef STAGE_HPP
#define STAGE_HPP

#include "MissionManager.hpp"
#include <string>
#include <vector>
#include <utility>

class GameMap;

class Stage {
private:
    int stageNumber;
    std::string stageName;
    std::vector<std::pair<int, int>> wallLayout;
    MissionManager missionManager;

public:
    // 생성자
    Stage(int number, const std::string& name);

    // 스테이지 정보
    int getStageNumber() const;
    std::string getStageName() const;

    // 벽 레이아웃 관리
    void setWallLayout(const std::vector<std::pair<int, int>>& walls);
    void applyToMap(GameMap& map) const;

    // 미션 관리
    void addMission(MissionType type, int targetValue, const std::string& description);
    void updateMissionProgress(MissionType type, int currentValue);
    bool allMissionsCompleted() const;
    int getMissionCount() const;
    int getCompletedMissionCount() const;
    float getOverallProgress() const;
    const Mission* getMission(int index) const;

    // 초기화
    void resetMissions();
};

#endif // STAGE_HPP 