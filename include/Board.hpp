#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class Board
{
private:
    int width;
    int height;
    int **grid;

    
    std::vector<std::vector<sf::RectangleShape>> cells;
    float cellSize;
    float offsetX, offsetY;

public:
    Board(int w, int h);
    ~Board();

    void initialize();
    void reset();
    void draw(sf::RenderWindow &window);
    void initializeGraphics();
    void drawGrid(sf::RenderWindow &window);


    // Helpers
    sf::Color getBallColor(int ballType);


    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};