#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <vector>
#include <ncurses.h>

class GameMap {
public:
    GameMap(int width, int height);
    ~GameMap();

    // 맵 크기 getter
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // 셀 값 getter/setter
    int getCellValue(int x, int y) const;
    void setCellValue(int x, int y, int value);

    // 특정 타입의 셀 설정
    void setWall(int x, int y);
    void setSnakeHead(int x, int y);
    void setSnakeBody(int x, int y);

    // 위치 유효성 검사
    bool isValidPosition(int x, int y) const;

    // 맵 그리기
    void draw() const;

private:
    int width;
    int height;
    std::vector<std::vector<int>> map;

    void initializeMap();
};

#endif // GAME_MAP_HPP 