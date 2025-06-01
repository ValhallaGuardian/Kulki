#pragma once

#include <SFML/Graphics.hpp>

enum class BallColor {
    Red,
    Green,
    Blue,
    Yellow,
    Purple,
    Orange
};

class Ball {
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    BallColor color;

    sf::Color getSFMLColor(BallColor ballColor) const;

public:
    Ball(BallColor color, sf::Vector2f position);
    ~Ball();

    void draw(sf::RenderWindow &window);
    void update();

    BallColor getColor() const;
    sf::Vector2f getPosition() const;

    void setColor(BallColor color);
    void setPosition(sf::Vector2f position);

};