#include "Player.hpp"

Player::Player(sf::Texture &textureLeft, sf::Texture &textureRight,
               sf::Texture &textureShootBody, sf::Texture &textureSnout)
    : isFacingLeft(false), shootTimer(0.f)
{

    // Store texture references for left and right facing sprites.
    texLeft = &textureLeft;
    texRight = &textureRight;
    texShootBody = &textureShootBody;
    texSnout = &textureSnout;

    // Start with the right-facing texture by default.
    sprite.setTexture(*texRight);

    // Scale the player sprite down to fit the game world.
    sprite.setScale(0.6f, 0.6f);

    // Center the sprite origin for more accurate movement and collision.
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    snoutSprite.setTexture(*texSnout);
    snoutSprite.setScale(0.6f, 0.6f);
    sf::FloatRect snoutBounds = snoutSprite.getLocalBounds();
    snoutSprite.setOrigin(snoutBounds.width / 2.f, snoutBounds.height / 2.f);

    // Initial spawn position and zero velocity.
    position = sf::Vector2f(250.f, 400.f);
    velocity = sf::Vector2f(0.f, 0.f);

    score = 0;
    movementSpeed = 400.f; // Horizontal movement speed.
    gravity = 900.f;       // Downward acceleration.
    jumpForce = -650.f;    // Upward jump velocity.

    sprite.setPosition(position);
}

void Player::handleInput()
{
    // Reset horizontal movement before checking keys.
    velocity.x = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        velocity.x = -movementSpeed;
        // sprite.setTexture(*texLeft); // Switch sprite to left-facing texture.
        isFacingLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        velocity.x = movementSpeed;
        // sprite.setTexture(*texRight); // Switch sprite to right-facing texture.
        isFacingLeft = false;
    }
}

void Player::update(float deltaTime, float windowWidth)
{
    // Apply gravity and update position using current velocity.
    velocity.y += gravity * deltaTime;
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    // Wrap the player horizontally when moving off-screen.
    if (position.x < 0.f)
    {
        position.x = windowWidth;
    }
    else if (position.x > windowWidth)
    {
        position.x = 0.f;
    }

    if (shootTimer > 0.f)
    {
        shootTimer -= deltaTime;
        if (shootTimer < 0.f)
        {
            shootTimer = 0.f;
        }
    }

    // تغییر تصویر بر اساس وضعیت شلیک
    if (shootTimer > 0.f)
    {
        sprite.setTexture(*texShootBody, true);
        sprite.setScale(1.1f, 1.1f); // <--- اسکیل بدنه در زمان شلیک (بزرگتر شد)
    }
    else
    {
        sprite.setTexture(isFacingLeft ? *texLeft : *texRight, true);
        sprite.setScale(0.6f, 0.6f); // <--- اسکیل عادی در حالت پرش
    }

    // بازتنظیم مجدد Origin در مرکز متناسب با ابعاد تکسچر فعال
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    sprite.setPosition(position);
}

void Player::triggerShoot()
{
    shootTimer = 0.2f; // فعال ماندن انیمیشن به مدت ۰.۲ ثانیه
}

void Player::draw(sf::RenderWindow &window)
{
    window.draw(sprite);

    // رسم و تنظیم موقعیت بینی دقیقا روی بالای بدنه
    if (shootTimer > 0.f)
    {
        sf::FloatRect snoutBounds = snoutSprite.getLocalBounds();
        snoutSprite.setOrigin(snoutBounds.width / 2.f, snoutBounds.height / 2.f);
        snoutSprite.setScale(0.6f, 0.6f);

        // محاسبه لبه بالایی بدنه به صورت پویا
        sf::FloatRect bodyBounds = sprite.getGlobalBounds();

        float snoutX = position.x;
        // اتصال بینی به لبه بالای بدنه با اندکی هم‌پوشانی (۶ پیکسل) جهت یکپارچگی کامل
        float snoutY = position.y - (bodyBounds.height / 2.f) + 40.f;

        snoutSprite.setPosition(snoutX, snoutY);

        window.draw(snoutSprite);
    }
}

void Player::jump() { velocity.y = jumpForce; }
void Player::springJump() { velocity.y = jumpForce * 1.7f; }
sf::Vector2f Player::getPosition() const { return position; }
sf::Vector2f Player::getVelocity() const { return velocity; }
void Player::setVelocity(sf::Vector2f vel) { velocity = vel; }
sf::FloatRect Player::getBounds() const { return sprite.getGlobalBounds(); }
int Player::getScore() const { return score; }

void Player::setPosition(sf::Vector2f pos)
{
    position = pos;
    sprite.setPosition(position);
}


void Player::updateSuction(sf::Vector2f targetPos, float deltaTime, bool& finished) {
    sf::Vector2f direction = targetPos - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distance > 2.f) {
        position += direction * (4.f * deltaTime);
    }
    
    sf::Vector2f currentScale = sprite.getScale();
    currentScale.x -= 1.2f * deltaTime;
    currentScale.y -= 1.2f * deltaTime;
    
    if (currentScale.x <= 0.f) {
        currentScale.x = 0.f;
        currentScale.y = 0.f;
        finished = true;
    }
    
    sprite.setScale(currentScale);
    sprite.setPosition(position);
}