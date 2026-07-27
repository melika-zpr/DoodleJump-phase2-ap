#include "Hole.hpp"

Hole::Hole(sf::Texture& smallTex, sf::Texture& largeTex, sf::Vector2f pos, bool isSmall)
    : active(true), small(isSmall), position(pos)
{
    if (small) {
        sprite.setTexture(smallTex);
    } else {
        sprite.setTexture(largeTex);
    }
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(position);
}

void Hole::update(float deltaTime) {
    // سیاه‌چاله‌ها عناصر ثابت محیطی هستند
}

void Hole::draw(sf::RenderWindow& window) const {
    if (active) {
        window.draw(sprite);
    }
}

sf::FloatRect Hole::getBounds() const {
    if (!active) return sf::FloatRect();
    return sprite.getGlobalBounds();
}

sf::Vector2f Hole::getPosition() const {
    return position;
}

void Hole::setPosition(const sf::Vector2f& pos) {
    position = pos;
    sprite.setPosition(position);
}

bool Hole::isActive() const {
    return active;
}

void Hole::setActive(bool activeState) {
    active = activeState;
}

bool Hole::isSmall() const {
    return small;
}