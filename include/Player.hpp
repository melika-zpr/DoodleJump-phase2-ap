#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

class Player {
private:
    sf::Sprite sprite;
    sf::Sprite snoutSprite; 

    sf::Vector2f position;
    sf::Vector2f velocity;

    // پوینتر برای نگهداری آدرس دو تصویر بدون کپی کردن اضافی در حافظه
    sf::Texture* texLeft;
    sf::Texture* texRight;
    sf::Texture* texShootBody;   
    sf::Texture* texSnout;      
    bool isFacingLeft;
    float shootTimer;
    
    int score;
    float movementSpeed;
    float gravity;
    float jumpForce;

public:

    Player(sf::Texture& textureLeft, sf::Texture& textureRight, 
        sf::Texture& textureShootBody, sf::Texture& textureSnout);

    void handleInput();
    void update(float deltaTime, float windowWidth);
    void draw(sf::RenderWindow& window);
    void jump();
    void springJump();

    void triggerShoot();
    
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    void setVelocity(sf::Vector2f vel);
    sf::FloatRect getBounds() const;
    int getScore() const;

    void setPosition(sf::Vector2f pos);
    void updateSuction(sf::Vector2f targetPos, float deltaTime, bool& finished);
};

#endif
