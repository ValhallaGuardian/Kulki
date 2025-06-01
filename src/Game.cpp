#include "../include/Game.hpp"

Game::Game()
    : window(sf::VideoMode({800, 600}), "Kulki Game"), board(10, 10)
{
    window.setFramerateLimit(60);
}

Game::~Game() {}

int Game::run()
{
    while (window.isOpen())
    {
        handleEvents();
        update();
        render();
    }
    return 0;
}

void Game::handleEvents()
{
    while (const std::optional event = window.pollEvent())
    {

        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (event->is<sf::Event::KeyPressed>())
        {
            if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvent->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
            }
        }
    }
}

void Game::update()
{
    // Update game logic here
}

void Game::render()
{
    window.clear(sf::Color::Black);
    board.draw(window);
    window.display();
}
