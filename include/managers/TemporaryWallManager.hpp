#ifndef TEMPORARY_WALL_MANAGER_HPP
#define TEMPORARY_WALL_MANAGER_HPP

#include "TemporaryWall.hpp"
#include "GameMap.hpp"
#include <vector>
#include <chrono>

class TemporaryWallManager {
public:
    TemporaryWallManager(GameMap& map);
    ~TemporaryWallManager();

    // Temporary Wall 관리
    void addTemporaryWall(Position pos, std::chrono::milliseconds lifetime);
    void update();  // 만료된 벽들을 제거
    void updateMap();  // GameMap에 현재 임시 벽들을 반영
    void clear();  // 모든 임시 벽 제거

    // Temporary Wall 정보
    int getTemporaryWallCount() const { return temporaryWalls.size(); }
    bool hasTemporaryWallAt(Position pos) const;
    const std::vector<TemporaryWall>& getTemporaryWalls() const { return temporaryWalls; }

private:
    GameMap& gameMap;
    std::vector<TemporaryWall> temporaryWalls;
    
    // 헬퍼 메서드
    void removeExpiredWalls();
    bool isValidPosition(Position pos) const;
    void removeWallAt(Position pos);
};

#endif // TEMPORARY_WALL_MANAGER_HPP 