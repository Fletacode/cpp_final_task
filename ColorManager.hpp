#ifndef COLORMANAGER_HPP
#define COLORMANAGER_HPP

#include <ncurses.h>

// 색상 타입 열거형
enum class ColorType {
    DEFAULT,      // 기본 색상
    WALL,         // 일반 벽 (#)
    IMMUNE_WALL,  // 면역 벽 (*)
    SNAKE_HEAD,   // 뱀 머리 (@)
    SNAKE_BODY,   // 뱀 몸통 (o)
    GROWTH_ITEM,  // 성장 아이템 (+)
    POISON_ITEM,  // 독 아이템 (-)
    GATE          // 게이트 (G)
};

// 색상 관리 클래스
class ColorManager {
private:
    bool colorSupported;  // 색상 지원 여부
    
    // 색상 쌍 상수
    static const int WALL_PAIR = 1;
    static const int IMMUNE_WALL_PAIR = 2;
    static const int SNAKE_HEAD_PAIR = 3;
    static const int SNAKE_BODY_PAIR = 4;
    static const int GROWTH_ITEM_PAIR = 5;
    static const int POISON_ITEM_PAIR = 6;
    static const int GATE_PAIR = 7;

public:
    // 생성자
    ColorManager();
    
    // 색상 시스템 초기화
    bool initializeColors();
    
    // 색상 쌍 설정
    void setupColorPairs();
    
    // 색상 적용
    void applyColor(ColorType colorType);
    
    // 색상 해제 (기본 색상으로 복원)
    void resetColor();
    
    // 색상 지원 여부 확인
    bool hasColorSupport() const;
    
    // 특정 색상 타입의 색상 쌍 번호 반환
    int getColorPair(ColorType colorType) const;
};

#endif // COLORMANAGER_HPP 