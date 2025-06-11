#include "Snake.hpp"

Snake::Snake(int startX, int startY) : direction(Direction::RIGHT), shouldGrow(false) {
    // 초기 길이 3으로 설정
    body.push_back(Position(startX, startY));       // 머리
    body.push_back(Position(startX - 1, startY));   // 몸통
    body.push_back(Position(startX - 2, startY));   // 꼬리
}

Snake::~Snake() {
    // vector는 자동으로 메모리 해제
}

void Snake::move() {
    Position newHead = body[0];
    
    // 방향에 따라 새로운 머리 위치 계산
    switch (direction) {
        case Direction::UP:
            newHead.y--;
            break;
        case Direction::DOWN:
            newHead.y++;
            break;
        case Direction::LEFT:
            newHead.x--;
            break;
        case Direction::RIGHT:
            newHead.x++;
            break;
    }
    
    // 새로운 머리를 앞에 추가
    body.insert(body.begin(), newHead);
    
    // 성장하지 않는 경우 꼬리 제거
    if (!shouldGrow) {
        body.pop_back();
    } else {
        shouldGrow = false;
    }
}

void Snake::setDirection(Direction newDirection) {
    // 반대 방향으로는 이동할 수 없음
    if (!isOppositeDirection(newDirection)) {
        direction = newDirection;
    }
}

void Snake::grow() {
    shouldGrow = true;
    // 즉시 길이를 늘리기 위해 꼬리 복사
    if (!body.empty()) {
        body.push_back(body.back());
    }
}

bool Snake::checkSelfCollision() const {
    const Position& head = body[0];
    
    // 머리가 몸통과 충돌하는지 확인 (머리 제외)
    for (size_t i = 1; i < body.size(); i++) {
        if (head == body[i]) {
            return true;
        }
    }
    return false;
}

bool Snake::isOppositeDirection(Direction newDir) const {
    switch (direction) {
        case Direction::UP:
            return newDir == Direction::DOWN;
        case Direction::DOWN:
            return newDir == Direction::UP;
        case Direction::LEFT:
            return newDir == Direction::RIGHT;
        case Direction::RIGHT:
            return newDir == Direction::LEFT;
    }
    return false;
} 