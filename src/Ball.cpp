#include <SFML/Graphics.hpp>
#include "../include/Ball.hpp"


Ball::Ball(BallColor color, sf::Vector2f position)
    : color(color), position(position)
{
    shape.setRadius(20.0f);
    shape.setFillColor(getSFMLColor(color)); // Default color   
    shape.setPosition(position);
}
Ball::~Ball() {}









sf::Color Ball::getSFMLColor(BallColor ballColor) const {
    switch (ballColor) {
        case BallColor::Red:
            return sf::Color::Red;
        case BallColor::Green:
            return sf::Color::Green;
        case BallColor::Blue:
            return sf::Color::Blue;
        case BallColor::Yellow:
            return sf::Color::Yellow;
        case BallColor::Purple:
            return sf::Color::Magenta; // SFML nie ma Purple, używamy Magenta
        case BallColor::Orange:
            return sf::Color(255, 165, 0); // Custom orange color (RGB)
        default:
            return sf::Color::White;
    }
}