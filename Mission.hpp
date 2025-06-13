#ifndef MISSION_HPP
#define MISSION_HPP

#include <string>

enum class MissionType {
    LENGTH,
    GROWTH_ITEMS,
    POISON_ITEMS,
    GATES
};

class Mission {
private:
    MissionType type;
    int targetValue;
    int currentValue;
    std::string description;

public:
    // 생성자
    Mission(MissionType missionType, int target, const std::string& desc);

    // Getter 메서드
    MissionType getType() const;
    int getTargetValue() const;
    int getCurrentValue() const;
    std::string getDescription() const;

    // 진행도 관리
    void updateProgress(int value);
    float getProgress() const;
    bool isCompleted() const;

    // 초기화
    void reset();
};

#endif // MISSION_HPP 