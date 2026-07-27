#ifndef HOLE_HPP
#define HOLE_HPP

#include <SFML/Graphics.hpp>

class Hole {
public:
    Hole(sf::Texture& smallTex, sf::Texture& largeTex, sf::Vector2f pos, bool isSmall);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);
    bool isActive() const;
    void setActive(bool activeState);
    bool isSmall() const;

private:
    sf::Sprite sprite;
    bool active;
    bool small;
    sf::Vector2f position;
};

#endif
