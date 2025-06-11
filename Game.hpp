#ifndef GAME_HPP
#define GAME_HPP

#include "GameMap.hpp"
#include "Snake.hpp"
#include <ncurses.h>

class Game {
public:
    Game(int width, int height);
    ~Game();

    // 게임 상태
    bool isGameOver() const { return gameOver; }
    
    // 게임 객체 접근
    Snake& getSnake() { return snake; }
    const Snake& getSnake() const { return snake; }
    GameMap& getMap() { return map; }

    // 게임 로직
    void update();
    void handleInput(int key);
    void draw();

    // 게임 루프
    void run();

private:
    GameMap map;
    Snake snake;
    bool gameOver;

    // 충돌 감지
    bool checkWallCollision() const;
    void updateMap();
};

#endif // GAME_HPP 