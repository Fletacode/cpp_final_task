#ifndef GATE_MANAGER_HPP
#define GATE_MANAGER_HPP

#include "Gate.hpp"
#include "GameMap.hpp"
#include "Snake.hpp"
#include <vector>
#include <optional>
#include <random>
#include <map>

class GateManager {
public:
    GateManager(GameMap& map);
    ~GateManager();

    // Gate 관리
    void generateGates(const Snake& snake);
    void removeExpiredGates();
    void restoreGatePositionsToWalls();  // 게이트 위치를 원래 벽으로 복원
    void updateMap();
    
    // Gate 정보
    int getGateCount() const { return gates.size(); }
    const std::vector<Gate>& getGates() const { return gates; }
    
    // 충돌 감지
    std::optional<Gate> checkCollision(const Snake& snake);
    
    // Gate 이동 로직
    Position calculateExitPosition(const Gate& entrance, Direction snakeDirection, const Snake& snake);
    Position calculateBidirectionalExitPosition(const Gate& currentGate, Direction snakeDirection, const Snake& snake);
    
    // 새로운 Gate 진입 로직 메서드들
    void setSnakeEntering(const Position& gatePos, bool entering);
    bool isSnakeEntering(const Position& gatePos) const;
    Direction calculateOuterWallExitDirection(const Position& gatePos, Direction entryDirection);
    std::vector<Direction> calculateInnerWallDirectionPriority(const Position& gatePos, Direction entryDirection);
    Direction applyInnerWallSpecialRules(const Position& gatePos, Direction entryDirection, bool isHorizontalExit);
    
    // Direction 유틸리티 함수들
    Direction getOppositeDirection(Direction dir);
    Direction getClockwiseDirection(Direction dir);
    Direction getCounterClockwiseDirection(Direction dir);
    
    // 테스트용 public 함수
    bool isSameOuterWall(const Position& pos1, const Position& pos2);
    
    static constexpr int MAX_GATES = 1;  // 최대 Gate 수 (입구/출구 쌍)

private:
    GameMap& map;
    std::vector<Gate> gates;
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;
    int nextPairId;  // 다음 게이트 쌍 ID
    
    // Snake 진입 상태 추적
    std::map<Position, bool> snakeEnteringStates;
    
    // Gate 생성 헬퍼 메서드
    std::vector<Position> findWallPositions(const Snake& snake);
    WallType determineWallType(int x, int y);
    bool isValidGatePosition(int x, int y, const Snake& snake);
    Position findValidExitPosition(const Position& entrance, Direction preferredDirection, const Snake& snake);
    std::vector<Direction> getDirectionPriority(const Position& gatePos, Direction snakeDirection);
};

#endif // GATE_MANAGER_HPP 