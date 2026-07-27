#ifndef MONSTER_HPP
#define MONSTER_HPP

#include <SFML/Graphics.hpp>

class Monster {
public:
    Monster(sf::Texture& texture, float startX, float startY, int health);
    Monster(sf::Texture& tex1, sf::Texture& tex2, sf::Vector2f pos, int health);

    void update(float deltaTime, float screenWidth);
    void draw(sf::RenderWindow& window) const;
    void moveDown(float amount);
    
    sf::FloatRect getBounds() const;
    void takeDamage(int damage = 1);
    bool isDead() const;
    bool isOffScreen() const;

    void setSpeedMultiplier(float multiplier);
    bool isActive() const;
    void setActive(bool activeState);
    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);

private:
    sf::Sprite sprite;
    float speed;
    int hp;
    bool active = true;
    float baseSpeed = 80.f;
};

#endif