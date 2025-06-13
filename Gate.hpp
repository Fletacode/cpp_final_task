#ifndef GATE_HPP
#define GATE_HPP

#include "Snake.hpp"  // Position 구조체 사용
#include <chrono>

enum class GateType {
    ENTRANCE,
    EXIT
};

enum class WallType {
    OUTER,  // 외곽 벽
    INNER   // 내부 벽
};

class Gate {
public:
    Gate(int x, int y, GateType type, WallType wallType, int pairId = 0, int originalWallValue = 1);
    ~Gate();

    // 위치 관련
    int getX() const { return position.x; }
    int getY() const { return position.y; }
    Position getPosition() const { return position; }

    // 타입 관련
    GateType getType() const { return type; }
    WallType getWallType() const { return wallType; }
    
    bool isEntrance() const { return type == GateType::ENTRANCE; }
    bool isExit() const { return type == GateType::EXIT; }
    bool isOuterWall() const { return wallType == WallType::OUTER; }
    bool isInnerWall() const { return wallType == WallType::INNER; }

    // 쌍 관련
    int getPairId() const { return pairId; }

    // 원래 벽 값 관련
    int getOriginalWallValue() const { return originalWallValue; }

    // 시간 관련
    std::chrono::steady_clock::time_point getCreationTime() const { return creationTime; }
    bool isExpired() const;
    static constexpr int GATE_DURATION_SECONDS = 10;

private:
    Position position;
    GateType type;
    WallType wallType;
    int pairId;  // 게이트 쌍 식별자
    int originalWallValue;  // 원래 벽 값 (1: Wall, 2: Immune Wall)
    std::chrono::steady_clock::time_point creationTime;
};

#endif // GATE_HPP 