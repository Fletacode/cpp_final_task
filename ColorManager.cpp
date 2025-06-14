#include "ColorManager.hpp"

// 생성자
ColorManager::ColorManager() : colorSupported(false) {
    // 색상 지원 여부는 initializeColors()에서 확인
}

// 색상 시스템 초기화
bool ColorManager::initializeColors() {
    // 색상 지원 여부 확인 (initscr() 이후에 호출되어야 함)
    colorSupported = has_colors();
    
    if (!colorSupported) {
        return false;
    }
    
    // 색상 시스템 시작
    if (start_color() == ERR) {
        colorSupported = false;
        return false;
    }
    
    // 색상 쌍 설정
    setupColorPairs();
    return true;
}

// 색상 쌍 설정
void ColorManager::setupColorPairs() {
    if (!colorSupported) {
        return;
    }
    
    // 색상 쌍 정의
    init_pair(WALL_PAIR, COLOR_WHITE, COLOR_BLACK);         // 일반 벽: 흰색
    init_pair(IMMUNE_WALL_PAIR, COLOR_RED, COLOR_BLACK);    // 면역 벽: 빨간색
    init_pair(SNAKE_HEAD_PAIR, COLOR_GREEN, COLOR_BLACK);   // 뱀 머리: 녹색
    init_pair(SNAKE_BODY_PAIR, COLOR_CYAN, COLOR_BLACK);    // 뱀 몸통: 청록색
    init_pair(GROWTH_ITEM_PAIR, COLOR_YELLOW, COLOR_BLACK); // 성장 아이템: 노란색
    init_pair(POISON_ITEM_PAIR, COLOR_MAGENTA, COLOR_BLACK); // 독 아이템: 자홍색
    init_pair(GATE_PAIR, COLOR_BLUE, COLOR_BLACK);          // 게이트: 파란색
    init_pair(SPEED_ITEM_PAIR, COLOR_WHITE, COLOR_BLACK);   // 속도 아이템: 흰색
}

// 색상 적용
void ColorManager::applyColor(ColorType colorType) {
    if (!colorSupported) {
        return;
    }
    
    int pairNumber = getColorPair(colorType);
    if (pairNumber > 0) {
        attron(COLOR_PAIR(pairNumber));
    }
}

// 색상 해제 (기본 색상으로 복원)
void ColorManager::resetColor() {
    if (!colorSupported) {
        return;
    }
    
    // 모든 색상 속성 해제
    attroff(COLOR_PAIR(WALL_PAIR));
    attroff(COLOR_PAIR(IMMUNE_WALL_PAIR));
    attroff(COLOR_PAIR(SNAKE_HEAD_PAIR));
    attroff(COLOR_PAIR(SNAKE_BODY_PAIR));
    attroff(COLOR_PAIR(GROWTH_ITEM_PAIR));
    attroff(COLOR_PAIR(POISON_ITEM_PAIR));
    attroff(COLOR_PAIR(GATE_PAIR));
    attroff(COLOR_PAIR(SPEED_ITEM_PAIR));
}

// 색상 지원 여부 확인
bool ColorManager::hasColorSupport() const {
    return colorSupported;
}

// 특정 색상 타입의 색상 쌍 번호 반환
int ColorManager::getColorPair(ColorType colorType) const {
    switch (colorType) {
        case ColorType::DEFAULT:
            return 0;
        case ColorType::WALL:
            return WALL_PAIR;
        case ColorType::IMMUNE_WALL:
            return IMMUNE_WALL_PAIR;
        case ColorType::SNAKE_HEAD:
            return SNAKE_HEAD_PAIR;
        case ColorType::SNAKE_BODY:
            return SNAKE_BODY_PAIR;
        case ColorType::GROWTH_ITEM:
            return GROWTH_ITEM_PAIR;
        case ColorType::POISON_ITEM:
            return POISON_ITEM_PAIR;
        case ColorType::GATE:
            return GATE_PAIR;
        case ColorType::SPEED_ITEM:
            return SPEED_ITEM_PAIR;
        default:
            return 0;
    }
} 