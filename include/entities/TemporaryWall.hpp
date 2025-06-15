#ifndef TEMPORARY_WALL_HPP
#define TEMPORARY_WALL_HPP

#include "Snake.hpp"  // Position 구조체 사용
#include <chrono>

class TemporaryWall {
public:
    TemporaryWall(Position position, std::chrono::milliseconds lifetime);
    ~TemporaryWall();

    // 위치 관련
    Position getPosition() const { return position; }
    int getX() const { return position.x; }
    int getY() const { return position.y; }

    // 시간 관련
    std::chrono::steady_clock::time_point getCreationTime() const { return creationTime; }
    bool isExpired() const;
    std::chrono::milliseconds getLifetime() const { return lifetime; }

private:
    Position position;
    std::chrono::steady_clock::time_point creationTime;
    std::chrono::milliseconds lifetime;
};

#endif // TEMPORARY_WALL_HPP 