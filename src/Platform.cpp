#include "Platform.hpp"

namespace {
const int SpringWidth = 25;
const int SpringCompressedHeight = 17;
const int SpringOpenY = 17;
const int SpringOpenHeight = 35;
const float SpringOpenDuration = 0.18f;
const float SpringScale = 1.f;
const float BrokenPieceFallSpeed = 260.f;
const float BrokenPieceSideSpeed = 45.f;
const float BrokenPieceRotationSpeed = 120.f;

void setSpringCompressed(sf::Sprite& springSprite) {
    springSprite.setTextureRect(sf::IntRect(0, 0, SpringWidth, SpringCompressedHeight));
}

void setSpringOpen(sf::Sprite& springSprite) {
    springSprite.setTextureRect(sf::IntRect(0, SpringOpenY, SpringWidth, SpringOpenHeight));
}

void positionSpring(sf::Sprite& springSprite, const sf::Sprite& platformSprite, sf::Vector2f position) {
    sf::FloatRect platformBounds = platformSprite.getGlobalBounds();
    sf::FloatRect springBounds = springSprite.getGlobalBounds();
    springSprite.setPosition(position.x + platformBounds.width * 0.5f - springBounds.width * 0.5f, position.y - springBounds.height + 8.f);
}

void configureBrokenPieces(sf::Sprite& leftSprite, sf::Sprite& rightSprite, const sf::Sprite& platformSprite, sf::Vector2f position) {
    const sf::Texture* texture = platformSprite.getTexture();
    if (!texture) {
        return;
    }

    int width = static_cast<int>(texture->getSize().x);
    int height = static_cast<int>(texture->getSize().y);
    int leftWidth = width / 2;
    int rightWidth = width - leftWidth;

    leftSprite.setTexture(*texture);
    leftSprite.setTextureRect(sf::IntRect(0, 0, leftWidth, height));
    leftSprite.setOrigin(leftWidth * 0.5f, height * 0.5f);
    leftSprite.setPosition(position.x + leftWidth * 0.5f, position.y + height * 0.5f);
    leftSprite.setRotation(0.f);

    rightSprite.setTexture(*texture);
    rightSprite.setTextureRect(sf::IntRect(leftWidth, 0, rightWidth, height));
    rightSprite.setOrigin(rightWidth * 0.5f, height * 0.5f);
    rightSprite.setPosition(position.x + leftWidth + rightWidth * 0.5f, position.y + height * 0.5f);
    rightSprite.setRotation(0.f);
}
}

Platform::Platform(sf::Texture& texture, sf::Vector2f pos, PlatformType platformType, sf::Texture* springTexture, bool platformHasSpring)
    : position(pos), type(platformType), active(true), falling(false), hasSpring(platformHasSpring), springOpenTimer(0.f), moveSpeed(0.f), fallSpeed(160.f), moveDirection(1)
{
    sprite.setTexture(texture, true);
    sprite.setPosition(position);
    configureBrokenPieces(brokenLeftSprite, brokenRightSprite, sprite, position);
    if (hasSpring && springTexture) {
        springSprite.setTexture(*springTexture);
        setSpringCompressed(springSprite);
        springSprite.setScale(SpringScale, SpringScale);
        positionSpring(springSprite, sprite, position);
    } else {
        hasSpring = false;
    }
    if (type == PlatformType::Moving) {
        moveSpeed = 80.f; // Moving platforms slide horizontally.
    }
}

void Platform::update(float deltaTime, float windowWidth) {
    if (type == PlatformType::Moving && active) {
        // Move the platform left or right, reversing at screen edges.
        position.x += moveSpeed * static_cast<float>(moveDirection) * deltaTime;
        if (position.x < 0.f) {
            position.x = 0.f;
            moveDirection = 1;
        } else if (position.x + sprite.getGlobalBounds().width > windowWidth) {
            position.x = windowWidth - sprite.getGlobalBounds().width;
            moveDirection = -1;
        }
    }

    if (falling) {
        // Broken platforms fall after the player lands on them.
        position.y += fallSpeed * deltaTime;
        brokenLeftPosition.x -= BrokenPieceSideSpeed * deltaTime;
        brokenLeftPosition.y += BrokenPieceFallSpeed * deltaTime;
        brokenRightPosition.x += BrokenPieceSideSpeed * deltaTime;
        brokenRightPosition.y += BrokenPieceFallSpeed * deltaTime;
        brokenLeftSprite.setPosition(brokenLeftPosition);
        brokenRightSprite.setPosition(brokenRightPosition);
        brokenLeftSprite.rotate(-BrokenPieceRotationSpeed * deltaTime);
        brokenRightSprite.rotate(BrokenPieceRotationSpeed * deltaTime);
        if (position.y > 900.f) {
            active = false;
            falling = false;
        }
    }

    if (springOpenTimer > 0.f) {
        springOpenTimer -= deltaTime;
        if (springOpenTimer <= 0.f) {
            springOpenTimer = 0.f;
            setSpringCompressed(springSprite);
        }
    }

    sprite.setPosition(position);
    if (hasSpring) {
        positionSpring(springSprite, sprite, position);
    }
}

void Platform::draw(sf::RenderWindow& window) {
    if (active) {
        window.draw(sprite);
        if (hasSpring && active) {
            window.draw(springSprite);
        }
    } else if (falling) {
        window.draw(brokenLeftSprite);
        window.draw(brokenRightSprite);
    }
}

sf::FloatRect Platform::getBounds() const {
    if (!active) {
        return sf::FloatRect();
    }
    return sprite.getGlobalBounds();
}

sf::FloatRect Platform::getSpringBounds() const {
    if (!active || !hasSpring) {
        return sf::FloatRect();
    }
    return springSprite.getGlobalBounds();
}

sf::Vector2f Platform::getPosition() const {
    return position;
}

void Platform::setPosition(sf::Vector2f pos) {
    sf::Vector2f offset = pos - position;
    position = pos;
    sprite.setPosition(position);
    if (falling) {
        brokenLeftPosition += offset;
        brokenRightPosition += offset;
        brokenLeftSprite.setPosition(brokenLeftPosition);
        brokenRightSprite.setPosition(brokenRightPosition);
    }
    if (hasSpring) {
        positionSpring(springSprite, sprite, position);
    }
}

void Platform::reset(sf::Texture& texture, PlatformType newType, sf::Vector2f pos, sf::Texture* springTexture, bool platformHasSpring) {
    type = newType;
    active = true;
    falling = false;
    position = pos;
    sprite.setTexture(texture, true);
    sprite.setPosition(position);
    configureBrokenPieces(brokenLeftSprite, brokenRightSprite, sprite, position);
    hasSpring = platformHasSpring;
    if (hasSpring && springTexture) {
        springSprite.setTexture(*springTexture);
        setSpringCompressed(springSprite);
        springSprite.setScale(SpringScale, SpringScale);
        positionSpring(springSprite, sprite, position);
    } else {
        hasSpring = false;
    }
    springOpenTimer = 0.f;
    moveDirection = 1;
    moveSpeed = (type == PlatformType::Moving) ? 80.f : 0.f;
}

bool Platform::isActive() const {
    return active;
}

void Platform::breakPlatform() {
    if (type == PlatformType::Broken) {
        active = false;
        falling = true;
        configureBrokenPieces(brokenLeftSprite, brokenRightSprite, sprite, position);
        brokenLeftPosition = brokenLeftSprite.getPosition();
        brokenRightPosition = brokenRightSprite.getPosition();
    }
}

void Platform::activateSpring() {
    if (!hasSpring) {
        return;
    }

    setSpringOpen(springSprite);
    springOpenTimer = SpringOpenDuration;
    positionSpring(springSprite, sprite, position);
}

Platform::PlatformType Platform::getType() const {
    return type;
}

bool Platform::containsSpring() const {
    return hasSpring;
}
