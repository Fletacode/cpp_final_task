#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <vector>

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class Snake {
public:
    Snake(int startX, int startY);
    ~Snake();

    // 이동 관련
    void move();
    void setDirection(Direction newDirection);
    Direction getDirection() const { return direction; }

    // 위치 관련
    int getHeadX() const { return body[0].x; }
    int getHeadY() const { return body[0].y; }
    const std::vector<Position>& getBody() const { return body; }

    // 크기 관련
    int getLength() const { return body.size(); }
    void grow();

    // 충돌 감지
    bool checkSelfCollision() const;

private:
    std::vector<Position> body;
    Direction direction;
    bool shouldGrow;

    bool isOppositeDirection(Direction newDir) const;
};

#endif // SNAKE_HPP 