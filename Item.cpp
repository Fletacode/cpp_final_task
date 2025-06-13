#include "Item.hpp"
#include "Snake.hpp"  // Position 구조체를 위해 필요

// 생성자
Item::Item(int x, int y, ItemType type, std::chrono::milliseconds duration)
    : x(x), y(y), type(type), duration(duration) {
    creationTime = std::chrono::steady_clock::now();
}

// 위치 관련 메서드
int Item::getX() const {
    return x;
}

int Item::getY() const {
    return y;
}

Position Item::getPosition() const {
    return Position(x, y);
}

// 타입 관련 메서드
ItemType Item::getType() const {
    return type;
}

// 만료 관련 메서드
bool Item::isExpired() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - creationTime);
    return elapsed >= duration;
}

std::chrono::milliseconds Item::getRemainingTime() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - creationTime);
    auto remaining = duration - elapsed;
    return remaining.count() > 0 ? remaining : std::chrono::milliseconds(0);
} 