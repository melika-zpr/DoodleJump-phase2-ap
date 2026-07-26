#include "Player.hpp"

Player::Player(sf::Texture &textureLeft, sf::Texture &textureRight)
{
    // Store texture references for left and right facing sprites.
    texLeft = &textureLeft;
    texRight = &textureRight;

    // Start with the right-facing texture by default.
    sprite.setTexture(*texRight);

    // Scale the player sprite down to fit the game world.
    sprite.setScale(0.6f, 0.6f);

    // Center the sprite origin for more accurate movement and collision.
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

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
        sprite.setTexture(*texLeft); // Switch sprite to left-facing texture.
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        velocity.x = movementSpeed;
        sprite.setTexture(*texRight); // Switch sprite to right-facing texture.
    }

    // Manual jump input is disabled because jumps happen on platform collision.
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && velocity.y > 0.f) {
    //     jump();
    // }
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

    sprite.setPosition(position);
}

void Player::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
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
