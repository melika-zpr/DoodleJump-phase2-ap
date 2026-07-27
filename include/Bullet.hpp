#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

class Bullet {
private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    bool active;

public:
    Bullet(sf::Vector2f startPosition, sf::Vector2f velocity);
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    sf::FloatRect getBounds() const;
    bool isActive() const;
    void setActive(bool status);
};

#endif