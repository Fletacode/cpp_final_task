#include "TemporaryWall.hpp"

TemporaryWall::TemporaryWall(Position position, std::chrono::milliseconds lifetime)
    : position(position), lifetime(lifetime), creationTime(std::chrono::steady_clock::now()) {
}

TemporaryWall::~TemporaryWall() {
    // 자동으로 메모리 해제
}

bool TemporaryWall::isExpired() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - creationTime);
    return elapsed >= lifetime;
} 