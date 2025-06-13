#include "Gate.hpp"

Gate::Gate(int x, int y, GateType type, WallType wallType, int pairId, int originalWallValue)
    : position(x, y), type(type), wallType(wallType), pairId(pairId), originalWallValue(originalWallValue), 
      creationTime(std::chrono::steady_clock::now()) {
}

Gate::~Gate() {
    // 자동으로 메모리 해제
}

bool Gate::isExpired() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - creationTime);
    return elapsed.count() >= GATE_DURATION_SECONDS;
} 