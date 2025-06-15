#include "ScoreManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

ScoreManager::ScoreManager() 
    : currentLength(3), maxLength(3), growthItemsCollected(0), 
      poisonItemsCollected(0), gatesUsed(0) {
    gameStartTime = std::chrono::steady_clock::now();
}

void ScoreManager::updateSnakeLength(int length) {
    currentLength = length;
    maxLength = std::max(maxLength, length);
}

int ScoreManager::getCurrentLength() const {
    return currentLength;
}

int ScoreManager::getMaxLength() const {
    return maxLength;
}

void ScoreManager::incrementGrowthItems() {
    growthItemsCollected++;
}

void ScoreManager::incrementPoisonItems() {
    poisonItemsCollected++;
}

void ScoreManager::incrementGatesUsed() {
    gatesUsed++;
}

int ScoreManager::getGrowthItemsCollected() const {
    return growthItemsCollected;
}

int ScoreManager::getPoisonItemsCollected() const {
    return poisonItemsCollected;
}

int ScoreManager::getGatesUsed() const {
    return gatesUsed;
}

// 생존시간 관련 메서드들
void ScoreManager::setGameStartTime() {
    gameStartTime = std::chrono::steady_clock::now();
}

void ScoreManager::setGameStartTime(const std::chrono::steady_clock::time_point& startTime) {
    gameStartTime = startTime;
}

int ScoreManager::getSurvivalTimeSeconds() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - gameStartTime);
    return static_cast<int>(duration.count());
}

std::string ScoreManager::getFormattedSurvivalTime() const {
    int totalSeconds = getSurvivalTimeSeconds();
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}

int ScoreManager::calculateScore() const {
    int score = 0;
    
    // 길이 점수: 현재 길이 * 10점
    score += currentLength * 10;
    
    // Growth Item 점수: 아이템당 50점
    score += growthItemsCollected * 50;
    
    // Poison Item 점수: 아이템당 30점 (위험을 감수한 보상)
    score += poisonItemsCollected * 30;
    
    // Gate 사용 점수: 사용당 100점
    score += gatesUsed * 100;
    
    return score;
}

int ScoreManager::getTotalScore() const {
    return calculateScore();
}

void ScoreManager::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "current_length=" << currentLength << "\n";
        file << "max_length=" << maxLength << "\n";
        file << "growth_items=" << growthItemsCollected << "\n";
        file << "poison_items=" << poisonItemsCollected << "\n";
        file << "gates_used=" << gatesUsed << "\n";
        file << "total_score=" << calculateScore() << "\n";
        
        // 게임 시작 시간을 epoch 이후 밀리초로 저장
        auto epoch = gameStartTime.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
        file << "game_start_time=" << millis.count() << "\n";
        
        file.close();
    }
}

void ScoreManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return; // 파일이 없으면 기본값 유지
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            if (key == "current_length") {
                currentLength = std::stoi(value);
            } else if (key == "max_length") {
                maxLength = std::stoi(value);
            } else if (key == "growth_items") {
                growthItemsCollected = std::stoi(value);
            } else if (key == "poison_items") {
                poisonItemsCollected = std::stoi(value);
            } else if (key == "gates_used") {
                gatesUsed = std::stoi(value);
            } else if (key == "game_start_time") {
                // epoch 이후 밀리초에서 time_point로 복원
                long long millis = std::stoll(value);
                auto duration = std::chrono::milliseconds(millis);
                gameStartTime = std::chrono::steady_clock::time_point(duration);
            }
        }
    }
    file.close();
}

void ScoreManager::reset() {
    currentLength = 3;  // 초기 Snake 길이
    maxLength = 3;
    growthItemsCollected = 0;
    poisonItemsCollected = 0;
    gatesUsed = 0;
    gameStartTime = std::chrono::steady_clock::now();  // 시작 시간도 리셋
}

void ScoreManager::resetStageSpecificCounters() {
    // 스테이지 전환 시 Growth Items와 Gates 사용 횟수만 초기화
    // 현재 길이, 최대 길이, Poison Items는 유지
    growthItemsCollected = 0;
    gatesUsed = 0;
} 