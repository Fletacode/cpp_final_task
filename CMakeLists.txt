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
target_link_libraries(game_map color_manager ${CURSES_LIBRARIES})

add_library(snake Snake.cpp)

add_library(item Item.cpp)

add_library(gate Gate.cpp)
target_link_libraries(gate snake)

add_library(temporary_wall TemporaryWall.cpp)
target_link_libraries(temporary_wall snake)

add_library(temporary_wall_manager TemporaryWallManager.cpp)
target_link_libraries(temporary_wall_manager temporary_wall game_map)

add_library(gate_manager GateManager.cpp)
target_link_libraries(gate_manager gate game_map snake)

add_library(item_manager ItemManager.cpp)
target_link_libraries(item_manager item game_map snake)

add_library(color_manager ColorManager.cpp)
target_link_libraries(color_manager ${CURSES_LIBRARIES})

add_library(score_manager ScoreManager.cpp)

add_library(mission Mission.cpp)

add_library(mission_manager MissionManager.cpp)
target_link_libraries(mission_manager mission)

add_library(stage Stage.cpp)
target_link_libraries(stage mission_manager game_map)

add_library(stage_manager StageManager.cpp)
target_link_libraries(stage_manager stage)

add_library(game Game.cpp)
target_link_libraries(game game_map snake item_manager gate_manager temporary_wall_manager color_manager score_manager stage_manager ${CURSES_LIBRARIES})

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

add_executable(item_test ItemTest.cpp)
target_link_libraries(item_test
    item
    GTest::gtest_main
)

add_executable(item_manager_test ItemManagerTest.cpp)
target_link_libraries(item_manager_test
    item_manager
    GTest::gtest_main
)

add_executable(color_manager_test ColorManagerTest.cpp)
target_link_libraries(color_manager_test
    color_manager
    GTest::gtest_main
)

add_executable(gate_test GateTest.cpp)
target_link_libraries(gate_test
    gate
    GTest::gtest_main
)

add_executable(temporary_wall_test TemporaryWallTest.cpp)
target_link_libraries(temporary_wall_test
    temporary_wall
    GTest::gtest_main
)

add_executable(temporary_wall_manager_test TemporaryWallManagerTest.cpp)
target_link_libraries(temporary_wall_manager_test
    temporary_wall_manager
    GTest::gtest_main
)

add_executable(gate_manager_test GateManagerTest.cpp)
target_link_libraries(gate_manager_test
    gate_manager
    GTest::gtest_main
)

add_executable(score_manager_test ScoreManagerTest.cpp)
target_link_libraries(score_manager_test
    score_manager
    GTest::gtest_main
)

add_executable(mission_test MissionTest.cpp)
target_link_libraries(mission_test
    mission
    GTest::gtest_main
)

add_executable(mission_manager_test MissionManagerTest.cpp)
target_link_libraries(mission_manager_test
    mission_manager
    GTest::gtest_main
)

add_executable(stage_test StageTest.cpp)
target_link_libraries(stage_test
    stage
    GTest::gtest_main
)

add_executable(stage_manager_test StageManagerTest.cpp)
target_link_libraries(stage_manager_test
    stage_manager
    GTest::gtest_main
)

# 메인 프로그램 실행 파일 생성
add_executable(snake_game main.cpp)
target_link_libraries(snake_game
    game_map
    color_manager
)

add_executable(snake_game_v2 main_game.cpp)
target_link_libraries(snake_game_v2
    game
) 