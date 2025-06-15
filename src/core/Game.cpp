#include "Game.hpp"
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>
#include <cstdlib>
#include "Stage.hpp"

// static 멤버 변수 정의
const int Game::baseTickDuration;
const int Game::minTickDuration;

Game::Game(int width, int height) 
    : map(width, height), snake(width/2, height/2), itemManager(map), gateManager(map), 
      temporaryWallManager(map), gameOver(false), gameCompleted(false), 
      currentTickDuration(baseTickDuration), speedBoostCount(0),
      temporaryWallCreationInterval(20000) {  // 20초 간격
    // ColorManager 초기화
    colorManager = std::make_shared<ColorManager>();
    map.setColorManager(colorManager);
    
    // ScoreManager 초기화
    scoreManager.updateSnakeLength(snake.getLength());
    scoreManager.setGameStartTime();  // 게임 시작 시간 설정
    
    // StageManager 초기화 및 첫 번째 스테이지 맵 적용
    stageManager.applyCurrentStageToMap(map);
    
    // 자동 생성 타이머 초기화 (게임 시작 시 즉시 생성 가능하도록)
    lastTemporaryWallCreation = std::chrono::steady_clock::now() - temporaryWallCreationInterval;
}

Game::~Game() {
    // 자동으로 메모리 해제
}

void Game::update() {
    if (gameOver) return;
    
    // Snake 이동
    snake.move();
    
    // Gate 관리
    gateManager.removeExpiredGates();
    gateManager.generateGates(snake);
    
    // Temporary Wall 관리
    temporaryWallManager.update();
    
    // 자동 Temporary Wall 생성 체크
    checkTemporaryWallCreation();
    
    // Gate 충돌 처리 (벽 충돌 감지 이전에 처리)
    handleGateCollision();
    
    // 충돌 감지
    if (checkWallCollision() || snake.checkSelfCollision()) {
        gameOver = true;
        return;
    }
    
    // 아이템 관리
    itemManager.removeExpiredItems();
    itemManager.generateItems(snake);
    
    // 아이템 충돌 처리
    handleItemCollision();
    
    // Snake 길이 업데이트
    scoreManager.updateSnakeLength(snake.getLength());
    
    // 스테이지 완료 확인
    checkStageCompletion();
    
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
        case 't':
        case 'T':
            // Temporary Wall 생성 (뱀 머리 주변에)
            createTemporaryWallAroundSnake();
            break;
    }
}

void Game::draw() {
    map.draw();
    drawScoreBoard();
    drawMissionInfo();
}

void Game::run() {
    // ncurses 초기화
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);  // non-blocking input
    
    // 색상 시스템 초기화
    colorManager->initializeColors();
    
    updateMap();
    
    auto lastUpdate = std::chrono::steady_clock::now();
    
    while (!gameOver) {
        auto now = std::chrono::steady_clock::now();
        
        // 키 입력 처리
        int key = getch();
        if (key != ERR) {
            handleInput(key);
        }
        
        // 일정 시간마다 게임 업데이트 (동적 속도 사용)
        auto tickDuration = std::chrono::milliseconds(currentTickDuration);
        if (now - lastUpdate >= tickDuration) {
            update();
            draw();
            lastUpdate = now;
        }
        
        // CPU 사용률 줄이기
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // 게임 종료 메시지 표시 (클리어 vs 오버 구분)
    if (gameCompleted) {
        // 게임 클리어 메시지
        mvprintw(map.getHeight()/2, map.getWidth()/2 - 8, "CONGRATULATIONS!");
        mvprintw(map.getHeight()/2 + 1, map.getWidth()/2 - 7, "GAME COMPLETED!");
        mvprintw(map.getHeight()/2 + 2, map.getWidth()/2 - 10, "Press any key to exit");
    } else {
        // 게임 오버 메시지
        mvprintw(map.getHeight()/2, map.getWidth()/2 - 5, "GAME OVER!");
        mvprintw(map.getHeight()/2 + 1, map.getWidth()/2 - 10, "Press any key to exit");
    }
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
    
    // 벽과의 충돌 확인 (게이트는 제외)
    int cellValue = map.getCellValue(headX, headY);
    return cellValue == 1 || cellValue == 2 || cellValue == 9;  // Wall, Immune Wall, Temporary Wall 충돌
}

void Game::updateMap() {
    // 맵 초기화 (벽과 Temporary Wall 제외하고 모든 셀을 0으로)
    for (int y = 1; y < map.getHeight() - 1; y++) {
        for (int x = 1; x < map.getWidth() - 1; x++) {
            if (map.getCellValue(x, y) != 1 && map.getCellValue(x, y) != 2 && map.getCellValue(x, y) != 9) {
                map.setCellValue(x, y, 0);
            }
        }
    }
    
    // 아이템 위치 업데이트
    itemManager.updateMap();
    
    // Gate 위치 업데이트
    gateManager.updateMap();
    
    // Temporary Wall 위치 업데이트
    temporaryWallManager.updateMap();
    
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

void Game::handleItemCollision() {
    auto collectedItem = itemManager.checkCollision(snake);
    if (collectedItem.has_value()) {
        switch (collectedItem->getType()) {
            case ItemType::GROWTH:
                snake.applyGrowthItem();
                scoreManager.incrementGrowthItems();
                // 미션 진행상황 업데이트
                stageManager.updateMissionProgress(MissionType::GROWTH_ITEMS, scoreManager.getGrowthItemsCollected());
                break;
            case ItemType::POISON:
                if (!snake.applyPoisonItem()) {
                    // 길이가 최소값 미만이 되면 게임 오버
                    gameOver = true;
                } else {
                    scoreManager.incrementPoisonItems();
                }
                break;
            case ItemType::SPEED:
                // 속도 부스트 적용
                applySpeedBoost();
                // TODO: ScoreManager에 SPEED 아이템 카운터 추가 시 여기서 증가
                break;
        }
    }
}

void Game::handleGateCollision() {
    auto collisionGate = gateManager.checkCollision(snake);
    if (collisionGate.has_value()) {
        Position gatePos = collisionGate->getPosition();
        
        // Snake가 Gate에 진입 중임을 표시
        gateManager.setSnakeEntering(gatePos, true);
        
        // 새로운 진출 방향 계산 로직 적용
        Position exitPos;
        Direction exitDirection;
        
        if (collisionGate->isOuterWall()) {
            // 외부벽 Gate: 고정 방향으로 진출
            exitDirection = gateManager.calculateOuterWallExitDirection(gatePos, snake.getDirection());
        } else {
            // 내부벽 Gate: 우선순위에 따른 방향 결정
            auto priorities = gateManager.calculateInnerWallDirectionPriority(gatePos, snake.getDirection());
            exitDirection = priorities[0];  // 첫 번째 우선순위 방향 사용
            
            // 특별 규칙 적용 (요구사항에 따라)
            // 여기서는 기본 우선순위를 사용하지만, 필요시 특별 규칙 적용 가능
        }
        
        // 양방향 텔레포트 처리 (기존 로직 유지)
        exitPos = gateManager.calculateBidirectionalExitPosition(*collisionGate, exitDirection, snake);
        
        if (exitPos.x != -1 && exitPos.y != -1) {
            snake.teleportTo(exitPos);
            scoreManager.incrementGatesUsed();
            // 미션 진행상황 업데이트
            stageManager.updateMissionProgress(MissionType::GATES, scoreManager.getGatesUsed());
        }
        
        // 텔레포트 완료 후 진입 상태 해제
        gateManager.setSnakeEntering(gatePos, false);
    }
}

void Game::drawScoreBoard() {
    // 점수판 제목 (31x31 맵 오른쪽으로 이동)
    mvprintw(2, 35, "=== SCORE BOARD ===");
    
    // 현재 길이 / 최대 길이
    mvprintw(4, 35, "B: %d/%d", scoreManager.getCurrentLength(), scoreManager.getMaxLength());
    
    // Growth Items 수집 수
    mvprintw(5, 35, "+: %d", scoreManager.getGrowthItemsCollected());
    
    // Poison Items 수집 수
    mvprintw(6, 35, "-: %d", scoreManager.getPoisonItemsCollected());
    
    // Gates 사용 수
    mvprintw(7, 35, "G: %d", scoreManager.getGatesUsed());
    
    // 생존시간
    mvprintw(8, 35, "Time: %s", scoreManager.getFormattedSurvivalTime().c_str());
    
    // 총 점수
    mvprintw(10, 35, "Score: %d", scoreManager.getTotalScore());
    
    // 화면 갱신
    refresh();
}

void Game::checkStageCompletion() {
    if (stageManager.isCurrentStageCompleted()) {
        if (stageManager.isLastStage()) {
            // 게임 완료 - 별도 플래그 설정
            gameCompleted = true;
            gameOver = true;
        } else {
            // 다음 스테이지로 이동
            stageManager.nextStage();
            stageManager.applyCurrentStageToMap(map);
            
            // 스테이지별 카운터 초기화 (Growth Items, Gates 사용 횟수)
            scoreManager.resetStageSpecificCounters();
            
            // Temporary Wall 초기화
            temporaryWallManager.clear();
            
            // 뱀을 안전한 위치로 초기화
            auto safePos = map.findSafePosition();
            if (safePos.has_value()) {
                snake.reset(safePos.value().first, safePos.value().second);
            } else {
                // 안전한 위치를 찾지 못한 경우 기본 위치로 초기화
                snake.reset(10, 10);
            }
        }
    }
}

void Game::drawMissionInfo() {
    const Stage* currentStage = stageManager.getCurrentStage();
    if (!currentStage) return;
    
    // 스테이지 정보 (점수판과 함께 오른쪽으로 이동)
    mvprintw(11, 35, "=== STAGE %d ===", stageManager.getCurrentStageNumber());
    mvprintw(12, 35, "%s", currentStage->getStageName().c_str());
    
    // 미션 정보
    mvprintw(14, 35, "=== MISSIONS ===");
    
    int missionCount = currentStage->getMissionCount();
    for (int i = 0; i < missionCount; i++) {
        const Mission* mission = currentStage->getMission(i);
        if (mission) {
            std::string status = mission->isCompleted() ? "[V]" : "[ ]";
            mvprintw(15 + i, 35, "%s %s (%d/%d)", 
                status.c_str(),
                mission->getDescription().c_str(),
                mission->getCurrentValue(),
                mission->getTargetValue());
        }
    }
    
    // 전체 진행률
    mvprintw(15 + missionCount + 1, 35, "Progress: %.1f%%", 
        currentStage->getOverallProgress() * 100.0f);
    
    refresh();
}

// 속도 관리 메서드들
void Game::applySpeedBoost() {
    speedBoostCount++;
    
    // 새로운 속도 계산: baseTickDuration / (1 + speedBoostCount * 0.2)
    double speedMultiplier = 1.0 + speedBoostCount * 0.2;
    int newTickDuration = static_cast<int>(baseTickDuration / speedMultiplier);
    
    // 최소 속도 제한 적용
    currentTickDuration = std::max(newTickDuration, minTickDuration);
}

void Game::resetSpeed() {
    currentTickDuration = baseTickDuration;
    speedBoostCount = 0;
}

void Game::createTemporaryWallAroundSnake() {
    Position headPos = snake.getHead();
    auto lifetime = std::chrono::milliseconds(5000);  // 5초 생존
    
    // 뱀 머리 주변 8방향에 Temporary Wall 생성
    std::vector<Position> positions = {
        {headPos.x - 1, headPos.y - 1}, // 좌상
        {headPos.x, headPos.y - 1},     // 상
        {headPos.x + 1, headPos.y - 1}, // 우상
        {headPos.x - 1, headPos.y},     // 좌
        {headPos.x + 1, headPos.y},     // 우
        {headPos.x - 1, headPos.y + 1}, // 좌하
        {headPos.x, headPos.y + 1},     // 하
        {headPos.x + 1, headPos.y + 1}  // 우하
    };
    
    for (const auto& pos : positions) {
        // 유효한 위치이고 빈 공간인 경우에만 Temporary Wall 생성
        if (map.isValidPosition(pos.x, pos.y) && map.getCellValue(pos.x, pos.y) == 0) {
            temporaryWallManager.addTemporaryWall(pos, lifetime);
        }
    }
}

void Game::checkTemporaryWallCreation() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTemporaryWallCreation);
    
    // 설정된 간격이 지났으면 자동 생성
    if (elapsed >= temporaryWallCreationInterval) {
        createRandomTemporaryWalls();
        lastTemporaryWallCreation = now;
    }
}

void Game::createRandomTemporaryWalls() {
    Position snakeHead = snake.getHead();
    auto lifetime = std::chrono::milliseconds(5000);  // 5초 생존
    const int wallsToCreate = 2;  // 한 번에 2개 생성
    const int minDistance = 3;    // 뱀과의 최소 거리
    
    std::vector<Position> validPositions;
    
    // 맵에서 유효한 위치들 수집
    for (int y = 1; y < map.getHeight() - 1; y++) {
        for (int x = 1; x < map.getWidth() - 1; x++) {
            // 빈 공간인지 확인
            if (map.getCellValue(x, y) == 0) {
                Position pos(x, y);
                
                // 뱀과의 거리 확인 (맨하탄 거리)
                int distance = abs(pos.x - snakeHead.x) + abs(pos.y - snakeHead.y);
                if (distance >= minDistance) {
                    // 뱀의 몸통과도 충돌하지 않는지 확인
                    bool tooCloseToSnake = false;
                    const auto& snakeBody = snake.getBody();
                    for (const auto& bodyPart : snakeBody) {
                        int bodyDistance = abs(pos.x - bodyPart.x) + abs(pos.y - bodyPart.y);
                        if (bodyDistance < minDistance) {
                            tooCloseToSnake = true;
                            break;
                        }
                    }
                    
                    if (!tooCloseToSnake) {
                        validPositions.push_back(pos);
                    }
                }
            }
        }
    }
    
    // 랜덤하게 위치 선택해서 Temporary Wall 생성
    if (!validPositions.empty()) {
        // C++17에서는 std::shuffle 사용
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(validPositions.begin(), validPositions.end(), g);
        
        int wallsCreated = 0;
        for (const auto& pos : validPositions) {
            if (wallsCreated >= wallsToCreate) break;
            
            temporaryWallManager.addTemporaryWall(pos, lifetime);
            wallsCreated++;
        }
    }
} 