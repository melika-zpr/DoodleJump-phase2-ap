#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

class Player {
private:
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f velocity;

    sf::Texture* texLeft;
    sf::Texture* texRight;
    
    int score;
    float movementSpeed;
    float gravity;
    float jumpForce;

public:
    Player(sf::Texture& textureLeft, sf::Texture& textureRight);

    void handleInput();
    void update(float deltaTime, float windowWidth);
    void draw(sf::RenderWindow& window);
    void jump();
    void springJump();
    
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    void setVelocity(sf::Vector2f vel);
    sf::FloatRect getBounds() const;
    int getScore() const;

    void setPosition(sf::Vector2f pos);
};

#endif
