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

class WorldManager
{
private:
    // Monsters
    std::vector<Monster> monsters;
    sf::Texture* monsterTex1;
    sf::Texture* monsterTex2;
    bool gameOver = false;
    float previousPlayerBottom = 0.f;

    // Helper functions
    void spawnMonsterNearPlatform(const Platform& platform, float windowWidth);
    bool isPositionOverlappingWithAnyObject(sf::Vector2f pos, float width, float height) const;

    float getSpeedMultiplier() const;


    std::vector<Platform> platforms;
    
    ResourceManager<sf::Texture>& textureManager;
    std::mt19937 gen;
    float lastPlatformX;
    Platform::PlatformType lastPlatformType;
    Difficulty difficulty;

public:
    bool isGameOver() const { return gameOver; }
    WorldManager(ResourceManager<sf::Texture> &texMgr, Difficulty diff);
    void spawnInitialPlatforms();
    
    float update(Player& player, float deltaTime);
    
    void draw(sf::RenderWindow& window);
};

#endif