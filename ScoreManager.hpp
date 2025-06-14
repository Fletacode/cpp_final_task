#ifndef SCOREMANAGER_HPP
#define SCOREMANAGER_HPP

#include <string>
#include <chrono>

class ScoreManager {
private:
    int currentLength;
    int maxLength;
    int growthItemsCollected;
    int poisonItemsCollected;
    int gatesUsed;
    std::chrono::steady_clock::time_point gameStartTime;

public:
    // 생성자
    ScoreManager();

    // Snake 길이 관련
    void updateSnakeLength(int length);
    int getCurrentLength() const;
    int getMaxLength() const;

    // 아이템 수집 관련
    void incrementGrowthItems();
    void incrementPoisonItems();
    void incrementGatesUsed();
    int getGrowthItemsCollected() const;
    int getPoisonItemsCollected() const;
    int getGatesUsed() const;

    // 생존시간 관련
    void setGameStartTime();
    void setGameStartTime(const std::chrono::steady_clock::time_point& startTime);
    int getSurvivalTimeSeconds() const;
    std::string getFormattedSurvivalTime() const;

    // 점수 계산
    int calculateScore() const;
    int getTotalScore() const;

    // 파일 I/O
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

    // 초기화
    void reset();
    
    // 스테이지 전환 시 특정 값들만 초기화
    void resetStageSpecificCounters();
};

#endif // SCOREMANAGER_HPP 