#pragma once
#include <vector>
#include <memory>
#include <random>
#include <SFML/Graphics.hpp>
#include "Ball.hpp"

class Board
{
private:
    int width;
    int height;
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<std::unique_ptr<Ball>>> balls;

    
    std::vector<std::vector<sf::RectangleShape>> cells;
    float cellSize;
    float offsetX, offsetY;
    
    // Random generation
    std::mt19937 rng;
    std::uniform_int_distribution<int> colorDist;
    
    // Game logic
    int selectedX, selectedY;
    bool hasBallSelected;
    sf::Clock blinkClock;
    bool blinkState;
    
    // Line detection and animation
    std::vector<std::vector<bool>> lineMarked;
    sf::Clock lineAnimationClock;
    sf::Clock blinkTimer; // Oddzielny timer dla migania
    bool lineAnimationActive;
    int animationPhase; // 0=highlight, 1=fast_blink, 2=remove
    bool fastBlinkState;
    
    // Scoring system
    int score;
    int comboMultiplier;
    std::vector<std::pair<sf::Vector2f, int>> floatingScores; // pozycja, punkty
    sf::Clock scoreAnimationClock;
    
    // New balls system
    std::vector<BallColor> nextBalls; // 2 następne kulki
    bool gameOver;
    int ballsToAdd; // Ile kulek dodać po ruchu

    // UI Elements
    sf::Font font;
    sf::Text scoreText;
    sf::Text gameOverText;
    sf::Text restartText;
    bool fontLoaded;

public:
    Board(int w, int h);
    ~Board();

    void initialize();
    void reset();
    void draw(sf::RenderWindow &window);
    void initializeGraphics();
    void drawGrid(sf::RenderWindow &window);
    void update(); // Nowa metoda do aktualizacji logiki
    
    // Ball management
    void generateBalls();
    void drawBalls(sf::RenderWindow &window);
    void placeBallAt(int x, int y, BallColor color);
    BallColor getRandomColor();
    
    // Game logic
    void handleMouseClick(float mouseX, float mouseY);
    std::pair<int, int> getGridPosition(float mouseX, float mouseY) const;
    bool canMoveTo(int fromX, int fromY, int toX, int toY);
    std::vector<std::pair<int, int>> findPath(int fromX, int fromY, int toX, int toY);
    void moveBall(int fromX, int fromY, int toX, int toY);
    void selectBall(int x, int y);
    void deselectBall();
    
    // Line detection system
    std::vector<std::vector<std::pair<int, int>>> findAllLines();
    std::vector<std::pair<int, int>> checkDirection(int startX, int startY, int dx, int dy, BallColor color);
    void markLinesForRemoval(const std::vector<std::vector<std::pair<int, int>>>& lines);
    void startLineAnimation();
    void updateLineAnimation();
    void removeLinesAndUpdateScore();
    bool hasMarkedLines();
    
    // Scoring and effects
    void addScore(int points, sf::Vector2f position);
    void updateFloatingScores();
    void drawFloatingScores(sf::RenderWindow& window);
    int calculateLineScore(int lineLength);
    
    // New balls system
    void generateNextBalls();
    void addNewBalls();
    std::vector<std::pair<int, int>> getEmptyPositions();
    bool hasAvailableMoves();
    void checkGameOver();
    void drawNextBalls(sf::RenderWindow& window);

    // Helpers
    sf::Color getBallColor(int ballType);
    sf::Color getSFMLColorFromBallColor(BallColor ballColor) const;
    sf::Vector2f getCellPosition(int x, int y) const;
    bool isValidPosition(int x, int y) const;
    bool isEmpty(int x, int y) const;
    
    // Getters
    int getScore() const { return score; }
    int getCombo() const { return comboMultiplier; }
    const std::vector<BallColor>& getNextBalls() const { return nextBalls; }
    bool isGameOver() const { return gameOver; }


    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};