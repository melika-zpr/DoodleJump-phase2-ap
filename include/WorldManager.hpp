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

    // ارجاع به مدیر منابع برای دسترسی به عکس سکوها
    ResourceManager<sf::Texture> &textureManager;
    std::mt19937 gen;
    Difficulty difficulty;

    float lastPlatformX;
    Platform::PlatformType lastPlatformType;

    // متغیر محاسبه ارتفاع طی شده برای تاخیر در تولید موانع
    float totalScrolledDistance;

public:
    bool isGameOver() const { return gameOver; }
    WorldManager(ResourceManager<sf::Texture> &texMgr, Difficulty diff);
    void spawnInitialPlatforms();

    // بازیکن به عنوان ورودی داده می‌شود تا برخوردها و دوربین محاسبه شود
    float update(Player &player, float deltaTime);
    void draw(sf::RenderWindow &window);
    
    // تابع مشترک برای بررسی تداخل (با حاشیه امن پیش‌فرض 30 پیکسل)
    bool isAreaClear(const sf::FloatRect& area, float padding = 30.f) const;
    
    // تابعی که Game.cpp برای شلیک کردن صدا می‌زند
    void spawnBullet(sf::Vector2f startPosition);
};

#endif