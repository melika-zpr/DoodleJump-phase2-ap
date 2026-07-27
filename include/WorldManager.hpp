#ifndef WORLDMANAGER_HPP
#define WORLDMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include "Platform.hpp"
#include "Player.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"
#include "Monster.hpp"
#include "Hole.hpp"
#include "Bullet.hpp"

class WorldManager
{
private:
    std::vector<Bullet> bullets;
    std::vector<Monster> monsters;
    sf::Texture* monsterTex1;
    sf::Texture* monsterTex2;
    std::vector<Hole> holes;
    sf::Texture* holeSmallTex;
    sf::Texture* holeLargeTex;
    bool gameOver = false;
    float previousPlayerBottom = 0.f;

    void spawnMonsterNearPlatform(const Platform& platform, float windowWidth);
    bool isPositionOverlappingWithAnyObject(sf::Vector2f pos, float width, float height) const;
    void spawnHoleNearPlatform(const Platform& platform, float windowWidth);
    float getSpeedMultiplier() const;

    std::vector<Platform> platforms;

    ResourceManager<sf::Texture> &textureManager;
    std::mt19937 gen;
    Difficulty difficulty;

    float lastPlatformX;
    Platform::PlatformType lastPlatformType;

    float totalScrolledDistance;

public:
    bool isGameOver() const { return gameOver; }
    WorldManager(ResourceManager<sf::Texture> &texMgr, Difficulty diff);
    void spawnInitialPlatforms();
    bool checkHoleCollision(Player& player, sf::Vector2f& outHoleCenter);
    
    float update(Player &player, float deltaTime);
    void draw(sf::RenderWindow &window);
    
    void updateBullets(float deltaTime);
    void renderBullets(sf::RenderWindow& window);
    
    bool isAreaClear(const sf::FloatRect& area, float padding = 30.f) const;
    
    void spawnBullet(sf::Vector2f startPosition);
};



#endif