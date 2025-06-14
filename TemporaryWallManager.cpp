#include "TemporaryWallManager.hpp"
#include <algorithm>

TemporaryWallManager::TemporaryWallManager(GameMap& map) : gameMap(map) {
}

TemporaryWallManager::~TemporaryWallManager() {
    // vector는 자동으로 메모리 해제
}

void TemporaryWallManager::addTemporaryWall(Position pos, std::chrono::milliseconds lifetime) {
    // 유효하지 않은 위치는 무시
    if (!isValidPosition(pos)) {
        return;
    }
    
    // 같은 위치에 이미 임시 벽이 있다면 제거
    removeWallAt(pos);
    
    // 새로운 임시 벽 추가
    temporaryWalls.emplace_back(pos, lifetime);
}

void TemporaryWallManager::update() {
    removeExpiredWalls();
}

void TemporaryWallManager::updateMap() {
    // 현재 모든 임시 벽을 맵에 반영
    for (const auto& wall : temporaryWalls) {
        Position pos = wall.getPosition();
        if (isValidPosition(pos)) {
            gameMap.setTemporaryWall(pos.x, pos.y);
        }
    }
}

void TemporaryWallManager::clear() {
    temporaryWalls.clear();
}

bool TemporaryWallManager::hasTemporaryWallAt(Position pos) const {
    return std::any_of(temporaryWalls.begin(), temporaryWalls.end(),
        [pos](const TemporaryWall& wall) {
            Position wallPos = wall.getPosition();
            return wallPos.x == pos.x && wallPos.y == pos.y;
        });
}

void TemporaryWallManager::removeExpiredWalls() {
    temporaryWalls.erase(
        std::remove_if(temporaryWalls.begin(), temporaryWalls.end(),
            [](const TemporaryWall& wall) {
                return wall.isExpired();
            }),
        temporaryWalls.end()
    );
}

bool TemporaryWallManager::isValidPosition(Position pos) const {
    return gameMap.isValidPosition(pos.x, pos.y);
}

void TemporaryWallManager::removeWallAt(Position pos) {
    temporaryWalls.erase(
        std::remove_if(temporaryWalls.begin(), temporaryWalls.end(),
            [pos](const TemporaryWall& wall) {
                Position wallPos = wall.getPosition();
                return wallPos.x == pos.x && wallPos.y == pos.y;
            }),
        temporaryWalls.end()
    );
} 