#ifndef WORLDMANAGER_HPP
#define WORLDMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include "Platform.hpp"
#include "Player.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"

class WorldManager
{
private:
    std::vector<Platform> platforms;

    // ارجاع به مدیر منابع برای دسترسی به عکس سکوها
    ResourceManager<sf::Texture> &textureManager;
    std::mt19937 gen;
    float lastPlatformX;
    Platform::PlatformType lastPlatformType;
    Difficulty difficulty;

public:
    WorldManager(ResourceManager<sf::Texture> &texMgr, Difficulty diff);
    void spawnInitialPlatforms();

    // بازیکن به عنوان ورودی داده می‌شود تا برخوردها و دوربین محاسبه شود
    float update(Player &player, float deltaTime);

    void draw(sf::RenderWindow &window);
};

#endif