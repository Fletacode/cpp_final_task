#include "GateManager.hpp"
#include <algorithm>
#include <chrono>
#include <set>

GateManager::GateManager(GameMap& map) 
    : map(map), rng(std::random_device{}()), dist(0, 100), nextPairId(1) {
}

GateManager::~GateManager() {
    // vector는 자동으로 메모리 해제
}

void GateManager::generateGates(const Snake& snake) {
    // 이미 게이트가 존재하면 생성하지 않음
    if (gates.size() >= MAX_GATES * 2) {
        return;
    }
    
    // Snake가 어떤 Gate에 진입 중이면 새로운 Gate 생성하지 않음
    for (const auto& [pos, entering] : snakeEnteringStates) {
        if (entering) {
            return;
        }
    }
    
    // 기존 게이트가 있다면 벽으로 복원하고 제거
    if (!gates.empty()) {
        restoreGatePositionsToWalls();
    }
    
    // 벽 위치 찾기
    std::vector<Position> wallPositions = findWallPositions(snake);
    
    if (wallPositions.size() < 2) {
        return;  // 게이트를 생성할 수 있는 벽이 충분하지 않음
    }
    
    // 랜덤하게 입구 위치 선택
    std::uniform_int_distribution<int> wallDist(0, wallPositions.size() - 1);
    Position entrancePos = wallPositions[wallDist(rng)];
    
    // 입구 게이트의 벽 타입 결정
    WallType entranceWallType = determineWallType(entrancePos.x, entrancePos.y);
    
    // 입구 위치의 원래 벽 값 저장
    int entranceOriginalValue = map.getCellValue(entrancePos.x, entrancePos.y);
    
    // 출구 위치 찾기 (입구와 다른 위치)
    Position exitPos = findValidExitPosition(entrancePos, Direction::UP, snake);
    
    if (exitPos.x == -1 || exitPos.y == -1) {
        return;  // 유효한 출구 위치를 찾을 수 없음
    }
    
    // 출구 게이트의 벽 타입 결정
    WallType exitWallType = determineWallType(exitPos.x, exitPos.y);
    
    // 출구 위치의 원래 벽 값 저장
    int exitOriginalValue = map.getCellValue(exitPos.x, exitPos.y);
    
    // 게이트 쌍 생성
    int pairId = nextPairId++;
    gates.emplace_back(entrancePos.x, entrancePos.y, GateType::ENTRANCE, entranceWallType, pairId, entranceOriginalValue);
    gates.emplace_back(exitPos.x, exitPos.y, GateType::EXIT, exitWallType, pairId, exitOriginalValue);
    
    // 맵에 게이트 설정
    map.setGate(entrancePos.x, entrancePos.y);
    map.setGate(exitPos.x, exitPos.y);
}

void GateManager::removeExpiredGates() {
    auto currentTime = std::chrono::steady_clock::now();
    
    // 만료된 게이트들의 위치를 벽으로 복원
    for (auto it = gates.begin(); it != gates.end();) {
        Position gatePos = it->getPosition();
        
        // Snake가 진입 중인 Gate는 만료되지 않음
        if (it->isExpired() && !isSnakeEntering(gatePos)) {
            // 게이트 위치를 원래 벽 값으로 복원
            int x = it->getX();
            int y = it->getY();
            int originalValue = it->getOriginalWallValue();
            
            if (originalValue == 1) {
                map.setWall(x, y);
            } else if (originalValue == 2) {
                map.setCellValue(x, y, 2);  // Immune Wall
            }
            
            // 진입 상태 정보도 제거
            snakeEnteringStates.erase(gatePos);
            
            it = gates.erase(it);
        } else {
            ++it;
        }
    }
}

void GateManager::restoreGatePositionsToWalls() {
    // 현재 모든 게이트 위치를 원래 벽으로 복원
    for (const auto& gate : gates) {
        int x = gate.getX();
        int y = gate.getY();
        int originalValue = gate.getOriginalWallValue();
        
        if (originalValue == 1) {
            map.setWall(x, y);
        } else if (originalValue == 2) {
            map.setCellValue(x, y, 2);  // Immune Wall
        }
    }
    
    // 모든 게이트 제거
    gates.clear();
}

void GateManager::updateMap() {
    for (const auto& gate : gates) {
        map.setCellValue(gate.getX(), gate.getY(), 7);  // Gate 값: 7
    }
}

std::optional<Gate> GateManager::checkCollision(const Snake& snake) {
    Position headPos = snake.getHead();
    
    for (const auto& gate : gates) {
        if (gate.getPosition() == headPos) {
            return gate;
        }
    }
    
    return std::nullopt;
}

Position GateManager::calculateExitPosition(const Gate& entrance, Direction snakeDirection, const Snake& snake) {
    // 출구 Gate 찾기
    for (const auto& gate : gates) {
        if (gate.isExit()) {
            Position exitGatePos = gate.getPosition();
            
            // 출구 방향 우선순위 결정
            auto directions = getDirectionPriority(exitGatePos, snakeDirection);
            
            // 각 방향에 대해 유효한 위치 찾기
            for (Direction dir : directions) {
                Position newPos = exitGatePos;
                switch (dir) {
                    case Direction::UP:    newPos.y--; break;
                    case Direction::DOWN:  newPos.y++; break;
                    case Direction::LEFT:  newPos.x--; break;
                    case Direction::RIGHT: newPos.x++; break;
                }
                
                // 유효한 위치인지 확인
                if (map.isValidPosition(newPos.x, newPos.y) && 
                    map.getCellValue(newPos.x, newPos.y) == 0) {
                    
                    // Snake와 겹치지 않는지 확인
                    bool collision = false;
                    for (const auto& bodyPart : snake.getBody()) {
                        if (bodyPart == newPos) {
                            collision = true;
                            break;
                        }
                    }
                    
                    if (!collision) {
                        return newPos;
                    }
                }
            }
            
            // 유효한 위치를 찾지 못한 경우 출구 Gate 위치 반환
            return exitGatePos;
        }
    }
    
    // 출구 Gate를 찾지 못한 경우
    return Position(-1, -1);
}

Position GateManager::calculateBidirectionalExitPosition(const Gate& currentGate, Direction snakeDirection, const Snake& snake) {
    // 현재 게이트와 같은 pairId를 가진 다른 게이트 찾기
    for (const auto& gate : gates) {
        if (gate.getPairId() == currentGate.getPairId() && 
            gate.getPosition() != currentGate.getPosition()) {
            
            Position targetGatePos = gate.getPosition();
            
            // 목표 게이트 주변의 방향 우선순위 결정
            auto directions = getDirectionPriority(targetGatePos, snakeDirection);
            
            // 각 방향에 대해 유효한 위치 찾기
            for (Direction dir : directions) {
                Position newPos = targetGatePos;
                switch (dir) {
                    case Direction::UP:    newPos.y--; break;
                    case Direction::DOWN:  newPos.y++; break;
                    case Direction::LEFT:  newPos.x--; break;
                    case Direction::RIGHT: newPos.x++; break;
                }
                
                // 유효한 위치인지 확인
                if (map.isValidPosition(newPos.x, newPos.y) && 
                    map.getCellValue(newPos.x, newPos.y) == 0) {
                    
                    // Snake와 겹치지 않는지 확인
                    bool collision = false;
                    for (const auto& bodyPart : snake.getBody()) {
                        if (bodyPart == newPos) {
                            collision = true;
                            break;
                        }
                    }
                    
                    if (!collision) {
                        return newPos;
                    }
                }
            }
            
            // 유효한 위치를 찾지 못한 경우 목표 게이트 위치 반환
            return targetGatePos;
        }
    }
    
    // 쌍 게이트를 찾지 못한 경우
    return Position(-1, -1);
}

std::vector<Position> GateManager::findWallPositions(const Snake& snake) {
    std::vector<Position> wallPositions;
    
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            int cellValue = map.getCellValue(x, y);
            if ((cellValue == 1 || cellValue == 2) && isValidGatePosition(x, y, snake)) {
                wallPositions.push_back(Position(x, y));
            }
        }
    }
    
    return wallPositions;
}

WallType GateManager::determineWallType(int x, int y) {
    // 외곽 벽인지 확인 (맵 경계)
    if (x == 0 || x == map.getWidth() - 1 || y == 0 || y == map.getHeight() - 1) {
        return WallType::OUTER;
    }
    return WallType::INNER;
}

bool GateManager::isValidGatePosition(int x, int y, const Snake& snake) {
    // 모서리 좌표 제외 (NxN 맵에서 (0,0), (0,N-1), (N-1,0), (N-1,N-1))
    int width = map.getWidth();
    int height = map.getHeight();
    
    if ((x == 0 && y == 0) ||                           // 좌상단 모서리
        (x == 0 && y == height - 1) ||                  // 좌하단 모서리
        (x == width - 1 && y == 0) ||                   // 우상단 모서리
        (x == width - 1 && y == height - 1)) {          // 우하단 모서리
        return false;
    }
    
    // Snake와 겹치지 않는지 확인
    for (const auto& bodyPart : snake.getBody()) {
        if (bodyPart.x == x && bodyPart.y == y) {
            return false;
        }
    }
    
    // 기존 Gate와 겹치지 않는지 확인
    for (const auto& gate : gates) {
        if (gate.getX() == x && gate.getY() == y) {
            return false;
        }
    }
    
    return true;
}

Position GateManager::findValidExitPosition(const Position& entrance, Direction preferredDirection, const Snake& snake) {
    auto wallPositions = findWallPositions(snake);
    
    // 입구가 외부벽인지 확인
    bool entranceIsOuter = (entrance.x == 0 || entrance.x == map.getWidth() - 1 || 
                           entrance.y == 0 || entrance.y == map.getHeight() - 1);
    
    // 입구와 다른 위치 중에서 선택
    std::vector<Position> validExitPositions;
    for (const auto& pos : wallPositions) {
        if (!(pos == entrance)) {
            // 입구가 외부벽인 경우, 출구는 다른 벽에 위치해야 함
            if (entranceIsOuter) {
                if (!isSameOuterWall(entrance, pos)) {
                    validExitPositions.push_back(pos);
                }
            } else {
                // 내부벽인 경우 기존 로직 유지
                validExitPositions.push_back(pos);
            }
        }
    }
    
    if (validExitPositions.empty()) {
        return Position(-1, -1);
    }
    
    // 랜덤하게 출구 위치 선택
    std::uniform_int_distribution<size_t> exitDist(0, validExitPositions.size() - 1);
    return validExitPositions[exitDist(rng)];
}

std::vector<Direction> GateManager::getDirectionPriority(const Position& gatePos, Direction snakeDirection) {
    std::vector<Direction> directions;
    
    // 외곽 벽인지 확인
    bool isOuter = (gatePos.x == 0 || gatePos.x == map.getWidth() - 1 || 
                    gatePos.y == 0 || gatePos.y == map.getHeight() - 1);
    
    if (isOuter) {
        // 외곽 벽: 고정된 안쪽 방향만 사용 (규칙에 따라)
        Direction fixedDirection = calculateOuterWallExitDirection(gatePos, snakeDirection);
        directions.push_back(fixedDirection);
        
        // 나머지 방향들은 추가하지 않음 (외부벽은 고정 방향만 사용)
        return directions;
    } else {
        // 내부 벽: Snake 진입 방향 우선순위 적용
        auto innerDirections = calculateInnerWallDirectionPriority(gatePos, snakeDirection);
        return innerDirections;
    }
}

// 새로운 Gate 진입 로직 메서드들 구현

void GateManager::setSnakeEntering(const Position& gatePos, bool entering) {
    snakeEnteringStates[gatePos] = entering;
}

bool GateManager::isSnakeEntering(const Position& gatePos) const {
    auto it = snakeEnteringStates.find(gatePos);
    return it != snakeEnteringStates.end() && it->second;
}

Direction GateManager::calculateOuterWallExitDirection(const Position& gatePos, Direction entryDirection) {
    // 외부벽 Gate의 고정 진출 방향
    if (gatePos.y == 0) {
        // 상단 벽 => 아래 방향
        return Direction::DOWN;
    } else if (gatePos.y == map.getHeight() - 1) {
        // 하단 벽 => 위 방향
        return Direction::UP;
    } else if (gatePos.x == 0) {
        // 좌측 벽 => 오른쪽 방향
        return Direction::RIGHT;
    } else if (gatePos.x == map.getWidth() - 1) {
        // 우측 벽 => 왼쪽 방향
        return Direction::LEFT;
    }
    
    // 기본값 (외부벽이 아닌 경우)
    return entryDirection;
}

std::vector<Direction> GateManager::calculateInnerWallDirectionPriority(const Position& gatePos, Direction entryDirection) {
    std::vector<Direction> priorities;
    
    // 1. 진입 방향과 일치하는 방향이 우선
    priorities.push_back(entryDirection);
    
    // 2. 진입 방향의 시계방향으로 회전하는 방향
    priorities.push_back(getClockwiseDirection(entryDirection));
    
    // 3. 진입 방향의 역시계방향으로 회전하는 방향
    priorities.push_back(getCounterClockwiseDirection(entryDirection));
    
    // 4. 진입 방향과 반대방향
    priorities.push_back(getOppositeDirection(entryDirection));
    
    return priorities;
}

Direction GateManager::applyInnerWallSpecialRules(const Position& gatePos, Direction entryDirection, bool isHorizontalExit) {
    if (isHorizontalExit) {
        // 진출 방향이 좌-우인 경우
        switch (entryDirection) {
            case Direction::RIGHT:
                return Direction::RIGHT;  // 오른쪽에서 진입시 => 우로 진출
            case Direction::DOWN:
                return Direction::LEFT;   // 아래로 진입시 => 좌로 진출
            case Direction::LEFT:
                return Direction::LEFT;   // 왼쪽에서 진입시 => 좌로 진출
            case Direction::UP:
                return Direction::LEFT;   // 위로 진입시 => 좌로 진출
        }
    } else {
        // 진출 방향이 상-하인 경우
        switch (entryDirection) {
            case Direction::RIGHT:
                return Direction::UP;     // 오른쪽에서 진입시 => 위로 진출
            case Direction::UP:
                return Direction::UP;     // 위쪽으로 진입시 => 위로 진출
            case Direction::LEFT:
                return Direction::DOWN;   // 왼쪽에서 진입시 => 아래로 진출
            case Direction::DOWN:
                return Direction::DOWN;   // 아래로 진입시 => 아래로 진출
        }
    }
    
    return entryDirection;  // 기본값
}

// Direction 유틸리티 함수들
Direction GateManager::getOppositeDirection(Direction dir) {
    switch (dir) {
        case Direction::UP:    return Direction::DOWN;
        case Direction::DOWN:  return Direction::UP;
        case Direction::LEFT:  return Direction::RIGHT;
        case Direction::RIGHT: return Direction::LEFT;
    }
    return dir;  // 기본값
}

Direction GateManager::getClockwiseDirection(Direction dir) {
    switch (dir) {
        case Direction::UP:    return Direction::RIGHT;
        case Direction::RIGHT: return Direction::DOWN;
        case Direction::DOWN:  return Direction::LEFT;
        case Direction::LEFT:  return Direction::UP;
    }
    return dir;  // 기본값
}

Direction GateManager::getCounterClockwiseDirection(Direction dir) {
    switch (dir) {
        case Direction::UP:    return Direction::LEFT;
        case Direction::LEFT:  return Direction::DOWN;
        case Direction::DOWN:  return Direction::RIGHT;
        case Direction::RIGHT: return Direction::UP;
    }
    return dir;  // 기본값
}

bool GateManager::isSameOuterWall(const Position& pos1, const Position& pos2) {
    int width = map.getWidth();
    int height = map.getHeight();
    
    // 두 위치가 모두 외부벽인지 확인
    bool pos1IsOuter = (pos1.x == 0 || pos1.x == width - 1 || pos1.y == 0 || pos1.y == height - 1);
    bool pos2IsOuter = (pos2.x == 0 || pos2.x == width - 1 || pos2.y == 0 || pos2.y == height - 1);
    
    if (!pos1IsOuter || !pos2IsOuter) {
        return false;  // 둘 중 하나라도 외부벽이 아니면 false
    }
    
    // 같은 외부벽에 있는지 확인
    // 상단 벽 (y == 0)
    if (pos1.y == 0 && pos2.y == 0) return true;
    
    // 하단 벽 (y == height-1)
    if (pos1.y == height - 1 && pos2.y == height - 1) return true;
    
    // 좌측 벽 (x == 0)
    if (pos1.x == 0 && pos2.x == 0) return true;
    
    // 우측 벽 (x == width-1)
    if (pos1.x == width - 1 && pos2.x == width - 1) return true;
    
    return false;  // 다른 벽에 있음
} 