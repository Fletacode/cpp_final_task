#include "ItemManager.hpp"
#include <algorithm>

// 생성자
ItemManager::ItemManager(GameMap& gameMap) : gameMap(gameMap), gen(rd()) {
    items.reserve(MAX_ITEMS);
}

// 아이템 생성
void ItemManager::generateItems(const Snake& snake) {
    // 최대 아이템 수에 도달했으면 생성하지 않음
    if (items.size() >= MAX_ITEMS) {
        return;
    }
    
    // 빈 공간 찾기
    auto emptyPos = findEmptyPosition(snake);
    if (!emptyPos.has_value()) {
        return;  // 빈 공간이 없으면 생성하지 않음
    }
    
    // 랜덤 타입 선택
    ItemType type = getRandomItemType();
    
    // 아이템 생성
    items.emplace_back(emptyPos->x, emptyPos->y, type);
}

// 아이템 추가 (테스트용)
void ItemManager::addItem(int x, int y, ItemType type, std::chrono::milliseconds duration) {
    if (items.size() < MAX_ITEMS) {
        items.emplace_back(x, y, type, duration);
    }
}

// 만료된 아이템 제거
void ItemManager::removeExpiredItems() {
    items.erase(
        std::remove_if(items.begin(), items.end(),
                      [](const Item& item) { return item.isExpired(); }),
        items.end()
    );
}

// 맵에 아이템 위치 업데이트
void ItemManager::updateMap() {
    // 먼저 기존 아이템 위치를 맵에서 제거 (값 5, 6, 8을 0으로)
    for (int y = 0; y < gameMap.getHeight(); ++y) {
        for (int x = 0; x < gameMap.getWidth(); ++x) {
            int cellValue = gameMap.getCell(x, y);
            if (cellValue == 5 || cellValue == 6 || cellValue == 8) {  // Growth, Poison, Speed Item
                gameMap.setCell(x, y, 0);  // 빈 공간으로 설정
            }
        }
    }
    
    // 현재 아이템들을 맵에 표시
    for (const auto& item : items) {
        int value;
        switch (item.getType()) {
            case ItemType::GROWTH:
                value = 5;
                break;
            case ItemType::POISON:
                value = 6;
                break;
            case ItemType::SPEED:
                value = 8;
                break;
            default:
                value = 0;
                break;
        }
        gameMap.setCell(item.getX(), item.getY(), value);
    }
}

// 충돌 감지
std::optional<Item> ItemManager::checkCollision(const Snake& snake) {
    Position headPos = snake.getHead();
    
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->getPosition() == headPos) {
            Item collectedItem = *it;
            items.erase(it);
            return collectedItem;
        }
    }
    
    return std::nullopt;
}

// 빈 공간 찾기
std::optional<Position> ItemManager::findEmptyPosition(const Snake& snake) {
    std::uniform_int_distribution<int> xDist(1, gameMap.getWidth() - 2);
    std::uniform_int_distribution<int> yDist(1, gameMap.getHeight() - 2);
    
    // 최대 100번 시도
    for (int attempts = 0; attempts < 100; ++attempts) {
        int x = xDist(gen);
        int y = yDist(gen);
        
        if (isPositionValid(x, y, snake)) {
            return Position(x, y);
        }
    }
    
    return std::nullopt;
}

// 위치 유효성 검사
bool ItemManager::isPositionValid(int x, int y, const Snake& snake) const {
    // 맵 경계 확인
    if (x <= 0 || x >= gameMap.getWidth() - 1 || y <= 0 || y >= gameMap.getHeight() - 1) {
        return false;
    }
    
    // 맵에서 빈 공간인지 확인
    if (gameMap.getCell(x, y) != 0) {
        return false;
    }
    
    // Snake 몸통과 겹치지 않는지 확인
    Position pos(x, y);
    const auto& body = snake.getBody();
    for (const auto& segment : body) {
        if (segment == pos) {
            return false;
        }
    }
    
    // 기존 아이템과 겹치지 않는지 확인
    for (const auto& item : items) {
        if (item.getPosition() == pos) {
            return false;
        }
    }
    
    return true;
}

// 정보 조회 메서드
int ItemManager::getItemCount() const {
    return items.size();
}

const std::vector<Item>& ItemManager::getItems() const {
    return items;
}

// 랜덤 타입 생성
ItemType ItemManager::getRandomItemType() {
    std::uniform_int_distribution<int> typeDist(0, 2);  // 0: GROWTH, 1: POISON, 2: SPEED
    int randomValue = typeDist(gen);
    switch (randomValue) {
        case 0:
            return ItemType::GROWTH;
        case 1:
            return ItemType::POISON;
        case 2:
            return ItemType::SPEED;
        default:
            return ItemType::GROWTH;  // 기본값
    }
} 