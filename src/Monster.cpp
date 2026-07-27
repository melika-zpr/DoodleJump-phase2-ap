#include "Monster.hpp"
#include <cstdlib>

// سازنده اول
Monster::Monster(sf::Texture& texture, float startX, float startY, int health) 
    : hp(health), active(true) 
{
    sprite.setTexture(texture);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(startX, startY);

    speed = 50.f + static_cast<float>(rand() % 100);
    if (rand() % 2 == 0) speed = -speed;
}

// سازنده دوم
Monster::Monster(sf::Texture& tex1, sf::Texture& tex2, sf::Vector2f pos, int health) 
    : hp(health), active(true) 
{
    sprite.setTexture(tex1); 
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(pos);
    speed = baseSpeed;
}

void Monster::update(float deltaTime, float screenWidth) {
    sprite.move(speed * deltaTime, 0.f);

    sf::FloatRect bounds = sprite.getGlobalBounds();
    if (bounds.left <= 0.f) {
        sprite.setPosition(bounds.width / 2.f, sprite.getPosition().y);
        speed = std::abs(speed); 
    } else if (bounds.left + bounds.width >= screenWidth) {
        sprite.setPosition(screenWidth - bounds.width / 2.f, sprite.getPosition().y);
        speed = -std::abs(speed); 
    }
}

void Monster::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

void Monster::moveDown(float amount) {
    sprite.move(0.f, amount);
}

sf::FloatRect Monster::getBounds() const {
    return sprite.getGlobalBounds();
}

void Monster::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        active = false;
    }
}

bool Monster::isDead() const {
    return hp <= 0;
}

bool Monster::isOffScreen() const {
    return sprite.getPosition().y > 900.f; 
}

void Monster::setSpeedMultiplier(float multiplier) {
    speed = baseSpeed * multiplier;
}

bool Monster::isActive() const {
    return active && hp > 0;
}

void Monster::setActive(bool activeState) {
    active = activeState;
}

sf::Vector2f Monster::getPosition() const {
    return sprite.getPosition();
}

void Monster::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}