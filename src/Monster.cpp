#include "Monster.hpp"
#include <cstdlib>

// سازنده اول
Monster::Monster(sf::Texture &texture, float startX, float startY, int health)
    : hp(health)
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
    : hp(health)
{
    speed = baseSpeed;

    // انتخاب تصادفی با شانس ۵۰-۵۰ بین هیولای سبز (tex1) و آبی (tex2)
    if (rand() % 2 == 0) {
        // --- حالت هیولای سبز (معمولی - بدون نصف شدن) ---
        isSplitMonster = false;
        sprite.setTexture(tex1);
        
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    } else {
        // --- حالت هیولای آبی (دوتکه) ---
        isSplitMonster = true;
        sprite.setTexture(tex2);
        
        sf::Vector2u texSize = tex2.getSize();
        int frameWidth = texSize.x / 2;
        int frameHeight = texSize.y;

        // تنظیم کادر عکس بر اساس جهت حرکت اولیه (چون baseSpeed معمولا مثبته، اینجا چک میکنیم)
        if (speed >= 0) {
            sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight)); // حرکت به راست
        } else {
            sprite.setTextureRect(sf::IntRect(frameWidth, 0, frameWidth, frameHeight)); // حرکت به چپ
        }

        // تنظیم Origin برای نصف عکس
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    }
    
    sprite.setPosition(pos);
}

void Monster::update(float deltaTime, float screenWidth)
{
     sprite.move(speed * deltaTime, 0.f);

    sf::FloatRect bounds = sprite.getGlobalBounds();
    
    // برخورد به دیوار سمت چپ
    if (bounds.left <= 0.f) {
        sprite.setPosition(bounds.width / 2.f, sprite.getPosition().y);
        speed = std::abs(speed); // تغییر جهت به سمت راست
        
        // فقط اگر هیولا از نوع دوتکه بود عکسش رو عوض کن
        if (isSplitMonster) {
            int frameWidth = sprite.getTextureRect().width;
            int frameHeight = sprite.getTextureRect().height;
            sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        }
        
    } 
    // برخورد به دیوار سمت راست
    else if (bounds.left + bounds.width >= screenWidth) {
        sprite.setPosition(screenWidth - bounds.width / 2.f, sprite.getPosition().y);
        speed = -std::abs(speed); // تغییر جهت به سمت چپ
        
        // فقط اگر هیولا از نوع دوتکه بود عکسش رو عوض کن
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
}

bool Monster::isDead() const
{
    return hp <= 0;
}

bool Monster::isOffScreen() const
{
    return sprite.getPosition().y > 900.f;
}

// پیاده‌سازی متدهای جدید و گمشده[cite: 13]
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