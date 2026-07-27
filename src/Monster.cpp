#include "Monster.hpp"
#include <cstdlib>

Monster::Monster(sf::Texture &texture, float startX, float startY, int health)
    : hp(health), active(true)
{
    isSplitMonster = false; 

    sprite.setTexture(texture);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(startX, startY);

    speed = 50.f + static_cast<float>(rand() % 100);
    if (rand() % 2 == 0)
        speed = -speed;
}

Monster::Monster(sf::Texture &tex1, sf::Texture &tex2, sf::Vector2f pos, int health)
    : hp(health), active(true)
{
    speed = baseSpeed;

    if (rand() % 2 == 0) {
        isSplitMonster = false;
        sprite.setTexture(tex1);
        
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    } else {
        isSplitMonster = true;
        sprite.setTexture(tex2);
        
        sf::Vector2u texSize = tex2.getSize();
        int frameWidth = texSize.x / 2;
        int frameHeight = texSize.y;

        if (speed >= 0) {
            sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight)); // حرکت به راست
        } else {
            sprite.setTextureRect(sf::IntRect(frameWidth, 0, frameWidth, frameHeight)); // حرکت به چپ
        }

        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    }
    
    sprite.setPosition(pos);
}

void Monster::update(float deltaTime, float screenWidth)
{
     sprite.move(speed * deltaTime, 0.f);

    sf::FloatRect bounds = sprite.getGlobalBounds();
    
    if (bounds.left <= 0.f) {
        sprite.setPosition(bounds.width / 2.f, sprite.getPosition().y);
        speed = std::abs(speed); 
        
        if (isSplitMonster) {
            int frameWidth = sprite.getTextureRect().width;
            int frameHeight = sprite.getTextureRect().height;
            sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        }
        
    } 
    else if (bounds.left + bounds.width >= screenWidth) {
        sprite.setPosition(screenWidth - bounds.width / 2.f, sprite.getPosition().y);
        speed = -std::abs(speed); 
        
        if (isSplitMonster) {
            int frameWidth = sprite.getTextureRect().width;
            int frameHeight = sprite.getTextureRect().height;
            sprite.setTextureRect(sf::IntRect(frameWidth, 0, frameWidth, frameHeight));
        }
    }
}

void Monster::draw(sf::RenderWindow &window) const
{
    window.draw(sprite);
}

void Monster::moveDown(float amount)
{
    sprite.move(0.f, amount);
}

sf::FloatRect Monster::getBounds() const
{
    return sprite.getGlobalBounds();
}

void Monster::takeDamage(int damage)
{
    hp -= damage;
    if (hp <= 0) {
        active = false;
    }
}

bool Monster::isDead() const
{
    return hp <= 0;
}

bool Monster::isOffScreen() const
{
    return sprite.getPosition().y > 900.f;
}

void Monster::setSpeedMultiplier(float multiplier)
{
    speed = baseSpeed * multiplier;
}

bool Monster::isActive() const
{
    return active;
}

void Monster::setActive(bool activeState)
{
    active = activeState;
}

sf::Vector2f Monster::getPosition() const
{
    return sprite.getPosition();
}

void Monster::setPosition(const sf::Vector2f &pos)
{
    sprite.setPosition(pos);
}