#include <SFML/Graphics.hpp>
#include "../include/Ball.hpp"


Ball::Ball(BallColor color, sf::Vector2f position)
    : position(position), color(color)
{
    shape.setRadius(20.0f);
    shape.setFillColor(getSFMLColor(color)); // Default color   
    shape.setPosition(position);
}
Ball::~Ball() {}

void Ball::draw(sf::RenderWindow &window) 
{
    window.draw(shape);
}

void Ball::update() 
{
    // Na razie pusta - można dodać animacje, ruch itp. w przyszłości
}

BallColor Ball::getColor() const 
{
    return color;
}

sf::Vector2f Ball::getPosition() const 
{
    return position;
}

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

void Ball::setColor(BallColor newColor) 
{
    color = newColor;
    shape.setFillColor(getSFMLColor(newColor));
}

void Ball::setPosition(sf::Vector2f newPosition) 
{
    position = newPosition;
    shape.setPosition(newPosition);
}