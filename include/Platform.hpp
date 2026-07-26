#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SFML/Graphics.hpp>

class Platform {
public:
    enum class PlatformType {
        Normal,
        Broken,
        Moving
    };

private:
    sf::Sprite sprite;
    sf::Sprite springSprite;
    sf::Sprite brokenLeftSprite;
    sf::Sprite brokenRightSprite;
    sf::Vector2f position;
    sf::Vector2f brokenLeftPosition;
    sf::Vector2f brokenRightPosition;
    PlatformType type;
    bool active;
    bool falling;
    bool hasSpring;
    float springOpenTimer;
    float moveSpeed;
    float fallSpeed;
    int moveDirection;

public:
    Platform(sf::Texture& texture, sf::Vector2f pos, PlatformType type = PlatformType::Normal, sf::Texture* springTexture = nullptr, bool hasSpring = false);

    void update(float deltaTime, float windowWidth);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::FloatRect getSpringBounds() const;
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);
    void reset(sf::Texture& texture, PlatformType type, sf::Vector2f pos, sf::Texture* springTexture = nullptr, bool hasSpring = false);
    bool isActive() const;
    void breakPlatform();
    void activateSpring();
    PlatformType getType() const;
    bool containsSpring() const;
};

#endif
