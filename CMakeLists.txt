cmake_minimum_required(VERSION 3.10)
project(SnakeGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ncurses 라이브러리 찾기
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Google Test 설치
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# 라이브러리 생성
add_library(game_map GameMap.cpp)
target_link_libraries(game_map ${CURSES_LIBRARIES})

add_library(snake Snake.cpp)

add_library(game Game.cpp)
target_link_libraries(game game_map snake ${CURSES_LIBRARIES})

# 테스트 실행 파일 생성
add_executable(game_map_test GameMapTest.cpp)
target_link_libraries(game_map_test
    game_map
    GTest::gtest_main
)

add_executable(snake_test SnakeTest.cpp)
target_link_libraries(snake_test
    snake
    GTest::gtest_main
)

add_executable(game_test GameTest.cpp)
target_link_libraries(game_test
    game
    GTest::gtest_main
)

# 메인 프로그램 실행 파일 생성
add_executable(snake_game main.cpp)
target_link_libraries(snake_game
    game_map
)

add_executable(snake_game_v2 main_game.cpp)
target_link_libraries(snake_game_v2
    game
) 