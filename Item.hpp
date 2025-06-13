#ifndef ITEM_HPP
#define ITEM_HPP

#include <chrono>

// Position 구조체는 Snake.hpp에서 가져옴
struct Position;  // 전방 선언

// 아이템 타입 열거형
enum class ItemType {
    GROWTH,  // 성장 아이템 (+)
    POISON   // 독 아이템 (-)
};

// 아이템 클래스
class Item {
private:
    int x, y;  // 아이템 위치
    ItemType type;  // 아이템 타입
    std::chrono::steady_clock::time_point creationTime;  // 생성 시간
    std::chrono::milliseconds duration;  // 지속 시간

public:
    // 생성자 (기본 지속시간 5초)
    Item(int x, int y, ItemType type, 
         std::chrono::milliseconds duration = std::chrono::seconds(5));
    
    // 위치 관련 메서드
    int getX() const;
    int getY() const;
    Position getPosition() const;
    
    // 타입 관련 메서드
    ItemType getType() const;
    
    // 만료 관련 메서드
    bool isExpired() const;
    std::chrono::milliseconds getRemainingTime() const;
};

#endif // ITEM_HPP 