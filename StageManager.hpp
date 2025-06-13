#ifndef STAGEMANAGER_HPP
#define STAGEMANAGER_HPP

#include "Stage.hpp"
#include <vector>
#include <memory>

class GameMap;

class StageManager {
private:
    std::vector<std::unique_ptr<Stage>> stages;
    int currentStageIndex;

    void initializeStages();

public:
    // 생성자
    StageManager();

    // 스테이지 정보
    int getCurrentStageNumber() const;
    int getTotalStageCount() const;
    const Stage* getCurrentStage() const;
    bool isLastStage() const;

    // 스테이지 전환
    bool nextStage();
    void resetGame();
    void resetCurrentStage();

    // 맵 적용
    void applyCurrentStageToMap(GameMap& map) const;

    // 미션 관리
    void updateMissionProgress(MissionType type, int currentValue);
    bool isCurrentStageCompleted() const;
    bool isGameCompleted() const;
};

#endif // STAGEMANAGER_HPP 