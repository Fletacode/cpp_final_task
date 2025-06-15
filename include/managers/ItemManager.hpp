#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP

#include "Item.hpp"
#include "GameMap.hpp"
#include "Snake.hpp"
#include <vector>
#include <random>
#include <optional>
#include <chrono>
#include <thread>

class ItemManager {
private:
    GameMap& gameMap;  // 게임 맵 참조
    std::vector<Item> items;  // 현재 활성 아이템들
    std::random_device rd;  // 랜덤 시드
    std::mt19937 gen;  // 랜덤 엔진
    
    static const int MAX_ITEMS = 3;  // 최대 아이템 수

public:
    // 생성자
    explicit ItemManager(GameMap& gameMap);
    
    // 아이템 관리 메서드
    void generateItems(const Snake& snake);  // 아이템 생성
    void addItem(int x, int y, ItemType type, 
                 std::chrono::milliseconds duration = std::chrono::seconds(5));
    void removeExpiredItems();  // 만료된 아이템 제거
    void updateMap();  // 맵에 아이템 위치 업데이트
    
    // 충돌 감지
    std::optional<Item> checkCollision(const Snake& snake);
    
    // 위치 관련 메서드
    std::optional<Position> findEmptyPosition(const Snake& snake);
    bool isPositionValid(int x, int y, const Snake& snake) const;
    
    // 정보 조회 메서드
    int getItemCount() const;
    const std::vector<Item>& getItems() const;
    
    // 랜덤 타입 생성
    ItemType getRandomItemType();
};

#endif // ITEMMANAGER_HPP 