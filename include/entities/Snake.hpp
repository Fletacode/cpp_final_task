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
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
    bool operator<(const Position& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
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
    
    // 아이템 효과 적용
    void applyGrowthItem();
    bool applyPoisonItem();  // 길이가 최소값 미만이 되면 false 반환
    int getMinLength() const { return 3; }
    Position getHead() const { return body[0]; }

    // 충돌 감지
    bool checkSelfCollision() const;
    
    // Gate 이동
    void teleportTo(const Position& newPosition);
    
    // 리셋
    void reset(int startX, int startY);

private:
    std::vector<Position> body;
    Direction direction;
    bool shouldGrow;

    bool isOppositeDirection(Direction newDir) const;
};

#endif // SNAKE_HPP 