#include "Game.hpp"
#include <chrono>
#include <thread>

Game::Game(int width, int height) 
    : map(width, height), snake(width/2, height/2), gameOver(false) {
}

Game::~Game() {
    // 자동으로 메모리 해제
}

void Game::update() {
    if (gameOver) return;
    
    // Snake 이동
    snake.move();
    
    // 충돌 감지
    if (checkWallCollision() || snake.checkSelfCollision()) {
        gameOver = true;
        return;
    }
    
    // 맵 업데이트
    updateMap();
}

void Game::handleInput(int key) {
    switch (key) {
        case KEY_UP:
            snake.setDirection(Direction::UP);
            break;
        case KEY_DOWN:
            snake.setDirection(Direction::DOWN);
            break;
        case KEY_LEFT:
            snake.setDirection(Direction::LEFT);
            break;
        case KEY_RIGHT:
            snake.setDirection(Direction::RIGHT);
            break;
        case 'q':
        case 'Q':
            gameOver = true;
            break;
    }
}

void Game::draw() {
    map.draw();
}

void Game::run() {
    // ncurses 초기화
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);  // non-blocking input
    
    updateMap();
    
    auto lastUpdate = std::chrono::steady_clock::now();
    const auto tickDuration = std::chrono::milliseconds(200);  // 200ms마다 업데이트
    
    while (!gameOver) {
        auto now = std::chrono::steady_clock::now();
        
        // 키 입력 처리
        int key = getch();
        if (key != ERR) {
            handleInput(key);
        }
        
        // 일정 시간마다 게임 업데이트
        if (now - lastUpdate >= tickDuration) {
            update();
            draw();
            lastUpdate = now;
        }
        
        // CPU 사용률 줄이기
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // 게임 오버 메시지 표시
    mvprintw(map.getHeight()/2, map.getWidth()/2 - 5, "GAME OVER!");
    mvprintw(map.getHeight()/2 + 1, map.getWidth()/2 - 10, "Press any key to exit");
    refresh();
    
    // 키 입력 대기
    nodelay(stdscr, FALSE);
    getch();
    
    // ncurses 종료
    endwin();
}

bool Game::checkWallCollision() const {
    int headX = snake.getHeadX();
    int headY = snake.getHeadY();
    
    // 맵 경계 확인
    if (headX < 0 || headX >= map.getWidth() || 
        headY < 0 || headY >= map.getHeight()) {
        return true;
    }
    
    // 벽과의 충돌 확인
    int cellValue = map.getCellValue(headX, headY);
    return cellValue == 1 || cellValue == 2;  // Wall 또는 Immune Wall
}

void Game::updateMap() {
    // 맵 초기화 (벽 제외하고 모든 셀을 0으로)
    for (int y = 1; y < map.getHeight() - 1; y++) {
        for (int x = 1; x < map.getWidth() - 1; x++) {
            if (map.getCellValue(x, y) != 1 && map.getCellValue(x, y) != 2) {
                map.setCellValue(x, y, 0);
            }
        }
    }
    
    // Snake 위치 업데이트
    const auto& body = snake.getBody();
    for (size_t i = 0; i < body.size(); i++) {
        int x = body[i].x;
        int y = body[i].y;
        
        if (map.isValidPosition(x, y)) {
            if (i == 0) {
                map.setSnakeHead(x, y);  // 머리
            } else {
                map.setSnakeBody(x, y);  // 몸통
            }
        }
    }
} 