#include "GameMap.hpp"

GameMap::GameMap(int width, int height) : width(width), height(height), colorManager(nullptr) {
    map.resize(height, std::vector<int>(width, 0));
    initializeMap();
}

GameMap::~GameMap() {
    // vector는 자동으로 메모리 해제
}

void GameMap::setColorManager(std::shared_ptr<ColorManager> colorMgr) {
    colorManager = colorMgr;
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

void GameMap::setGate(int x, int y) {
    setCellValue(x, y, 7);
}

bool GameMap::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

std::optional<std::pair<int, int>> GameMap::findSafePosition() const {
    // 뱀의 초기 길이는 3이므로, 머리 위치에서 왼쪽으로 2칸이 필요
    // 따라서 x는 최소 3, 최대 width-1이어야 함
    // y는 벽에서 최소 1칸 떨어져야 하므로 최소 1, 최대 height-2
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 3; x < width - 1; x++) {
            // 머리, 몸통, 꼬리 위치가 모두 빈 공간인지 확인
            if (map[y][x] == 0 &&           // 머리 위치
                map[y][x-1] == 0 &&         // 몸통 위치
                map[y][x-2] == 0) {         // 꼬리 위치
                
                return std::make_pair(x, y);
            }
        }
    }
    
    // 안전한 위치를 찾지 못한 경우
    return std::nullopt;
}

void GameMap::draw() const {
    clear();  // 화면 지우기

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            move(y, x);  // 커서 이동
            
            // 색상 적용
            if (colorManager) {
                switch (map[y][x]) {
                    case 1:  // Wall
                        colorManager->applyColor(ColorType::WALL);
                        break;
                    case 2:  // Immune Wall
                        colorManager->applyColor(ColorType::IMMUNE_WALL);
                        break;
                    case 3:  // Snake Head
                        colorManager->applyColor(ColorType::SNAKE_HEAD);
                        break;
                    case 4:  // Snake Body
                        colorManager->applyColor(ColorType::SNAKE_BODY);
                        break;
                    case 5:  // Growth Item
                        colorManager->applyColor(ColorType::GROWTH_ITEM);
                        break;
                    case 6:  // Poison Item
                        colorManager->applyColor(ColorType::POISON_ITEM);
                        break;
                    case 7:  // Gate
                        colorManager->applyColor(ColorType::GATE);
                        break;
                    default:
                        colorManager->applyColor(ColorType::DEFAULT);
                        break;
                }
            }
            
            // 문자 출력
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
                case 5:  // Growth Item
                    addch('+');
                    break;
                case 6:  // Poison Item
                    addch('-');
                    break;
                case 7:  // Gate
                    addch('G');
                    break;
                default:
                    addch('?');
            }
            
            // 색상 해제
            if (colorManager) {
                colorManager->resetColor();
            }
        }
    }
    refresh();  // 화면 갱신
} 