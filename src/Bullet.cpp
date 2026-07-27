#include "Bullet.hpp"

Bullet::Bullet(sf::Vector2f startPosition, sf::Vector2f velocity)
    : velocity(velocity), active(true) {
    // تنظیمات ظاهری گلوله (ابعاد و رنگ)
    shape.setSize(sf::Vector2f(6.f, 16.f));
    shape.setFillColor(sf::Color::Yellow);
    shape.setOrigin(shape.getSize().x / 2.f, shape.getSize().y / 2.f);
    shape.setPosition(startPosition);
}

void Bullet::update(float deltaTime) {
    // حرکت گلوله به سمت بالا (یا جهت مشخص شده)
    shape.move(velocity * deltaTime);

    // غیرفعال کردن گلوله اگر از بالای صفحه خارج شود
    if (shape.getPosition().y < 0.f) {
        active = false;
    }
}

void Bullet::render(sf::RenderWindow& window) {
    if (active) {
        window.draw(shape);
    }
}

sf::FloatRect Bullet::getBounds() const {
    return shape.getGlobalBounds();
}

bool Bullet::isActive() const {
    return active;
}

void Bullet::setActive(bool status) {
    active = status;
}