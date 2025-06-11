#include "GameMap.hpp"

GameMap::GameMap(int width, int height) : width(width), height(height) {
    map.resize(height, std::vector<int>(width, 0));
    initializeMap();
}

GameMap::~GameMap() {
    // vector는 자동으로 메모리 해제
}

void GameMap::initializeMap() {
    // 맵 테두리를 Immune Wall(2)로 초기화
    for (int i = 0; i < width; i++) {
        map[0][i] = 2;      // 상단 벽
        map[height-1][i] = 2; // 하단 벽
    }
    for (int i = 0; i < height; i++) {
        map[i][0] = 2;      // 좌측 벽
        map[i][width-1] = 2; // 우측 벽
    }
}

int GameMap::getCellValue(int x, int y) const {
    if (!isValidPosition(x, y)) return -1;
    return map[y][x];
}

void GameMap::setCellValue(int x, int y, int value) {
    if (!isValidPosition(x, y)) return;
    map[y][x] = value;
}

void GameMap::setWall(int x, int y) {
    setCellValue(x, y, 1);
}

void GameMap::setSnakeHead(int x, int y) {
    setCellValue(x, y, 3);
}

void GameMap::setSnakeBody(int x, int y) {
    setCellValue(x, y, 4);
}

bool GameMap::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void GameMap::draw() const {
    clear();  // 화면 지우기

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            move(y, x);  // 커서 이동
            switch (map[y][x]) {
                case 0:  // 빈 공간
                    addch(' ');
                    break;
                case 1:  // Wall
                    addch('#');
                    break;
                case 2:  // Immune Wall
                    addch('*');
                    break;
                case 3:  // Snake Head
                    addch('@');
                    break;
                case 4:  // Snake Body
                    addch('o');
                    break;
                default:
                    addch('?');
            }
        }
    }
    refresh();  // 화면 갱신
} 