#include "Player.hpp"

Player::Player(sf::Texture &textureLeft, sf::Texture &textureRight,
               sf::Texture &textureShootBody, sf::Texture &textureSnout)
    : isFacingLeft(false), shootTimer(0.f)
{

    texLeft = &textureLeft;
    texRight = &textureRight;
    texShootBody = &textureShootBody;
    texSnout = &textureSnout;

    sprite.setTexture(*texRight);

    sprite.setScale(0.6f, 0.6f);

    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    snoutSprite.setTexture(*texSnout);
    snoutSprite.setScale(0.6f, 0.6f);
    sf::FloatRect snoutBounds = snoutSprite.getLocalBounds();
    snoutSprite.setOrigin(snoutBounds.width / 2.f, snoutBounds.height / 2.f);

    position = sf::Vector2f(250.f, 400.f);
    velocity = sf::Vector2f(0.f, 0.f);

    score = 0;
    movementSpeed = 400.f; 
    gravity = 900.f;       
    jumpForce = -650.f;    

    sprite.setPosition(position);
}

void Player::handleInput()
{
    velocity.x = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        velocity.x = -movementSpeed;
        isFacingLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        velocity.x = movementSpeed;
        isFacingLeft = false;
    }
}

void Player::update(float deltaTime, float windowWidth)
{
    velocity.y += gravity * deltaTime;
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

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

    if (shootTimer > 0.f)
    {
        sprite.setTexture(*texShootBody, true);
        sprite.setScale(1.1f, 1.1f);
    }
    else
    {
        sprite.setTexture(isFacingLeft ? *texLeft : *texRight, true);
        sprite.setScale(0.6f, 0.6f); 
    }

    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    sprite.setPosition(position);
}

void Player::triggerShoot()
{
    shootTimer = 0.2f;
}

void Player::draw(sf::RenderWindow &window)
{
    window.draw(sprite);

    if (shootTimer > 0.f)
    {
        sf::FloatRect snoutBounds = snoutSprite.getLocalBounds();
        snoutSprite.setOrigin(snoutBounds.width / 2.f, snoutBounds.height / 2.f);
        snoutSprite.setScale(0.6f, 0.6f);

        sf::FloatRect bodyBounds = sprite.getGlobalBounds();

        float snoutX = position.x;
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