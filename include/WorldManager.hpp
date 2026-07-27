#ifndef WORLDMANAGER_HPP
#define WORLDMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include "Platform.hpp"
#include "Player.hpp"
#include "ResourceManager.hpp"

class WorldManager {
private:
    std::vector<Platform> platforms;
    
    ResourceManager<sf::Texture>& textureManager;
    std::mt19937 gen;
    float lastPlatformX;
    Platform::PlatformType lastPlatformType;

public:
    WorldManager(ResourceManager<sf::Texture>& texMgr);

    void spawnInitialPlatforms();
    
    float update(Player& player, float deltaTime);
    
    void draw(sf::RenderWindow& window);
};

#endif