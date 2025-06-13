#include "Mission.hpp"
#include <algorithm>

Mission::Mission(MissionType missionType, int target, const std::string& desc)
    : type(missionType), targetValue(target), currentValue(0), description(desc) {
}

MissionType Mission::getType() const {
    return type;
}

int Mission::getTargetValue() const {
    return targetValue;
}

int Mission::getCurrentValue() const {
    return currentValue;
}

std::string Mission::getDescription() const {
    return description;
}

void Mission::updateProgress(int value) {
    currentValue = value;
}

float Mission::getProgress() const {
    if (targetValue == 0) {
        return 0.0f;
    }
    
    float progress = static_cast<float>(currentValue) / static_cast<float>(targetValue);
    return std::min(progress, 1.0f);  // 최대 1.0으로 제한
}

bool Mission::isCompleted() const {
    return currentValue >= targetValue;
}

void Mission::reset() {
    currentValue = 0;
} 