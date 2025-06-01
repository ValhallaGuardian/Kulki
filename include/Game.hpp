#pragma once
#include <SFML/Graphics.hpp>
#include "../include/Board.hpp"


class Game {
private:
    sf::RenderWindow window;
    Board board;

public: 
    Game();
    ~Game();

    int run();
    void handleEvents();
    void update();
    void render();
    void gameLoop();


};



