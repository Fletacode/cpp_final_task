#include "GameMap.hpp"
#include <ncurses.h>

int main() {
    // ncurses 초기화
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);  // 커서 숨기기

    // 맵 생성
    GameMap map(21, 21);

    // 테스트용 벽 추가
    map.setWall(5, 5);
    map.setWall(5, 6);
    map.setWall(5, 7);

    // 테스트용 뱀 추가
    map.setSnakeHead(10, 10);
    map.setSnakeBody(10, 11);
    map.setSnakeBody(10, 12);

    // 맵 그리기
    map.draw();

    // 아무 키나 누를 때까지 대기
    getch();

    // ncurses 종료
    endwin();
    return 0;
} 