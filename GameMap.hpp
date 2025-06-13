#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <vector>
#include <ncurses.h>
#include "ColorManager.hpp"
#include <memory>
#include <optional>
#include <utility>

class GameMap {
public:
    GameMap(int width, int height);
    ~GameMap();
    
    // 색상 관리자 설정
    void setColorManager(std::shared_ptr<ColorManager> colorMgr);

    // 맵 크기 getter
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // 셀 값 getter/setter
    int getCellValue(int x, int y) const;
    void setCellValue(int x, int y, int value);
    
    // 간편한 셀 접근 메서드 (ItemManager에서 사용)
    int getCell(int x, int y) const { return getCellValue(x, y); }
    void setCell(int x, int y, int value) { setCellValue(x, y, value); }

    // 특정 타입의 셀 설정
    void setWall(int x, int y);
    void setSnakeHead(int x, int y);
    void setSnakeBody(int x, int y);
    void setGate(int x, int y);

    // 위치 유효성 검사
    bool isValidPosition(int x, int y) const;
    
    // 안전한 위치 찾기 (뱀 초기화용)
    std::optional<std::pair<int, int>> findSafePosition() const;

    // 맵 그리기
    void draw() const;

private:
    int width;
    int height;
    std::vector<std::vector<int>> map;
    std::shared_ptr<ColorManager> colorManager;

    void initializeMap();
};

#endif // GAME_MAP_HPP 