#include "../include/Board.hpp"

Board::Board(int w, int h) : width(w), height(h)
{
    initialize();
    initializeGraphics();
}

Board::~Board() {}

void Board::initialize()
{
    grid = new int *[height];
    for (int i = 0; i < height; ++i)
    {
        grid[i] = new int[width];
        for (int j = 0; j < width; ++j)
        {
            grid[i][j] = 0;
        }
    }
}

void Board::initializeGraphics()
{
    // Stałe dla rysowania
    cellSize = 50.0f;
    offsetX = 100.0f;
    offsetY = 50.0f;

    // Utwórz kształty dla pól planszy
    cells.resize(height);
    for (int i = 0; i < height; ++i)
    {
        cells[i].resize(width);
        for (int j = 0; j < width; ++j)
        {
            // Każde pole to prostokąt
            cells[i][j].setSize({cellSize - 2.0f, cellSize - 2.0f});
            cells[i][j].setPosition({offsetX + j * cellSize + 1.0f,
                                     offsetY + i * cellSize + 1.0f});
            cells[i][j].setFillColor(sf::Color(40, 40, 40));
            cells[i][j].setOutlineThickness(1.0f);
            cells[i][j].setOutlineColor(sf::Color(100, 100, 100));
        }
    }
}

void Board::reset()
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            grid[i][j] = 0;
        }
    }
}

void Board::draw(sf::RenderWindow &window)
{
    // Rysuj pola planszy
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            // Ustaw kolor w zależności od zawartości pola
            if (grid[i][j] == 0)
            {
                // Puste pole
                cells[i][j].setFillColor(sf::Color(40, 40, 40));
            }
            else
            {
                // Pole z kulką - kolor zależy od wartości
                cells[i][j].setFillColor(getBallColor(grid[i][j]));
            }

            window.draw(cells[i][j]);
        }
    }

    // Rysuj linie siatki (opcjonalnie)
    drawGrid(window);
}

void Board::drawGrid(sf::RenderWindow &window)
{
    sf::RectangleShape line;
    line.setFillColor(sf::Color(80, 80, 80));

    // Linie pionowe
    for (int j = 0; j <= width; ++j)
    {
        line.setSize({1.0f, height * cellSize});
        line.setPosition({offsetX + j * cellSize, offsetY});
        window.draw(line);
    }

    // Linie poziome
    for (int i = 0; i <= height; ++i)
    {
        line.setSize({width * cellSize, 1.0f});
        line.setPosition({offsetX, offsetY + i * cellSize});
        window.draw(line);
    }
}

sf::Color Board::getBallColor(int ballType)
{
    switch (ballType)
    {
    case 1:
        return sf::Color::Red;
    case 2:
        return sf::Color::Green;
    case 3:
        return sf::Color::Blue;
    case 4:
        return sf::Color::Yellow;
    case 5:
        return sf::Color::Magenta;
    case 6:
        return sf::Color::Cyan;
    case 7:
        return sf::Color(255, 165, 0); // Orange
    default:
        return sf::Color(40, 40, 40); // Puste pole
    }
}
